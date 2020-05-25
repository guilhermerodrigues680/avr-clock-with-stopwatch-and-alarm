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

LiquidCrystal lcd(8,9,10,11,12,13);

// #define  TIMER1_MODULE 15625
// #define  FALSE     0
// #define  TRUE      1
// #define _ltrans(x,xi,xm,yi,ym)  (long)((long)(x-xi)*(long)(ym-yi))/(long)(xm-xi)+yi


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
} Cron;

/** Variaveis globais */
Rel *pts;
Alar *pta;
Cron *ptc;

/** Prototipos */


//void setup(void)
int main(void)
{

  Rel Relogio = {12, 30, 10};
  Alar Alarme = {12, 30, 15};
  Cron Cronometro = {00, 00, 00};
  pts = &Relogio;
  pta = &Alarme;
  ptc = &Cronometro;

  cli();
  init_dsp(2, 16);
  putmessage(0, 3, "Hora Certa");
  putmessage(1, 0, "  :  :  ");

  OCR1A = 15625 - 1;
  // EIMSK = 0x01;
  // EICRA = _BV(ISC01) | _BV(ISC00);
  TCCR1A = 0x00;
  TCCR1B = _BV(WGM12) | _BV(CS12) | _BV(CS10); // CS -> Preescaler de 1024
  TIMSK1 = _BV(OCIE1A);
  sei();

  for (;;)
  {
    if (Relogio.segundoAnt != Relogio.segundo)
    {
      Relogio.segundoAnt = Relogio.segundo;
      putnumber_i(1, 0, Relogio.hora, 2);
      putnumber_i(1, 3, Relogio.minuto, 2);
      putnumber_i(1, 6, Relogio.segundo, 2);
    }

    if (Relogio.hora == Alarme.hora && Relogio.minuto == Alarme.minuto && Relogio.segundo == Alarme.segundo)
    {
      DDRD |= _BV(PD4);
      PORTD |= _BV(PD4);
    }
    else
    {
      DDRD |= _BV(PD4);
      PORTD &= ~(_BV(PD4));
    }
  }
}

// ISR(INT0_vect) // Pára o relógio e acerta
// {
//     Acerto = TRUE;
//     Seleciona = TRUE;
// }

ISR(TIMER1_COMPA_vect)
{
  // Relogio.segundo++;
  // pts->segundo++;

  if (++pts->segundo == 60)
  {
    pts->segundo = 0;
    if (++pts->minuto == 60)
    {
      pts->minuto = 0;
      if (++pts->hora == 24)
      {
        pts->hora = 0;
      }
    }
  }

  if (++ptc->segundo == 60)
  {
    ptc->segundo = 0;
    if (++ptc->minuto == 60)
    {
      ptc->minuto = 0;
      if (++ptc->hora == 24)
      {
        ptc->hora = 0;
      }
    }
  }

  // Relogio.segundo++;
  // if (Relogio.segundo==60)
  // {
  //     Relogio.segundo = 0;
  //     Relogio.minuto++;
  //     if (Relogio.minuto == 60)
  //     {
  //         Relogio.minuto=0;
  //         Relogio.hora++;
  //         if (Relogio.hora == 24)
  //         {
  //             Relogio.hora = 0;
  //         }
  //     }
  // }
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