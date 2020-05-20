### Código do professor Hegler - P_Guia_5_P9_P10_1
```cpp
#include <avr/io.h>
#include <LiquidCrystal.h>
#include <avr/interrupt.h>

LiquidCrystal lcd(8,9,10,11,12,13);

#define	TIMER1_MODULE	15625
#define	FALSE			0
#define	TRUE			1
#define _ltrans(x,xi,xm,yi,ym)  (long)((long)(x-xi)*(long)(ym-yi))/(long)(xm-xi)+yi

typedef struct{
	char hora;
   	char minuto;
   	char segundo;
   	char segundoAnt;
} Rel;

char Acerto = FALSE;
char Seleciona = FALSE;
Rel *pts;
Rel *r;

unsigned int Le_AD(void);
void Acerta(void);

//void setup(void)
int main(void)
{
  	Rel Relogio = {0,0,0,0};
    pts = &Relogio;
    r = pts;
  
    init_dsp(2,16);
    putmessage(0,3,"Hora Certa");
    putmessage(1,4,"  :  :  ");

  	OCR1A = TIMER1_MODULE-1;
	EIMSK = 0x01;
    EICRA = _BV(ISC01) | _BV(ISC00);
    TCCR1B = _BV(WGM12) | _BV(CS12) | _BV(CS10); // FPS = 1024
    TIMSK1 = _BV(OCIE1A);
    sei();
    for(;;) 
    {	
     	if (Acerto)
          Acerta();
      	else
        {
      		
	    	if (Relogio.segundoAnt!=Relogio.segundo)
	      	{
	        	Relogio.segundoAnt = Relogio.segundo;
		   		putnumber_i(1,4,Relogio.hora,2);
		   		putnumber_i(1,7,Relogio.minuto,2);
		   		putnumber_i(1,10,Relogio.segundo,2);
	      	}
        }
    }
}

void Acerta(void)
{
	static char State = -1;
  
  	if (Seleciona)
    {
      	Seleciona = FALSE;
      	State++;
      	State %= 4;
    }
  	switch(State)
    {
      case 0 :	r->hora =_ltrans(Le_AD(0),0,1023,0,23);
      			putnumber_i(1,4,r->hora,2);
      			break;
      case 1 :	r->minuto =_ltrans(Le_AD(0),0,1023,0,59);
      			putnumber_i(1,7,r->minuto,2);
      			break;
      case 2 :	r->segundo =_ltrans(Le_AD(0),0,1023,0,59);
      			putnumber_i(1,10,r->segundo,2);
      			break;
      case 3 :  Acerto = FALSE;
      			break;
    }
}

ISR(INT0_vect) // Pára o relógio e acerta
{
    Acerto = TRUE;
    Seleciona = TRUE;
}

ISR(TIMER1_COMPA_vect)
{
	if (!Acerto)
    {
    	if (++pts->segundo==60)
	  	{
	    	pts->segundo = 0;
	      	if (++pts->minuto==60)
	      	{
	        	pts->minuto = 0;
		   		if (++pts->hora==24)	
		       		pts->hora = 0;
	      	}
	  	}
    }
}

unsigned int Le_AD(char channel)
{
	static char FirstTime=1;

	if (FirstTime)
    {
    	FirstTime = 0;
        ADMUX = _BV(REFS0);// Seleciona Vref igual a 5V
        ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // AD e preesc. de 128
    }
    DIDR0 = _BV(channel);// Liga a entrada analogica no pino PC0 e desliga a funcao digital
    ADMUX &= 0xF0;
    ADMUX |= channel;
    ADCSRA |= _BV(ADSC);// dispara ao conversor
    
  	while (!(ADCSRA & _BV(ADIF)))// espera finalizar a conversao
       continue;
    return (ADC); //devolve o resultado da conversao
}

/*******    PARA USO DO DISPLAY    ***********************/
void init_dsp(int l,int c)
{
  lcd.begin(c,l);
}

void putmessage(int l,int c,char * msg)
{
  lcd.setCursor(c,l);
  lcd.print(msg);
}


void putnumber_i(int l,int c,long ni,int nd)
{
  lcd.setCursor(c,l);
  if (ni<10)
  	lcd.print('0');  
  lcd.print(ni);
}

void putnumber_f(int l,int c,float ni,int nd)
{
  lcd.setCursor(c,l);
  lcd.print(ni,nd);
}
```