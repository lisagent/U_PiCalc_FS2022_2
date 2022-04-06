/*
 * U_PiCalc_FS2022
 *
 * Created: 20.03.2018 18:32:07
 * Author : -
 */ 

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "avr_compiler.h"
#include "pmic_driver.h"
#include "TC_driver.h"
#include "clksys_driver.h"
#include "sleepConfig.h"
#include "port_driver.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "stack_macros.h"
//#include "semphr. h"

#include "mem_check.h"

#include "init.h"
#include "utils.h"
#include "errorHandler.h"
#include "NHD0420Driver.h"

#include "ButtonHandler.h"
#include "avr_f64.h"

//SemaphoreHandle_t MeinMutex; //A-Ressource
uint8_t Datenuebertragung; //B-Ressource

float pi4;
float pi;
float piN;
float pi_l;
float64_t pi_n;
int action;

//Eventgroup erstellen
EventGroupHandle_t EventGroupPiCalc;
#define StartTask		1<<0
#define StopTask		1<<1
#define ResetTask		 1<<2
#define WechsleTask		1<<3
#define PiCalc_Reset	0x03FC


//Forwardsdecleration Tasks
void controllerTask(void* pvParameters);
void leibniz_task(void* pvParameters);
void Nilakantha_task(void* pvParameters);
void vButtonTask(void *pvParameters);

int main(void)
{
    vInitClock();
	vInitDisplay();
	
	EventGroupPiCalc = xEventGroupCreate();
	//MeinMutex = xSemaphoreCreateMutex();
	
	//die unterschiedlichen Tasks
	xTaskCreate( controllerTask, (const char *) "control_tsk", configMINIMAL_STACK_SIZE+150, NULL, 3, NULL);
	xTaskCreate( leibniz_task, (const char *) "leibniz_tsk", configMINIMAL_STACK_SIZE+150, NULL, 1, NULL);
	xTaskCreate( Nilakantha_task, (const char *) "Somayaji_tsk", configMINIMAL_STACK_SIZE+150, NULL, 1, NULL);
//	xTaskCreate( vButtonTask, (const char *) "buttonTask", configMINIMAL_STACK_SIZE+50, NULL, 1, NULL);

	vDisplayClear();
	vDisplayWriteStringAtPos(0,0,"PI-Calc FS2022");
	
	vTaskStartScheduler();
	return 0;
}

void leibniz_task(void* pvParameters){
	pi4 = 1;
	uint32_t zaehler = 3;
	float pi = 0;
 for (;;) {
	//action = xEventGroupWaitBits (EventGroupPiCalc, StartLeibniz,pdTRUE,pdFALSE, portMAX_DELAY);
	//if ((action & StartLeibniz) !=0 ){
	if((xEventGroupGetBits(EventGroupPiCalc)&StartTask)){
		pi4 = pi4-(1.0/zaehler);
		 zaehler += 2;
		pi4 = pi4 +(1.0/zaehler);
		zaehler += 2;
		pi_l = pi4*4;
	}	// else {
		//if((xEventGroupGetBits(EventGroupPiCalc)&StartTask)){
		//pi_l = 45621;
	//}
	//
	//if xSemaphoreTake(MeinMutex,10/portTICK_RATE_MS) == pdTRUE {
		//Datenuebertragung = pi_l;
			//xSemaphoreGive(MeinMutex);
		//}
		//
		//vTaskDelay(10/portTICK_RATE_MS);

	}
	//xEventGroupClearBits(EventGroupPiCalc, StartTask);
	// informiere_task();
	 //warte_hier();
// }
}

void Nilakantha_task(void* pvParameters){
	piN = 3;
	uint64_t zaehler_s = 3;
 for (;;) {	
	 if((xEventGroupGetBits(EventGroupPiCalc)&StartTask)){
	 piN = piN+(4.0/(pow(zaehler_s,3)-zaehler_s));
	 zaehler_s += 2;
	 piN = piN-(4.0/(pow(zaehler_s,3)-zaehler_s));
	 zaehler_s += 2;
	 pi_n = piN;}	
	// informiere_task();
	 //warte_hier();
 }
}


