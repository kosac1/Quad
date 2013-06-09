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

unsigned int i=0,a=0;
unsigned int pwm_pct=100;
unsigned int stimearr[]={0,0,0,0},stime,stime1,stime2,stime3,stime4; //stime urcuje delku trvani TMR0
unsigned int svaluearr[]={0,0,0,0},svalue,svalue1,svalue2,svalue3,svalue4; //svalue nastavuje PORTD resp. vypina porty a tim urcuje vykon pwm
unsigned int tmr0inc=0,tmr0end=1;
unsigned int pwm=1,repinc=0,repdec=0;

void wait_1ms() {
  svalue=PORTA;
  T0IE=1;
  tmr0end=1;
}

void ISR() __interrupt 0 {
	if (TMR0IF) {
	   PORTD++;
		TMR0IF=0; //smazeme flag preteceni timeru0
		PORTA=svalue; //prvni shodime pozadovane motory presne s vyprsenim timeru
		if (PORTA==0) { //testujeme, zda-li uz nejsou vypnute vsechy motory, abychom mohli uplne vypnout timer0 (vsechny motory mohou byt vypnute klidne v prvnim prubehu)
			TMR0IE=0; // vypnutí TMR0 dokud nedoběhne zase další cyklus TMR1
			tmr0end=0; //zajisti vypadnuti z cekaciho cyklu v TMR1
			return;
		}
		PORTD++;
		tmr0end=0; //zajisti vypadnuti z cekaciho cyklu v TMR1
		return;
	}
	if (TMR2IF) { //s pretecenim TMR1 spustime funkce pro inicializaci TMR0 a rizeni pwm
		TMR2IF=0; //smazeme flag preteceni timeru1
		PORTD++;
		tmr0inc=0; //nulovani citace pro ukazatel pole pro hodnoty rizeni pwm
		tmr0end=1;		 
		PORTA=0b00000001; //nahod signal pwm
		wait_1ms(); //funkce pro nastaveni 0pct pwm !!!!!!!!!!!!!!!! MUSIME DOPSAT
		TMR0=stime1; //nastaveni inicializacni hodnoty TMR0 dle vypoctu
		svalue=svalue1;
		TMR0IE=1; //zapnuti TMR0
		while (tmr0end) { //smycka 1. prubehu - ukonci se po ukonceni TMR0
		  PORTD++;
		}
		/*tmr0end=1;
		TMR0=stime2;
		svalue=svalue2;
		while (tmr0end) { //smycka 2. prubehu - ukonci se po ukonceni TMR0
		}
		tmr0end=1;
		TMR0=stime3;
		svalue=svalue3;
		while (tmr0end) { //smycka 3. prubehu - ukonci se po ukonceni TMR0
		}
		tmr0end=1;
		TMR0=stime4;
		svalue=svalue4;
		while (tmr0end) { //smycka 4. prubehu - ukonci se po ukonceni TMR0
		}*/
		return;
	}
}

void init() {
  PR2=105;
	T0CS=0; //nastaveni timer0 do timer modu (1=counter)
	PSA=0; //povoleni prescaler
	PS2=0; //prescaler 8 pro 1ms pri cpu freq 8MHz
	PS1=1;
	PS0=0;
	T2CON=0b00000010; //nastaveni prescaler TMR2 1:16 a postscaler 1:1
	GIE=0; //globalni aktivace interuptu
	T0IE=0;
	TMR2ON=0;
	TRISD=0;
	PORTD=0;
	ANSEL=0;
	TRISA=0;
	TRISB=0b00000011;    //tlacitko na portu B2 a B1 a B0
	PORTB=0;
	PORTA=0;
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
	long_wait();
	GIE=1;
	TMR2ON=1; // Timer1 initial value
  while (1) {
    RA3=0;
    RA3=1;
    if (RB1==0 && repinc==0) {
	   RD7=1;
	   repinc=1;      
      if (pwm<10) pwm+=1;
    }
	  if (RB1==1) {
	    RD7=0;
	    repinc=0;
    }
    if (RB0==0 && repdec==0) {
	    RD6=1;
	    repdec=1;
      if (pwm>0) pwm-=1;
    }
	  if (RB0==1) {
	    RD6=0;
	    repdec=0;
    }           
    PORTD=pwm;
    stime1=255-((10-pwm)*25);       
  }
}
		
