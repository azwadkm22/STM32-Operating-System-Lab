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

#include <syscall.h>
#include <syscall_def.h>
#include <errno.h>
#include <errmsg.h>
#include <kstdio.h>
#include <usart.h>
#include <cm4.h>
#include <types.h>

void __sys_start_task(void)
{
	__asm volatile ("POP {LR}");

	unsigned int * svc_args;
	__asm volatile ("MOV %0, R1" : "=r" (svc_args));
		uint32_t task_psp = svc_args[3];
		// for(int i = 0; i < 8; i++) {
		// 	kprintf("svcarg[%d] = %x\n\r", i, svc_args[i]);
	
	__asm volatile ("MOV R0, %0": :"r"(task_psp));
	
	
	__asm volatile ("LDMIA R0!,{R4-R11}");
	__asm volatile ("MSR PSP, R0");
	// __asm volatile ("MRS R1, CONTROL");
	// __asm volatile ("ORRS R1, R1, #0x1"); //unprivileged
	// __asm volatile ("MSR CONTROL, R1");
	__asm volatile ("ISB");

	
	
	__asm volatile ("MOV LR, 0xFFFFFFFD");
	__asm volatile ("BX LR");
}


void __sys_yield(void)
{
    // kprintf("Yield called");
    SCB->ICSR |= (1 << 28);
}

void __sys_getpid(void)
{
	unsigned int * svc_args;
	__asm volatile ("MOV %0, R1" : "=r" (svc_args));
	*( (unsigned int *) svc_args[0] ) = (*((TCB_TypeDef*)svc_args[4])).task_id;
	return;
}

//  sys (· · ·)
// setTaskStatus
void __sys_exit(void)
{
	unsigned int * svc_args;
	__asm volatile ("MOV %0, R1" : "=r" (svc_args));
	
	(*((TCB_TypeDef*)svc_args[4])).status = 4;
	return;
}

void __sys_gettime(void)
{
	unsigned int * svc_args;
	__asm volatile ("MOV %0, R1" : "=r" (svc_args) : );
	*( (unsigned int *) svc_args[1] ) = __getTime(); //R1
}

void __sys_reboot(void) 
{
	kprintf("rebooting...");
	SCB->AIRCR = (0x05FA << 16) | (1 << 2);

	while(1);
}

void __sys_write(void) 
{
	unsigned char *s;
	unsigned int * svc_args;
	__asm volatile ("MOV %0, R1" : "=r" (svc_args) : );
	s = (unsigned char *)svc_args[1]; //R1
	int len = _USART_WRITE(USART2, s);
	*((int*)svc_args[4]) = len;
	return;
}

void __sys_read(void)
{
	unsigned int * svc_args;
	__asm volatile ("MOV %0, R1" : "=r" (svc_args) : );

	int *bytes_read = (int *)svc_args[4]; //R12
	int len = (int)svc_args[3]; //R3
	unsigned char* buff = (unsigned char*)svc_args[2];
	
	if(len == 1) 
	{
		buff[0] = UART_GetChar(USART2);
		*bytes_read = 1;
	} 
	else
	{
		*bytes_read = _USART_READ_STR(USART2,buff,50);
	}
	return;
}

void syscall(uint16_t callno)
{
/* The SVC_Handler calls this function to evaluate and execute the actual function */
/* Take care of return value or code */
	switch(callno)
	{
		/* Write your code to call actual function (kunistd.h/c or times.h/c and handle the return value(s) */
		case SYS_read: 
			__sys_read();
			break;
		case SYS_write:
			__sys_write();
			break;
		case SYS_reboot:
			__sys_reboot();
			break;	
		case SYS__exit:
			__sys_exit();
			break;
		case SYS_getpid:
			__sys_getpid();
			break;
		case SYS___time:
			__sys_gettime();
			break;
		case SYS_yield:
			__sys_yield();
			break;	
		case SYS_start_task:
			__sys_start_task();
			break;			
		/* return error code see error.h and errmsg.h ENOSYS sys_errlist[ENOSYS]*/	
		default: 
			// return some negative value in r0
			break;
	}
	
	__asm volatile ("POP {LR}");
/* Handle SVC return here */
}

