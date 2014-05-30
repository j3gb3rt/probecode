#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <time.h>

#define DEBUG
#define TRACE

static volatile int tick;

void ticker(void) {
	tick++;
}


int main(int argc, char* argv) {
	wiringPiSetup();
	pinMode(0, INPUT);
	pullUpDnControl(0, PUD_OFF);
	wiringPiISR(0, INT_EDGE_RISING, &ticker);
	
	tick = 0;
	time_t last_time;
	time_t curr_time;
	last_time = time(NULL);
	double speed;

	while (1) {
		curr_time = time(NULL);
		if (difftime(curr_time, last_time) >= 1) {
			last_time = curr_time;
			speed = tick * 2.5;
			printf( "%i, %.2f, %.2f\n",tick, 2.5, speed);
			tick = 0;
		}
	}
	return 0;
}
