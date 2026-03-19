/*
 *  main.c
 *  Created on: 18 mar. 2026
 *  Author: Kevochila
 */
#include <msp430.h>
#include "lcd.h"
#include "stepper.h"

// ===== Botones en P1 =====
#define BTN_POWER   BIT0
#define BTN_SPEED   BIT5
#define BTN_SOAK    BIT1
#define BTN_WASH    BIT2
#define BTN_SPIN    BIT3
#define BUTTONS_ALL (BTN_POWER | BTN_SPEED | BTN_SOAK | BTN_WASH | BTN_SPIN)

// ===== Estados globales =====
volatile unsigned char sistema_encendido = 0;
volatile unsigned char velocidad_alta = 0;
volatile unsigned char evento_boton = 0;
volatile unsigned char ciclo_en_proceso = 0;

// ===== Estados FSM ===== 
#define EV_NONE   0
#define EV_POWER  1
#define EV_SPEED  2
#define EV_SOAK   3
#define EV_WASH   4
#define EV_SPIN   5

// ===== Delays del motor =====
#define DELAY_BAJO  5
#define DELAY_ALTO  3

void config_buttons(void);
void mostrar_bienvenida(void);
void mostrar_apagado(void);
void mostrar_estado_listo(void);
void mostrar_ciclo(unsigned char programa);
void mostrar_terminado(void);
void ejecutar_remojado(void);
void ejecutar_lavado(void);
void ejecutar_exprimido(void);

int main(void)
{
    /* ====== Inicialización del sistema ====== */
    WDTCTL = WDTPW | WDTHOLD;

    LCD_INIT();
    config_stepper_pins();
    config_buttons();

    __bis_SR_register(GIE);

    mostrar_bienvenida();
    mostrar_apagado();
    /* =========================================================
       Bucle principal con FSM 
       para manejar eventos de botones
       y controlar el sistema de lavado
       ========================================================= */
    while(1)
    {
        /*  FSM:
        NONE -> es el estado default, siempre se regresa a este estado cuando se procesa un evento
        POWER -> es el estado de encendido/apagado del sistema, no se puede pasar a otro evento
        SPEED -> es el estado en el que se ajusta la velocidad
        SOAK -> es el estado de remojado 5 vueltas en cada dirección
        WASH -> es el estado de lavado 10 vueltas en cada dirección
        SPIN -> es el estado de exprimido 5 vueltas en cada dirección a alta velocidad
        */
        switch(evento_boton)
        {
        case EV_POWER:
            evento_boton = EV_NONE;

            if(ciclo_en_proceso == 0)
            {
                sistema_encendido ^= 1;

                if(sistema_encendido)
                {
                    mostrar_estado_listo();
                }
                else
                {
                    mostrar_apagado();
                }
            }
            break;

        case EV_SPEED:
            evento_boton = EV_NONE;

            if(sistema_encendido && (ciclo_en_proceso == 0))
            {
                velocidad_alta ^= 1;
                mostrar_estado_listo();
            }
            break;

        case EV_SOAK:
            evento_boton = EV_NONE;

            if(sistema_encendido && (ciclo_en_proceso == 0))
            {
                ciclo_en_proceso = 1;
                mostrar_ciclo(EV_SOAK);
                ejecutar_remojado();
                mostrar_terminado();
                ciclo_en_proceso = 0;

                if(sistema_encendido)
                    mostrar_estado_listo();
                else
                    mostrar_apagado();
            }
            break;

        case EV_WASH:
            evento_boton = EV_NONE;

            if(sistema_encendido && (ciclo_en_proceso == 0))
            {
                ciclo_en_proceso = 1;
                mostrar_ciclo(EV_WASH);
                ejecutar_lavado();
                mostrar_terminado();
                ciclo_en_proceso = 0;

                if(sistema_encendido)
                    mostrar_estado_listo();
                else
                    mostrar_apagado();
            }
            break;

        case EV_SPIN:
            evento_boton = EV_NONE;

            if(sistema_encendido && (ciclo_en_proceso == 0))
            {
                ciclo_en_proceso = 1;
                mostrar_ciclo(EV_SPIN);
                ejecutar_exprimido();
                mostrar_terminado();
                ciclo_en_proceso = 0;

                if(sistema_encendido)
                    mostrar_estado_listo();
                else
                    mostrar_apagado();
            }
            break;

        default:
            break;
        }
    }
}


//Configuración de botones con pull-up e interrupciones
void config_buttons(void)
{
    P1SEL  &= ~BUTTONS_ALL; //como GPIO
    P1SEL2 &= ~BUTTONS_ALL;
    P1DIR  &= ~BUTTONS_ALL; //como entrada
    P1REN  |= BUTTONS_ALL;  //resistencia pull-up
    P1OUT  |= BUTTONS_ALL;  //pull-up

    P1IES  |= BUTTONS_ALL;   //interrupción por flanco de bajada (presión del botón)
    P1IFG  &= ~BUTTONS_ALL;  //limpiar flags de interrupción
    P1IE   |= BUTTONS_ALL;   //habilitar interrupciones
}

