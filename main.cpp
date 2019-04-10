#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#define STACK_SIZE 100

//for shift register (front LEDs)
#define LATCH_PIN 7
#define CLOCK_PIN 8
#define DATA_PIN 13

//for rear LEDs
#define REAR_LED_PIN 12

//for audio buzzer
#define BUZZER_PIN 4

QueueHandle_t leftMotorQueue = NULL;
QueueHandle_t rightMotorQueue = NULL;

//void audio2Task(void *p) {
//	TickType_t xLastWakeTime = xTaskGetTickCount();
//	while (1) {
//		tone(BUZZER_PIN, 392, 10000);
//		vTaskDelayUntil(&xLastWakeTime, 10000);
//	}
//}

void leftMotorTask(void *p) {
	int offset = 0;
	char output = 0;
	int move = 0;
	int direction = 0;
	TickType_t xLastWakeUpTime = 0;

	for (;;) {
		if (xQueueReceive(leftMotorQueue, &output, 0) == pdTRUE) {
			offset = (output & B00001111) - 7;
			if (offset > 0) offset = 0;
			move = output & B00100000;
			direction = (output & B10000000);
		}
		if (!move) {
			//bit 5 indicates no movement
			digitalWrite(5, LOW);
			digitalWrite(6, LOW);
			vTaskDelayUntil(&xLastWakeUpTime, 20);
		} else if (direction) {
			//bit 7 indicates forward turn for left wheel
			digitalWrite(5, LOW);
			digitalWrite(6, HIGH);
			vTaskDelayUntil(&xLastWakeUpTime, 8 + offset);
			digitalWrite(6, LOW);
			vTaskDelayUntil(&xLastWakeUpTime, 20 - (8 + offset));
		} else {
			//bit 7 indicates backward turn for left wheel
			digitalWrite(5, HIGH);
			digitalWrite(6, LOW);
			vTaskDelayUntil(&xLastWakeUpTime, 8 + offset);
			digitalWrite(5, LOW);
			vTaskDelayUntil(&xLastWakeUpTime, 20 - (8 + offset));
		}
	}
}

void rightMotorTask(void *p) {
	int offset = 0;
	int direction = 0;
	int move = 0;
	char output;
	TickType_t xLastWakeUpTime = 0;

	for (;;) {
		if (xQueueReceive(rightMotorQueue, &output, 0) == pdTRUE) {
			offset = (output & B00001111) - 7;
			if (offset < 0) offset = 0;
			move = output & B00100000;
			direction = (output & B01000000);
		}
		if (!move) {
			//bit 5 indicates no movement
			digitalWrite(11, LOW);
			digitalWrite(3, LOW);
			vTaskDelayUntil(&xLastWakeUpTime, 20);
		} else if (direction) {
			//bit 6 indicates forward turn for right wheel
			digitalWrite(11, HIGH);
			digitalWrite(3, LOW);
			vTaskDelayUntil(&xLastWakeUpTime, 8 - offset);
			digitalWrite(11, LOW);
			vTaskDelayUntil(&xLastWakeUpTime, 20 - (8 - offset));
		} else {
			//bit 6 indicates backward turn for right wheel
			digitalWrite(11, LOW);
			digitalWrite(3, HIGH);
			vTaskDelayUntil(&xLastWakeUpTime, 8 - offset);
			digitalWrite(3, LOW);
			vTaskDelayUntil(&xLastWakeUpTime, 20 - (8 - offset));
		}
	}
}

void stopLedTask(void *p) {

	for (int x = 0; x < 2; x++) {
		//First connection
		digitalWrite(LATCH_PIN, LOW);
		shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 255);
		digitalWrite(LATCH_PIN, HIGH);
		vTaskDelay(200);
		digitalWrite(LATCH_PIN, LOW);
		shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0);
		digitalWrite(LATCH_PIN, HIGH);
		vTaskDelay(200);
	}

	for (;;) {
		digitalWrite(LATCH_PIN, LOW);
		shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 255);
		digitalWrite(LATCH_PIN, HIGH);

		// all rear led on and off for 250ms each
		digitalWrite(REAR_LED_PIN, HIGH);
		vTaskDelay(250);
		digitalWrite(REAR_LED_PIN, LOW);
		vTaskDelay(250);
	}
}

void moveLedTask(void *p) {
	int numberToDisplay = 1;

	for (;;) {
		//one front led lighted up according to numberToDisplay
		digitalWrite(LATCH_PIN, LOW);
		shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, numberToDisplay);
		digitalWrite(LATCH_PIN, HIGH);

		//all rear led on for 500ms
		digitalWrite(REAR_LED_PIN, HIGH);
		vTaskDelay(500);

		//updates next front led to light up
		numberToDisplay = (numberToDisplay == 128) ? 1 : numberToDisplay << 1;

		//all rear led off for 500ms
		digitalWrite(REAR_LED_PIN, LOW);
		vTaskDelay(500);
	}
}