void controllerTask(void* pvParameters) {
	char pistring[12];
	uint32_t fivehundertmillisecondscounter = 0;
	initButtons();
	for(;;) {
		updateButtons();
		//if (xSemaphoreTake(MeinMutex,10/portTICK_RATE_MS) == pdTRUE) {
					//pi_l = Datenuebertragung;
					//xSemaphoreGive(MeinMutex);
				//}
		if (fivehundertmillisecondscounter == 0){
			vDisplayClear();
			sprintf(&pistring[0], "PI_L: %.8f", pi_l);
			vDisplayWriteStringAtPos(1,0, "%s", pistring);
			sprintf(&pistring[0], "PI_N: %.8f", pi_n);
			vDisplayWriteStringAtPos(2,0, "%s", pistring);
			fivehundertmillisecondscounter = 50;
		} else {
			fivehundertmillisecondscounter --;
		}
				//vDisplayClear();		
		//vDisplayWriteStringAtPos(0,0, "MeineVariabe: %d",localy);		
		//vTaskDelay(200/portTICK_RATE_MS);
		if(getButtonPress(BUTTON1) == SHORT_PRESSED) {
			xEventGroupSetBits(EventGroupPiCalc, StartTask);
				//vDisplayClear();
				//sprintf(&pistring[0], "PI_L: %.8f", pi_l);
				//vDisplayWriteStringAtPos(0,0, "Leibniz-Task");
				//vDisplayWriteStringAtPos(1,0, "%s", pistring);			
			//xEventGroupSetBits(EventGroupPiCalc,StartTask);

			
			//char pistring[12];
			//sprintf(&pistring[0], "PI: %.8f", M_PI);
			//vDisplayWriteStringAtPos(1,0, "%s", pistring);
		}
		if(getButtonPress(BUTTON2) == SHORT_PRESSED) {
			xEventGroupSetBits(EventGroupPiCalc, StopTask);
			xEventGroupClearBits(EventGroupPiCalc, StartTask);
							//vDisplayClear();
				//sprintf(&pistring[0], "PI_L: %.8f", pi_n);
				//vDisplayWriteStringAtPos(0,0, "Nilakantha-Task");
				//vDisplayWriteStringAtPos(1,0, "%s", pistring);			
			//xEventGroupSetBits(EventGroupPiCalc,StartLeibniz);
					//if (fivehundertmillisecondscounter == 0){
			//vDisplayClear();
			//sprintf(&pistring[0], "PI: %.8f", pi_l);
			//vDisplayWriteStringAtPos(1,0, "%s", pistring);
			//fivehundertmillisecondscounter = 50;
		//} else {
			//fivehundertmillisecondscounter --;
		//}
		}
		if(getButtonPress(BUTTON3) == SHORT_PRESSED) {
			xEventGroupSetBits(EventGroupPiCalc, ResetTask);
			xEventGroupClearBits(EventGroupPiCalc, StopTask);
			xEventGroupClearBits(EventGroupPiCalc, StartTask);
			xEventGroupClearBits(EventGroupPiCalc, WechsleTask);
					//if (fivehundertmillisecondscounter == 0){
			//vDisplayClear();
			//sprintf(&pistring[0], "PI: %.8f", pi_s);
			//vDisplayWriteStringAtPos(1,0, "%s", pistring);
			//fivehundertmillisecondscounter = 50;
		//} else {
			//fivehundertmillisecondscounter --;
		//}
		}
		if(getButtonPress(BUTTON4) == SHORT_PRESSED) {
		xEventGroupSetBits(EventGroupPiCalc, WechsleTask);

		}
		if(getButtonPress(BUTTON1) == LONG_PRESSED) {
			
		}
		if(getButtonPress(BUTTON2) == LONG_PRESSED) {
			
		}
		if(getButtonPress(BUTTON3) == LONG_PRESSED) {
			
		}
		if(getButtonPress(BUTTON4) == LONG_PRESSED) {
			
		}
		vTaskDelay(10/portTICK_RATE_MS);
	}
}


////Mutex ausprobieren
//void task1(void *pvParameters) {
	//uint8_t localx = 0;
	//for (;;) {
		//localx++;
		//if xSemaphoreTake(MeinMutex,10/portTICK_RATE_MS) == pdTRUE {
			//meinevariable = localx;
			//xSemaphoreGive(MeinMutex);
		//}
		//
		//
		//
		//vTaskDelay(10/portTICK_RATE_MS);
	//}
//}
//void task2(void *pvParameters) {
	//uint8_t localy =0;
	//for (;;) {
				//if (xSemaphoreTake(MeinMutex,10/portTICK_RATE_MS) == pdTRUE) {
					//localy = meinevariable;
					//xSemaphoreGive(MeinMutex);
				//}
		//vDisplayClear();		
		//vDisplayWriteStringAtPos(0,0, "MeineVariabe: %d",localy);		
		//vTaskDelay(200/portTICK_RATE_MS);
	//}
//}
//
//#define LOCK_DATA	1<<0
//#define LOCK_DATA	1<<1
//void task3(void*pvParameters){
	//
	//for(;;){
		//
		//vTaskDelay(10/portTICK_RATE_MS);
	//}
	//
//}
//versuch