/* =========================================================
   Pantallas LCD
   ========================================================= */
void mostrar_bienvenida(void)
{
    LCD_CLEAR();
    LCD_ROW0();
    ESCRIBE_CADENA("Lavadora MSP430");
    LCD_ROW1();
    ESCRIBE_CADENA("Bienvenido");
    time_retard(5000);
}

void mostrar_apagado(void)
{
    LCD_CLEAR();
    LCD_ROW0();
    ESCRIBE_CADENA("Sistema OFF");
    LCD_ROW1();
    ESCRIBE_CADENA("Presiona Power");
}

void mostrar_estado_listo(void)
{
    LCD_CLEAR();
    LCD_ROW0();
    ESCRIBE_CADENA("Sistema ON");

    LCD_ROW1();
    if(velocidad_alta)
        ESCRIBE_CADENA("Velocidad: Alta");
    else
        ESCRIBE_CADENA("Velocidad: Baja");
}

void mostrar_ciclo(unsigned char programa)
{
    LCD_CLEAR();
    LCD_ROW0();

    if(programa == EV_SOAK)
        ESCRIBE_CADENA("Modo: Remojado");
    else if(programa == EV_WASH)
        ESCRIBE_CADENA("Modo: Lavado");
    else
        ESCRIBE_CADENA("Modo: Exprimido");

    LCD_ROW1();
    if(velocidad_alta)
        ESCRIBE_CADENA("Trabajando Alta");
    else
        ESCRIBE_CADENA("Trabajando Baja");
    time_retard(3000);
}



void mostrar_terminado(void)
{
    LCD_CLEAR();
    LCD_ROW0();
    ESCRIBE_CADENA("Ciclo terminado");
    LCD_ROW1();
    ESCRIBE_CADENA("Listo");
    time_retard(3000);
}

/* =========================================================
   Ciclos de lavado
   ========================================================= */
void ejecutar_remojado(void)
{
    int delay_usado;

    if(velocidad_alta)
        delay_usado = DELAY_ALTO;
    else
        delay_usado = DELAY_BAJO;

    LCD_CLEAR();
    LCD_ROW0();
    ESCRIBE_CADENA("Remojado CW");
    complete_step(una_vuelta_cs * 5, 0, delay_usado); //numero de pasos, dirección, delay entre pasos

    LCD_CLEAR();
    LCD_ROW0();
    ESCRIBE_CADENA("Remojado CCW");
    complete_step(una_vuelta_cs * 5, 1, delay_usado);
}

void ejecutar_lavado(void)
{
    int delay_usado;

    if(velocidad_alta)
        delay_usado = DELAY_ALTO;
    else
        delay_usado = DELAY_BAJO;

    LCD_CLEAR();
    LCD_ROW0();
    ESCRIBE_CADENA("Lavado CW");
    half_step(una_vuelta_hs * 5, 0, delay_usado); //Ejecutamos 5 vueltas 2 veces
    half_step(una_vuelta_hs * 5, 0, delay_usado); //para evitar desbordes de la función
    LCD_CLEAR();
    LCD_ROW0();
    ESCRIBE_CADENA("Lavado CCW");
    half_step(una_vuelta_hs * 5, 1, delay_usado);
    half_step(una_vuelta_hs * 5, 1, delay_usado);
}

void ejecutar_exprimido(void)
{
    int delay_usado;

    /* 
    Exprimido debe ser más rápido que cualquiera
    de las 2 velocidades existentes.
    */
    if(velocidad_alta)
        delay_usado = DELAY_ALTO-1;
    else
        delay_usado = DELAY_BAJO-2;

    LCD_CLEAR();
    LCD_ROW0();
    ESCRIBE_CADENA("Exprimiendo CW");
    complete_step(una_vuelta_cs * 5, 0, delay_usado);

    LCD_CLEAR();
    LCD_ROW0();
    ESCRIBE_CADENA("Exprimiendo CCW");
    complete_step(una_vuelta_cs * 5, 1, delay_usado);
}
/* =========================================================
   Interrupciones de los botones
   y su implementación como eventos para 
   cambio de estados en la FSM
   ========================================================= */
#pragma vector=PORT1_VECTOR
__interrupt void Port_1_ISR(void)
{
    if(P1IFG & BTN_POWER)
    {
        evento_boton = EV_POWER;
        P1IFG &= ~BTN_POWER;
    }
    else if(P1IFG & BTN_SPEED)
    {
        evento_boton = EV_SPEED;
        P1IFG &= ~BTN_SPEED;
    }
    else if(P1IFG & BTN_SOAK)
    {
        evento_boton = EV_SOAK;
        P1IFG &= ~BTN_SOAK;
    }
    else if(P1IFG & BTN_WASH)
    {
        evento_boton = EV_WASH;
        P1IFG &= ~BTN_WASH;
    }
    else if(P1IFG & BTN_SPIN)
    {
        evento_boton = EV_SPIN;
        P1IFG &= ~BTN_SPIN;
    }
}
