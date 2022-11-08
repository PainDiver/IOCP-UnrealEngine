// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

/**
 * 
 */

void NoneFunc();

class RunnableThread :public FRunnable
{
public:
	RunnableThread(TFunction<void()> job, const TCHAR* name, TFunction<void()> init);

	virtual bool Init()override;

	virtual uint32 Run()override;

	virtual void Stop()override;

	virtual void Exit()override;


private:
	TSharedPtr<FRunnableThread> _runningThread;
	TCHAR* _name;

	bool _bStop = false;

	TFunction<void()> _fInit;
	TFunction<void()> _fJob;
};

class MULTITEST_API ThreadHandler
{

	ThreadHandler() {}
public:

	~ThreadHandler();

	static ThreadHandler* GetInstance()
	{
		static ThreadHandler instance;
		return &instance;
	}

	void LaunchThread(TFunction<void()> job, const TCHAR* name = L"Default",TFunction<void()> init = NoneFunc);

	void JoinThreads();

private:

	TArray<TSharedPtr<RunnableThread>> _threads;

};


