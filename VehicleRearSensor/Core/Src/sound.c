/*
 * sound.c
 *
 *  Created on: Dec 3, 2024
 *      Author: rladb
 */

#include "sound.h"

extern TIM_HandleTypeDef htim1;

void initSound() {
	HAL_TIM_PMW_Start(&htim1, TIM_CHANNEL_1);
	htim1.Instance->CCR1 = 0;
}

void startSound() {
	htim1.Instance->ARR = 2000-1;
	htim1.Instance->CCR1 = htim1.Instance->ARR / 2;
	//htim1.Instance->CNT = 0;
}

void stopSound() {
	htim1.Instance->CCR1 = 0;
}
