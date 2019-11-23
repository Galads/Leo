#include "stm32f10x.h"          										        // Device header

volatile char flags = 0;

void delay(uint32_t takts)
{
	uint32_t i;
	
	for (i = 0; i < takts; i++) {};
}

void gpioInitIntup(){
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;	// cnf mode 10 00 
	
	GPIOC->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);					// (11 01 10) - CNFx & MODx config
	GPIOC->CRL |= GPIO_CRL_CNF0_1;
	GPIOC->BSRR |= GPIO_BSRR_BS0;
	
	GPIOC->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1);					// (11 01 10) - CNFx & MODx config
	GPIOC->CRL |= GPIO_CRL_CNF1_1;
	GPIOC->BSRR |= GPIO_BSRR_BS1;
//----------------------------------------------------------------------------------
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PC; 								// EXTI CONFIGURE PC0
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PC; 								// EXTI CONFIGURE PC1  //  EXTICR[0] - write in exticr1 register / array numeric
	
	EXTI->IMR |= (EXTI_IMR_MR0 | EXTI_IMR_MR1);               // MASK EN
	
	EXTI->FTSR |= EXTI_FTSR_TR0;
	EXTI->FTSR |= EXTI_FTSR_TR1;	
	
	
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_SetPriority(EXTI0_IRQn,0);
	
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_SetPriority(EXTI1_IRQn,0);	
}

void tim3Enable(f){
	AFIO->MAPR &= ~AFIO_MAPR_TIM3_REMAP;
	AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_FULLREMAP;
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	GPIOC->CRH &= ~GPIO_CRH_CNF9;	
	GPIOC->CRH &= ~GPIO_CRH_MODE9;
		
	GPIOC->CRH |= GPIO_CRH_CNF9_1;    										   // ALTERNATE FUNC PUSH-PULL -> CNF = 10 |  MODE >00 (11)
	GPIOC->CRH |= GPIO_CRH_MODE9;       										 // 11
	
	TIM3->PSC = 24-1;  																	     // 24 000 000 / 24 = 1 MHZ
	TIM3->ARR = 160;                                         // auto-reload reg
	TIM3->CCR4 = f;									 										     // FILLING FACTOR (50%)   100
	
	TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;      // CONFIG POLARITY = 110
	TIM3->CCER |= TIM_CCER_CC4E;                             // подача сигнала ШИМ на выход внутри таймера    1 - включили 0 - выключили
	TIM3->CCER &= ~TIM_CCER_CC4P;                            // output Polarity     logic = 0 == activ high level
	
	TIM3->CR1 &= ~TIM_CR1_DIR;                               // count up if cr1 bit == 0; - its default
	TIM3->CR1 |= TIM_CR1_CEN;																 // ENABLE TIMER
}

void EXTI1_IRQHandler(void){
		 delay(1000000);
		 EXTI->PR |= EXTI_PR_PR1;
	   flags--;
	
		if(flags == 0){
	    tim3Enable(0);
			return;
	}
		if(flags == 1){
	    tim3Enable(10);
			return;
	}
		if(flags == 2){
	    tim3Enable(20);
			return;
	}
		if(flags == 3){
	    tim3Enable(40);
			return;
	}
		if(flags == 4){
	    tim3Enable(80);
			return;
	}
		if(flags >= 5){														// if 0-- => 0xFF
			flags=0;
			return;
	}			
}

void EXTI0_IRQHandler(void){
		 delay(1000000);
		 EXTI->PR |= EXTI_PR_PR1;
     flags++;
	
		if(flags == 1){
	    tim3Enable(10);
			return;
	}
		if(flags == 2){
	    tim3Enable(20);
			return;
	}
		if(flags == 3){
	    tim3Enable(40);
			return;
	}
		if(flags == 4){
	    tim3Enable(80);
			return;
	}
		if(flags >= 5){
	    tim3Enable(160);
		  flags = 5;
			return;
	}		   
}

