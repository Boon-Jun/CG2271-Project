/*2E

 #include <Arduino.h>
 #include <avr/io.h>
 #include <FreeRTOS.h>
 #include <task.h>
 #define STACK_SIZE 200
 #define PIN_6 6
 #define PIN_7 7
 #define PIN_8 8


 void task1(void *p) {
 bool ledLighted = false;
 const TickType_t xPeriod = TickType_t( 1500 );
 TickType_t xLastWakeUpTime = xTaskGetTickCount();

 for (;;) {
 if (ledLighted) {
 ledLighted = false;
 digitalWrite(PIN_6, LOW);
 } else {
 ledLighted = true;
 digitalWrite(PIN_6, HIGH);
 }
 vTaskDelayUntil(&xLastWakeUpTime, xPeriod);
 }
 }

 void task2(void *p) {
 bool ledLighted = false;
 const TickType_t xPeriod = TickType_t( 2000 );

 TickType_t xLastWakeUpTime = xTaskGetTickCount();

 for (;;) {
 if (ledLighted) {
 ledLighted = false;
 digitalWrite(PIN_7, LOW);
 } else {
 ledLighted = true;
 digitalWrite(PIN_7, HIGH);
 }
 vTaskDelayUntil(&xLastWakeUpTime, xPeriod);
 }
 }

 void task3(void *p) {
 bool ledLighted = false;
 const TickType_t xPeriod = TickType_t( 4000 );
 TickType_t xLastWakeUpTime = xTaskGetTickCount();

 for (;;) {
 if (ledLighted) {
 ledLighted = false;
 digitalWrite(PIN_8, LOW);
 } else {
 ledLighted = true;
 digitalWrite(PIN_8, HIGH);
 }
 vTaskDelayUntil(&xLastWakeUpTime, xPeriod);
 }
 }

 void setup() {
 pinMode(PIN_6, OUTPUT);
 pinMode(PIN_7, OUTPUT);
 pinMode(PIN_8, OUTPUT);
 }
 void loop() {
 xTaskCreate(task1,"Task1",STACK_SIZE,NULL,3, NULL);
 xTaskCreate(task2,"Task2",STACK_SIZE,NULL,2, NULL);
 xTaskCreate(task3,"Task3",STACK_SIZE,NULL,1, NULL);
 vTaskStartScheduler();
 }
*/
/*3
 #include <Arduino.h>
 #include <avr/io.h>
 #include <FreeRTOS.h>
 #include <task.h>
 #define STACK_SIZE 200
 #define PIN_6 6

 void task1(void *p) {
 int onPeriod = 0;

 TickType_t xLastWakeUpTime = xTaskGetTickCount();
 for (;;) {
 digitalWrite(PIN_6, HIGH);
 onPeriod = (onPeriod + 1) % 20;
 vTaskDelayUntil(&xLastWakeUpTime, onPeriod);

 digitalWrite(PIN_6, LOW);
 vTaskDelayUntil(&xLastWakeUpTime, 20 - onPeriod);
 }
 }

 void setup() {
 pinMode(PIN_6, OUTPUT);
 }
 void loop() {
 xTaskCreate(task1, "Task1", STACK_SIZE, NULL, 1, NULL);
 vTaskStartScheduler();
 }
*/
/*4a
 #include <Arduino.h>
 #include <avr/io.h>
 #include <FreeRTOS.h>
 #include <task.h>
 #define STACK_SIZE 200
 #define PIN_6 6
 #define PIN_7 7
 #define PIN_8 8
 #define PIN_9 9

 void myDelay(int ms) {
 for (int i = 0; i < ms; i++) {
 delayMicroseconds(1000);
 }
 }

 void task1(void *p) {

 const TickType_t xPeriod = TickType_t(5000);
 TickType_t xLastWakeUpTime = xTaskGetTickCount();
 const int TASK_WCET = 1500;
 while (true) {
 int count = 0;
 while (count < TASK_WCET) {
 digitalWrite(PIN_6, HIGH);
 myDelay(50);
 digitalWrite(PIN_6, LOW);
 myDelay(50);
 count += 100;
 }
 vTaskDelayUntil(&xLastWakeUpTime, xPeriod);
 }
 }

 void task2(void *p) {
 const TickType_t xPeriod = TickType_t(10000);
 TickType_t xLastWakeUpTime = xTaskGetTickCount();
 const int TASK_WCET = 2500;
 while (true) {
 int count = 0;
 while (count < TASK_WCET) {
 digitalWrite(PIN_7, HIGH);
 myDelay(50);
 digitalWrite(PIN_7, LOW);
 myDelay(50);
 count += 100;
 }
 vTaskDelayUntil(&xLastWakeUpTime, xPeriod);
 }
 }

 void task3(void *p) {
 const int TASK_WCET = 1700;
 const TickType_t xPeriod = TickType_t(10000);
 TickType_t xLastWakeUpTime = xTaskGetTickCount();
 while (true) {
 int count = 0;
 while (count < TASK_WCET) {
 digitalWrite(PIN_8, HIGH);
 myDelay(50);
 digitalWrite(PIN_8, LOW);
 myDelay(50);
 count += 100;
 }
 vTaskDelayUntil(&xLastWakeUpTime, xPeriod);
 }
 }

 void task4(void *p) {
 const TickType_t xPeriod = TickType_t(20000);
 TickType_t xLastWakeUpTime = xTaskGetTickCount();
 const int TASK_WCET = 700;
 while (true) {
 int count = 0;
 while (count < TASK_WCET) {
 digitalWrite(PIN_9, HIGH);
 myDelay(50);
 digitalWrite(PIN_9, LOW);
 myDelay(50);
 count += 100;
 }
 vTaskDelayUntil(&xLastWakeUpTime, xPeriod);
 }
 }

 void setup() {
 pinMode(PIN_6, OUTPUT);
 pinMode(PIN_7, OUTPUT);
 pinMode(PIN_8, OUTPUT);
 pinMode(PIN_9, OUTPUT);
 }

 void loop() {
 xTaskCreate(task1, "Task1", STACK_SIZE, NULL, 4, NULL);
 xTaskCreate(task2, "Task2", STACK_SIZE, NULL, 3, NULL);
 xTaskCreate(task3, "Task3", STACK_SIZE, NULL, 2, NULL);
 xTaskCreate(task4, "Task4", STACK_SIZE, NULL, 1, NULL);
 vTaskStartScheduler();
 }
*/
/*4C

 #include <Arduino.h>
 #include <avr/io.h>
 #include <FreeRTOS.h>
 #include <task.h>
 #define STACK_SIZE 200
 #define PIN_6 6
 #define PIN_7 7
 #define PIN_8 8
 #define PIN_9 9

 void myDelay(int ms) {
 for (int i = 0; i < ms; i++) {
 delayMicroseconds(1000);
 }
 }

 void task1(void *p) {

 const TickType_t xPeriod = TickType_t(5000);
 TickType_t xLastWakeUpTime = xTaskGetTickCount();
 const int TASK_WCET = 1500;
 while (true) {
 int count = 0;
 while (count < TASK_WCET) {
 digitalWrite(PIN_6, HIGH);
 myDelay(50);
 digitalWrite(PIN_6, LOW);
 myDelay(50);
 count += 100;
 }
 vTaskDelayUntil(&xLastWakeUpTime, xPeriod);
 }
 }

 void task2(void *p) {
 const TickType_t xPeriod = TickType_t(10000);
 TickType_t xLastWakeUpTime = xTaskGetTickCount();
 const int TASK_WCET = 2500;
 while (true) {
 int count = 0;
 while (count < TASK_WCET) {
 digitalWrite(PIN_7, HIGH);
 myDelay(50);
 digitalWrite(PIN_7, LOW);
 myDelay(50);
 count += 100;
 }
 vTaskDelayUntil(&xLastWakeUpTime, xPeriod);
 }
 }

 void task3(void *p) {
 const int TASK_WCET = 1700;
 const TickType_t xPeriod = TickType_t(10000);
 TickType_t xLastWakeUpTime = xTaskGetTickCount();
 while (true) {
 int count = 0;
 while (count < TASK_WCET) {
 digitalWrite(PIN_8, HIGH);
 myDelay(50);
 digitalWrite(PIN_8, LOW);
 myDelay(50);
 count += 100;
 }
 vTaskDelayUntil(&xLastWakeUpTime, xPeriod);
 }
 }

 void task4(void *p) {
 const TickType_t xPeriod = TickType_t(20000);
 TickType_t xLastWakeUpTime = xTaskGetTickCount();
 const int TASK_WCET = 700;
 while (true) {
 int count = 0;
 while (count < TASK_WCET) {
 digitalWrite(PIN_9, HIGH);
 myDelay(50);
 digitalWrite(PIN_9, LOW);
 myDelay(50);
 count += 100;
 }
 vTaskDelayUntil(&xLastWakeUpTime, xPeriod);
 }
 }

 void setup() {
 pinMode(PIN_6, OUTPUT);
 pinMode(PIN_7, OUTPUT);
 pinMode(PIN_8, OUTPUT);
 pinMode(PIN_9, OUTPUT);
 }

 void loop() {
 xTaskCreate(task1, "Task1", STACK_SIZE, NULL, 3, NULL);
 xTaskCreate(task2, "Task2", STACK_SIZE, NULL, 2, NULL);
 xTaskCreate(task3, "Task3", STACK_SIZE, NULL, 2, NULL);
 xTaskCreate(task4, "Task4", STACK_SIZE, NULL, 1, NULL);
 vTaskStartScheduler();
 }
*/

