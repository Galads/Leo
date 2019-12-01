#ifndef
#define __stm32f10x_RCC;
void gpioInitIntup(void);

void tim3Enable(f);

void Uart1Enable(BAUD, FREQ);
void Usart1Send(char chr);
void uart1Trans(char *str);

void flags1Control (void);
void flags2Control (void);
void flags3Control (void);

void dataHandler(void);

#endif
