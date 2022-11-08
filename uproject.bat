echo off

rmdir /s /q .vs
rmdir /s /q Binaries
rmdir /s /q Intermediate
rmdir /s /q Saved
rmdir /s /q DerivedDataCache

if not exist "Binaries" mkdir Binaries

if not exist "Binaries/Win64" mkdir "Binaries/Win64"

Call "Source/ThirdParty/ProtoBuf/bin/GenPackets.bat"