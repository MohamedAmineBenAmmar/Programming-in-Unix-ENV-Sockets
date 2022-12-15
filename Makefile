BUILD_DIR = build

permission_management: clean_up
	@for f in $(shell ls ${BUILD_DIR}); do chmod 700 ./${BUILD_DIR}/$${f} ; done

clean_up: FIFO_Executables TCP_Executables UDP_Executables
	rm ${BUILD_DIR}/*.o

TCP_Executables:
	gcc communication/TCP/server.c -o build/TCP_Server && gcc communication/TCP/client.c -o build/TCP_Client

UDP_Executables: 
	gcc communication/UDP/server.c -o build/UDP_Server && gcc communication/UDP/client.c -o build/UDP_Client

FIFO_Executables: FIFO_Client.o FIFO_Client_Handlers.o FIFO_Server.o FIFO_Server_Handlers.o 
	gcc ${BUILD_DIR}/FIFO_Server.o ${BUILD_DIR}/FIFO_Server_Handlers.o -o ${BUILD_DIR}/FIFO_Server && gcc ${BUILD_DIR}/FIFO_Client.o ${BUILD_DIR}/FIFO_Client_Handlers.o -o ${BUILD_DIR}/FIFO_Client  

FIFO_Client.o: communication/FIFO/client/client.c communication/FIFO/client/handlers_cli.h global/*
	gcc -c communication/FIFO/client/client.c -o build/FIFO_Client.o

FIFO_Client_Handlers.o: communication/FIFO/client/handlers_cli.c communication/FIFO/client/handlers_cli.h global/*
	gcc -c communication/FIFO/client/handlers_cli.c -o build/FIFO_Client_Handlers.o

FIFO_Server.o: communication/FIFO/server/server.c communication/FIFO/server/handlers_serv.h global/*
	gcc -c communication/FIFO/server/server.c -o build/FIFO_Server.o

FIFO_Server_Handlers.o: communication/FIFO/server/handlers_serv.c communication/FIFO/server/handlers_serv.h global/*
	gcc -c communication/FIFO/server/handlers_serv.c -o build/FIFO_Server_Handlers.o
