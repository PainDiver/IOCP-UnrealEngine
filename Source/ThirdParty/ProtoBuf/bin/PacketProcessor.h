// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetHandler.h"


/**
 *
 *
 */

struct PacketHeader
{
	uint16 size;
	uint16 id;
};

class Packet
{
public:

	uint16 GetPacketType() { return _header.id; }
	uint16 GetSize() { return _header.size; }

private:
	PacketHeader _header;
};

//AutoGen Enum
enum
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_GAME = 1002,
	PKT_S_ENTER_GAME = 1003,
	PKT_C_CHAT = 1004,
	PKT_S_CHAT = 1005,
	PKT_C_MOVE = 1006,
	PKT_S_MOVE = 1007,
};


bool Handle_INVALID(TSharedPtr<NetHandler> netHandler, BYTE* buffer, uint32 size);

// AutoGen
bool Handle_S_LOGIN(TSharedPtr<NetHandler> netHandler,Protocol::S_LOGIN pkt);
bool Handle_S_ENTER_GAME(TSharedPtr<NetHandler> netHandler,Protocol::S_ENTER_GAME pkt);
bool Handle_S_CHAT(TSharedPtr<NetHandler> netHandler,Protocol::S_CHAT pkt);
bool Handle_S_MOVE(TSharedPtr<NetHandler> netHandler,Protocol::S_MOVE pkt);

class MULTITEST_API PacketProcessor : public TSharedFromThis<PacketProcessor>
{
public:

	PacketProcessor()
	{
		for (int i = 0; i < UINT16_MAX; i++)
		{
			HandleFunction[i] = Handle_INVALID;
		}

		//Auto Gen
		HandleFunction[PKT_S_LOGIN] = [this](TSharedPtr<NetHandler> netHandler,BYTE* buffer, uint32 size) {return ExecHandleFunction < Protocol::S_LOGIN > (Handle_S_LOGIN,netHandler, buffer, size); };
		HandleFunction[PKT_S_ENTER_GAME] = [this](TSharedPtr<NetHandler> netHandler,BYTE* buffer, uint32 size) {return ExecHandleFunction < Protocol::S_ENTER_GAME > (Handle_S_ENTER_GAME,netHandler, buffer, size); };
		HandleFunction[PKT_S_CHAT] = [this](TSharedPtr<NetHandler> netHandler,BYTE* buffer, uint32 size) {return ExecHandleFunction < Protocol::S_CHAT > (Handle_S_CHAT,netHandler, buffer, size); };
		HandleFunction[PKT_S_MOVE] = [this](TSharedPtr<NetHandler> netHandler,BYTE* buffer, uint32 size) {return ExecHandleFunction < Protocol::S_MOVE > (Handle_S_MOVE,netHandler, buffer, size); };
	}

	~PacketProcessor();

	template<typename PacketType>
	bool ExecHandleFunction(TFunction<bool(TSharedPtr<NetHandler>,PacketType)> func, TSharedPtr<NetHandler> netHandler, BYTE* buffer, uint32 size)
	{
		PacketType pkt;
		pkt.ParseFromArray(buffer, size);
		return func(netHandler, pkt);
	}

	bool HandlePacket(TSharedPtr<NetHandler> netHandler, BYTE* bufferWithHeader, uint32 sizeWithHeader)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(bufferWithHeader);
		return HandleFunction[header->id](netHandler,bufferWithHeader + sizeof(PacketHeader), header->size - sizeof(PacketHeader));
	}

private:

	TFunction<bool(TSharedPtr<NetHandler> netHandler, BYTE* buffer, uint32 size)> HandleFunction[UINT16_MAX];


};
