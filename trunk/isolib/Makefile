include ./Make.defines

PROGS = iso8583-test utilities_test

all:	lib	${PROGS}

lib:	${LIB_OBJS} 
		$(AR) ${LIB_NAME} $?
		@rm ${LIB_OBJS}
		${RANLIB} ${LIB_NAME}

${PROGS}: 	
		${CC} ${CFLAGS} -o $@ $< ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
