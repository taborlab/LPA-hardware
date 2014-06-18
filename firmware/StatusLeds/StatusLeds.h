/*
 * StatusLeds.h
 *
 * Created: 18/06/2014 03:29:57 PM
 *  Author: Sebastian Castillo
 */ 


#ifndef STATUSLEDS_H_
#define STATUSLEDS_H_

// LED identifiers
#define StatusLeds_LedOn 3
#define StatusLeds_LedErr 4
#define StatusLeds_LedFin 5

// LED status
#define StatusLeds_On 1
#define StatusLeds_Off 0

// Functions
void StatusLeds_Init(void);
void StatusLeds_Set(uint8_t led, uint8_t status);

#endif /* STATUSLEDS_H_ */