/*
 * PollingRoutine.c
 *
 *  Created on: Oct 24, 2023
 *      Author: karl.yamashita
 *
 *
 *      Template for projects.
 *
 */


#include "main.h"

extern TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart2;

extern TimerCallbackStruct timerCallback;

UART_DMA_QueueStruct uart2_msg =
{
	.huart = &huart2,
	.rx.queueSize = UART_DMA_QUEUE_SIZE,
	.tx.queueSize = UART_DMA_QUEUE_SIZE
};

EncodreStruct encoder = {0};

void PollingInit(void)
{
	HAL_TIM_Encoder_Start_IT(&htim1, TIM_CHANNEL_ALL);
	TimerCallbackRegisterOnly(&timerCallback, PushButtonPressed);
}

void PollingRoutine(void)
{
	TimerCallbackCheck(&timerCallback);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	char str[16] = {0};

	// keep CNT between 0 and 120 no matter how much the rotary encoder is turned.
	if(TIM1->CNT > 65000)
	{
		TIM1->CNT = 0;
	}
	else if(TIM1->CNT > 120)
	{
		TIM1->CNT = 120;
	}

	encoder.tim1_cnt = TIM1->CNT >> 2; // divide by 4.
	// 120/4 = 30, so 30 will be the highest number being printed.

	if(encoder.tim1_cnt != encoder.lastCnt) // check if value changed from last interrupt.
	{
		encoder.lastCnt = encoder.tim1_cnt; // update lastCnt
		sprintf(str, "%ld", encoder.tim1_cnt); // save value in str as ASCII value
		UART_DMA_NotifyUser(&uart2_msg, str, strlen(str), true); // add str to tx queue
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == PB_Pin) // validate Push Button caused interrupt
	{
		if(HAL_GPIO_ReadPin(PB_GPIO_Port, PB_Pin) == GPIO_PIN_SET) // Push Button is released
		{
			TimerCallbackDisable(&timerCallback, PushButtonPressed); // disable callback
			PushButtonReleased();
		}
		else // Push Button is pressed
		{
			TimerCallbackTimerStart(&timerCallback, PushButtonPressed, 20, TIMER_NO_REPEAT); // start timer for debounce
		}
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart == uart2_msg.huart)
	{
		RingBuff_Ptr_Input(&uart2_msg.rx.ptr, uart2_msg.rx.queueSize);
		UART_DMA_EnableRxInterrupt(&uart2_msg);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == uart2_msg.huart)
	{
		uart2_msg.tx.txPending = false;
		UART_DMA_SendMessage(&uart2_msg);
	}
}

void PushButtonPressed(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET); // set LED
}

void PushButtonReleased(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET); // clear LED
}

