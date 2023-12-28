
void PUSH_ALL_STACK(void)
{
        __asm PUSH PSW
        __asm PUSH ACC
        __asm PUSH B
        __asm PUSH DPL
        __asm PUSH DPH
        __asm MOV  A,R0	;R0~R7»Î’ª
        __asm PUSH ACC
        __asm MOV  A,R1
        __asm PUSH ACC
        __asm MOV  A,R2
        __asm PUSH ACC
        __asm MOV  A,R3
        __asm PUSH ACC
        __asm MOV  A,R4
        __asm PUSH ACC
        __asm MOV  A,R5
        __asm PUSH ACC
        __asm MOV  A,R6
        __asm PUSH ACC
        __asm MOV  A,R7
        __asm PUSH ACC
	
}
void POP_ALL_STACK(void)
{

        __asm POP  ACC	;R0~R7≥ˆ’ª
        __asm MOV  R7,A
        __asm POP  ACC
        __asm MOV  R6,A
        __asm POP  ACC
        __asm MOV  R5,A
        __asm POP  ACC
        __asm MOV  R4,A
        __asm POP  ACC
        __asm MOV  R3,A
        __asm POP  ACC
        __asm MOV  R2,A
        __asm POP  ACC
        __asm MOV  R1,A
        __asm POP  ACC
        __asm MOV  R0,A
        __asm POP  DPH
        __asm POP  DPL
        __asm POP  B
        __asm POP  ACC
        __asm POP  PSW	
}