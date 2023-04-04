/*
 * Copyright (c) 2022 
 * Computer Science and Engineering, University of Dhaka
 * Credit: CSE Batch 25 (starter) and Prof. Mosaddek Tushar
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
 

#include <kmain.h>

#include <schedule.h>
#define stop 10000000
#define STOP 10000000

TCB_TypeDef task[10], _sleep;

int global_count = 0;

void task_sleep(void)
{
	int reboot_ = 0;
	uprintf("\n\r\n\rAll tasks completed.\n\r\n\r\tEnter 1 to reboot. \n\r\tEnter any other number to enter sleeping state.\n\r");
	uscanf("%d", &reboot_);
	if(reboot_ == 1) reboot();
	
	uprintf("Entering sleep...\n\r");
	while(1);
}


void task_func(void) {
	uint32_t value;
	uint32_t inc_count=0;
	while(1){
		value=global_count;
		value++;
		if(value != global_count+1){ //we check is someother task(s) increase the count
			uprintf("Error %d != %d\n\r",value,global_count+1); /* It is an SVC call*/
		} else{
			global_count=value;
			inc_count++;
		}
		if(global_count >= stop){
			uint16_t task_id = getpid(); /* It is an SVC call*/
			// /* display how many increments it has successfully done!! */
			uprintf("Total increment done by task %d is: %d\n\r",task_id,inc_count);
			// uprintf("Total increment done by task is: %d\n\r",inc_count);
			/* above is an SVC call */
			break;
		}
	}
	exit();
	// yield();
}


void task_func2(void) {
	uint32_t value;
	uint32_t inc_count=0;
	while(1){
		value=global_count;
		value++;
		if(value != global_count+1){ //we check is someother task(s) increase the count
			uprintf("Error %d != %d\n\r",value,global_count+1); /* It is an SVC call*/
		} else{
			global_count=value;
			inc_count++;
		}
		if(global_count >= stop){
			uint16_t task_id = getpid(); /* It is an SVC call*/
			// /* display how many increments it has successfully done!! */
			uprintf("Total increment done by task %d is: %d\n\r",task_id,inc_count);
			// uprintf("Total increment done by task is: %d\n\r",inc_count);
			/* above is an SVC call */
			break;
		}
	}
	exit();
	// yield();
}

void task_func3(void) {
	uint32_t value;
	uint32_t inc_count=0;
	while(1){
		value=global_count;
		value++;
		if(value != global_count+1){ //we check is someother task(s) increase the count
			uprintf("Error %d != %d\n\r",value,global_count+1); /* It is an SVC call*/
		} else{
			global_count=value;
			inc_count++;
		}
		if(global_count >= stop){
			uint16_t task_id = getpid(); /* It is an SVC call*/
			/* display how many increments it has successfully done!! */
			uprintf("Total increment done by task %d is: %d\n\r",task_id,inc_count);
			// uprintf("Total increment done by task is: %d\n\r",inc_count);
			/* above is an SVC call */
			break;
		}
	}
	exit();
	// yield();
}




void unpriv (void) {
	__asm volatile ("MRS R0, CONTROL");
	__asm volatile ("ORRS R0, R0, #1");
	__asm volatile ("MSR CONTROL, R0");
}

void __set_interrupt_priorities(void)
{
	__NVIC_SetPriority(SVCall_IRQn, 1);
	__NVIC_SetPriority(SysTick_IRQn, 0x2);
	__NVIC_SetPriority(PendSV_IRQn, 0xFF); // lowest possible priority
}

void kmain(void)
{
	__sys_init();
	__set_interrupt_priorities();
	
	// creating tasks
	create_task(task, task_func, (uint32_t*)TASK_STACK_START);
	create_task(task + 1, task_func2, (uint32_t*)(TASK_STACK_START - TASK_STACK_SIZE) );
	create_task(task + 2, task_func3, (uint32_t*)(TASK_STACK_START - (2 * TASK_STACK_SIZE) ) );
	create_task(&_sleep, task_sleep, (uint32_t*)(TASK_STACK_START - (3 * TASK_STACK_SIZE) ));

	initialize_queue();
	

	add_to_queue(task);
	add_to_queue(task + 1);
	add_to_queue(task + 2);
	// add_to_queue(&_sleep);
	set_sleeping_task(&_sleep);

	
	unpriv();
	set_task_pending(1);

	start_task();
	uprintf("\n\r\tall tasks done\n\r");

	while(1);
}



