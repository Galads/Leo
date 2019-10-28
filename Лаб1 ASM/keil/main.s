	PRESERVE8							; 8-битное выравнивание стека директива для компилятора 
	THUMB								; Режим Thumb (AUL) инструкций будем работать с тамб 2 инструкциями

	GET	config.s						; include-файлы
	GET	stm32f10x.s						;

	AREA RESET, CODE, READONLY			; ареа говорит в какой области памяти будет распологаться то что иде после сброса (вектор сброса)

	; Таблица векторов прерываний
	DCD STACK_TOP						; Указатель на вершину стека в первую ячейку памяти программ будет записан стек топ
	DCD Reset_Handler					; Вектор сброса укзывает ветор сброс, указатель на программу после сброса программы (загружает а программ каунтер)

	ENTRY								; Точка входа в программу директива 

Reset_Handler	PROC					; Вектор сброса (переходим по этой метке после сброса) // процедура подпрогрмама заканчивается ЭГНДПЭ - граница
	EXPORT  Reset_Handler				; Делаем Reset_Handler видимым вне этого файла // нужно для того чтобы вектор сброса был виден во всех файлах

main									; Основная подпрограмма // ; есть мов и мовт 1- младшие 16 бит записывает в регистр 2- старшие 16 бит регистр // пренос на новую строку 
	MOV32	R0, PERIPH_BB_BASE + \
			RCC_APB2ENR * 32 + \
			4 * 4						; вычисляем адрес для BitBanding 5-го бита регистра //RCC_APB2ENR адресация побайтовая  и поэтому 4*8 = 32 
										; BitAddress = BitBandBase + (RegAddr * 32) + BitNumber * 4
	MOV		R1, #1						; включаем тактирование порта C (D) (в 5-й бит RCC_APB2ENR пишем '1`) // клок энейбл регистр в RCC 
	STR 	R1, [R0]					; загружаем это значение // тактирования настроили
init	
	MOV32	R0, GPIOC_CRH				; адрес порта // считываем адрес в  r0
	MOV		R1, #0x03					; 4-битная маска настроек для Output mode 50mHz, Push-Pull ("0011") то ч то хотим загрузить
	LDR		R2, [R0]					; считать порт // содержимое по адресу считали
    BFI		R2, R1, #4, #4    			; скопировать биты маски в позицию PIN7 забисываем с 28 бита до 32 - см crl
    STR		R2, [R0]					; загрузить результат в регистр настройки порта // настройка портов завершена из r2 в r0
										; config PC0
	MOV32	R4, GPIOC_CRL
	MOV		R5, #0x08					; 0x23	 input mode 10 00 cnf & mode
	LDR		R6, [R4]
	BFI 	R6, R5, #0, #4				; вставка битового поля 4 бита вставляем в r6 начиная с нулевого 
	STR 	R6, [R4]					; download config in R4
										; config PC1
	MOV32	R7, GPIOC_CRL
	MOV		R8, #0x08					; 0x23	 input mode 
	LDR		R9, [R7]
	BFI 	R9, R8, #4, #4				
	STR 	R9, [R7]					; download config in R7 
	
	MOV	R9, #0
	
softPwmButt	
	LDR R1, =GPIO_IDR_IDR0
	LDR R2, =GPIO_IDR_IDR1
	
    MOV32	R3, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			0 * 4	
			
	LDR R4,[R3]
	CMP R1, R4	
	BNE	softPwm

    MOV32	R5, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			1 * 4	
			
	LDR R6,[R5]
	ADD R6, #0x1
	CMP R2, R6	
	BNE	softPwm
	
	B	softPwmButt
	

softPwm
	LDR R8, =DELAY_VAL
	BL delay
	
    MOV32	R3, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			0 * 4	
			
	LDR R4,[R3]
	CMP R1, R4	
	BNE	flagsControl				;if butt push pc0
	
    MOV32	R5, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			1 * 4	
			
	LDR R6,[R5]
	ADD R6, #0x1
	CMP R2, R6	
	BNE	flagsControl				;if butt push pc1
	
	B	softPwm
	
