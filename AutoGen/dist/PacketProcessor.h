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
	uint32 GetSize() { return _header.size; }

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
};


bool Handle_INVALID(BYTE* buffer, uint32 size);

// AutoGen
bool Handle_C_LOGIN(Protocol::C_LOGIN pkt);
bool Handle_C_ENTER_GAME(Protocol::C_ENTER_GAME pkt);
bool Handle_C_CHAT(Protocol::C_CHAT pkt);

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
		HandleFunction[C_LOGIN] = [this](BYTE* buffer, uint32 size) {return ExecHandleFunction < Protocol::C_LOGIN > (Handle_C_LOGIN, buffer, size); };
		HandleFunction[C_ENTER_GAME] = [this](BYTE* buffer, uint32 size) {return ExecHandleFunction < Protocol::C_ENTER_GAME > (Handle_C_ENTER_GAME, buffer, size); };
		HandleFunction[C_CHAT] = [this](BYTE* buffer, uint32 size) {return ExecHandleFunction < Protocol::C_CHAT > (Handle_C_CHAT, buffer, size); };
	}

	~PacketProcessor();

	template<typename PacketType>
	bool ExecHandleFunction(TFunction<bool(PacketType)> func, BYTE* buffer, uint32 size)
	{
		PacketType pkt;
		pkt.ParseFromArray(buffer, size);
		return func(pkt);
	}

	bool HandlePacket(BYTE* bufferWithHeader, uint32 sizeWithHeader)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(bufferWithHeader);
		header->id;
		return HandleFunction[header->id](bufferWithHeader + sizeof(PacketHeader), sizeWithHeader - sizeof(PacketHeader));
	}

private:

	TFunction<bool(BYTE* buffer, uint32 size)> HandleFunction[UINT16_MAX];


};
