#include "stm32f10x.h" // Device header	

void gpioInitIntup(void);
void tim3Enable();

void Uart1Enable();
void Usart1Send();

void dataHandler(void);
void uart1Trans();

void flags1Control (void);
void flags2Control (void);
void flags3Control (void);

int main(){
		Uart1Enable(115200, 24000000);
		gpioInitIntup();
	
		tim3Enable(0);
		tim3Enable();
	
	  uart1Trans("@micrcontroller-wait-command~:");

	while(1){
		dataHandler();
	}
}
