#ifndef __REGTARGE_H
#define __REGTARGE_H

#include <rt_misc.h>
#include "stm32f10x.h"
#pragma import(__use_no_semihosting_swi)//��ʹ�ð�����ģʽ

#ifdef __cplusplus
 extern "C" {
#endif
#include <stdio.h>
int  SendChar(int ch); // �����ⲿ��������main�ļ��ж���

int  GetKey(void);


struct __FILE {

  int handle;                 // Add whatever you need here 

	} ;

#ifdef __cplusplus
}
#endif
#endif
