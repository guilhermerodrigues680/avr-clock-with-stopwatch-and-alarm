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
#include <util/delay.h>

LiquidCrystal lcd(8,9,10,11,12,13);

#define BUZZER                      PORTD4
#define FALSE                       0
#define TRUE                        1
#define TELA_REL                    0
#define TELA_CONF_REL               1
#define TELA_CRONOMETRO             2
#define TELA_CONF_ALARM             3
#define DURAC_ALARM_ON              6     /* Em segundos */
#define _ltrans(x, xi, xm, yi, ym)  (long)((long)(x - xi) * (long)(ym - yi)) / (long)(xm - xi) + yi


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
} Alar;

typedef struct{
  char hora;
  char minuto;
  char segundo;
} Cron;

/** Variaveis globais */
char tela = TELA_REL;
char acertoRel = FALSE;         // Flag acerto relogio
char selecionaRel = FALSE;      // Flag para auxiliar no acerto relogio
char selecionaAlarm = FALSE;    // Flag para auxiliar no acerto alarme
char tocarAlarm = FALSE;        // Flag para indicar se o alarme deve ser tocado
Rel *r;
Alar *a;
Cron *ptc;

/** Prototipos */
unsigned int Le_AD(char channel);
void acertaAlarm(void);
void acertaRel(void);
void checarAlarm(void);
/***    PARA USO DO DISPLAY    **************/
void init_dsp(int l,int c);
void putmessage(int l,int c,char * msg);
void putnumber_i(int l,int c,long ni,int nd);
void putnumber_f(int l,int c,float ni,int nd);

//void setup(void)
int main(void)
{

  Rel Relogio = {12, 30, 40};
  Alar Alarme = {12, 30};
  Cron Cronometro = {00, 00, 00};
  r = &Relogio;
  a = &Alarme;
  ptc = &Cronometro;

  cli();
  init_dsp(2, 16);

  DDRD |= _BV(BUZZER);                                        // Pino do buzzer como saida digital

  OCR1A = 15625 - 1;
  EIMSK = _BV(INT1) | _BV(INT0);                              // Habilita interupcao externa INT0 e INT1
  EICRA = _BV(ISC11) | _BV(ISC10) | _BV(ISC01) | _BV(ISC00);  // Ambas interrupções na borda de subida
  TCCR1A = 0x00;
  TCCR1B = _BV(WGM12) | _BV(CS12) | _BV(CS10);                // WGM12 -> CTC, CS -> Preescaler de 1024
  TIMSK1 = _BV(OCIE1A);
  sei();

  for (;;)
  {
    switch (tela)
    {
    case TELA_REL:
      if (Relogio.segundoAnt != Relogio.segundo)
      {
        Relogio.segundoAnt = Relogio.segundo;
        lcd.clear();
        putmessage(0, 3, "Hora Certa");
        putmessage(1, 4, "  :  :  ");
        putnumber_i(1, 4, Relogio.hora, 2);
        putnumber_i(1, 7, Relogio.minuto, 2);
        putnumber_i(1, 10, Relogio.segundo, 2);
      }
      checarAlarm();
      break;

    case TELA_CONF_ALARM:
      acertaAlarm();
      break;

    case TELA_CONF_REL:
      acertaRel();
      break;
    }

    _delay_ms(10);  // **Delay somente para melhor simulacao no Tinkercad
  }
}

// ==> Interrupção pelo PORTD2 (INT0)
ISR(INT0_vect)
{
  switch (tela)
  {
  case TELA_REL:
    tela = TELA_CONF_ALARM;
    selecionaAlarm = TRUE;
    break;

  case TELA_CONF_ALARM:
    selecionaAlarm = TRUE;
    break;

  case TELA_CONF_REL:
    selecionaRel = TRUE;
    break;
  }
}

// ==> Interrupção pelo PORTD3 (INT1)
ISR(INT1_vect)
{
  switch (tela)
  {
  case TELA_CONF_ALARM:
    tela = TELA_CONF_REL;
    selecionaRel = TRUE;
    break;
  }
}

