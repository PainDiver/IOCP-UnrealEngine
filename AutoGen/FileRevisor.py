import csv

class FileRevisor:
    def __init__(self,path = r"C:\Users\hyo29\Documents\Unreal Projects\MultiTest\Source\ThirdParty\ProtoBuf\bin\Protocol.pb.cc"):
        self._path = path

    def ReviseFile(self):
        booleanName = ["success","moving"]
        
        f = open(self._path, 'r',encoding = "UTF8")
        lines=[]

        if(f.closed == True):
            print("file not opened")
            return
        else:
            lines = f.readlines()
            f.close()
   
        f = open(self._path, 'w',encoding = "UTF8")
    
        if(f.closed == True):
            print("filed not opened")
            print("Job Failed")
            return
        else:
            f.write("#define PROTOBUF_USE_DLLS\n")
            for line in lines:
                if(line == "#define PROTOBUF_USE_DLLS\n"):
                    continue
                for boolName in booleanName:
                    if("_impl_."+boolName+"_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);" in line):
                        print("found")
                        line = r"          _impl_."+boolName+"_ = (bool)::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);"
                        break
                f.write(line)    
            f.close()
            print("Job Done")
        
        return
