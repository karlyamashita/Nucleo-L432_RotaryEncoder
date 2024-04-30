/*
 * PollingRoutine.h
 *
 *  Created on: Oct 24, 2023
 *      Author: karl.yamashita
 *
 *
 *      Template
 */

#ifndef INC_POLLINGROUTINE_H_
#define INC_POLLINGROUTINE_H_


/*

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>

*/


typedef struct
{
	uint32_t tim1_cnt;
	uint32_t lastCnt;
}EncodreStruct;

void PollingInit(void);
void PollingRoutine(void);

void CheckEncoder(volatile EncodreStruct *enc);
void PushButtonPressed(void);
void PushButtonReleased(void);

#endif /* INC_POLLINGROUTINE_H_ */
