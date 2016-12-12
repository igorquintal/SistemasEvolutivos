all: cv	

cv:	
	gcc -o cvtest cvtest.c `pkg-config --cflags --libs opencv` -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc


