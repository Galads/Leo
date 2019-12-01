#include "stm32f10x.h"          										        // Device header
#include "string.h"
#include "stdbool.h"

#define BUFF_SIZE   256

unsigned char flagh = 0;
volatile char flags = 0;
bool end;
char RXBUFFER [BUFF_SIZE];

void delay(uint32_t takts)
{
	uint32_t i;
	
	for (i = 0; i < takts; i++) {};
}



void gpioInitIntup(){
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;	// cnf mode 10 00 
	
	GPIOC->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);					// (11 01 10) - CNFx & MODx config
	GPIOC->CRL |= GPIO_CRL_CNF0_1;                            //10
	GPIOC->BSRR |= GPIO_BSRR_BS0;
	
	GPIOC->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1);					// (11 01 10) - CNFx & MODx config
	GPIOC->CRL |= GPIO_CRL_CNF1_1;
	GPIOC->BSRR |= GPIO_BSRR_BS1;
//----------------------------------------------------------------------------------
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PC; 								// EXTI CONFIGURE PC0
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PC; 								// EXTI CONFIGURE PC1  //  EXTICR[0] - write in exticr1 register / array numeric
	
	EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR1;                 // MASK EN
	
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
	
	TIM3->CR1 &= ~TIM_CR1_DIR;                               // count up if cr1 bit == 0; - its default  NIRECTION COUNT
	TIM3->CR1 |= TIM_CR1_CEN;																 // ENABLE TIMER
}

void Uart1Enable(BAUD, FREQ){ 
	uint16_t BRRX = 0;

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN \
																			 | RCC_APB2ENR_AFIOEN;
	
	GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
	GPIOA->CRH |= GPIO_CRH_MODE9;         							  	    //11
	GPIOA->CRH |= GPIO_CRH_CNF9_1;                              //10     ==>10 11      PA9 ALTERNATIV FUCTION PUSH-PULL  OUT TX

	GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);         
	GPIOA->CRH |= GPIO_CRH_CNF10_0;                       	    //01     ==> 01 00     FLOATING INPUT     RX


	BRRX |= ((FREQ + (BAUD/2))/BAUD);                           // 208.83   ==       D0

	USART1->BRR = BRRX;

	USART1->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;  // RX | TX | USART == ENABLE

	USART1->CR1 |= USART_CR1_RXNEIE;													  // INTERRUP PO PRIEM BYTE ENABLE
	NVIC_EnableIRQ(USART1_IRQn);                             
}

void Usart1Send(char chr){
	while (!(USART1->SR & USART_SR_TC));
	USART1->DR = chr;
}

void uart1Trans(char *str){
	uint8_t i = 0;
	
	while (str[i])
		Usart1Send(str[i++]);
	
	while(!(USART1->SR & USART_SR_TC));  		// == 0
		USART1->DR = '\r';
}

void USART1_IRQHandler(void)
{
	char chr;
	if (USART1->SR & USART_SR_RXNE) 			// считываем значения с не пустого статус регистра	
	{
		chr = USART1->DR;
		
		if (chr == 0x0D){	
			end = true;
			return;	
			}	
		RXBUFFER[strlen(RXBUFFER)] = chr; 		// помещаем размер строки в рессивер буфер
	} 	
}

void flags1Control (void){
	switch (flagh){  
		case 1:	tim3Enable(16*1);  flags = 1; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 2:	tim3Enable(16*2);  flags = 2; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 3:	tim3Enable(16*3);  flags = 3; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 4:	tim3Enable(16*4);  flags = 4; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 5:	tim3Enable(16*5);  flags = 5; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 6:	tim3Enable(16*6);  flags = 6; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 7:	tim3Enable(16*7);  flags = 7; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 8:	tim3Enable(16*8);  flags = 8; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 9:	tim3Enable(16*9);  flags = 9; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 10:	tim3Enable(16*10);  flags = 10; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		default: {flagh = 10; flags =10; uart1Trans("  |"); uart1Trans("   -> ERROR");}
	}
}

void flags2Control (void){
	switch (flagh){  
		case 0:	tim3Enable(16*0);  flags = 0; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 1:	tim3Enable(16*1);  flags = 1; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 2:	tim3Enable(16*2);  flags = 2; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 3:	tim3Enable(16*3);  flags = 3; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 4:	tim3Enable(16*4);  flags = 4; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 5:	tim3Enable(16*5);  flags = 5; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 6:	tim3Enable(16*6);  flags = 6; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 7:	tim3Enable(16*7);  flags = 7; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 8:	tim3Enable(16*8);  flags = 8; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 9:	tim3Enable(16*9);  flags = 9; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		case 10:	tim3Enable(16*10);  flags = 10; uart1Trans("  |"); uart1Trans("   -> OK"); break;
		default: {flagh = 0; flags = 0; uart1Trans("  |"); uart1Trans("   -> ERROR");}
	}
}

void flags3Control (void){
	switch (flags) {
		case 0: {uart1Trans("  |"); uart1Trans("   -> BR 0");} break;
		case 1: {uart1Trans("  |"); uart1Trans("   -> BR 1");} break;
		case 2: {uart1Trans("  |"); uart1Trans("   -> BR 2");} break;
		case 3: {uart1Trans("  |"); uart1Trans("   -> BR 3");} break;
		case 4: {uart1Trans("  |"); uart1Trans("   -> BR 4");} break;
		case 5: {uart1Trans("  |"); uart1Trans("   -> BR 5");} break;
		case 6: {uart1Trans("  |"); uart1Trans("   -> BR 6");} break;
		case 7: {uart1Trans("  |"); uart1Trans("   -> BR 7");} break;
		case 8: {uart1Trans("  |"); uart1Trans("   -> BR 8");} break;
		case 9: {uart1Trans("  |"); uart1Trans("   -> BR 9");} break;
		case 10: {uart1Trans("  |"); uart1Trans("   -> BR 10");} break;
	}
}

void dataHandler(void){
		if(end == true)	{
			end = false;
			if ((strcmp(RXBUFFER, "*IDN?"))== false){
				 uart1Trans("  |");
	       uart1Trans("   -> Shumkin Nikolay IU4-71");
			}
			else if ((strcmp(RXBUFFER, "COMMAND?"))== false){
				 uart1Trans("  |");
         uart1Trans("   IDN? - name and group author");
				 uart1Trans("   BR0...10 - Led on pwm 0...10");
				 uart1Trans("   BR ? - Search BR in present time");
				 uart1Trans("   BR INC - Led increment");
				 uart1Trans("   BR DEC - Led decrement");	
				 uart1Trans("   BOB - Secrets");
			}
			else if ((strcmp(RXBUFFER, "BOB")) == false){
				uart1Trans("  |");
				uart1Trans("0000000000000000000000000000000000001111111111100000000000000000");
				uart1Trans("0000000000000000000000000000000001111111111111111111100000000000");
				uart1Trans("0000000000000000000000000000000111111111111111111111111100000000");
				uart1Trans("0000000000000000000000000000011111111111111111111111111111000000");
				uart1Trans("0000000000000000000000000000011111111111111111111111111111110000");
				uart1Trans("0000000000000000000000000000111111111111111111111111111111111100");
				uart1Trans("0000000000000000000000000000111111111111111111111111111111111100");
				uart1Trans("0000000000000000000000000000111111111111111111111111111111111100");
				uart1Trans("0000000000000000000000000000111111111111111111111111111111111110");
				uart1Trans("0000000000000000000000000000111010111111111111111100000000001100");
				uart1Trans("0000000000000000000000000000110000000000011111110000000000000000");
				uart1Trans("0000000000000000000000000000100000000000001111100000000000000000");
				uart1Trans("0000000000000000000000000000100110000000001111100000000000000000");
				uart1Trans("0000000000000000000000000000111000000000001111100000000000000000");
				uart1Trans("0000000000000000000000000000111000000111111111100001000000000000");
				uart1Trans("0000000000000000000000000000110001111111111111100111110000000000");
				uart1Trans("0000000000000000000000000001111001111111111111000011111000000000");
				uart1Trans("0000000000000000000000000001111111111111111111000111111100000000");
				uart1Trans("0000000000000000000000000001111111111111111111111111111110100100");
				uart1Trans("0000000000000000000000000001111111111111111111101111111111111100");
				uart1Trans("0000000000000000000000000001111111111111111111111111111111111100");
				uart1Trans("0000000000000000000000000001111111111111111111111111111111111000");
				uart1Trans("0000000000000000000000000001111111111111111111111111111111111000");
				uart1Trans("0000000000000000000000000001111111111111000110000011111111100000");
				uart1Trans("0000000000000000000000000001111111111111000000000001111111000000");
				uart1Trans("0000000000000000000000000000111111111111111101100001111110000000");
				uart1Trans("0000000000000000000000000000111111111111111111100111111100000000");
				uart1Trans("0000000000000000000000000000111111111111111111101111111100000000");
				uart1Trans("0000000000000000000000000000011111111111111111000111111100000000");
				uart1Trans("0000000000000000000000000000011111111111101111000001111110000000");
				uart1Trans("0000000000000000000000000000011111111111111111100000001110000000");
				uart1Trans("0000000000000000000000000000001111111001111100000000000100000000");
				uart1Trans("0000000000000000000000000000001111100111000000000000000000000000");                               
				uart1Trans("0000000000000000000000000000001111101100011110100000000000000000");                                
				uart1Trans("0000000000000000000000000000000111101111111111110000000000000000");
				uart1Trans("0000000000000000000000000000000111001111111111100000000000000000");
				uart1Trans("0000000000000000000000000000000011011111100000000000000000000000");
				uart1Trans("0000000000000000000000000000000011111111100000000000000000000000");
				uart1Trans("0000000000000000000000000000000011111111111111100000000000000000");
				uart1Trans("0000000000000000000000000000000011111111111111111110000000000000");
				uart1Trans("0000000000000000000000000000000011011111111111111110000000000000");
				uart1Trans("0000000000000000000000000000000001001111111111111110000000000000");
				uart1Trans("0000000000000000000000000000000000000111111111111100000000000000");
				uart1Trans("0000000000000000000000000000000000000011111111100000000000000000");
				uart1Trans("0000000000000000000000000000000000000000001110000000000000000000");
				uart1Trans("0000000000000000000000000000000000000000000000000000000000000000");
				uart1Trans("0000000000000000000000000000000000000000000000000000000000000000");
				uart1Trans("0000000000000000000000000000000000000000000000000000000000000000");
			}
			else if ((strcmp(RXBUFFER, "BR 0")) == false){
				flags = 0; flagh = 0; tim3Enable(0); uart1Trans("  |"); uart1Trans("   -> OK");
			}
			else if ((strcmp(RXBUFFER, "BR 1")) == false){
				flags = 1; flagh = 1; tim3Enable(16*1); uart1Trans("  |"); uart1Trans("   -> OK");
			}
			else if ((strcmp(RXBUFFER, "BR 2")) == false){
				flags = 2; flagh = 2; tim3Enable(16*2); uart1Trans("  |"); uart1Trans("   -> OK");
			}	
			else if ((strcmp(RXBUFFER, "BR 3")) == false){
				flags = 3; flagh = 3; tim3Enable(16*3); uart1Trans("  |"); uart1Trans("   -> OK");
			}	
			else if ((strcmp(RXBUFFER, "BR 4")) == false){
				flags = 4; flagh = 4; tim3Enable(16*4); uart1Trans("  |"); uart1Trans("   -> OK");
			}	
			else if ((strcmp(RXBUFFER, "BR 5")) == false){
				flags = 5; flagh = 5; tim3Enable(16*5); uart1Trans("  |"); uart1Trans("   -> OK");
			}	
			else if ((strcmp(RXBUFFER, "BR 6")) == false){
				flags = 6; flagh = 6; tim3Enable(16*6); uart1Trans("  |"); uart1Trans("   -> OK");
			}	
			else if ((strcmp(RXBUFFER, "BR 7")) == false){
				flags = 7; flagh = 7; tim3Enable(16*7); uart1Trans("  |"); uart1Trans("   -> OK");
			}	
			else if ((strcmp(RXBUFFER, "BR 8")) == false){
				flags = 8; flagh = 8; tim3Enable(16*8); uart1Trans("  |"); uart1Trans("   -> OK");
			}	
			else if ((strcmp(RXBUFFER, "BR 9")) == false){
				flags = 9; flagh = 9; tim3Enable(16*9); uart1Trans("  |"); uart1Trans("   -> OK");
			}
			else if ((strcmp(RXBUFFER, "BR 10")) == false){
				flags = 10; flagh = 10; tim3Enable(16*10); uart1Trans("  |"); uart1Trans("   -> OK");
			}	
			else if ((strcmp(RXBUFFER, "BR INC")) == false){
				flagh++;
					flags1Control();
			}
			else if ((strcmp(RXBUFFER, "BR DEC")) == false){
					flagh--;
           flags2Control();
			}
			else if ((strcmp(RXBUFFER, "BR ?")) == false){
          flags3Control();
			}	
		 else {
				 uart1Trans("  |");
			   uart1Trans("   -> ERROR");
			}
				 uart1Trans(" ");
				 uart1Trans("@micrcontroller-wait-command~:");
		memset(RXBUFFER, 0, BUFF_SIZE);
	}
}