delay 
	SUBS R8, #1
	IT	NE
	BNE	delay
	
	BX	LR
	
flagsControl
	CMP	R9, #0 
	BEQ.W	while0
	
	CMP	R9, #1 
	BEQ.W	while1
	
	CMP	R9, #2 
	BEQ.W	while2
	
	CMP	R9, #3 
	BEQ.W	while3	
	
	CMP	R9, #4
	BEQ.W	while4
	
	CMP	R9, #5
	BEQ.W	while5
	
	CMP	R9, #6 
	BEQ.W	while6
	
	B	flagsControl

while0
	MOV32 R10, GPIOC_BSRR
	MOV   R11, #(PIN9<<16)
	STR   R11, [R10]

    MOV32	R3, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			0 * 4	
			
	LDR R4,[R3]
	CMP R1, R4	
	BNE.W	flagsSet				;if push butt
	
	B while0 
	
while1 								; переход либо на №0 либо на №2 с задержкой вайла 1
	MOV32 R10, GPIOC_BSRR
	MOV   R11, #(PIN9)
	STR   R11, [R10]
	
	BL whileDelay1
	
	MOV	  R11, #(PIN9 << 16)
	STR   R11, [R10]
	
	BL	delayWhile1
	
	MOV32	R3, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			1 * 4	
	LDR R4,[R3]
	ADD R4, #0x1
	CMP R2, R4	
	BNE.W	flagsSet0		
	
    MOV32	R3, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			0 * 4	
			
	LDR R4,[R3]
	CMP R1, R4	
	BNE.W	flagsSet1			;if push butt

	B while1
	
while2							; переход либо на №1 либо на №3 с задержкой вайла №2
	MOV32 R10, GPIOC_BSRR
	MOV   R11, #(PIN9)
	STR   R11, [R10]
	
	BL whileDelay2
	
	MOV	  R11, #(PIN9 << 16)
	STR   R11, [R10]
	
	BL	delayWhile2

	MOV32	R3, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			1 * 4	
	LDR R4,[R3]
	ADD R4, #0x1
	CMP R2, R4	
	BNE.W	flagsSet		
	
    MOV32	R3, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			0 * 4	
			
	LDR R4,[R3]
	CMP R1, R4	
	BNE.W	flagsSet2		;смещение инструкции велико для перемещения по 16 разрядной Thumb, принудительно задаем Thumb-2
							;Суффикс .W может быть добавлен к инструкции, чтобы дать ассемблеру команду генерировать 32-битную ветвь
	
	B	while2

	
while3									;flags number #3 переход назад либо на №2 либо на №4, с задержкой вайла №2 
	MOV32 R10, GPIOC_BSRR
	MOV   R11, #(PIN9)
	STR   R11, [R10]
	
	BL whileDelay2
	
	MOV	  R11, #(PIN9 << 16)
	STR   R11, [R10]
	
	BL	delayWhile2
	
	MOV32	R5, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			1 * 4	
			
	LDR R4,[R5]
	ADD R4, #0x1
	CMP R2, R4	
	BNE	flagsSet1
	
	MOV32	R3, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			0 * 4	
			
	LDR R4,[R3]
	CMP R1, R4	
	BNE	flagsSet3
	
	B while3

while4										; переход либо на 3 либо на 5 с задержкой вайла 3 
	MOV32 R10, GPIOC_BSRR
	MOV   R11, #(PIN9)
	STR   R11, [R10]
	
	BL whileDelay3
	
	MOV	  R11, #(PIN9 << 16)
	STR   R11, [R10]
	
	BL	delayWhile3
	
	MOV32	R5, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			1 * 4	
			
	LDR R4,[R5]
	ADD R4, #0x1
	CMP R2, R4	
	BNE	flagsSet2
	
	MOV32	R3, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			0 * 4	
			
	LDR R4,[R3]
	CMP R1, R4	
	BNE	flagsSet4
	B	while4
	
