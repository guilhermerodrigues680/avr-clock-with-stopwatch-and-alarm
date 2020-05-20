/*
 * Part 2 - Guia 5 - Timer/Counter 1, Estrutura e Ponteiro
 * ***************************
 * 
 * Autores:
 * - Arthur Siqueira - Graduando, Eng. Controle e Automação - PUC Minas
 * - Guilherme Rodrigues - Graduando, Eng. Controle e Automação - PUC Minas
 * 
 * ***************************
 * 
 */


#include <avr/io.h>
#include <LiquidCrystal.h>
#include <avr/interrupt.h>


/** Definicoes gerais */
#define	FALSE               0
#define	TRUE                1
#define BUZZER              PORTD4
#define TELA_RELOGIO        0
#define TELA_CRONOMETRO     1
#define TELA_AJUSTE_ALARME  2
#define TELA_AJUSTE_RELOGIO 3


/** Definicoes de Macro Funcoes */
// #define _ltrans(x,xi,xm,yi,ym)  (long)((long)(x-xi)*(long)(ym-yi))/(long)(xm-xi)+yi
#define ACIONAR_BUZZER()    PORTD |= _BV(BUZZER);
#define DESLIGAR_BUZZER()   PORTD &= ~(_BV(PD4));


/** Structs */
typedef struct{
	char hora;
   	char minuto;
   	char segundo;
    char segundoAnt;
} Rel;
typedef struct{
	char hora;
   	char minuto;
   	char segundo;
} Alar;
typedef struct{
	char hora;
   	char minuto;
   	char segundo;
    char segundoAnt;
    char ativo;
} Cron;


/** Variaveis globais */
LiquidCrystal lcd(8,9,10,11,12,13);
Rel *pts;
Alar *pta;
Cron *ptc;
unsigned char *ptela;


/** Prototipos */
unsigned int Le_AD(char channel);
/*******    PARA USO DO DISPLAY    ***********************/
void init_dsp(int l,int c);
void putmessage(int l,int c,char * msg);
void putnumber_i(int l,int c,long ni,int nd);
void putnumber_f(int l,int c,float ni,int nd);


/** Interupções */
ISR(INT0_vect) // Botao para colocar o relogio em modo acerto e configurar
{
    switch (*ptela)
    {
    case TELA_CRONOMETRO:
        ptc->ativo = !ptc->ativo; // Inicia e pausa contagem
        break;
    
    default:
        break;
    }
}


ISR(INT1_vect) // Botao Mudar Tela
{
    switch (*ptela)
    {
    case TELA_CRONOMETRO:
        if (ptc->hora > 0 || ptc->minuto > 0 || ptc->segundo > 0)
        {
            ptc->ativo = FALSE;
            ptc->hora = 0;
            ptc->minuto = 0;
            ptc->segundo = 0;
            ptc->segundoAnt = 0;
            break;
        }
    default:
        *ptela = *ptela + 1;
        if (*ptela > TELA_AJUSTE_RELOGIO)
        {
            *ptela = TELA_RELOGIO;
        }
        break;
    }
    
}


ISR(TIMER1_COMPA_vect)
{
    // Incremento do Relogio
    if (++pts->segundo==60)
    {
        pts->segundo = 0;
        if (++pts->minuto == 60)
        {
            pts->minuto=0;
            if (++pts->hora == 24)
            {
                pts->hora = 0;
            }
        }
    }

    // Incremento do cronometro
    if (*ptela == TELA_CRONOMETRO && ptc->ativo == TRUE)
    {
        if (++ptc->segundo==60)
        {
            ptc->segundo = 0;
            if (++ptc->minuto == 60)
            {
                ptc->minuto=0;
                if (++ptc->hora == 24)
                {
                    ptc->hora = 0;
                }
            }
        }
    }
}


/** Função main */
int main(void)
{
  	unsigned char tela = 0;
    Rel Relogio = {12, 30, 10};
    Alar Alarme = {12, 31, 15};
    Cron Cronometro = {0, 0, 0, 0, FALSE};
    ptela = &tela;
    pts = &Relogio;
    pta = &Alarme;
    ptc = &Cronometro;

  
    cli(); // CLear Interrupt (desliga as interupcoes do ATmega328)
    init_dsp(2,16);

    // Configuracao de portas
    DDRD = _BV(BUZZER); // Porta do Buzzer como saida

  	OCR1A = 15625 - 1; // Modulo do TIMER1
    
    EIMSK = _BV(INT1) | _BV(INT0); // Habilita interupcao externa INT0 e INT1
    EICRA = _BV(ISC11) | _BV(ISC10) | _BV(ISC01) | _BV(ISC00); // Ambas interupcoes na borda de subida
    
    TCCR1A = 0x00;
    TCCR1B = _BV(WGM12) | _BV(CS12) | _BV(CS10); // Modo CTC e Preescaler de 1024
    TIMSK1 = _BV(OCIE1A); // Habilita a inturpcao por comparacao com OCR1A no TIMER1
    sei(); // SEt Interrupt (Liga as interupcoes do ATmega328)

    for(;;) 
    {
        switch (tela)
        {
        case TELA_RELOGIO:
            if (Relogio.segundoAnt != Relogio.segundo)
            {
                Relogio.segundoAnt = Relogio.segundo;
                putmessage(0,0,"   Hora Certa   ");
                putmessage(1,0,"  :  :  ");
                putnumber_i(1,0, Relogio.hora, 2);
                putnumber_i(1,3, Relogio.minuto, 2);
                putnumber_i(1,6, Relogio.segundo, 2);
                if (Relogio.hora == Alarme.hora && Relogio.minuto == Alarme.minuto && Relogio.segundo == Alarme.segundo) {
                    ACIONAR_BUZZER();
                } else {
                    DESLIGAR_BUZZER();
                }
            }
            break;
        case TELA_CRONOMETRO:
            if (Cronometro.ativo)
            {
                if (Cronometro.segundoAnt != Cronometro.segundo)
                {
                    Cronometro.segundoAnt = Cronometro.segundo;
                    putmessage(0,0, "Cronometro: Cont");
                    putmessage(1,0,"  :  :  ");
                    putnumber_i(1,0, Cronometro.hora, 2);
                    putnumber_i(1,3, Cronometro.minuto, 2);
                    putnumber_i(1,6, Cronometro.segundo, 2);
                }
            } else {
                putmessage(0,0, "Cronometro: Paus");
                putmessage(1,0,"  :  :  ");
                putnumber_i(1,0, Cronometro.hora, 2);
                putnumber_i(1,3, Cronometro.minuto, 2);
                putnumber_i(1,6, Cronometro.segundo, 2);
            }
            break;
        case TELA_AJUSTE_ALARME:
            putmessage(0,0, "Ajuste Alarme:  ");
            break;
        case TELA_AJUSTE_RELOGIO:
            putmessage(0,0, "Ajuste Relogio: ");
            break;
        }
    }
}


// ==> Faz a leitura de uma porta Analogica
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
/*******    FIM PARA USO DO DISPLAY    *******************/