void EXTI1_IRQHandler(void){
		 delay(1000000);
		 EXTI->PR |= EXTI_PR_PR1;
	   flags--;
		 flagh--;		
	
		if(flags == 0){
	    tim3Enable(0);
			return;
	}
		if(flags == 1){
	    tim3Enable(16*1);
			return;
	}
		if(flags == 2){
	    tim3Enable(16*2);
			return;
	}
		if(flags == 3){
	    tim3Enable(16*3);
			return;
	}
		if(flags == 4){
	    tim3Enable(16*4);
			return;
	}
		if(flags == 5){														// if 0-- => 0xFF
			tim3Enable(16*5);
			return;
	}
		if(flags == 6){
		  tim3Enable(16*6);
			return;
		}
		if(flags == 7){
		  tim3Enable(16*7);
			return;
		}
		if(flags == 8){
		  tim3Enable(16*8);
			return;
		}	
		if(flags == 9){
		  tim3Enable(16*9);
			return;
		}	
		if(flags >= 10){
		   flags = 0;
			 flagh = 0;
			return;
		}			
		
}

void EXTI0_IRQHandler(void){
		 delay(1000000);
		 EXTI->PR |= EXTI_PR_PR1;
     flags++;                     //1
		 flagh++;											//1
	
		if(flags == 1){
	    tim3Enable(16*1);
			return;
	}
		if(flags == 2){
	    tim3Enable(16*2);
			return;
	}
		if(flags == 3){
	    tim3Enable(16*3);
			return;
	}
		if(flags == 4){
	    tim3Enable(16*4);
			return;
	}
		if(flags == 5){
	    tim3Enable(16*5);
			return;
	}		 
		if(flags == 6){
	    tim3Enable(16*6);
			return;
	}	
		if(flags == 7){
	    tim3Enable(16*7);
			return;
	}	
		if(flags == 8){
	    tim3Enable(16*8);
			return;
	}	
		if(flags == 9){
	    tim3Enable(16*9);
			return;
	}
		if(flags >= 10){
	    tim3Enable(16*10);
		  flags = 10;
			flagh = 10;
			return;
	}			
}

