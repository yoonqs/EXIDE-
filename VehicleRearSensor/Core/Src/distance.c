/*
 * distance.c
 *
 *  Created on: Dec 3, 2024
 *      Author: rladb
 */
#include "distance.h"

//장치선언
extern TIM_HandleTypeDef htim3;  //for ultrasonic
extern TIM_HandleTypeDef htim11; //for dealyUs

//전역변수
uint32_t ic_val1 = 0;    //상승 위치 저장
uint32_t ic_val2 = 0;    //하강 위치 저장
uint32_t difference = 0; //하강 - 상승 (뒤에서 앞에것을 뺀다 거리측정)
uint8_t  is_first_captured = 0; //현재 캡춰 단계
uint8_t  distance = 0;   //계산된거리(cm) 8bit여서 200cm이상은 저장안됨

//us delay
void delayUs(uint16_t t) {  //타이머 2번5번 32비트, 나머지는 16비트
	htim11.Instance->CNT = 0;          //
	while(htim11.Instance->CNT < t);   //cnt값이 t보다 작다, up카운터의 값이 t와 같으면 종료
}
void initDistance() {
	HAL_TIM_Base_Start(&htim11);
	HAL_TIM_IC_Start(&htim3, TIM_CHANNEL_2);
}

//인터럽트 서비스 루틴(isr)
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	//인터럽트가 발생된 타이머의 채널 확인
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
		//현재 캡춰 단계 (0일때는 상승단계)
		if(is_first_captured == 0) {
			ic_val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);//htim핸들을 읽겠다 채널은 2번
			//다음 인터럽트 발생을 위한 설정
			is_first_captured = 1;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
		}
		//현재 캡춰 단계 (1일때는 하강단계)
		else if(is_first_captured == 1) {
			ic_val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

			//거리계산 (val1이 클경우와 val2가 클 경우를 고려해야함
    if(ic_val2 > ic_val1) {
    	difference = ic_val2 - ic_val1;
    }
    else {
    	difference = 65535 - ic_val1 + ic_val2;
    }
			distance = difference * 0.034 / 2;

			//다음 인터럽트 발생을 위한 설정
			is_first_captured = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
			htim->Instance->CNT = 0;
			//인터럽트 중지
			__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_CC2);
		}
	}
}

//거리를 측정하여 반환
uint32_t getDistance() {
  //트리거 핀 동작
	HAL_GPIO_WritePin(Trigger_GPIO_Port, Trigger_Pin, GPIO_PIN_SET);//상태를 1 0 으로 변경하는 변수  hal(로시작)_gpio(이름)
	delayUs(10); //함수생성
	HAL_GPIO_WritePin(Trigger_GPIO_Port, Trigger_Pin, GPIO_PIN_RESET);
	//인터럽트 설정
	__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC2);
	return distance;
}

