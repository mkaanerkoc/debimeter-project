CC			=	gcc
CFLAGS		= 	-DGATEWAY_CODE -DRASPBERRY_COMPILE -std=gnu99
MONGOOSE	=	libraries/mongoose
MODBUS 		=	Modbus
INC			= 	-I../
LIBS		=	-lpthread -lbcm2835 -lm


test: test.o ConfigurationManager.o 
	$(CC) $(CFLAGS)  $^ -o test.exe

test.o: test.c
	$(CC) $(CFLAGS) $(INC) -c test.c $



ConfigurationManager.o: ../ConfigurationManager.c
	$(CC) $(CLAGS) $(INC) -c ../ConfigurationManager.c $<

clean:
	rm -rf *.o debimeter
