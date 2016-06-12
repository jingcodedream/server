INCLUDE=-I./
FLAGS=-std=c++11

OBJDIR=obj
#中间.o文件生成的目录

SRCS=${wildcard */*.cc} #得到所有以后缀.cc结尾的文件
NOTDIRSRCS=${notdir ${SRCS}} #将目录去掉
OBJS=${patsubst %.cc, ${OBJDIR}/%.o, ${NOTDIRSRCS}} #得到要声称的.o文件，包括obj目录

LIBNAME=server.a
SONAME=server.so #动态库文件名

all:${OBJS} lib

${OBJDIR}/%.o:*/%.cc
	if [ ! -d obj ] ; then mkdir obj; fi
	g++ -fPIC ${INCLUDE} $< ${FLAGS} -c -o $@

lib:
	ar -cr ${LIBNAME} ${OBJDIR}/*.o
	g++ -fPIC -shared ${OBJDIR}/*.o -o ${SONAME}

clean:
	rm ${OBJDIR}/* *.a