/*5B*/

#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#define STACK_SIZE 200
#define PIN_6 6
#define PIN_7 7
#define PIN_8 8
#define PIN_9 9

void myDelay(int ms) {
	for (int i = 0; i < ms; i++) {
		delayMicroseconds(1000);
	}
}

void function1() {
	int count = 0;
	const int TASK_WCET = 1500;
	while (count < TASK_WCET) {
		digitalWrite(PIN_6, HIGH);
		myDelay(50);
		digitalWrite(PIN_6, LOW);
		myDelay(50);
		count += 100;
	}
}

void function2() {
	const int TASK_WCET = 2500;
	int count = 0;
	while (count < TASK_WCET) {
		digitalWrite(PIN_7, HIGH);
		myDelay(50);
		digitalWrite(PIN_7, LOW);
		myDelay(50);
		count += 100;
	}
}

void function3() {
	const int TASK_WCET = 1700;
	int count = 0;
	while (count < TASK_WCET) {
		digitalWrite(PIN_8, HIGH);
		myDelay(50);
		digitalWrite(PIN_8, LOW);
		myDelay(50);
		count += 100;
	}
}

void function4() {
	const int TASK_WCET = 700;
	int count = 0;
	while (count < TASK_WCET) {
		digitalWrite(PIN_9, HIGH);
		myDelay(50);
		digitalWrite(PIN_9, LOW);
		myDelay(50);
		count += 100;
	}
}

void task1(void *p) {
	TickType_t xLastWakeUpTime = xTaskGetTickCount();
	while (1) {
		function1();
		function2();
		function4();
		vTaskDelayUntil(&xLastWakeUpTime, 5000);
		function1();
		function3();
		vTaskDelayUntil(&xLastWakeUpTime, 5000);
		function1();
		function2();
		vTaskDelayUntil(&xLastWakeUpTime, 5000);
		function1();
		function3();
		vTaskDelayUntil(&xLastWakeUpTime, 5000);
	}
}
void setup() {
	pinMode(PIN_6, OUTPUT);
	pinMode(PIN_7, OUTPUT);
	pinMode(PIN_8, OUTPUT);
	pinMode(PIN_9, OUTPUT);
}
void loop() {
	xTaskCreate(task1, "Task1", STACK_SIZE, NULL, 1, NULL);
	vTaskStartScheduler();
}

