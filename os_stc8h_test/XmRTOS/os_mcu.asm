NAME	OS_CPU_A_ASM	; 定义一个名称为 “OS_CPU_A_ASM”汇编段

?PR?timeIsp_handle?OS_CPU_A_ASM  	SEGMENT CODE	; 设置函数存放到code区
?PR?OSCtxSw?OS_CPU_A_ASM  	SEGMENT CODE	; 设置函数存放到code区

EXTRN CODE  (_?time0_handle)
EXTRN CODE  (_?os_taskSwtich)



PUBLIC	OSCtxSw			; 这里就是将这个OSCtxSw函数公开出去


;定义压栈宏
PUSHALL 	MACRO  
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
POPALL MACRO   
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




; ---------------- OSCtxSw 任务切换函数 ----------------
RSEG  ?PR?OSCtxSw?OS_CPU_A_ASM
OSCtxSw:

	CLR EA 		; 关中断
	PUSHALL		; 压栈

	LCALL _?os_taskSwtich	; 调用中断处理函数

	POPALL		; 出栈
	SETB EA		; 开中断
RETI;
; ---------------- OSCtxSw 任务切换函数 ----------------





; ---------------- 定时器0 相关调用 ----------------
; --- 定时器0 中断服务程序 ---
RSEG  ?PR?timeIsp_handle?OS_CPU_A_ASM
timeIsp_handle:     
	CLR EA 		; 关中断
	PUSHALL		; 压栈
	
	LCALL _?time0_handle	; 调用中断处理函数
	
	POPALL		; 出栈
	SETB EA		; 开中断
	
RETI
; --- 定时器0 中断服务程序 ---


CSEG AT 000BH	;定时器T0中断
 LJMP timeIsp_handle

; ---------------- 定时器0 相关调用 ----------------

END
	
	
	
	
	
;	;-- 数组的起始地址就是指向第一个索引的第一个结构体下的变量
;    MOV R0, #tcb_list ; 将任务控制块数组的起始地址加载到 R0
;	MOV @R0, #0x44	; tcb_list[0].sp = 0x44;
;	MOV R0,#tcb_list+1;	sp 是uint8_t 所以是偏移量+1跳到下一个tcb_list[0].delay_tick = 0x00000066
;	MOV @R0,#0x00000066;	
;	MOV R0,#tcb_list+1+4;	delay_tick 是uint32_t 所以是偏移量+4跳到下一个tcb_list[0].os_status_type = 0x77;
;	MOV @R0,#0x77;	
;	MOV R0,#tcb_list+1+4+1;	os_status_type 是uint8_t 所以是偏移量+1跳到下一个tcb_list[0].test_value = 0x88;
;	MOV @R0,#0x88;
