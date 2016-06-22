##include的目录都是以core下面起始，所以-I要包含core目录，否则会找不到
INCLUDE=-I./
#需要c++11的支持
FLAGS=-std=c++11 -g

#中间.o文件生成的目录
OBJDIR=build

#得到所有以后缀.cc结尾的文件
SRCS=${wildcard src/*/*.cc src/*/*.h *.cc}
#将目录去掉
NOTDIRSRCS=${notdir ${SRCS}}
#得到需要的.o文件，包括obj目录
OBJS=${patsubst %.cc, ${OBJDIR}/%.o, ${NOTDIRSRCS}}

#静态库名称
#LIBNAME=server.a
#动态库文件名
#SONAME=server.so

#all:${OBJS} lib

#生成所有.o文件
#${OBJDIR}/%.o:*/%.cc
	#检查是否存在obj目录，没有就创建
#	if [ ! -d ${OBJDIR} ] ; then mkdir obj; fi
#	g++ -fPIC ${INCLUDE} $< ${FLAGS} -c -o $@

#lib:
	#打包成静态库
#	ar -cr ${LIBNAME} ${OBJDIR}/*.o
	#打包成动态库
#	g++ -fPIC -shared ${OBJDIR}/*.o -o ${SONAME}

#删除创建的的文件
#clean:
#	rm ${OBJDIR}/* *.a *.so

all:server

server:${SRCS}
	g++ ${SRCS} ${INCLUDE} ${FLAGS} -o $@

clean:
	rm server
