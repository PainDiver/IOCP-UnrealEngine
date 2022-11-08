pushd %~dp0

setlocal

SET MultiTest="../../../MultiTest"
SET ProtoPath="./Protocol.proto"
SET DistPath="../../../../AutoGen/dist

protoc.exe -I=./ --cpp_out=./ ./Protocol.proto

CALL %DistPath%/packetGenerator.exe" --path=%ProtoPath% --output=PacketProcessor --recv=S_ --send=C

XCOPY /Y PacketProcessor.h %MultiTest%
XCOPY /Y Protocol.pb.h %MultiTest%
XCOPY /Y Protocol.pb.cc %MultiTest%

if not exist "../../../../Binaries/Win64/libprotobuf.dll" XCOPY /Y libprotobuf.dll "../../../../Binaries/Win64"


PAUSE