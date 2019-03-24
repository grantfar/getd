CFLAGS = -Wall -pedantic -std=gnu99 -g
LFLAGS = -lnanomsg

getd: main.o requestHandlers.o requestVerify.o
	gcc $(CFLAGS) main.o requestHandlers.o requestVerify.o $(LFLAGS) -o getd

main.o: main.c requestHandlers.h requests.h

requestHandlers.o: requestHandlers.c requestHandlers.h requestVerify.h requests.h

requestVerify.o: requestVerify.c requestVerify.h requests.h

clean:
	rm -f *.o getd