juanipc: main.o server.o camera.o
	gcc -o juanipc main.o server.o camera.o

main.o: main.c server.h camera.h
	gcc -c main.c

server.o: server.c server.h
	gcc -c server.c

camera.o: camera.c camera.h
	gcc -c camera.c

clean: juanipc main.o server.o camera.o
	rm juanipc main.o server.o camera.o

