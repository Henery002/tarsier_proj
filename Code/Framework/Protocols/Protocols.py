import os
import platform
import subprocess

def Protoc(fileName):
    print(fileName)

    protoc = "protoc"

    if platform.system() == "Windows":
        protoc = "./../../../Tools/protoc"

    protobufSrcPath = "../../Thirdparty/protobuf-2.6.1/src"

    subprocess.call([protoc, "--proto_path=.", "--proto_path=" + protobufSrcPath, "--cpp_out=./", fileName])

precwd = os.getcwd()
os.chdir(os.path.split(os.path.realpath(__file__))[0])

Protoc("./Net/NetProto.proto")
Protoc("./App/AppProto.proto")

os.chdir(precwd)
