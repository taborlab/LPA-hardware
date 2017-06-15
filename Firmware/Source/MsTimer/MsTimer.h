/*
 * MsTimer.h
 *
 * Created: 01/07/2014 04:32:35 PM
 *  Author: Sebastian Castillo
 */ 

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MSTIMER_H_
#define MSTIMER_H_

#include <avr/io.h>

// Functions for enabling/disabling the timer and the interrupts
#define MsTimer_EnableTimer() TCCR1B |= (1 << CS10);
#define MsTimer_EnableInt() TIMSK1 = (1 << OCIE1A);
#define MsTimer_DisableTimer() TCCR1B &= 0b11111000;
#define MsTimer_DisableInt() TIMSK1 = 0x00;
#define MsTimer_ResetCounter() TCNT1 = 0;

// Functions
void MsTimer_Init(void);
void MsTimer_Start(void);
void MsTimer_Stop(void);
void MsTimer_AddCallback(void (*callback)(void), uint32_t period);

// Clock counter: Indicates time since counter startup in ms.
extern uint32_t MsTimer_Counter;

// Callback setup
#ifndef MsTimer_CallbackMax
#define MsTimer_CallbackMax 2
#endif
extern uint8_t MsTimer_CallbackN;
extern uint32_t MsTimer_CallbackPeriod[MsTimer_CallbackMax];
extern uint32_t MsTimer_CallbackCounter[MsTimer_CallbackMax];
extern void (*MsTimer_Callback[MsTimer_CallbackMax])(void);

#endif /* MSTIMER_H_ */

#ifdef __cplusplus
}
#endif