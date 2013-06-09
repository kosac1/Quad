#include "pic16f887.h"
#include "debug.h"

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

unsigned int i=0,a=0,pwm=10,repinc=0,repdec=0;

void ISR() __interrupt 0 {
	if (TMR0IF) {
		TMR0IF=0;
    if (a>=pwm){
		//if (a>=15) {
			//PORTD=0;
			RA0=0;
      }
		if (a==31) {
			//PORTD=0;
			RA0=1;
			a=0;
      }
		a++;
    }
//		PORTD=a;
	
}


void init() {
	T0CS=0; //nastaveni timer0 do timer modu (1=counter)
	PSA=0; //povoleni prescaler
	PS2=0; //prescaler 256
	PS1=1;
	PS0=1;
	GIE=0; //globalni aktivace interuptu
	T0IE=0;
	TRISD=0;
	PORTD=0;
	ANSEL=0;
	ANSELH=0; //PORTB digital input
	TRISA=0;
	TRISB=0b00000011;    //tlacitko na portu B2 a B1 a B0
	PORTA=0;
	PORTB=0;
	IRCF2=1; //frekvence cpu 8MHz
	IRCF1=1;
	IRCF0=1;
}

void cpu_info() {
	RA2=IRCF2;
	RA1=IRCF1;
	RA0=IRCF0;
	long_wait();
}

void main() {
	init();
	cpu_info();
	PORTD=0b11111111;
	//long_wait();
	GIE=1;
	T0IE=1; // zapnuti timer0 interruptu
 	TMR0=0; // Timer0 initial value
PORTA=0;
  while (1) {
	   if (RB1==0 && repinc==0) {
	     RD7=1;
	     repinc=1;      
        if (pwm<31) pwm+=1;
        }
	   if (RB1==1) {
	     RD7=0;
	     repinc=0;
//        pwm+=3.1;
//        if (pwm>26) pwm=26;
        }
	   
     if (RB0==0 && repdec==0) {
	     RD6=1;
	     repdec=1;
        if (pwm>0) pwm-=1;
        }
	   if (RB0==1) {
	     RD6=0;
	     repdec=0;
//        pwm+=3.1;
//        if (pwm>26) pwm=26;
        }           
PORTD=pwm;
RA3=1;
RA3=0;
//RD7=0;

/*		long_wait();
		long_wait();
RA3=1;
		pwm=80*0.31;
		
		long_wait();
		long_wait();
RA2=1;
		pwm=60*0.31;
		
		long_wait();
		long_wait();
RA1=1;		
		pwm=40*0.31;		
		
		long_wait();
		long_wait();
RA3=0;		
    pwm=20*0.31;
		
		long_wait();
		long_wait();
RA2=0;
		pwm=0;       
		
		long_wait();
		long_wait();
RA1=0;		
    pwm=100*0.31;
*/
	};
}
		
