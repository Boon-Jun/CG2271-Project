#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#define STACK_SIZE 200
#define BUZZER_PIN 4
QueueHandle_t audioQueue = NULL;

// notes in the melody:

int isCompleted = 0; //completion state of challenge

void tAudio(void *p) {
	TickType_t xLastWakeTime = xTaskGetTickCount();

	//for (;;) {
	if (!isCompleted) {

		for (int i = 0; i < 3; i++) {
			tone(BUZZER_PIN, 294, 200);
			vTaskDelayUntil(&xLastWakeTime, 250);
			tone(BUZZER_PIN, 330, 200);
			vTaskDelayUntil(&xLastWakeTime, 300);

			for (int k = 0; k < 3; k++) {
				tone(BUZZER_PIN, 392, 200);
				vTaskDelayUntil(&xLastWakeTime, 300);
			}
			for (int j = 0; j < 2; j++) {
				tone(BUZZER_PIN, 392, 200);
				vTaskDelayUntil(&xLastWakeTime, 210);
				tone(BUZZER_PIN, 392, 200);
				vTaskDelayUntil(&xLastWakeTime, 300);
			}
			vTaskDelayUntil(&xLastWakeTime, 75);
		}
		tone(BUZZER_PIN, 392, 200);
		vTaskDelayUntil(&xLastWakeTime, 300);
		tone(BUZZER_PIN, 392, 200);
		vTaskDelayUntil(&xLastWakeTime, 300);
		tone(BUZZER_PIN, 349, 200);
		vTaskDelayUntil(&xLastWakeTime, 2000);
	} else {

	}
	//}
}

void setup() {

}

void loop() {
	xTaskCreate(tAudio, "TaskAudio", STACK_SIZE, NULL, 1, NULL);
	vTaskStartScheduler();
}

void connectTone(void *p) {
	char output = 0;
	if (xQueueReceive(audioQueue, &output, 1) == pdTRUE) {

	}
}
