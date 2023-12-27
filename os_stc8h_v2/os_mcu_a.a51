NAME	OS_MCU_A_ASM	; 定义一个名称为 “OS_MCU_A_ASM”汇编段

?PR?OSStartHighRdy?OS_MCU_A_ASM SEGMENT CODE ; 这句应该是 在 OS_CPU_A_ASM端里面有一个OSStartHighRdy函数，放在了code区域
	
PUBLIC OSStartHighRdy	; 公开这个函数
EXTRN DATA  (?C_XBP)	;仿真堆栈指针用于可重入局部变量保存

EXTRN XDATA (tcb_Runing)	
EXTRN XDATA (task_stk_idle)	
	
; ---------------- 出入栈宏定义 ----------------
;定义压栈出栈宏
PUSHALL    MACRO
        PUSH PSW
        PUSH ACC
        PUSH B
        PUSH DPL
        PUSH DPH
        MOV  A,R0	;R0~R7入栈
        PUSH ACC
        MOV  A,R1
        PUSH ACC
        MOV  A,R2
        PUSH ACC
        MOV  A,R3
        PUSH ACC
        MOV  A,R4
        PUSH ACC
        MOV  A,R5
        PUSH ACC
        MOV  A,R6
        PUSH ACC
        MOV  A,R7
        PUSH ACC
        ;PUSH SP	;不必保存SP，任务切换时由相应程序调整
        ENDM
    
POPALL    MACRO
        ;POP  ACC	;不必保存SP，任务切换时由相应程序调整
        POP  ACC	;R0~R7出栈
        MOV  R7,A
        POP  ACC
        MOV  R6,A
        POP  ACC
        MOV  R5,A
        POP  ACC
        MOV  R4,A
        POP  ACC
        MOV  R3,A
        POP  ACC
        MOV  R2,A
        POP  ACC
        MOV  R1,A
        POP  ACC
        MOV  R0,A
        POP  DPH
        POP  DPL
        POP  B
        POP  ACC
        POP  PSW
        ENDM
; ---------------- 出入栈宏定义 ----------------


;---------------- 分配堆栈空间 ----------------
;分配堆栈空间。只关心大小，堆栈起点由keil决定，通过标号可以获得keil分配的SP起点
?STACK SEGMENT IDATA
        RSEG ?STACK

OSStack:
		;DS n：保留n个存储单元
        DS 40H		;分配硬件堆栈的大小

OSStkStart IDATA OSStack-1
;---------------- 分配堆栈空间 ----------------


	
; ---------------- OSStartHighRdy() ----------------
RSEG  ?PR?OSStartHighRdy?OS_MCU_A_ASM
OSStartHighRdy:

        USING 0		;上电后51自动关中断，此处不必用CLR EA指令，因为到此处还未开中断，本程序退出后，开中断

		; ----- 重写 -----
		
		MOV  DPTR, #task_stk_idle + 0	; 指向task_stk_idle[0] 
		MOVX A,@DPTR					; 将task_stk_idle[0]的值存入A(堆栈长度)
		MOV  R5,A		;R5=用户堆栈长度


		; ----- 重写 -----
   
   
   
   
   
		
;		; 1、堆栈指针拿出来 放到了R0
;		MOV DPTR,#tcb_Runing
;		MOVX  A,  @DPTR
;		MOV   R0, A
;		INC   DPTR
;		MOVX  A,  @DPTR
;		MOV   DPL, A
;		MOV   DPH, R0
;		; 2、任务堆栈长度拿出来(其实就是堆栈数组的第0个元素)
;		;*UserStkPtr ==> R5 用户堆栈起始地址内容(即用户堆栈长度放在此处)   
;        MOVX A,@DPTR	;用户堆栈中是unsigned char类型数据
;        MOV  R5,A		;R5=用户堆栈长度
;		; 3、恢复现场堆栈内容
;		MOV  R0,#OSStkStart
;		; 4、恢复堆栈指针SP
;restore_stack:
;        INC  DPTR
;        INC  R0
;        MOVX A,@DPTR
;        MOV  @R0,A
;        DJNZ R5,restore_stack
;        MOV  SP,R0
;		
;		; 5、恢复仿真堆栈指针?C_XBP  
;        INC  DPTR
;        MOVX A,@DPTR
;        MOV  ?C_XBP,A	;?C_XBP 仿真堆栈指针高8位
;        INC  DPTR
;        MOVX A,@DPTR
;        MOV  ?C_XBP+1,A	;?C_XBP 仿真堆栈指针低8位		
;		; 6、弹出栈

;        POPALL
;		; 7、开中断
;		SETB EA		;开中断
RETI
; ---------------- OSStartHighRdy() ----------------


;		MOV  DPTR, #task_stk_idle + 0	; 指向task_stk_idle[0] 
;		MOVX A,@DPTR					; 将task_stk_idle[0]的值存入A
;		MOV A,#0X55						; 然后给A赋值一下
;		MOVX @DPTR,A					; 把A的值存入task_stc_idle[0]中



;MOV DPTR,#tcb_Runing
;MOVX  A,  @DPTR
;MOV   R0, A
;INC   DPTR
;MOVX  A,  @DPTR
;MOV   DPL, A
;MOV   DPH, R0
;;-- 以上就拿到了tcb_Runing的地址

;-----------------
END
;-----------------