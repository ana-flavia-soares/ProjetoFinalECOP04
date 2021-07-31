#include "pic18f4520.h"
#include "bits.h"
#include "config.h"
#include "delay.h"
#include "keypad.h"
#include "lcd.h"
#include "pwm.h"
#include "timer.h"

//Tempos definidos para cada funcao
#define tvarrer 10
#define taspirar 15
#define tpano 20

//Funcoes criadas para utilizar na main
//Acende sequencialmente 2 vezes os leds do PORTD ao iniciar o robo
void iniciarRobo(void) {
    int i, k;
    for (k = 0; k < 2; k++) {
        for (i = 1; i > 0; i = i * 2) {
            PORTD = i;
            atraso_ms(400);
        }
    }
}

//Acende sequencialmente 2 vezes os leds do PORTB ao desligar o robo
void desligarRobo(void) {
    int i, k;
    for (k = 0; k < 2; k++) {
        for (i = 1; i > 0; i = i * 2) {
            PORTB = i;
            atraso_ms(400);
        }
    }
}

//Acende 2 vezes todos os leds do PORTD e apaga
void bateriaDescarregando(void) {
    float i, j;
    (*(volatile __near unsigned char*) 0xF95) = 0x00;
    for (j = 0; j < 2; j++) {
        (*(volatile __near unsigned char*) 0xF83) = 0xFF;
        for (i = 0; i < 1000; i++);
        (*(volatile __near unsigned char*) 0xF83) = 0x00;
        for (i = 0; i < 1000; i++);
    }
    atraso_s(2);
}

//Acende 2 vezes todos os leds do PORTD e mantem-se aceso
void bateriaCarregada(void) {
    float i, j;
    (*(volatile __near unsigned char*) 0xF95) = 0x00;
    for (j = 0; j < 2; j++) {
        (*(volatile __near unsigned char*) 0xF83) = 0xFF;
        for (i = 0; i < 1000; i++);
        (*(volatile __near unsigned char*) 0xF83) = 0x00;
        for (i = 0; i < 1000; i++);
    }
    PORTD = 0xFF;
    atraso_s(2);
}

//Realiza o teste do teclado para as funcoes realizadas pelo robo
unsigned int testeTeclado() {
    unsigned char tecla;
    do {
        TRISD = 0x0F;
        tecla = teclado(0) + 0x30;
        TRISD = 0x00;
        lcd_command(L_L1 + 15);
        lcd_string("*");
    } while (tecla != '1' && tecla != '2' && tecla != '3'); //enquanto o usuario nao apertar alguma opcao correta nao sai desse while
    return tecla - '0';
}

//Realiza o teste do teclado para as "funcoes" liga e deliga
unsigned char testeLD() {
    unsigned int atrasoMin = 15;
    unsigned char i, op;
    TRISB = 0xF8;
    TRISD = 0x0F;
    op = teclado(0) + 0x30;
    TRISD = 0x00;
    TRISB = 0x00;
    return op;
}

//Mostra as funcoes que podem ser realizadas pelo robo
void opcaoFuncoes(void) {
    lcd_command(L_CLR);
    lcd_command(L_L1);
    lcd_string("FUNCOES DO ROBO");
    lcd_command(L_L2);
    lcd_string("Varrer");
    lcd_command(L_L3);
    lcd_string("Aspirar");
    lcd_command(L_L4);
    lcd_string("Passar pano");
}

//Mostra os tipos de "areas" que podem ser limpas e o 
//usuario devera escolher entre uma delas
unsigned int opcaoComodo() {
    lcd_command(L_CLR);
    lcd_command(L_L1);
    lcd_string("TIPO AREA:");
    lcd_command(L_L2);
    lcd_string("1-Area total"); //limpa todo o imovel
    lcd_command(L_L3);
    lcd_string("2-Area externa"); //limpa um comodo da area externa
    lcd_command(L_L4);
    lcd_string("3-Area interna"); //limpa um comodo da area interna
    return testeTeclado();
}

//Mostra os tipos de limpeza que podem ser realizadas pelo robo e o
//usuario deve escolher entre umas delas
unsigned int opcaoLimpeza() {
    lcd_command(L_CLR);
    lcd_command(L_L1);
    lcd_string("TIPO LIMPEZA:");
    lcd_command(L_L2);
    lcd_string("1-Automatico");
    lcd_command(L_L3);
    lcd_string("2-Espiral");
    lcd_command(L_L4);
    lcd_string("3-Canto");
    return testeTeclado();
}

