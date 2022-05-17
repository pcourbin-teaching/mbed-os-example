/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
 // https://os.mbed.com/docs/mbed-os/v6.15/apis/eventqueue.html
 /*

Quelques idées de choses à évoquer éventuellement :
- Création d'un simple projet "blink" sur mbed IDE pour la DISCO-F429ZI et les 2 LEDs.
- Configuration d'un nouveau projet utilisant une installation de mbed-os existante (pour réduire la taille des projets)
- Utilisation de InterruptIn pour l'usage du bouton avec fonction sur rise et fall.
- Découverte du fonctionnement des queue d'exécution pour un lancement régulier de tâche
- Découverte du fonctionnement de plusieurs threads avec priorités différentes
- Utlisation d'une librairie externe, par exemple pour servo moteur (installation, import, utilisation). https://os.mbed.com/users/simon/code/Servo/
- Envoie du code sur un github pour les versions et la collaboration.
 */

#include "mbed.h"
#include "Servo/Servo.h"

// Initialise the digital pin LED1 as an output
DigitalOut L0(LED1);
DigitalOut L1(LED2);
InterruptIn button(BUTTON1);
Servo myservo(PC_7);

Thread thread_leds(osPriorityNormal1);
EventQueue queue_leds(32 * EVENTS_EVENT_SIZE);;

Thread thread_button(osPriorityNormal2);
EventQueue queue_button(32 * EVENTS_EVENT_SIZE);;

Thread thread_servo(osPriorityNormal3);
EventQueue queue_servo(32 * EVENTS_EVENT_SIZE);;

void button_rise(void)
{
    queue_button.call(printf, "Button Rise in context %p\n", ThisThread::get_id());
}

void button_fall(void)
{
    queue_button.call(printf, "Button Fall in context %p\n", ThisThread::get_id());
}

void led0()
{
   L0 = ! L0;
}
 
void led1()
{
   L1 = ! L1;
}

void servo(){
    myservo.position(-60);
    ThisThread::sleep_for(200ms);
    myservo.position(60);
    ThisThread::sleep_for(200ms);

    // Use CPU to test priority and preemption
    for (int i=0; i<10000; i++){
        wait_us(200);
    }
}

int main()
{
    queue_leds.call_every(1s, led0);
    queue_leds.call_every(2s, led1);

    button.rise(button_rise);
    button.fall(button_fall);

    queue_servo.call_every(5s, servo);

    printf("Starting event_queue...\n\r");
    thread_leds.start(callback(&queue_leds, &EventQueue::dispatch_forever));
    thread_button.start(callback(&queue_button, &EventQueue::dispatch_forever));
    thread_servo.start(callback(&queue_servo, &EventQueue::dispatch_forever));
}
