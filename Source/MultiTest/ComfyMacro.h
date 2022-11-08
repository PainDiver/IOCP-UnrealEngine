// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


FString ConvertUTF8toFString(std::string string)
{
	TCHAR* str = UTF8_TO_TCHAR(string.c_str());
	return FString(str);
}