// Fill out your copyright notice in the Description page of Project Settings.


#include "NetHandler.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Buffers.h"
#include "PacketProcessor.h"



NetHandler::NetHandler(const TCHAR* ip, UMyGameInstance* ownerInstance)
{
	_socketSubsystem = ISocketSubsystem::Get();

	_ownerInstance = ownerInstance;

	_packetHandler = MakeShared<PacketProcessor>();

	_socket = _socketSubsystem->CreateSocket(NAME_Stream, TEXT("default"), false);
	TSharedRef<FInternetAddr> bindAddr = _socketSubsystem->CreateInternetAddr();
	bindAddr->SetAnyAddress();
	_socket->Bind(bindAddr.Get());
	_socket->SetReuseAddr(false);
	
	_targetAddr = _socketSubsystem->CreateInternetAddr();

	bool isValid;
	_targetAddr->SetIp(ip,isValid);
	_targetAddr->SetPort(7777);

	_socket->Shutdown(ESocketShutdownMode::ReadWrite);
}

NetHandler::~NetHandler()
{	
	Exit();
}

void NetHandler::Connect()
{
	SetNonBlocking(true);
	while (!_socket->Connect(*_targetAddr.Get()))
	{
	}
	SetConnectionState(true);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Connected"));
}

void NetHandler::Receive()
{
	int32 bytesRead;
	TSharedPtr<ChunkBuffer> buffer = _recvBuffers.Reserve();
	if (_socket->Recv(buffer->GetBuffer(),buffer->GetSize(), bytesRead))
	{
		if (bytesRead <= 0)
			goto CHECK;
		
		_jobQueue.Enqueue([=]() {
			PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer->GetBuffer());
			_packetHandler->HandlePacket(SharedThis(this), reinterpret_cast<BYTE*>(header), header->size); 
			buffer->StopUse();
			});
	}
	else
	{
	CHECK:
		switch (_socketSubsystem->GetLastErrorCode())
		{
		case ESocketErrors::SE_EDISCON:
			Exit();
			break;
		}
		buffer->StopUse();
	}
}

void NetHandler::Send(TSharedPtr<ChunkBuffer> serializedBuffer,uint32 size)
{
	int32 bytesSent;
	if (_socket->Send(serializedBuffer->GetBuffer(), size, bytesSent))
	{
		serializedBuffer->StopUse();
	}
	else
	{
		if (_socketSubsystem->GetLastErrorCode() == ESocketErrors::SE_EDISCON)
		{
			Exit();
		}
	}
}

void NetHandler::Exit()
{
	_connection = false;
	if (_socket)
	{
		_socket->Close();
		_socket->Shutdown(ESocketShutdownMode::ReadWrite);
		_socketSubsystem->DestroySocket(_socket);
		_socket = nullptr;
	}
}


void NetHandler::SetNonBlocking(bool onOff)
{
	_socket->SetNonBlocking(true);
}

void NetHandler::ProcessDelayedRecieve()
{
	uint32 size = 0;
	std::vector<std::pair<TSharedPtr<ChunkBuffer>, uint32>> buffers;
	std::pair<TSharedPtr<ChunkBuffer>, uint32> chunkBuffer;
	while (_delayedBuffer.Dequeue(chunkBuffer))
	{
		size += chunkBuffer.second;
		if (size < sizeof(PacketHeader))
		{
			buffers.push_back(chunkBuffer);
			continue;
		}
		else
		{
			buffers.push_back(chunkBuffer);
			if (buffers.size() == 1)
				continue;

			uint32 pos = 0;
			auto& head = buffers[0];

			for (auto buff = buffers.begin()+1; buff != buffers.end(); buff++)
			{
				memcpy(head.first->GetBuffer()+pos,buff->first->GetBuffer(), buff->second);
				buff->first->StopUse();
				pos += buff->second;
			}
			memcpy(head.first->GetBuffer() + pos, chunkBuffer.first->GetBuffer(), chunkBuffer.second);
			pos += chunkBuffer.second;
			chunkBuffer.first->StopUse();

			PacketHeader* header = reinterpret_cast<PacketHeader*>(head.first->GetBuffer());

			while (header->size > pos)
			{
				if (_delayedBuffer.Dequeue(chunkBuffer))
				{
					memcpy(head.first->GetBuffer() + pos, chunkBuffer.first->GetBuffer(), chunkBuffer.second);
					pos += chunkBuffer.second;
					chunkBuffer.first->StopUse();
				}
				else
				{
					TSharedPtr<ChunkBuffer> notCompletechunk = _recvBuffers.Reserve();
					notCompletechunk->PushData(head.first->GetBuffer(), pos);
					_delayedBuffer.Enqueue(std::pair(notCompletechunk,pos));

					head.first->StopUse();
					buffers.clear();
					return;
				}
			}

			_packetHandler->HandlePacket(SharedThis(this), reinterpret_cast<BYTE*>(header), header->size);
			if (pos > header->size)
			{
				head.first->Used(header->size);
				_delayedBuffer.Enqueue(head);
			}

			buffers.clear();
			size = 0;
		}
	}
	
}
