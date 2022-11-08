// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buffers.h"
#include "MyGameInstance.h"

#pragma warning(disable : 4996)

THIRD_PARTY_INCLUDES_START
#include "Protocol.pb.h"
THIRD_PARTY_INCLUDES_END

/**
 *
 */



	struct PlayerInfo
{
	PlayerInfo() {}

	PlayerInfo(const PlayerInfo& copy)
	{
		id = copy.id;
		name = copy.name;
		playerType = copy.playerType;
		SetLocation(copy.posX, copy.posY, copy.posZ);
		SetRotation(copy.rotX, copy.rotY, copy.rotZ);
		SetSpeed(copy.speed);
		SetMoving(copy.isMoving);
	}

	PlayerInfo(TSharedPtr<PlayerInfo> copy)
	{
		id = copy->id;
		name = copy->name;
		playerType = copy->playerType;
		SetLocation(copy->posX, copy->posY, copy->posZ);
		SetRotation(copy->rotX, copy->rotY, copy->rotZ);
		SetSpeed(copy->speed);
		SetMoving(copy->isMoving);
	}

	void SetLocation(double x, double y, double z)
	{
		{
			FScopeLock lock(&_lock);
			posX = x; posY = y; posZ = z;
		}
	}
	void SetRotation(double x, double y, double z)
	{
		{
			FScopeLock lock(&_lock);
			rotX = x; rotY = y; rotZ = z;
		}
	}

	void SetSpeed(double s)
	{
		{
			FScopeLock lock(&_lock);
			speed = s;
		}
	}

	void SetMoving(bool onOff)
	{
		{
			FScopeLock lock(&_lock);
			isMoving = onOff;
		}
	}

	FRotator GetRotation()
	{
		return FRotator(rotY, rotZ, rotX);
	}

	FVector GetLocation()
	{
		return FVector(posX, posY, posZ);
	}

	uint64 id;
	FString name;
	Protocol::PlayerType playerType;
	double posX, posY, posZ;
	double rotX, rotY, rotZ;
	double speed;
	bool isMoving;
	FCriticalSection _lock;

	bool operator==(PlayerInfo copy)
	{
		if (
			id == copy.id &&
			name == copy.name &&
			playerType == copy.playerType)
			return true;
		else
			return false;
	}

	PlayerInfo& operator=(const PlayerInfo& copy)
	{
		id = copy.id;
		name = copy.name;
		playerType = copy.playerType;
		SetLocation(copy.posX, copy.posY, copy.posZ);
		SetRotation(copy.rotX, copy.rotY, copy.rotZ);
		SetSpeed(copy.speed);
		SetMoving(copy.isMoving);
		return *this;
	}

};


class PacketProcessor;

class MULTITEST_API NetHandler : public TSharedFromThis<NetHandler>
{
	friend class UMyGameInstance;

public:
	NetHandler(const TCHAR* ip, UMyGameInstance* _ownerInstance);
	~NetHandler();
	
	void Connect();

	void Receive();

	void Send(TSharedPtr<ChunkBuffer> serializedBuffer, uint32 size);

	void Exit();

	template<typename T>
	TSharedPtr<ChunkBuffer> Serialize(T& contents,uint32 id)
	{
		TSharedPtr<ChunkBuffer> buffer = _sendBuffers.Reserve();
		
		PacketHeader header;
		header.id = id;
		header.size = contents.ByteSize() + sizeof(PacketHeader);

		//직렬화
		buffer->PushData(&header, sizeof(PacketHeader));
		buffer->ManualWrite(contents.ByteSize());
		contents.SerializeToArray(buffer->GetBuffer() + sizeof(PacketHeader), header.size);
		
		return buffer;
	}


	void SetNonBlocking(bool onOff);

	bool GetConnectionState() { return _connection; }

	void SetConnectionState(bool onOff) { _connection = onOff; }

	void ProcessDelayedRecieve();

	void PushPlayerInfo(uint32 index, TSharedPtr<PlayerInfo> info) { _players.Add(index, info); }

	TSharedPtr<PlayerInfo> GetPlayerInfoRef(uint32 index) 
	{ 
		if (_players.Find(index))
			return _players[index];
		else
		{
			return nullptr;
		}
	}

	PlayerInfo GetPlayerInfo(uint32 index)
	{
		if (_players.Find(index))
		{
			PlayerInfo copy(_players[index]);
			return copy;
		}
		else
		{
			return PlayerInfo();
		}
	}

	TSharedPtr<PlayerInfo> GetMyPlayerInfo() 
	{ 
		if (_myPlayerIndex == -1)
			return nullptr;
		return _players[_myPlayerIndex]; 
	}



	void SetMyPlayerIndex(uint64 index) { _myPlayerIndex = index; }

	UMyGameInstance* GetOwnerInstance() { return _ownerInstance; }

	uint64 GetMyPlayerIndex() { return _myPlayerIndex; }

	void PushSpawnJob(TFunction<void()> job)
	{
		_spawnJobQueue.Enqueue(job);
	}

	void PushJob(TFunction<void()> job)
	{
		_jobQueue.Enqueue(job);
	}

	bool DequeSpawnJob(TFunction<void()>& outFunc)
	{
		if (_spawnJobQueue.Dequeue(outFunc))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	TQueue<TFunction<void()>> _jobQueue;

	FCriticalSection _lock;

private:

	UMyGameInstance* _ownerInstance = nullptr;

	FSocket* _socket;

	ISocketSubsystem* _socketSubsystem;

	TSharedPtr<PacketProcessor> _packetHandler = nullptr;


	TSharedPtr<FInternetAddr> _targetAddr;

	BufferPool _recvBuffers;

	BufferPool _sendBuffers;

	
	std::atomic<bool> _connection = false;


	TMap<uint32, TSharedPtr<PlayerInfo>> _players;

	std::atomic<uint64> _myPlayerIndex = -1;

	TQueue<TFunction<void()>> _spawnJobQueue;

	TQueue<std::pair<TSharedPtr<ChunkBuffer>,uint32>> _delayedBuffer;


};
