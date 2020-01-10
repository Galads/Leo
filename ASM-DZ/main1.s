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

	PUSH {R0, R1, R3, R4, R5, R6, R7}
	
sqrtArea	PROC									; Основная подпрограмма // ; есть мов и мовт 1- младшие 16 бит записывает в регистр 2- старшие 16 бит регистр // пренос на новую строку 	
	MOV32 R3, #0x01        ; iteration
	MOV32 R4, #0x00	       ; no use
	MOV32 R0, #0x00	
	
	MOV32 R5, #0x01	
	;R1 - HIGH BITS R2 - LOW BITS R0-RESULT
	
	;integer
	MOV32 	R1, #0xFFFFFFFF  ;0x01 mladshie slovo   0001 0000 0000 0000 0000 0000 0000 0000 1010 // 4 294 967 306
	MOV32 	R2, #0x05F   ;0x5F starshee slovo
	;integer 1
	
sqrt	
	
    SUBS 	R1, R1, R3  ; result low data  // заем есть, флаг С устанавливается в случае отсутствия  заема 
	SBC 	R2, R2, R4  ; result high data // если флаг заема снят, то результат уменьшается на 1, в ином случае вычитание R2
	
	CMP	    R2, #0
	BNE		next
	
	UBFX	R7, R1, #28, #4
	CMP		R7, #0x0F
	BEQ		next
	
	UBFX	R7, R1, #28, #4
	CMP		R7, #0x0E
	BEQ		next	

	UBFX	R7, R1, #28, #4
	CMP		R7, #0x0D
	BEQ		next
	
	UBFX	R7, R1, #28, #4
	CMP		R7, #0x0C
	BEQ		next
	
	UBFX	R7, R1, #28, #4
	CMP		R7, #0x0B
	BEQ		next	
	
	UBFX	R7, R1, #28, #4
	CMP		R7, #0x0A
	BEQ		next	 
	
	UBFX	R7, R1, #28, #4
	CMP		R7, #0x09
	BEQ		next
	
	UBFX	R7, R1, #28, #4
	CMP		R7, #0x08
	BEQ		next	
	
	ADD     R3, #2	
	CMP	    R3, R1
	IT		GT
	BGT     _else
	
	ADD R5, #1        ; HOW MANY VICHELY

 B sqrt
 
next
	ADD     R3, #2	
    ADD     R5, #1 
	
 B sqrt
 
_else
	MOV   R6, R1	  	
	MOV   R0, R5
    ENDP
		
	POP {R0, R1, R3, R4, R5, R6, R7}		
		
	END
