OBJ = pmms.o
CFLAGS = -std=c99 -lrt -pthread -D_XOPEN_SOURCE=500 -g
EXEC = pmms

$(EXEC) : $(OBJ)
	gcc $(CFLAGS) pmms.c -o $(EXEC)

clean : 
	rm $(EXEC)
