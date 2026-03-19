/*
  lcd.c
  Created on: 12 mar. 2026
  Author: Kevin Zoloeta
 */
#include "lcd.h"

unsigned char CONVIERTE_NIBBLE(unsigned char nibble){
    unsigned char salida = 0;
    if(nibble & 0x01) salida |= D4;
    if(nibble & 0x02) salida |= D5;
    if(nibble & 0x04) salida |= D6;
    if(nibble & 0x08) salida |= D7;
    return salida;
}

void ESCRIBE_LETRA(unsigned char letra){
    unsigned char byteIzq;
    unsigned char byteDer;

    byteIzq = (letra >> 4) & 0x0F;
    byteDer = letra & 0x0F;

    MANDA_NIBBLE(RS | CONVIERTE_NIBBLE(byteIzq));
    MANDA_NIBBLE(RS | CONVIERTE_NIBBLE(byteDer));
}

void ESCRIBE_CADENA(unsigned char cadena[]){
    int i = 0;
    while(cadena[i] != '\0'){
        ESCRIBE_LETRA(cadena[i]);
        i++;
    }
}

void MANDA_NIBBLE(unsigned char bits){
    P2OUT = bits;
    wait001(50);

    P2OUT = bits | E;
    wait001(100);

    P2OUT = bits;
    wait001(100);
}

void TURN_LCD_4B_MODE(void){
    P2OUT = 0x00;
    wait001(24000);

    /* Secuencia de arranque */
    MANDA_NIBBLE(CONVIERTE_NIBBLE(0x03));
    wait001(18000);
    MANDA_NIBBLE(CONVIERTE_NIBBLE(0x03));
    wait001(18000);
    MANDA_NIBBLE(CONVIERTE_NIBBLE(0x03));
    wait001(18000);

    /* Entrar a modo 4 bits */
    MANDA_NIBBLE(CONVIERTE_NIBBLE(0x02));
    wait001(18000);

    /* Configuración básica */
    MANDA_COMANDO(MODE_4B);
    MANDA_COMANDO(DP_ON_CURSOR_OFF_BLINKING_OFF);
    MANDA_COMANDO(CLEAR);
    wait001(30000);
    MANDA_COMANDO(ENTRY_MODE_STANDARD);
}

void MANDA_COMANDO(unsigned char comando){
    unsigned char byteIzq;
    unsigned char byteDer;

    byteIzq = (comando >> 4) & 0x0F;
    byteDer = comando & 0x0F;

    MANDA_NIBBLE(CONVIERTE_NIBBLE(byteIzq));
    MANDA_NIBBLE(CONVIERTE_NIBBLE(byteDer));
}

void wait001(unsigned volatile int i){
    while(i--);
}

void LCD_INIT(void){
    P2SEL  &= ~LCD_PINS;
    P2SEL2 &= ~LCD_PINS;
    P2DIR  |= LCD_PINS;
    TURN_LCD_4B_MODE();
}

void LCD_CLEAR(void){
    MANDA_COMANDO(CLEAR);
    wait001(30000);
}

void LCD_ROW0(void){
    MANDA_COMANDO(ROW0);
}

void LCD_ROW1(void){
    MANDA_COMANDO(ROW1);
}

void LCD_POSICION(unsigned char fila, unsigned char columna){
    if(fila == 0){
        MANDA_COMANDO(ROW0 + columna);
    }
    else{
        MANDA_COMANDO(ROW1 + columna);
    }
}