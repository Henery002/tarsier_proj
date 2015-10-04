TARSIER_CODE_PATH := $(shell pwd)/$(lastword $(MAKEFILE_LIST))
TARSIER_CODE_PATH := $(shell dirname $(TARSIER_CODE_PATH))
TARSIER_MODULES_PATH := $(TARSIER_CODE_PATH)/Framework/Modules
TARSIER_PROTOCOLS_PATH := $(TARSIER_CODE_PATH)/Framework/Protocols
TARSIER_RUNTIME_PATH := $(TARSIER_CODE_PATH)/../Runtime

CC = g++

CFLAGS += -g
CFLAGS += -I/usr/local/include
CFLAGS += -I$(TARSIER_MODULES_PATH)
CFLAGS += -I$(TARSIER_PROTOCOLS_PATH)
CFLAGS += -I..

DEPLIB += $(TARSIER_MODULES_PATH)/App/libApp.a 
DEPLIB += $(TARSIER_PROTOCOLS_PATH)/App/libAppProto.a 
DEPLIB += $(TARSIER_MODULES_PATH)/Net/libNet.a 
DEPLIB += $(TARSIER_PROTOCOLS_PATH)/Net/libNetProto.a 
DEPLIB += $(TARSIER_MODULES_PATH)/Core/libCore.a 
DEPLIB += /usr/local/lib/libboost_program_options.a
DEPLIB += /usr/local/lib/libboost_date_time.a
DEPLIB += /usr/local/lib/libboost_system.a
DEPLIB += /usr/local/lib/libboost_thread.a
DEPLIB += /usr/local/lib/libprotobuf.a
DEPLIB += -lrt
