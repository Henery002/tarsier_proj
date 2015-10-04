# 加载公共定义
include ../../../Tarsier.mk

# 扫描主目录下的代码
$(TARSIER_RUNTIME_PATH)/TestConnector : $(patsubst %.cpp, obj/%.o, $(wildcard *.cpp))
	$(CC) -o $@ $? $(DEPLIB)

# 编译主目录下的代码
obj/%.o : %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

# 依赖文件列表
DEP_FILES = $(patsubst %.cpp, obj/%.d, $(wildcard *.cpp))

obj/%.d : %.cpp
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	sed -i '1s/^\/obj\///' $@; \
	rm $@.$$$$

include $(DEP_FILES)

clean :
	find . -name "core.*" | xargs rm -f
	find . -name "*.[a|o|d]" | xargs rm -f
	rm -f $(TARSIER_RUNTIME_PATH)/TestConnector
	rm -f $(TARSIER_RUNTIME_PATH)/TestConnector.log
	rm -f $(TARSIER_RUNTIME_PATH)/TestConnector.pid

distclean :
	rm -f $(TARSIER_RUNTIME_PATH)/TestConnector

rebuild :
	make clean && make

distrebuild :
	make distclean && make