// ==> Interrupção pelo estouro do TIMER1
ISR(TIMER1_COMPA_vect)
{
  // Relogio conta somente se nao estiver na tela de AJUSTE
  if (tela != TELA_CONF_REL)
  {
    if (++r->segundo == 60)
    {
      r->segundo = 0;
      if (++r->minuto == 60)
      {
        r->minuto = 0;
        if (++r->hora == 24)
        {
          r->hora = 0;
        }
      }
    }
  }

  // Cronometro
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

  // Alarme
  if (r->hora == a->hora && r->minuto == a->minuto && r->segundo < DURAC_ALARM_ON)
  {
    tocarAlarm = TRUE;
  }
  else
  {
    tocarAlarm = FALSE;
  }
  
}

// ==> Leitura Analogica para digital de um canal
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

// ==> Acerta o alarm
void acertaAlarm(void)
{
  static char State = -1;

  if (selecionaAlarm)
  {
    selecionaAlarm = FALSE;
    State++;
    State = State > 2 ? 0 : State;
    lcd.clear();                        // Limpa o que está escrito no display
    // putmessage(0, 1, "Ajuste Alarme");
    State == 0 ? putmessage(0, 0, "Conf. Alarme HOR") : putmessage(0, 0, "Conf. Alarme MIN");
    putmessage(1, 4, "  :  ");
    putnumber_i(1, 4, a->hora, 2);
    putnumber_i(1, 7, a->minuto, 2);
  }
  switch (State)
  {
  case 0:
    a->hora = _ltrans(Le_AD(0), 0, 1023, 0, 23);
    putnumber_i(1, 4, a->hora, 2);
    break;
  case 1:
    a->minuto = _ltrans(Le_AD(0), 0, 1023, 0, 59);
    putnumber_i(1, 7, a->minuto, 2);
    break;
  case 2:
    tela = TELA_REL;  // Retorna para tela do Relogio
    break;
  }
}

// ==> Checa se o alarme deve ser tocado
void checarAlarm(void)
{
  unsigned char i;
  if (tocarAlarm)
  {
    // Gera uma onda de 833Hz para produzir som no Buzzer (BIP BIP .... BIP BIP .... BIP BIP)
    for (i = 1; i < 200; i++)
    {
      PORTD |= _BV(BUZZER);
      _delay_us(600);
      PORTD &= ~(_BV(BUZZER));
      _delay_us(600);

      if (!(i % 100))   // Se não há resto da divisao por 100
        _delay_ms(100);
    }
    _delay_ms(300);
  }
  else
  {
    PORTD &= ~(_BV(BUZZER));
  }
}

// ==> Acertar o relogio
void acertaRel(void)
{
  static char State = -1;

  if (selecionaRel)
  {
    selecionaRel = FALSE;
    State++;
    State = State > 3 ? 0 : State;
    lcd.clear();                        // Limpa o que está escrito no display
    // putmessage(0, 1, "Ajuste Relogio");
    switch (State)
    {
    case 0:
      putmessage(0, 0, "Conf. Relog HOR");
      break;
    case 1:
      putmessage(0, 0, "Conf. Relog MIN");
      break;
    case 2:
      putmessage(0, 0, "Conf. Relog SEG");
      break;
    }
    putmessage(1, 4, "  :  :  ");
    putnumber_i(1, 4, r->hora, 2);
    putnumber_i(1, 7, r->minuto, 2);
    putnumber_i(1, 10, r->segundo, 2);
  }
  switch (State)
  {
  case 0:
    r->hora = _ltrans(Le_AD(0), 0, 1023, 0, 23);
    putnumber_i(1, 4, r->hora, 2);
    break;
  case 1:
    r->minuto = _ltrans(Le_AD(0), 0, 1023, 0, 59);
    putnumber_i(1, 7, r->minuto, 2);
    break;
  case 2:
    r->segundo = _ltrans(Le_AD(0), 0, 1023, 0, 59);
    putnumber_i(1, 10, r->segundo, 2);
    break;
  case 3:
    tela = TELA_REL;  // Retorna para tela do Relogio
    break;
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