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
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};


bool Handle_INVALID(TSharedPtr<NetHandler> netHandler, BYTE* buffer, uint32 size);

// AutoGen
{%- for pkt in parser.recv_pkt %}
bool Handle_{{pkt.name}}(TSharedPtr<NetHandler> netHandler,Protocol::{{pkt.name}} pkt);
{%- endfor %}

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
		{%- for pkt in parser.recv_pkt %}
		HandleFunction[PKT_{{pkt.name}}] = [this](TSharedPtr<NetHandler> netHandler,BYTE* buffer, uint32 size) {return ExecHandleFunction < Protocol::{{pkt.name}} > (Handle_{{pkt.name}},netHandler, buffer, size); };
		{%- endfor %}
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

