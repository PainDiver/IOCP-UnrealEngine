// Fill out your copyright notice in the Description page of Project Settings.


#include "Buffers.h"
#include "Misc/ScopeLock.h"
#include "NetHandler.h"
#include "PacketProcessor.h"

BufferPool::BufferPool()
{
	_buffers.resize(DEFAULT_BUFFER_SIZE);
	_usedPos = 0;
}

BufferPool::~BufferPool()
{
	_buffers.clear();
}


TSharedPtr<ChunkBuffer> BufferPool::Reserve()
{
	uint32 chunkSize = DEFAULT_CHUNK_SIZE;
	{
		FScopeLock lock(&_lock);
		
		for (auto& chunkBuffer : _chunks)
		{
			if (chunkBuffer->IsUsed() == false  && chunkBuffer->GetSize() >= chunkSize)
			{
				chunkBuffer->StartUse(chunkSize,chunkBuffer->GetBuffer());
				return chunkBuffer;
			}
		}

		TSharedPtr<ChunkBuffer> chunkBuffer = MakeShared<ChunkBuffer>(chunkSize, &_buffers[_usedPos], true);
		_usedPos += chunkSize;
		_chunks.push_back(chunkBuffer);

		return chunkBuffer;
	}
}

BYTE* ChunkBuffer::GetContentBuffer()
{
	return _buffer + sizeof(PacketHeader);
}
