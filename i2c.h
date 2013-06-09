void com_status(void) {
	PORTA=0;
	RA3=BF;
	RA1=WCOL;
	RA2=R_W;
	RA0=SSPIF;
}

void i2c_init(void) {
	TRISC3=1; // nastavi RC3 a RC4 na input. Ma byt provedeno predtim, nez se inicializuje i2c
	TRISC4=1;
	SSPCON=0b00101000; //nastavi i2c master mod v PIC16F887
	SSPADD=0b0001010; //nastavi prenosovou rychlost 0b00001100 i2c na 400khz pri 20Mhz REG=((Fosc/bitrate)/4)-1
	CKE=1;     // use I2C levels      worked also with '0'
	SSPIF=0;      // clear SSPIF interrupt flag
 	BCLIF=0;      // clear bus collision flag
}

void i2c_idle(void) {
	while ((SSPCON2 & 0b00011111) | R_W) { //zkontroluje jestli je I2C idle a jestli nebezi komunikace na sbernici
		//RD0=1;
	}
	//RD0=0;
}

void i2c_start(void) {
	i2c_idle();
	SEN=1;
}

void i2c_restart(void) {
	i2c_idle();
	RSEN=1;
}

void i2c_stop(void) {
	i2c_idle();
	PEN=1;
}

int i2c_rx(int ack) {
	unsigned char i2c_data;
	i2c_idle();
	RCEN=1; //  !!!!!!!!!!! RCEN se musi pak nekdy vypnout uzivatelem, cpu nemuze vedet, kdy chci ukoncit komunikaci
	i2c_idle();
	i2c_data=SSPBUF;
	i2c_idle();
	if (ack) {
		ACKDT=0;
	} else {
		ACKDT=1;
	}
	ACKEN=1; //odesle ACKDT
	return i2c_data;
}

int i2c_tx(unsigned char data) {
	//com_status();
	i2c_idle();
	RD5=1;
	//com_status();
	if (RD5) {
		SSPBUF=data;
	}
	RD5=0;
	return ACKSTAT;
}

void mpu6050_init() {
	i2c_start();
	if (prenos(devaddr)) {
		RD0=1;
		while (1) {}
	}
	if (prenos(107)) {
		RD1=1;
		while (1) {}
	}
	if (prenos(0b00000000)) { //zapis do reg 107 - vypne sleep, zapne cycle, vypne teplomer a nastavi freq 8MHz
		RD2=1;
		while (1) {}
	}
	i2c_stop();
}