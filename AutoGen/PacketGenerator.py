import argparse
import jinja2
import os

import AutoGen
import FileRevisor


def main():
    
    fileRevisor = FileRevisor.FileRevisor(r"C:\Users\hyo29\Documents\Unreal Projects\MultiTest\Source\ThirdParty\ProtoBuf\bin\Protocol.pb.cc")
    fileRevisor.ReviseFile()

    argParser = argparse.ArgumentParser(description = "ProtocolReviser")
    # 아래 타입을 실행 시 인자로 받음
    # bat파일의 Call 파일, 인자순으로 넘겨주면 됨
    argParser.add_argument('--path',type = str, default = r'C:/Users/hyo29/Documents/Unreal Projects/MultiTest/Source/ThirdParty/ProtoBuf/bin/Protocol.proto', help = 'proto path')
    argParser.add_argument('--output',type = str, default = 'PacketProcessor', help = 'output file')
    argParser.add_argument('--recv',type = str, default = 'C_', help = 'recv convention')
    argParser.add_argument('--send',type = str, default = 'S_', help = 'send convention')
    
    args = argParser.parse_args() # main에서 얻어온 매개변수들 (bat파일에서 매개변수로 넘겨주는 인자들이라 보면 됨 )
    
    print(args.path)
    
    revisor = AutoGen.PacketRevisor(1000,args.recv,args.send)
    revisor.RevisePacket(args.path)

    file_loader = jinja2.FileSystemLoader('TemplatePackets')
    env = jinja2.Environment( loader = file_loader)

    template = env.get_template('PacketProcessor.h')
    output = template.render(parser = revisor , outputName = args.output)

    f = open(args.output+'.h', 'w+')
    f.write(output)
    f.close()

    print(output)
    
    return



if __name__ == '__main__':
    main()

