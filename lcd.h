/*
 * lcd.h
 *
 *  Created on: 12 mar. 2026
 *      Author: Kevochila
 */

#ifndef LCD_H_
#define LCD_H_

#include <msp430.h>

/* Pines del LCD */
#define D4 BIT3
#define D5 BIT4
#define D6 BIT5
#define D7 BIT7
#define RS BIT0
#define E  BIT6

#define LCD_PINS (RS | E | D4 | D5 | D6 | D7)

/* Comandos principales */
#define CLEAR                          0x01
#define HOME                           0x02

#define ROW0                           0x80
#define ROW1                           0xC0


#define DP_ON_CURSOR_OFF_BLINKING_OFF  0x0C
#define DP_ON_CURSOR_ON_BLINKING_OFF   0x0E
#define DP_ON_CURSOR_ON_BLINKING_ON    0x0F

#define MODE_4B                        0x28

#define ENTRY_MODE_STANDARD            0x06
#define ENTRY_MODE_SHIFT               0x07

/* Funciones originales */
void wait001(volatile unsigned int i);
void TURN_LCD_4B_MODE(void);
void MANDA_NIBBLE(unsigned char bits);
void MANDA_COMANDO(unsigned char comando);
void ESCRIBE_LETRA(unsigned char letra);
void ESCRIBE_CADENA(unsigned char cadena[]);
unsigned char CONVIERTE_NIBBLE(unsigned char nibble);

/* Funciones extra útiles */
void LCD_INIT(void);
void LCD_CLEAR(void);
void LCD_ROW0(void);
void LCD_ROW1(void);
void LCD_POSICION(unsigned char fila, unsigned char columna);

#endif
