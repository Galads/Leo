#include "stm32f10x.h" // Device header	

void gpioInit(void);
void InitEncoder(void);

unsigned char table[] = {           //3 -tochka
			 0x77,	      //0
       0x14,        //1
			 0xB3,        //2 
			 0xB6,        //3
	     0xD4,        //4
			 0xE6,        //5
	     0xE7,        //6
	     0x34,        //7
			 0xF7,        //8
			 0xF6,        //9
	     0xF5,        //A
	     0xC7,        //b
		   0x63,        //C
			 0x97,        //d
			 0xE3,        //E
			 0xE1,        //F
};

int main(){
		uint16_t a = 0;
		gpioInit();
	  InitEncoder();
	
	while(1){
		      if(TIM3->CNT < 16){
						TIM3->CNT = 16;
					}
		      if(TIM3->CNT == 16){
						GPIOC->ODR &= 0x0;		
						GPIOC->ODR |= table[0];
					}
					
					if (TIM3->CNT == 16*2){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[1];
					}
					if (TIM3->CNT == 16*3){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[2];
					}
					if (TIM3->CNT == 16*4){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[3];
					}
				  if (TIM3->CNT == 16*5){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[4];
					}
				  if (TIM3->CNT == 16*6){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[5];
					}
				  if (TIM3->CNT == 16*7 ){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[6];
					}
				  if (TIM3->CNT == 16*8 ){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[7];
					}
				  if (TIM3->CNT == 16*9 ){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[8];
					}
				  if (TIM3->CNT == 16*10 ){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[9];
					}
				  if (TIM3->CNT == 16*11 ){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[10];
					}
				  if (TIM3->CNT == 16*12 ){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[11];
					}
				  if (TIM3->CNT == 16*13 ){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[12];
					}
				  if (TIM3->CNT == 16*14 ){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[13];
					}
				  if (TIM3->CNT == 16*15 ){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[14];
					}
				  if (TIM3->CNT == 255 ){
						GPIOC->ODR &= 0x0;	
						GPIOC->ODR |= table[15];
					}
					if (TIM3->CNT > 255){
					   TIM3->CNT = 255;
		}
	}
}
