#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void signalRoutine(int);

int main(void) {
	printf("signal processing demo program\n");
	while (1) {
		signal(SIGINT, signalRoutine);
	}
}

void signalRoutine(int dummy) {
	printf("Signal routine called[%d]\n", dummy);
	exit(0);
}