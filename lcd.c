//Foi alterada
#include "pic18f4520.h"
#include "lcd.h"
#include "delay.h"

void lcd_write(unsigned char val) {
    PORTD = val;
}

void lcd_command(unsigned char val) {
    BitSet(PORTE, 1); // Enable - PORTEbits.RE1
    lcd_write(val);
    BitClr(PORTE, 2); // Data - PORTEbits.RE2
    atraso_ms(3);
    BitClr(PORTE, 1);
    atraso_ms(3);
    BitSet(PORTE, 1);
}

void lcd_data(unsigned char val) {
    BitSet(PORTE, 1);
    lcd_write(val);
    BitSet(PORTE, 2);
    atraso_ms(3);
    BitClr(PORTE, 1);
    atraso_ms(3);
    BitSet(PORTE, 1);
}

void lcd_init(void) {
    BitClr(PORTE, 1);
    BitClr(PORTE, 2);
    atraso_ms(20);
    BitSet(PORTE, 1);

    lcd_command(L_CFG);
    atraso_ms(5);
    lcd_command(L_CFG);
    atraso_ms(1);
    lcd_command(L_CFG); //configura
    lcd_command(L_OFF); //desliga
    lcd_command(L_ON); //liga
    lcd_command(L_CLR); //limpa
    lcd_command(L_CFG); //configura
    lcd_command(L_L1);
}

void lcd_string(const char* str) {
    unsigned char i = 0;

    while (str[i] != 0) {
        lcd_data(str[i]);
        i++;
    }
}