while5											; переход либо на 4 либо на 6 с задержкой вайла 4 
	MOV32 R10, GPIOC_BSRR
	MOV   R11, #(PIN9)
	STR   R11, [R10]
	
	BL whileDelay4
	
	MOV	  R11, #(PIN9 << 16)
	STR   R11, [R10]
	
	BL	delayWhile4
	
	MOV32	R5, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			1 * 4	
			
	LDR R4,[R5]
	ADD R4, #0x1
	CMP R2, R4	
	BNE	flagsSet3
	
	MOV32	R3, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			0 * 4	
			
	LDR R4,[R3]
	CMP R1, R4	
	BNE	flagsSet5
	B while5
	
while6
	MOV32 R10, GPIOC_BSRR
	MOV   R11, #(PIN9)
	STR   R11, [R10]
	 
	MOV32	R5, PERIPH_BB_BASE + \
			GPIOC_IDR * 32 + \
			1 * 4	
			
	LDR R4,[R5]
	ADD R4, #0x1
	CMP R2, R4	
	BNE	flagsSet4 
	 
	B	while6

flagsSet0
	BL delayRead
	MOV R9, #0
	B	flagsControl
flagsSet
	BL delayRead
	MOV R9, #1
	B   flagsControl		;softPwm
flagsSet1
	;BL delayRead
	MOV R9, #2
	B	flagsControl		;Butt
flagsSet2
	BL delayRead
	MOV R9, #3
	B	flagsControl		;Butt
flagsSet3
	BL delayRead
	MOV R9, #4
	B	flagsControl		;Butt
flagsSet4
	BL delayRead
	MOV R9, #5
	B	flagsControl		;Butt
flagsSet5
	BL delayRead
	MOV R9, #6
	B	flagsControl		;Butt

whileDelay1
	PUSH	{R0}	
	LDR		R0, =DELAY_LOW1
whileDelay1_1	
	SUBS	R0, #1
	BNE whileDelay1_1
	
	POP {R0}
	BX	LR
	
delayWhile1
	PUSH	{R0}	
	LDR		R0, =DELAY_HIGH1
delayWhile1_1
	SUBS	R0, #1
	BNE delayWhile1_1
	
	POP {R0}
	BX	LR

whileDelay2
	PUSH	{R0}	
	LDR		R0, =DELAY_LOW2
whileDelay2_1	
	SUBS	R0, #1
	BNE whileDelay2_1
	
	POP {R0}
	BX	LR
	
delayWhile2
	PUSH	{R0}	
	LDR		R0, =DELAY_HIGH2
delayWhile2_1
	SUBS	R0, #1
	BNE delayWhile2_1
	
	POP {R0}
	BX	LR
	
whileDelay3
	PUSH	{R0}	
	LDR		R0, =DELAY_LOW3
whileDelay3_1	
	SUBS	R0, #1
	BNE whileDelay3_1
	
	POP {R0}
	BX	LR
	
delayWhile3
	PUSH	{R0}	
	LDR		R0, =DELAY_HIGH3
delayWhile3_1
	SUBS	R0, #1
	BNE delayWhile2_1
	
	POP {R0}
	BX	LR

whileDelay4
	PUSH	{R0}	
	LDR		R0, =DELAY_LOW4
whileDelay4_1	
	SUBS	R0, #1
	BNE whileDelay4_1
	
	POP {R0}
	BX	LR
	
delayWhile4
	PUSH	{R0}	
	LDR		R0, =DELAY_HIGH4
delayWhile4_1
	SUBS	R0, #1
	BNE delayWhile4_1
	
	POP {R0}
	BX	LR

delayRead
	PUSH	{R0}	
	LDR		R0, =DELAY_READ
delayRead_1
	SUBS	R0, #1
	BNE delayRead_1

	POP {R0}
	BX	LR	

	END
