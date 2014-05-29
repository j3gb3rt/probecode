CC = gcc

mygps: mygps.c
	$(CC) mygps.c -L/usr/lib/arm-linux-gnueabihf/gps -L/usr/lib/arm-linux-gnueabihf/gpsd -o gpsClient

clean:
	rm *.o	
