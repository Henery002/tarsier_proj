include ../../Tarsier.mk

all : libNetProto libAppProto

libNetProto :
	@cd $(TARSIER_PROTOCOLS_PATH)/Net && make

libAppProto : libNetProto
	@cd $(TARSIER_PROTOCOLS_PATH)/App && make

clean :
	@cd $(TARSIER_PROTOCOLS_PATH)/Net/ && make clean
	@cd $(TARSIER_PROTOCOLS_PATH)/App/ && make clean

distclean :
	@cd $(TARSIER_PROTOCOLS_PATH)/Net/ && make distclean
	@cd $(TARSIER_PROTOCOLS_PATH)/App/ && make distclean

rebuild:
	@cd $(TARSIER_PROTOCOLS_PATH)/Net/ && make rebuild
	@cd $(TARSIER_PROTOCOLS_PATH)/App/ && make rebuild

distrebuild:
	@cd $(TARSIER_PROTOCOLS_PATH)/Net/ && make distrebuild
	@cd $(TARSIER_PROTOCOLS_PATH)/App/ && make distrebuild
