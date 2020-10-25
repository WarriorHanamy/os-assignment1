#include "kernel/types.h"
#include "user.h"

int main(int argc, char *argv[]) {
	int sleep_sec;
	if (argc < 2){
		printf("Invalid Usage: too few arguments\n");
		exit(1);
	}

	sleep_sec = atoi(argv[1]);
	if (sleep_sec > 0){
		sleep(sleep_sec);
	} else {
		printf("Invalid interval %s\n", argv[1]);
	}
	exit(0);
}