void babySharkTask(void *p) {
	//	//indicate first connection
	//	tone(BUZZER_PIN, 2349, 200);
	//	vTaskDelay(300);
	//	tone(BUZZER_PIN, 1760, 200);
	//	vTaskDelay(250);
	//	tone(BUZZER_PIN, 1568, 200);
	//	vTaskDelay(250);
	//	tone(BUZZER_PIN, 1174, 200);
	//	vTaskDelay(250);
	//	tone(BUZZER_PIN, 2349, 200);
	//	vTaskDelay(250);
	//	tone(BUZZER_PIN, 1760, 200);
	//	vTaskDelay(600);
	for (int i = 0; i < 3; i++) {
		//indicate first connection
		tone(BUZZER_PIN, 330, 500);
		vTaskDelay(600);
	}
	for (;;) {
		for (int i = 0; i < 3; i++) {
			tone(BUZZER_PIN, 294, 200);
			vTaskDelay(250);
			tone(BUZZER_PIN, 330, 200);
			vTaskDelay(300);

			for (int j = 0; j < 3; j++) {
				tone(BUZZER_PIN, 392, 200);
				vTaskDelay(300);
			}
			for (int k = 0; k < 2; k++) {
				tone(BUZZER_PIN, 392, 200);
				vTaskDelay(210);
				tone(BUZZER_PIN, 392, 200);
				vTaskDelay(300);
			}
			vTaskDelay(75);
		}
		tone(BUZZER_PIN, 392, 200);
		vTaskDelay(300);
		tone(BUZZER_PIN, 392, 200);
		vTaskDelay(300);
		tone(BUZZER_PIN, 369, 200);
		vTaskDelay(2000);
	}
}

void completeAudioTask(void *p) {
	//	for (;;) {
	//		tone(BUZZER_PIN, 3135, 200);
	//		vTaskDelay(250);
	//		tone(BUZZER_PIN, 2349, 200);
	//		vTaskDelay(250);
	//		tone(BUZZER_PIN, 1568, 200);
	//		vTaskDelay(250);
	//		tone(BUZZER_PIN, 1760, 200);
	//		vTaskDelay(2000);
	//	}
	for (;;) {
		tone(BUZZER_PIN, 392, 500);
		vTaskDelay(600);
	}
}

void serialTask(void *p) {
	char output = 0;
	int move = 1, completed = 1;
	TickType_t xLastWakeUpTime = 0;
	TaskHandle_t stopLedHandler, moveLedHandler, babySharkHandler,
			completeSoundHandler;
	xTaskCreate(moveLedTask, "moveLed", STACK_SIZE, NULL, 2, &moveLedHandler);
	xTaskCreate(stopLedTask, "stopLed", 50, NULL, 2, &stopLedHandler);
	xTaskCreate(babySharkTask, "babyShark", STACK_SIZE, NULL, 1,
			&babySharkHandler);
	xTaskCreate(completeAudioTask, "complete", 50, NULL, 1,
			&completeSoundHandler);
	xTaskCreate(leftMotorTask, "left", STACK_SIZE, NULL, 4, NULL);
	xTaskCreate(rightMotorTask, "right", STACK_SIZE, NULL, 4, NULL);
	for (;;) {
		if (Serial.available()) {
			output = Serial.read();
			if (move != (output & B00100000)) {
				move = output & B00100000;
				if (move) {
					vTaskSuspend(stopLedHandler);
					vTaskResume(moveLedHandler);
				} else {
					vTaskSuspend(moveLedHandler);
					vTaskResume(stopLedHandler);
				}
			}
			if (completed != (output & B00010000)) {
				completed = output & B00010000;
				if (completed) {
					vTaskSuspend(babySharkHandler);
					vTaskResume(completeSoundHandler);
				} else {
					vTaskSuspend(completeSoundHandler);
					vTaskResume(babySharkHandler);
				}
			}
			xQueueOverwrite(leftMotorQueue, &output);
			xQueueOverwrite(rightMotorQueue, &output);
		}
		vTaskDelayUntil(&xLastWakeUpTime, 50);
	}
}

void setup() {
	Serial.begin(9600);
	//right motor
	pinMode(11, OUTPUT);
	pinMode(3, OUTPUT);
	rightMotorQueue = xQueueCreate(1, sizeof(char));

	//left motor
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
	leftMotorQueue = xQueueCreate(1, sizeof(char));

	//shift register(green led)
	pinMode(LATCH_PIN, OUTPUT);
	pinMode(CLOCK_PIN, OUTPUT);
	pinMode(DATA_PIN, OUTPUT);
	digitalWrite(LATCH_PIN, LOW);
	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0);
	digitalWrite(LATCH_PIN, HIGH);

	//red led
	pinMode(REAR_LED_PIN, OUTPUT);

	//tone
	pinMode(BUZZER_PIN, OUTPUT);
}


void loop() {
	if (Serial) {
		xTaskCreate(serialTask, "serial", STACK_SIZE, NULL, 5, NULL);
		vTaskStartScheduler();
		while (1);
	}
}