//Mostra os tipos de funcoes que podem ser realizadas pelo robo e o
//usuario devera escolher entre uma delas
unsigned int opcaoFuncao() {
    lcd_command(L_CLR);
    lcd_command(L_L1);
    lcd_string("TIPO FUNCAO:");
    lcd_command(L_L2);
    lcd_string("1-Simples"); //varre
    lcd_command(L_L3);
    lcd_string("2-Normal"); //varre e aspira
    lcd_command(L_L4);
    lcd_string("3-Completa"); // varre, aspira e passa pano
    return testeTeclado();
}

//Realiza a contagem de tempo para a funcao que o robo esta realizando
void tempo(int tempo) {
    int valores[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x7F, 0x67};
    int i, k, cont = tempo;
    unsigned int atraso = 1500;

    while (1) {
        for (i = 0; i < 35; i++) {
            PORTD = valores[(cont / 1) % 10];
            bitSet(PORTA, 5);
            bitClr(PORTA, 4);
            bitClr(PORTA, 3);
            bitClr(PORTA, 2);
            for (k = 0; k < atraso; k++);
            PORTD = valores[(cont / 10) % 10];
            bitClr(PORTA, 5);
            bitSet(PORTA, 4);
            bitClr(PORTA, 3);
            bitClr(PORTA, 2);
            for (k = 0; k < atraso; k++);
            PORTD = valores[(cont / 100) % 10];
            bitClr(PORTA, 5);
            bitClr(PORTA, 4);
            bitSet(PORTA, 3);
            bitClr(PORTA, 2);
            for (k = 0; k < atraso; k++);
            PORTD = valores[(cont / 1000) % 10];
            bitClr(PORTA, 5);
            bitClr(PORTA, 4);
            bitClr(PORTA, 3);
            bitSet(PORTA, 2);
            for (k = 0; k < atraso; k++);
        }
        cont--;
        if (cont == -1) {
            break;
        }
    }

    bitClr(PORTA, 5);
    bitClr(PORTA, 4);
    bitClr(PORTA, 3);
    bitClr(PORTA, 2);
}

//Funcoes criadas para otimizacao do codigo
void varrer(void) {
    lcd_command(L_CLR);
    lcd_command(L_L1);
    lcd_string("Varrendo");
    lcd_command(0x0C);
    tempo(tvarrer);
    atraso_s(3);
}

void aspirar(void) {
    lcd_command(L_CLR);
    lcd_command(L_L1);
    lcd_string("Aspirando");
    lcd_command(0x0C);
    tempo(taspirar);
    atraso_s(3);
}

void pano(void) {
    lcd_command(L_CLR);
    lcd_command(L_L1);
    lcd_string("Passando pano");
    lcd_command(0x0C);
    tempo(tpano);
    atraso_s(3);
}

