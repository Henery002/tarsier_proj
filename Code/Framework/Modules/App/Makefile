# 加载公共定义
include ../../../Tarsier.mk

# 扫描主目录下的代码
libApp.a : $(patsubst %.cpp, obj/%.o, $(wildcard *.cpp))
	ar rcs $@ $?
	ranlib $@

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
	find . -name "*.[a|o|d]" | xargs rm -f

distclean :
	find . -name "*.[a]" | xargs rm -f

rebuild :
	make clean && make

distrebuild :
	make distclean && make
