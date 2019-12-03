#ifndef __REGTARGE_H
#define __REGTARGE_H

#include <rt_misc.h>
#include "stm32f10x.h"
#pragma import(__use_no_semihosting_swi)//不使用半主机模式

#ifdef __cplusplus
 extern "C" {
#endif
#include <stdio.h>
int  SendChar(int ch); // 声明外部函数，在main文件中定义

int  GetKey(void);


struct __FILE {

  int handle;                 // Add whatever you need here 

	} ;

#ifdef __cplusplus
}
#endif
#endif
