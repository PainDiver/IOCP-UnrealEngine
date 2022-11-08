// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreadHandler.h"



ThreadHandler::~ThreadHandler()
{
	JoinThreads();
}

void ThreadHandler::LaunchThread(TFunction<void()> job,const TCHAR* name, TFunction<void()> init)
{
	_threads.Push(MakeShared<RunnableThread>(job,name,init));
}

void ThreadHandler::JoinThreads()
{
	for (auto& thread : _threads)
	{
		thread->Stop();
	}
	UE_LOG(LogTemp,Warning,TEXT("All Thread joined"));

}


RunnableThread::RunnableThread(TFunction<void()> job,const TCHAR* name, TFunction<void()> init)
{
	_fJob = job;
	_fInit = init;
	_runningThread = MakeShareable(FRunnableThread::Create(this, name));
}

bool RunnableThread::Init()
{
	_fInit();
	return true;
}

uint32 RunnableThread::Run()
{
	while (_bStop == false)
	{
		_fJob();
	}
	return 0;
}

void RunnableThread::Stop()
{
	_bStop = true;
}

void RunnableThread::Exit()
{
}

void NoneFunc()
{
}