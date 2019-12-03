#include "redirect.h"

FILE __stdout,__stdin;


int fputc(int ch, FILE *f) {

  return (SendChar(ch));

}

int fgetc(FILE *f) {

  return (SendChar(GetKey()));

}

void _ttywrch(int ch) {

 SendChar (ch);

}

int ferror(FILE *f) {                            // Your implementation of ferror

  return EOF;

}

void _sys_exit(int return_code) {

label:  goto label;           // endless loop

}

USART_TypeDef* USART_TEST= USART2;
int SendChar (int ch)  {

  while (!(USART_TEST->SR & USART_FLAG_TXE)); // USART1 可换成你程序中通信的串口

  USART_TEST->DR = (ch & 0x1FF);

  return (ch);

}

int GetKey (void)  {

  while (!(USART_TEST->SR & USART_FLAG_RXNE));

  return ((int)(USART_TEST->DR & 0x1FF));

}


