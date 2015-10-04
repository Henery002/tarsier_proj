TARSIER_FRAMEWORK_PATH := $(shell pwd)/$(lastword $(MAKEFILE_LIST))
TARSIER_FRAMEWORK_PATH := $(shell dirname $(TARSIER_FRAMEWORK_PATH))

all : dirProtocols dirModules dirExamples dirServices dirTools

dirProtocols :
	@cd $(TARSIER_FRAMEWORK_PATH)/Protocols && make

dirModules : dirProtocols
	@cd $(TARSIER_FRAMEWORK_PATH)/Modules && make

dirExamples : dirModules
	@cd $(TARSIER_FRAMEWORK_PATH)/Examples && make

dirServices : dirModules
	@cd $(TARSIER_FRAMEWORK_PATH)/Services && make

dirTools : dirModules
	@cd $(TARSIER_FRAMEWORK_PATH)/Tools && make

clean :
	@cd $(TARSIER_FRAMEWORK_PATH)/Protocols && make clean
	@cd $(TARSIER_FRAMEWORK_PATH)/Modules && make clean
	@cd $(TARSIER_FRAMEWORK_PATH)/Examples && make clean
	@cd $(TARSIER_FRAMEWORK_PATH)/Services && make clean
	@cd $(TARSIER_FRAMEWORK_PATH)/Tools && make clean

distclean :
	@cd $(TARSIER_FRAMEWORK_PATH)/Protocols && make distclean
	@cd $(TARSIER_FRAMEWORK_PATH)/Modules && make distclean
	@cd $(TARSIER_FRAMEWORK_PATH)/Examples && make distclean
	@cd $(TARSIER_FRAMEWORK_PATH)/Services && make distclean
	@cd $(TARSIER_FRAMEWORK_PATH)/Tools && make distclean

rebuild :
	@cd $(TARSIER_FRAMEWORK_PATH)/Protocols && make rebuild
	@cd $(TARSIER_FRAMEWORK_PATH)/Modules && make rebuild
	@cd $(TARSIER_FRAMEWORK_PATH)/Examples && make rebuild
	@cd $(TARSIER_FRAMEWORK_PATH)/Services && make rebuild
	@cd $(TARSIER_FRAMEWORK_PATH)/Tools && make rebuild

distrebuild :
	@cd $(TARSIER_FRAMEWORK_PATH)/Protocols && make distrebuild
	@cd $(TARSIER_FRAMEWORK_PATH)/Modules && make distrebuild
	@cd $(TARSIER_FRAMEWORK_PATH)/Examples && make distrebuild
	@cd $(TARSIER_FRAMEWORK_PATH)/Services && make distrebuild
	@cd $(TARSIER_FRAMEWORK_PATH)/Tools && make distrebuild
