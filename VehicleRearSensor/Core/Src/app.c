/*
 * app.c
 *
 *  Created on: Nov 25, 2024
 *      Author: rladb
 */

#include "app.h"


int cycleDistance = 0;  //두개의 함수가 실행되는 루트 결정
int cycleSound = 0;

//1ms의 주기로 호출됨
void SysticCallback() {
	if(cycleDistance > 0) cycleDistance--;
	if(cycleSound > 0) cycleSound--;
}

void app() {
	uint8_t distance;
	uint8_t periodSound = 0;
	int periodOn = 0;
	//init
	initSound();
	initDistance();
	while(1){
	  //measure distance 거리측정
		if (cycleDistance == 0) {
			cycleDistance = 100;
			distance2 = getDistance();
			//judgement distance
		if(distance2 > 20) periodOn = 1000 / 2;
		  else if((distance2 <= 20) && (distance2 > 15)) periodOn = 800 / 2;
		  else if((distance2 <= 15) && (distance2 > 10)) periodOn = 600 / 2;
		  else if((distance2 <= 10) && (distance2 > 5)) periodOn = 400 / 2;
		  else if(distance2 <= 5) periodOn = 0;
		}
		//generate sound 소리발생
		if(cycleSound == 0) {
			cycleSound = periodOn;
			if(periodSound == 0) startSound();
			else {
			  if(periodSound == 1) startSound();
			  else if(periodSound == 0) stopSound();
			  periodSound ^= 1;
			}
		}
	}
}
