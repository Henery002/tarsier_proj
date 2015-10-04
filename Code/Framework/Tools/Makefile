TOOLS_PATH := $(shell pwd)/$(lastword $(MAKEFILE_LIST))
TOOLS_PATH := $(shell dirname $(TOOLS_PATH))

all : exeBackdoorClient

exeBackdoorClient :
	@cd $(TOOLS_PATH)/BackdoorClient && make

clean :
	@cd $(TOOLS_PATH)/BackdoorClient && make clean

distclean :
	@cd $(TOOLS_PATH)/BackdoorClient && make distclean

rebuild :
	@cd $(TOOLS_PATH)/BackdoorClient && make rebuild

distrebuild :
	@cd $(TOOLS_PATH)/BackdoorClient && make distrebuild
