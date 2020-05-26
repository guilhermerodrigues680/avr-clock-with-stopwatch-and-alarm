<p align="center"><strong>Part 2 - Guia 5 - Timer/Counter 1, Estrutura e Ponteiro</strong></p>
<p align="center">Trabalho do Laboratório de Sistemas Embutidos Microprocessados</p>

## Projeto no Tinkercad

<iframe width="725" height="453" src="https://www.tinkercad.com/embed/9SicSSXvD6G?editbtn=1" frameborder="0" marginwidth="0" marginheight="0" scrolling="no"></iframe>

## Sobre o Projeto

Foi ultilizada a idéia de máquinas de estado, assim os botões tem ações específicas em cada tela.

- **Botão conectado ao PORTD2:**
    - Tela Relogio: Entra na tela de configurar alarme.
    - Tela Ajuste Alarme: Seleciona hora ou minuto para ajustar e volta para tela do Relogio.
    - Tela Ajuste Relogio: Seleciona hora, minuto ou segundo para ajustar.
    - Tela Cronometro: Zera o cronometro e volta para tela do Relogio se pressionado com o cronometro zerado.
- **Botão conectado ao PORTD3:**
    - Tela Relogio: Entra na tela do cronometro.
    - Tela Ajuste Alarme: Entra na tela de ajustar relogio.
    - Tela Ajuste Relogio: Entra na tela de configurar relogio.
    - Tela Cronometro: Inicia/Pausa o cronometro
- **Potênciometro:**
    - Define valores nos ajustes do relógio e do alarme.

## Código Fonte do Projeto

[github.com/guilhermerodrigues680/avr-clock-with-stopwatch-and-alarm](https://github.com/guilhermerodrigues680/avr-clock-with-stopwatch-and-alarm)

### Bugs Conhecidos

- Nenhum até o momento

## Autores

* **Arthur Siqueira** - *Graduando, Eng. Controle e Automação - PUC Minas*
* **Guilherme Rodrigues** - *Graduando, Eng. Controle e Automação - PUC Minas* - [GuilhermeRodrigues680](https://github.com/guilhermerodrigues680)