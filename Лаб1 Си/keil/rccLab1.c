#include "stm32f10x.h" // Device header	

void gpioInit(void);
void softPwmButt(void);

int main(){
		
		gpioInit();
	
	while(1){
		
		softPwmButt();
		
	}
}