//Funcao main
void main(void) {
    TRISA = 0x00;
    TRISB = 0x03;
    TRISC = 0x01;
    TRISD = 0x00;
    TRISE = 0x00;

    unsigned int comodo, limpeza, funcao;
    unsigned char opcao, i, k;
    pwmInit(); //inicializa o pwm
    timerInit(); //inicializa o timer
    lcd_init(); //inicializa o lcd

    for (;;) {
        //introducao do programa
        lcd_command(L_CLR);
        lcd_command(L_L2);
        lcd_string("     PROJETO");
        lcd_command(L_L3);
        lcd_string("   PROGRAMACAO");
        lcd_command(L_L4);
        lcd_string("    EMBARCADA");
        atraso_s(5);

        //o usuario devera apertar o 7 para ligar o robo
        lcd_command(L_CLR);
        lcd_command(L_L1);
        lcd_string("Aperte 7 para");
        lcd_command(L_L2);
        lcd_string("ligar o robo");
        do {
            opcao = testeLD();
            if (opcao != '7') {
                lcd_command(L_L4);
                lcd_string("Invalido");
            } else {
                atraso_s(2);
                lcd_command(L_L4 + 14);
                lcd_string("OK");
                atraso_s(2);

                lcd_command(L_CLR);
                lcd_command(L_L2);
                lcd_string("  INICIALIZANDO");
                lcd_command(L_L3);
                lcd_string("      ROBO      ");
                iniciarRobo(); //referente aos leds do PORTD

                //liga e desliga o cooler e o rele
                pwmSet1(32);
                pwmFrequency(8000);
                atraso_s(3);
                pwmSet1(0);
            }
        } while (opcao != '7');

        //mostra as funcoes que podem ser realizadas pelo robo
        opcaoFuncoes();
        atraso_s(5);

        //o usuario devera escolher um tipo de comodo
        comodo = opcaoComodo();
        if (comodo == 2) { //caso o usuario tenha escolhido "2-Area externa" sao mostradas as opcoes abaixo
            lcd_command(L_CLR);
            lcd_command(L_L1);
            lcd_string("AREA EXTERNA:");
            lcd_command(L_L2);
            lcd_string("1-Area de lazer");
            lcd_command(L_L3);
            lcd_string("2-Garagem");
            lcd_command(L_L4);
            lcd_string("3-Varanda");
            testeTeclado();
        } else if (comodo == 3) { //caso o usuario tenha escolhido "3-Area interna" sao mostradas as opcoes abaixo
            lcd_command(L_CLR);
            lcd_command(L_L1);
            lcd_string("AREA INTERNA:");
            lcd_command(L_L2);
            lcd_string("1-Sala/Cozinha");
            lcd_command(L_L3);
            lcd_string("2-Quarto");
            lcd_command(L_L4);
            lcd_string("3-Banheiro");
            testeTeclado();
        }

        // o usuario devera escolher entre um tipo de limpeza fornecido
        limpeza = opcaoLimpeza();

        // o usuario devera escolher entre um tipo de funcao
        funcao = opcaoFuncao();

        //apos as escolhas do usuario, inicia-se a limpeza
        lcd_command(L_CLR);
        lcd_command(L_L2);
        lcd_string("    INICIANDO");
        lcd_command(L_L3);
        lcd_string("     LIMPEZA");
        atraso_s(5);

        //limpeza simples: varrer
        if (funcao == 1) {
            varrer();
        }
        //limpeza normal: varrer e aspirar
        else if (funcao == 2) {
            varrer();
            aspirar();
        }
        //limpeza completa: varrer, aspirar e passar pano
        else if (funcao == 3) {
            varrer();
            aspirar();
            pano();
        }

        bitClr(PORTA, 2);
        if (comodo == 1) { //caso o usuario tenha escolhido "1-Area total" a bateria do robo acaba sendo descarregada
                          //devido ao tempo gasto para limpar tudo e o usuario devera apertar o 8 para carrega-lo
            lcd_command(L_CLR);
            lcd_command(L_L2);
            lcd_string(" A BATERIA ESTA");
            lcd_command(L_L3);
            lcd_string(" DESCARREGANDO");
            bateriaDescarregando();
            lcd_command(L_CLR);
            lcd_command(L_L1);
            lcd_string("Aperte 8 para");
            lcd_command(L_L2);
            lcd_string("carregar a");
            lcd_command(L_L3);
            lcd_string("bateria");
            atraso_s(1);
            opcao = testeLD();
            atraso_s(1);
            if (opcao == '8') {
                lcd_command(L_CLR);
                lcd_command(L_L2);
                lcd_string(" A BATERIA ESTA ");
                lcd_command(L_L3);
                lcd_string(" SENDO CARREGADA");
                bateriaCarregada();
            }
        }

        //Mostra um resumo das opcoes que foram escolhidas pelo usuario, ou seja, realizadas pelo robo
        bitClr(PORTA, 2); //desliga o DIS1
        lcd_command(L_CLR);
        lcd_command(L_L1);
        lcd_string("RESUMO LIMPEZA:");
        if (comodo == 1) {
            lcd_command(L_L2);
            lcd_string("Area total");
        } else if (comodo == 2) {
            lcd_command(L_L2);
            lcd_string("Area externa");
        } else if (comodo == 3) {
            lcd_command(L_L2);
            lcd_string("Area interna");
        }

        if (limpeza == 1) {
            lcd_command(L_L3);
            lcd_string("Automatico");
        } else if (limpeza == 2) {
            lcd_command(L_L3);
            lcd_string("Espiral");
        } else if (limpeza == 3) {
            lcd_command(L_L3);
            lcd_string("Canto");
        }

        if (funcao == 1) {
            lcd_command(L_L4);
            lcd_string("Simples");
        } else if (funcao == 2) {
            lcd_command(L_L4);
            lcd_string("Normal");
        } else if (funcao == 3) {
            lcd_command(L_L4);
            lcd_string("Completa");
        }
        atraso_s(8);

        //o usuario devera apertar 9 para desligar o robo
        lcd_command(L_CLR);
        lcd_command(L_L1);
        lcd_string("Aperte 9 para");
        lcd_command(L_L2);
        lcd_string("desligar o robo");
        do {
            opcao = testeLD();
            if (opcao != '9') {
                lcd_command(L_L4);
                lcd_string("Invalido");
            } else {
                atraso_s(2);
                lcd_command(L_L4 + 14);
                lcd_string("OK");
                atraso_s(2);
                
                lcd_command(L_CLR);
                lcd_command(L_L2);
                lcd_string("   DESLIGANDO");
                lcd_command(L_L3);
                lcd_string("      ROBO      ");
                desligarRobo(); //referente aos leds do PORTB
                atraso_s(3);
            }
        } while (opcao != '9');

        atraso_s(3);
        timerWait();
        //apos isso o programa inicia-se novamente
    }
}