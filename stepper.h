#ifndef STEPPER_H_
#define STEPPER_H_

#include <msp430.h>
//entradas motor a pasos
#define IN1 BIT1 //2.1
#define IN2 BIT2 //2.2
#define IN3 BIT6 //1.6
#define IN4 BIT7 //1.7
#define una_vuelta_cs 2075
#define una_vuelta_hs una_vuelta_cs*2
#define time 124

//funciones
void complete_step(int numero_steps_user,int direccion,int delay_ms);
void half_step(unsigned int n_halfsteps_user,int direccion,int delay_ms);
void time_retard(unsigned int lapso);
void config_stepper_pins(void);

#endif


