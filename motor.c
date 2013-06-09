#include "pic16f887.h"
#include "debug.h"
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

unsigned int i=0,a=0;
unsigned int pwm_pct=100;
unsigned int stimearr[]={0,0,0,0},stime,stime1,stime2,stime3,stime4; //stime urcuje delku trvani TMR0
unsigned int svaluearr[]={0,0,0,0},svalue,svalue1,svalue2,svalue3,svalue4; //svalue nastavuje PORTD resp. vypina porty a tim urcuje vykon pwm
volatile unsigned int tmr0end=1,tmr2end=0;
unsigned int pwm=0,repinc=0,repdec=0,plus=0;

/*void wait_1ms() {
	tmr0end=1;
	GIE=1;
	T0CS=0; //zapiname TMR0 mode
	TMR0=stime;
	T0IE=1;
	while (tmr0end) {
	}
}*/

void ISR() __interrupt 0 {
	if (TMR2IF) { //s pretecenim TMR1 spustime funkce pro inicializaci TMR0 a rizeni pwm
		TMR2IF=0; //smazeme flag preteceni timeru2
		stime=32;
		PORTA=0b00001111; //nahod signal pwm
		TMR0IF=0;
		TMR0=32;
		//PORTD=1;
		while (!TMR0IF) { //1ms cyklus
		}
		//PORTD=2;
		if (pwm==0) {
			PORTA=0;
			return;
		}
		TMR0IF=0;
		TMR0=stime1;
		//PORTD=3;
		while (!TMR0IF) { //cyklus pro vypnuti 1. motoru
		}
		//PORTD=4;
		PORTA=svalue1;
		tmr2end=1;
		return;
	}
}

void init() {
	IRCF2=1; //frekvence cpu 8MHz
	IRCF1=1;
	IRCF0=1;
	T0IE=0;
	TMR0IF=0;
	PR2=39;
	T0CS=0; //nastaveni timer0 do counter modu (0=timer), v counter mode nedochazi k automatickemu inkrementovani hodnoty TMR0 => TMR0 je vypnuty
	PSA=0; //povoleni prescaler
	PS2=0; //prescaler 8 pro 1ms pri cpu freq 8MHz
	PS1=1;
	PS0=0;
	T2CON=0b01111010; //nastaveni prescaler TMR2 1:16 a postscaler 1:1
	GIE=1; //globalni aktivace interuptu
	TMR2ON=0;
	TRISD=0;
	PORTD=0;
	ANSEL=0;
	TRISA=0;
	//TRISB=0b00000011;    //tlacitko na portu B2 a B1 a B0
	//PORTB=0;
	PORTA=0;
}

void cpu_info() {
	RA2=IRCF2;
	RA1=IRCF1;
	RA0=IRCF0;
	long_wait();
}

void main() {
	init();
	i2c_init();
	mpu6050_init();
	cpu_info();
	PORTD=0b11111111;
	long_wait();
	TMR2ON=1; // Timer1 initial value
	PEIE=1;
	TMR2IE=1;
	TMR2=0;
	svalue1=0;
	stime1=0;
	for (a=0; a<1023; a++) {
		PORTD=a;
		wait();
	}
	while (1) {
		RA2=1;
		svalue1=0;
		stime1=255-(pwm*20);
		//long_wait();
		if ((!plus) & pwm==5) {
			plus=1;
		}
		if (plus & pwm==0) {
			plus=0;
		}
		if (plus) {
			pwm--;
		}
		else {
			pwm++;
		}
		PORTD=pwm;
		for (a=0; a<255; a++) {
			wait();
		}
		while (!tmr2end) {}
		tmr2end=0;
	}
}

