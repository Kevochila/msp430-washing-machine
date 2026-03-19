/*
 *  stepper.c
 *  Created on: 18 mar. 2026
 *  Author: Kevochila
 */
#include "stepper.h"

static int volatile actual_complete_step=1; //indice de paso 1-4 (paso completo)
static int volatile actual_half_step=1; //indice de paso 1-8 (paso medio)

static volatile unsigned int ms = 0;

void config_stepper_pins(void)
{
	P1SEL&=~(IN3+IN4);
	P1SEL2&=~(IN3+IN4);
    P2SEL&=~(IN1+IN2);
    P2SEL2&=~(IN1+IN2);
    P2DIR|=IN1+IN2;
	P1DIR|=IN3+IN4;
}

void time_retard(unsigned int lapso)
{
    ms = 0;

    TACCR0 = time;  // 1 ms = 124
    TACCTL0 = CCIE;
    TACTL = TASSEL_2+ID_3+MC_1+TACLR;
    /*Usamos submain clock (1MHz) - lo dividimos entre /8 y tenemos (125kHz), 
    cada 1ms son 125 pulsos de reloj, usamos modo up*/
    while(ms < lapso);
    TACTL = MC_0;                  // detener timer - modo stop
    TACCTL0 &= ~CCIE;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_ISR(void){ms++;}

void half_step(unsigned int n_halfsteps_user,int direccion,int delay_ms){
    int numero_halfsteps_actuales=0;
    while(numero_halfsteps_actuales<n_halfsteps_user){
        if(direccion==0){ //Dirección positiva
            switch(actual_half_step){
                case 1:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= IN1;
                    actual_half_step++;
                    break;

                case 2:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= (IN1 | IN2);
                    actual_half_step++;
                    break;

                case 3:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= IN2;
                    actual_half_step++;
                    break;

                case 4:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= IN2;
                    P1OUT |= IN3;
                    actual_half_step++;
                    break;

                case 5:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P1OUT |= IN3;
                    actual_half_step++;
                    break;

                case 6:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P1OUT |= (IN3 | IN4);
                    actual_half_step++;
                    break;

                case 7:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P1OUT |= IN4;
                    actual_half_step++;
                    break;

                case 8:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= IN1;
                    P1OUT |= IN4;
                    actual_half_step=1;
                    break;

                default:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    actual_half_step=1;
                    break;
            }
        }
        else{ //Dirección negativa
            switch(actual_half_step){
                case 1:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= IN1;
                    actual_half_step=8;
                    break;

                case 2:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= (IN1 | IN2);
                    actual_half_step--;
                    break;

                case 3:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= IN2;
                    actual_half_step--;
                    break;

                case 4:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= IN2;
                    P1OUT |= IN3;
                    actual_half_step--;
                    break;

                case 5:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P1OUT |= IN3;
                    actual_half_step--;
                    break;

                case 6:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P1OUT |= (IN3 | IN4);
                    actual_half_step--;
                    break;

                case 7:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P1OUT |= IN4;
                    actual_half_step--;
                    break;

                case 8:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= IN1;
                    P1OUT |= IN4;
                    actual_half_step--;
                    break;

                default:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    actual_half_step=1;
                    break;
            }
        }

        numero_halfsteps_actuales++;
        time_retard(delay_ms);
    }
}
void complete_step(int numero_steps_user,int direccion,int delay_ms){
    int numero_steps_actuales=0;

    while(numero_steps_actuales<numero_steps_user){
        if(direccion==0){ //Dirección Positiva
            switch(actual_complete_step){
                case 1:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= IN1;
                    actual_complete_step++;
                    break;

                case 2:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= IN2;
                    actual_complete_step++;
                    break;

                case 3:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P1OUT |= IN3 ;
                    actual_complete_step++;
                    break;

                case 4:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P1OUT |= IN4;
                    actual_complete_step=1;
                    break;

                default:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    actual_complete_step=1;
                    break;
            }
        }
        else{ //Dirección Negativa
            switch(actual_complete_step){
                case 1:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P1OUT |= IN4;
                    actual_complete_step++;
                    break;

                case 2:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P1OUT |= IN3;
                    actual_complete_step++;
                    break;

                case 3:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= IN2;
                    actual_complete_step++;
                    break;

                case 4:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    P2OUT |= IN1;
                    actual_complete_step=1;
                    break;

                default:
                    P2OUT &= ~(IN1 | IN2);
                    P1OUT &= ~(IN3 | IN4);
                    actual_complete_step=1;
                    break;
            }
        }

        numero_steps_actuales++;
        time_retard(delay_ms);
    }
}

