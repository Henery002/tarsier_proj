EXAMPLES_PATH := $(shell pwd)/$(lastword $(MAKEFILE_LIST))
EXAMPLES_PATH := $(shell dirname $(EXAMPLES_PATH))

all : exeHelloWorld exeEchoServer exeEchoClient exeTestConnector

exeHelloWorld :
	@cd $(EXAMPLES_PATH)/HelloWorld && make

exeEchoServer :
	@cd $(EXAMPLES_PATH)/EchoServer && make

exeEchoClient :
	@cd $(EXAMPLES_PATH)/EchoClient && make

exeTestConnector:
	@cd $(EXAMPLES_PATH)/TestConnector && make

clean :
	@cd $(EXAMPLES_PATH)/HelloWorld && make clean
	@cd $(EXAMPLES_PATH)/EchoServer && make clean
	@cd $(EXAMPLES_PATH)/EchoClient && make clean
	@cd $(EXAMPLES_PATH)/TestConnector && make clean

distclean :
	@cd $(EXAMPLES_PATH)/HelloWorld && make distclean
	@cd $(EXAMPLES_PATH)/EchoServer && make distclean
	@cd $(EXAMPLES_PATH)/EchoClient && make distclean
	@cd $(EXAMPLES_PATH)/TestConnector && make distclean

rebuild :
	@cd $(EXAMPLES_PATH)/HelloWorld && make rebuild
	@cd $(EXAMPLES_PATH)/EchoServer && make rebuild
	@cd $(EXAMPLES_PATH)/EchoClient && make rebuild
	@cd $(EXAMPLES_PATH)/TestConnector && make rebuild

distrebuild :
	@cd $(EXAMPLES_PATH)/HelloWorld && make distrebuild
	@cd $(EXAMPLES_PATH)/EchoServer && make distrebuild
	@cd $(EXAMPLES_PATH)/EchoClient && make distrebuild
	@cd $(EXAMPLES_PATH)/TestConnector && make distrebuild
