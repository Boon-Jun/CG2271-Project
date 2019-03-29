#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>

#define STACK_SIZE 200

//for shift register (front LEDs)
#define LATCH_PIN 7
#define CLOCK_PIN 8
#define DATA_PIN 13

//for rear LEDs
#define REAR_LED_PIN 12

int isMoving = 0; //moving state of robot

void tLED(void *p) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	int numberToDisplay = 1;

	for (;;) {
		if (isMoving) {
			//one front led lighted up according to numberToDisplay
			digitalWrite(LATCH_PIN, LOW);
			shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, numberToDisplay);
			digitalWrite(LATCH_PIN, HIGH);

			//all rear led on for 500ms
			digitalWrite(REAR_LED_PIN, HIGH);
			vTaskDelayUntil(&xLastWakeTime, 500);

			//updates next front led to light up
			numberToDisplay = (numberToDisplay == 128) ? 1 : numberToDisplay << 1;

			//all rear led off for 500ms
			digitalWrite(REAR_LED_PIN, LOW);
			vTaskDelayUntil(&xLastWakeTime, 500);
		}
		else {
			//all front led lighted up
			digitalWrite(LATCH_PIN, LOW);
			shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 255);
			digitalWrite(LATCH_PIN, HIGH);

			// all rear led on and off for 250ms each
			digitalWrite(REAR_LED_PIN, HIGH);
			vTaskDelayUntil(&xLastWakeTime, 250);
			digitalWrite(REAR_LED_PIN, LOW);
			vTaskDelayUntil(&xLastWakeTime, 250);
		}
	}
}

void setup() {
	pinMode(LATCH_PIN, OUTPUT);
	pinMode(CLOCK_PIN, OUTPUT);
	pinMode(DATA_PIN, OUTPUT);
	pinMode(REAR_LED_PIN, OUTPUT);
}

void loop() {
	xTaskCreate(tLED, "TaskLED", STACK_SIZE, NULL, 1, NULL);
	vTaskStartScheduler();
}

