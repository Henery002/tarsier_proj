include ../../Tarsier.mk

all : libCore libNet libApp

libCore :
	@cd $(TARSIER_MODULES_PATH)/Core && make

libNet : libCore
	@cd $(TARSIER_MODULES_PATH)/Net && make

libApp : libNet libCore
	@cd $(TARSIER_MODULES_PATH)/App && make

clean :
	@cd $(TARSIER_MODULES_PATH)/Core && make clean
	@cd $(TARSIER_MODULES_PATH)/Net && make clean
	@cd $(TARSIER_MODULES_PATH)/App && make clean

distclean :
	@cd $(TARSIER_MODULES_PATH)/Core && make distclean
	@cd $(TARSIER_MODULES_PATH)/Net && make distclean
	@cd $(TARSIER_MODULES_PATH)/App && make distclean

rebuild:
	@cd $(TARSIER_MODULES_PATH)/Core && make rebuild
	@cd $(TARSIER_MODULES_PATH)/Net && make rebuild
	@cd $(TARSIER_MODULES_PATH)/App && make rebuild

distrebuild:
	@cd $(TARSIER_MODULES_PATH)/Core && make distrebuild
	@cd $(TARSIER_MODULES_PATH)/Net && make distrebuild
	@cd $(TARSIER_MODULES_PATH)/App && make distrebuild
