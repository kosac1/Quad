#include "pic16f887.h"
#include "i2c.h"

typedef unsigned int word;
 word __at _CONFIG1 __CONFIG = (
                //_FCMEN_OFF &
                //_IESO_OFF &
                //_BOR_OFF &
                //_CPD_OFF &
                //_CP_OFF &
                //_MCLRE_OFF &
                //_PWRTE_ON //&
                _WDT_OFF //&
                //_INTRC_OSC_NOCLKOUT
	);


//Set the configuration words:
//unsigned int at _CONFIG1 configWord1 = 0x2FF4;
//unsigned int at _CONFIG2 configWord2 = 0x3fff;

unsigned char datachar = 0;
unsigned char devaddr = 0b11010000; // LSB znamena rovnou write mod. Pro cteni bude muset byt 0b11010000 OR s 0b00000001 nebo proste s 1
unsigned int uintDelayCount = 0;
unsigned int i;
		
void main(void) {
	IRCF2=1;
	IRCF1=1;
	IRCF0=1;
	TRISC=0;
	TRISD=0; //porty D pro diagnostiku rozsvicenim led
	PORTD=0b00000000;
	RD7=IRCF2; // Tahle cast kodu mi ukaze nastavenou frekv cipu
	RD6=IRCF1;
	RD5=IRCF0;
	long_wait();
	i2c_init();
	long_wait();
	i2c_start();
	PORTD=0;
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
	/*if (prenos(0b11111111)) { //zapis do reg 108 - zapne frekv cteni na 40Hz <7:6>, zbytek je XA,YA,ZA,XG,YG,ZG
		RD3=1;
		while (1) {}
	}*/
	i2c_stop();
	/*i2c_start();
	PORTD=0;
	if (prenos(devaddr)) {
		RD0=1;
		while (1) {}
	}
	if (prenos(26)) {
		RD1=1;
		while (1) {}
	}
	if (prenos(0b00000001)) { //zapis do reg 35 - povoli zapis do bufferu, aby  bylo mozne data nacitat
		RD2=1;
		while (1) {}
	}
	i2c_stop();
	PORTD=0b00000000;
	i2c_start();
	PORTD=0;
	if (prenos(devaddr)) {
		RD0=1;
		while (1) {}
	}
	if (prenos(35)) {
		RD1=1;
		while (1) {}
	}
	if (prenos(0b01111111)) { //zapis do reg 35 - povoli zapis do bufferu, aby  bylo mozne data nacitat
		RD2=1;
		while (1) {}
	}
	i2c_stop();
	PORTD=0b00000000;
	WCOL=0;*/
	i2c_start();
	if (prenos(devaddr)) { //kdyz neprijde ACK
		RD1=1;
		while (1) {}; //rozsviti diodu na RD1 a konec, nastala chyba i2c - neprislo ack
	}
	if (prenos(107)) { //testovaci 0x75 registr v MPU-6050, odpoved by mela prijit (0x3F osa z)
		RD2=1;
		while (1) {}; //neprislo ACK od MPU-6050 po zaslani adresy vnitrniho registru
	}
	i2c_restart();
	if (prenos(devaddr+1)) { //odeslan pozadavek do slave pro cteni - slave automaticky vybere posledni zapsanou adresu vnitrniho registru
		RD3=1;
		while (1) {}; //neprislo ACK od MPU-6050 po zaslani adresy s pozadavkem READ
	}
	PORTD=0;
	PORTD=i2c_rx(1); //data zapsana na PORTD a rozsviti diody, ack nebude odeslano
	long_wait();
	PORTD=i2c_rx(0);
	RCEN=0; // ukoncen prijimaci mod
	i2c_stop();
	long_wait();
	long_wait();
	while (1) {
		i2c_start();
		if (prenos(devaddr)) { //kdyz neprijde ACK
			RD1=1;
			while (1) {}; //rozsviti diodu na RD1 a konec, nastala chyba i2c - neprislo ack
		}
		if (prenos(0x3F)) { //testovaci 0x75 registr v MPU-6050, odpoved by mela prijit (0x3F osa z)
			RD2=1;
			while (1) {}; //neprislo ACK od MPU-6050 po zaslani adresy vnitrniho registru
		}
		i2c_restart();
		if (prenos(devaddr+1)) { //odeslan pozadavek do slave pro cteni - slave automaticky vybere posledni zapsanou adresu vnitrniho registru
			RD3=1;
			while (1) {}; //neprislo ACK od MPU-6050 po zaslani adresy s pozadavkem READ
		}
		PORTD=0;
		//PORTD=i2c_rx(1); //data zapsana na PORTD a rozsviti diody, ack nebude odeslano
		//long_wait();
		PORTD=i2c_rx(0);
		RCEN=0; // ukoncen prijimaci mod
		i2c_stop();
		//long_wait();
	}
	
}

