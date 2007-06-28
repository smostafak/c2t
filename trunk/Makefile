include ./Make.defines

OBJS = iso8583-test.o
PROGS = iso8583-test

all:	lib	${PROGS}

lib:	${LIB_OBJS} 
		$(AR) ${LIB_NAME} $?
		${RANLIB} ${LIB_NAME}

${PROGS}:	${OBJS}
		${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

${OBJ}: 
		${CC} ${CFLAGS} $?

clean:
		rm -f ${PROGS} ${CLEANFILES}
