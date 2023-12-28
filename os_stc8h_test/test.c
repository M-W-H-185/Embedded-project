
void push_all_(void)
{

	__asm PUSH	ACC   ;Ñ¹Õ»
	__asm PUSH	B
	__asm PUSH	DPH
	__asm PUSH	DPL
	__asm PUSH	PSW
	__asm PUSH	AR0
	__asm PUSH	AR1
	__asm PUSH	AR2
	__asm PUSH	AR3
	__asm PUSH	AR4
	__asm PUSH	AR5
	__asm PUSH	AR6
	__asm PUSH	AR7	
	
}
void pop_all_(void)
{

	__asm POP	AR7	;³öÕ»
	__asm POP	AR6
	__asm POP	AR5
	__asm POP	AR4
	__asm POP	AR3
	__asm POP	AR2
	__asm POP	AR1
	__asm POP	AR0
	__asm POP	PSW
	__asm POP	DPL
	__asm POP	DPH
	__asm POP	B
	__asm POP	ACC		
}