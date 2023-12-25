NAME	OS_CPU_A_ASM	; 定义一个名称为 “OS_CPU_A_ASM”汇编段

?PR?test?OS_CPU_A_ASM 			SEGMENT CODE ; 这句应该是 在 OS_CPU_A_ASM端里面有一个test函数，放在了code区域
?PR?TIMER0_ISR?OS_CPU_A_ASM  	SEGMENT CODE

EXTRN CODE  (_?time0_handle)


PUBLIC	test	; 这里就是将这个test函数公开出去


;定义压栈宏
PUSHALL    MACRO
        USING 0     ;工作寄存器0  AR0~AR7 对应00~07H地址
        PUSH PSW
        PUSH ACC
        PUSH B
        PUSH DPL
        PUSH DPH
		PUSH 0x00
		PUSH 0x01
		PUSH 0x02
		PUSH 0x03
		PUSH 0x04
		PUSH 0x05
		PUSH 0x06
		PUSH 0x07

ENDM
;定义出栈宏
POPALL    MACRO
        USING 0     ;工作寄存器0  AR0~AR7 对应00~07H地址
        POP 0x07
        POP 0x06
        POP 0x05
        POP 0x04
        POP 0x03
        POP 0x02
        POP 0x01
        POP 0x00
        POP DPH
        POP DPL
        POP B
        POP ACC
        POP PSW

ENDM

; ---------------- 测试函数 ----------------
RSEG  ?PR?test?OS_CPU_A_ASM
test:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
RETI
; ---------------- 测试函数 ----------------

; ---------------- 定时器0 相关调用 ----------------
; --- 定时器0 中断服务程序 ---
RSEG  ?PR?TIMER0_ISR?OS_CPU_A_ASM
TIMER0_ISR:     
	CLR EA 

	PUSHALL

	LCALL _?time0_handle

	SETB EA
	POPALL
;	
RETI
; --- 定时器0 中断服务程序 ---


CSEG AT 000BH	;定时器T0中断
LJMP TIMER0_ISR

; ---------------- 定时器0 相关调用 ----------------

END
