SERVICES_PATH := $(shell pwd)/$(lastword $(MAKEFILE_LIST))
SERVICES_PATH := $(shell dirname $(SERVICES_PATH))

all : exeGateServer

exeGateServer :
	@cd $(SERVICES_PATH)/GateServer && make

clean :
	@cd $(SERVICES_PATH)/GateServer && make clean

distclean :
	@cd $(SERVICES_PATH)/GateServer && make distclean

rebuild :
	@cd $(SERVICES_PATH)/GateServer && make rebuild

distrebuild :
	@cd $(SERVICES_PATH)/GateServer && make distrebuild
