/*
  ******************************************************************************
  *
  * @file    rccLab1.c
  * @file    stm32f10x_RCC.h
  * @file    stm32f10x_RCC.C
  * @author  Shumkin Nikolay
  * @email   nikola_2212@mail.ru
  * @version V1.0.0
  * @date    20.10.2019
  * @brief   Software PWM with button
  *			 
  *		Debug Board: STM32VLDISCOVERY 
  *
  ******************************************************************************
*/
#include "stm32f10x.h"          										        // Device header

unsigned char flags = 0;

void gpioInit(){
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;											  // cnf mode 10 00 
	
	GPIOC->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);					//(11 01 10) - CNFx & MODx config
	GPIOC->CRL |= GPIO_CRL_CNF0_1;
	
	GPIOC->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1);					//(11 01 10) - CNFx & MODx config
	GPIOC->CRL |= GPIO_CRL_CNF1_1;
	
	GPIOC->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9);
	GPIOC->CRH |= GPIO_CRH_MODE9_1;													 //2mhz output configure
}

void softPwmButt(){
	
	if(((GPIOC->IDR & GPIO_IDR_IDR0) == 0)||((GPIOC->IDR & GPIO_IDR_IDR1) == 0) ){
		for (uint32_t i = 0; i<1800000; i++);
	
		if((GPIOC->IDR & GPIO_IDR_IDR1) == GPIO_IDR_IDR1){
			if(flags == 0){
				while(1){
				
					GPIOC->BSRR = GPIO_BSRR_BR9;
					
					if((GPIOC->IDR & GPIO_IDR_IDR0) == 0){
						flags = 1;
						return;
						}
					}
			  }
			}
	
		flags++;
		
		if((GPIOC->IDR & GPIO_IDR_IDR0) == GPIO_IDR_IDR0){
			if(flags == 2){
				while(1){
				
					for(uint32_t i = 0; i<10000; i++); 
					GPIOC->BSRR = GPIO_BSRR_BS9; 
					for(uint32_t i = 0; i<1000; i++); 
					GPIOC->BSRR = GPIO_BSRR_BR9;
					
					if((GPIOC->IDR & GPIO_IDR_IDR0) == 0){
						
						for(uint32_t i = 0; i<1000; i++); 
						return;
						}
					
					if((GPIOC->IDR & GPIO_IDR_IDR1) == 0){
						flags = 0;
						for(uint32_t i = 0; i<1000; i++); 
						return;
						}
					}
				}
			if(flags == 3){
				while(1){
					
					for(uint32_t i = 0; i<5000; i++); 
					GPIOC->BSRR = GPIO_BSRR_BS9; 
					for(uint32_t i = 0; i<1000; i++); 
					GPIOC->BSRR = GPIO_BSRR_BR9;
					
					if((GPIOC->IDR & GPIO_IDR_IDR0) == 0){
						
						for(uint32_t i = 0; i<1000; i++); 
						return;
						}
					
					if((GPIOC->IDR & GPIO_IDR_IDR1) == 0){
						flags = 1;
						for(uint32_t i = 0; i<1000; i++); 
						return;
						}
					}
				}
			if(flags == 4){
				while(1){
					
					for(uint32_t i = 0; i<2000; i++); 
					GPIOC->BSRR = GPIO_BSRR_BS9; 
					for(uint32_t i = 0; i<1000; i++); 
					GPIOC->BSRR = GPIO_BSRR_BR9;
					
					if((GPIOC->IDR & GPIO_IDR_IDR0) == 0){
						
						for(uint32_t i = 0; i<1000; i++); 
						return;
						}
					
					if((GPIOC->IDR & GPIO_IDR_IDR1) == 0){
						flags = 2;
						for(uint32_t i = 0; i<1000; i++); 
						return;
						}
					}
				}
			if(flags == 5){
				while(1){
					
					for(uint32_t i = 0; i<1000; i++); 
					GPIOC->BSRR = GPIO_BSRR_BS9; 
					for(uint32_t i = 0; i<1000; i++); 
					GPIOC->BSRR = GPIO_BSRR_BR9;
					
					if((GPIOC->IDR & GPIO_IDR_IDR0) == 0){
						for(uint32_t i = 0; i<1000; i++); 
						return;
						}
					
					if((GPIOC->IDR & GPIO_IDR_IDR1) == 0){
						flags = 3;
						for(uint32_t i = 0; i<1000; i++); 
						return;
						}
					}
				}
		 			if(flags >= 6){
				while(1){
					
					GPIOC->BSRR = GPIO_BSRR_BS9;
					
					if((GPIOC->IDR & GPIO_IDR_IDR0) == 0){
						for(uint32_t i = 0; i<1000; i++); 
						return;
						}
					
					if((GPIOC->IDR & GPIO_IDR_IDR1) == 0){
						flags = 4;
						for(uint32_t i = 0; i<1000; i++); 
						return;
					}
				}
			}
		}
	}
}

