// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ChunkBuffer
{
public:
	ChunkBuffer(uint32 size, BYTE* buffer,bool used) :_used(used),_size(size), _buffer(buffer) {}

	void StopUse() { _used = false;}

	void StartUse(uint32 size, BYTE* buffer) 
	{
		_used = true, _writeSize = 0, _size = size, _buffer = buffer; _usedSize= 0;
	}

	BYTE* GetBuffer() { return _buffer + _usedSize; }

	BYTE* GetContentBuffer();

	bool IsUsed() { return _used; }

	void Used(uint32 size) { _usedSize += size; }

	uint32 GetSize() { return _size; }

	uint32 GetFreeSize() { return _size - _writeSize; }

	uint32 GetWriteSize() { return _writeSize; }

	void ManualWrite(uint32 size)
	{	
		_writeSize += size;
	}

	bool PushData(void* buffer, uint32 size) 
	{
		if (size > _size)
			return false;

		memcpy(_buffer, buffer, size);
		_writeSize += size;
		return true;
	}

private:
	bool _used = false;
	uint32 _size = 0;
	uint32 _writeSize = 0;
	BYTE* _buffer = nullptr;

	uint32 _usedSize = 0;
};


class MULTITEST_API BufferPool
{
	enum
	{
		DEFAULT_CHUNK_SIZE = 1024,
		DEFAULT_BUFFER_SIZE = 0x10000,
	};

public:
	BufferPool();
	~BufferPool();

	TSharedPtr<ChunkBuffer> Reserve();

private:
	BYTE* GetUsedPos() { return &_buffers[_usedPos]; }

	FCriticalSection _lock;

	std::vector<BYTE> _buffers;
	std::vector<TSharedPtr<ChunkBuffer>> _chunks;


	uint32 _usedPos;

};