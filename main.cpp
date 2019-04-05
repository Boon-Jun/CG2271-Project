#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#define STACK_SIZE 200

//for shift register (front LEDs)
#define LATCH_PIN 7
#define CLOCK_PIN 8
#define DATA_PIN 13

//for rear LEDs
#define REAR_LED_PIN 12

//for audio buzzer
#define AUDIO_PIN 4

QueueHandle_t leftMotorQueue = NULL;
QueueHandle_t rightMotorQueue = NULL;
QueueHandle_t ledQueue = NULL;
QueueHandle_t audioQueue = NULL;

void serialTask(void *p) {
	char output = 0;
	TickType_t xLastWakeUpTime = 0;
	for (;;) {
		if (Serial.available()) {
			output = Serial.read();
			xQueueSend(leftMotorQueue, &output, 1);
			xQueueSend(rightMotorQueue, &output, 1);
			xQueueSend(ledQueue, &output, 1);
			xQueueSend(audioQueue, &output, 1);
		}
	}
	vTaskDelayUntil(&xLastWakeUpTime, 50);
}

void leftMotorTask(void *p) {
	int onPeriod = 0;
	char output = 0;
	int move = 0;
	int direction = 0;
	TickType_t xLastWakeUpTime = 0;
	for (;;) {
		if (xQueueReceive(leftMotorQueue, &output, 0) == pdTRUE) {
			onPeriod = (output & B00000111) - 4;
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
			vTaskDelayUntil(&xLastWakeUpTime, 12 - onPeriod);
			digitalWrite(6, LOW);
			vTaskDelayUntil(&xLastWakeUpTime, 20 - (12 - onPeriod));
		} else {
			//bit 7 indicates backward turn for left wheel
			digitalWrite(5, HIGH);
			digitalWrite(6, LOW);
			vTaskDelayUntil(&xLastWakeUpTime, 12 + onPeriod);
			digitalWrite(5, LOW);
			vTaskDelayUntil(&xLastWakeUpTime, 20 - (12 + onPeriod));
		}
	}
}

void rightMotorTask(void *p) {
	int onPeriod = 0;
	int direction = 0;
	int move = 0;
	char output;
	TickType_t xLastWakeUpTime = 0;
	for (;;) {
		if (xQueueReceive(rightMotorQueue, &output, 0) == pdTRUE) {
			onPeriod = (output & B00000111) - 4;
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
			vTaskDelayUntil(&xLastWakeUpTime, 12 + onPeriod);
			digitalWrite(11, LOW);
			vTaskDelayUntil(&xLastWakeUpTime, 20 - (12 + onPeriod));
		} else {
			//bit 6 indicates backward turn for right wheel
			digitalWrite(11, LOW);
			digitalWrite(3, HIGH);
			vTaskDelayUntil(&xLastWakeUpTime, 12 - onPeriod);
			digitalWrite(3, LOW);
			vTaskDelayUntil(&xLastWakeUpTime, 20 - (12 - onPeriod));
		}
	}
}

void ledTask(void *p) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	int numberToDisplay = 1;
	char output;
	int move = 0;
	for (;;) {
		if (xQueueReceive(ledQueue, &output, 0) == pdTRUE) {
			move = output & B00100000;
		}
		if (move) {
			//one front led lighted up according to numberToDisplay
			digitalWrite(LATCH_PIN, LOW);
			shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, numberToDisplay);
			digitalWrite(LATCH_PIN, HIGH);

			//all rear led on for 500ms
			digitalWrite(REAR_LED_PIN, HIGH);
			vTaskDelayUntil(&xLastWakeTime, 500);

			//updates next front led to light up
			numberToDisplay =
					(numberToDisplay == 128) ? 1 : numberToDisplay << 1;

			//all rear led off for 500ms
			digitalWrite(REAR_LED_PIN, LOW);
			vTaskDelayUntil(&xLastWakeTime, 500);
		} else {
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

void audioTask(void *p) {
	// notes in the melody:
	int melody[] = {
	NOTE_D4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4,
	NOTE_D4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4,
	NOTE_D4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4,
	NOTE_G4, NOTE_G4, NOTE_FS4 };

	// note durations: how long each note should be play for (in ms)
	int noteDurations[] = { 500, 750, 250, 250, 250, 250, 250, 250, 250, 250,
			750, 250, 250, 250, 250, 250, 250, 250, 250, 750, 250, 250, 250,
			250, 250, 250, 750, 250, 500, 2000 };

	int pauseBetweenNotes[] = { 300, 300, 500, 500, 500, 500, 250, 500, 250,
			300, 300, 500, 500, 500, 500, 250, 500, 250, 300, 300, 500, 500,
			500, 500, 250, 500, 250, 250, 400, 250 };

	for (;;) {
		for (int thisNote = 0; thisNote < 30; thisNote++) {
			tone(AUDIO_PIN, melody[thisNote], noteDurations[thisNote]);

			// to distinguish the notes, set a minimum time between them.
			vTaskDelay(pauseBetweenNotes[thisNote]);

			// stop the tone playing:
			noTone(AUDIO_PIN);
		}
	}
}

void setup() {
	Serial.begin(9600);
	//right motor
	pinMode(11, OUTPUT);
	pinMode(3, OUTPUT);
	rightMotorQueue = xQueueCreate(10, sizeof(char));

	//left motor
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
	leftMotorQueue = xQueueCreate(10, sizeof(char));

	//shift register(green led)
	pinMode(LATCH_PIN, OUTPUT);
	pinMode(CLOCK_PIN, OUTPUT);
	pinMode(DATA_PIN, OUTPUT);

	//red led
	pinMode(REAR_LED_PIN, OUTPUT);
	ledQueue = xQueueCreate(10, sizeof(char));

	//tone
	pinMode(AUDIO_PIN, OUTPUT);
	audioQueue = xQueueCreate(10, sizeof(char));
}

void loop() {
	xTaskCreate(leftMotorTask, "left", STACK_SIZE, NULL, 4, NULL);
	xTaskCreate(rightMotorTask, "right", STACK_SIZE, NULL, 4, NULL);

	xTaskCreate(serialTask, "serial", STACK_SIZE, NULL, 2, NULL);

	xTaskCreate(ledTask, "led", STACK_SIZE, NULL, 3, NULL);

	xTaskCreate(audioTask, "audio", STACK_SIZE, NULL, 1, NULL);

	vTaskStartScheduler();
}
