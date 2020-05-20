/*
 * AVR: Clock With Stopwatch And Alarm
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


#define F_CPU 16000000UL


/** Inclusoes de arquivos */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h>


/** Definicoes gerais */
#define TIMER1_MODULE   15625
#define FALSE           0
#define TRUE            1


/** Definicoes de Macro Funcoes */
#define CONF_DISP()     init_dsp(2, 16); \
                        putmessage(0, 3, "Hora Certa"); \
                        putmessage(1, 4, "  :  :  ");
#define CONF_PORT()     DDRB = _BV(PB5);
#define CONF_INT01()    EICRA = _BV(ISC11) | _BV(ISC01); /* Interupcao por Borda de descida */ \
                        EIMSK |= _BV(INT1) | _BV(INT0);  /* Turns on INT1 and INT0 */
#define CONF_TIMER1()   OCR1A = TIMER1_MODULE - 1; /* No Tinkerkad o OCR1A deve ser inicializado primeiro*/ \
                        TCCR1A = 0x00; \
                        TCCR1B = _BV(WGM12) | _BV(CS12) | _BV(CS10); \
                        TIMSK1 = _BV(OCIE1A);
#define _ltrans(x,xi,xm,yi,ym) (long)((long)(x-xi)*(long)(ym-yi))/(long)(xm-xi)+yi


/** Structures */
typedef struct main
{
    unsigned char hora;
    unsigned char minuto;
    unsigned char segundo;
    unsigned char segundoAnt; // Limita a exibicao no display
} Relogio;


/** Declaracao de variaveis Globais */
Relogio *ptrel;
LiquidCrystal lcd(4, 5, 6, 7, 8, 9); //RS, E, DB4, DB5, DB6, DB7


/** Prototipos */
void updateTime(void);
/*******    PARA USO DO DISPLAY    ***********************/
void init_dsp(int l,int c);
void putmessage(int l,int c,char * msg);
void putnumber_i(int l,int c,long ni,int nd);
void putnumber_f(int l,int c,float ni,int nd);


/** Interupcoes */
ISR(TIMER1_COMPA_vect)
{
    updateTime();
}


ISR(INT0_vect) // Botao 1
{
    PORTB |= _BV(PB5);
}


ISR(INT1_vect) // Botao 2
{
    PORTB &= ~(_BV(PB5));
}


/** Corpo da funcao main */
int main(void)
{
    Relogio relogio = {0, 0, 0, 0};
    ptrel = &relogio;

    cli();
    CONF_DISP();
    CONF_PORT();
    CONF_INT01();
    CONF_TIMER1();
    sei();

    for (;;)
    {
        if (relogio.segundo != relogio.segundoAnt)
        {
          relogio.segundoAnt = relogio.segundo;
          putnumber_i(1, 4, relogio.hora, 2);
          putnumber_i(1, 7, relogio.minuto, 2);
          putnumber_i(1, 10, relogio.segundo, 2);
        }
        
    }
}


/** Funcoes do programa */
void updateTime(void)
{
    ptrel->segundo++;
    if (ptrel->segundo == 60){
        ptrel->minuto++, ptrel->segundo = 0;
        if (ptrel->minuto == 60){
            ptrel->hora++, ptrel->minuto = 0;
            if (ptrel->hora == 24){
                ptrel->hora = 0;
            }
        }
    }
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
  char zero[] = {"0000000000000000"};
  long int nx;
  int i, j;
  
  nx = ni;
  for (i=10, j=1;nx > 9; i*=10, j++)
    nx = ni/i;
  lcd.setCursor(c,l);
  lcd.print(&zero[16-nd+j]);  
  lcd.print(ni);
}

void putnumber_f(int l,int c,float ni,int nd)
{
  lcd.setCursor(c,l);
  lcd.print(ni,nd);
}
/*******    FIM PARA USO DO DISPLAY    *******************/