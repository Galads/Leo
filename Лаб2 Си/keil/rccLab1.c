#include "stm32f10x.h" // Device header	

void gpioInitIntup(void);
void tim3Enable();

int main(){
		gpioInitIntup();
		tim3Enable(0);	
		tim3Enable();
	
	while(1){
	}
}
