void wait(void) {
	int i=255;
	while (i--) {
	}
}

void long_wait(void) {
	int i=255;
	while (i) {
		wait();
		i--;
	}
}



