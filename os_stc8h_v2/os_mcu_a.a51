NAME	OS_MCU_A_ASM	; ����һ������Ϊ ��OS_MCU_A_ASM������

?PR?OSStartHighRdy?OS_MCU_A_ASM SEGMENT CODE ; ���Ӧ���� �� OS_CPU_A_ASM��������һ��OSStartHighRdy������������code����
	
PUBLIC OSStartHighRdy	; �����������
EXTRN DATA  (?C_XBP)	;�����ջָ�����ڿ�����ֲ���������

EXTRN XDATA (tcb_Runing)	
EXTRN XDATA (task_stk_idle)	
	
; ---------------- ����ջ�궨�� ----------------
;����ѹջ��ջ��
PUSHALL    MACRO
        PUSH PSW
        PUSH ACC
        PUSH B
        PUSH DPL
        PUSH DPH
        MOV  A,R0	;R0~R7��ջ
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
        ;PUSH SP	;���ر���SP�������л�ʱ����Ӧ�������
        ENDM
    
POPALL    MACRO
        ;POP  ACC	;���ر���SP�������л�ʱ����Ӧ�������
        POP  ACC	;R0~R7��ջ
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
; ---------------- ����ջ�궨�� ----------------


;---------------- �����ջ�ռ� ----------------
;�����ջ�ռ䡣ֻ���Ĵ�С����ջ�����keil������ͨ����ſ��Ի��keil�����SP���
?STACK SEGMENT IDATA
        RSEG ?STACK

OSStack:
		;DS n������n���洢��Ԫ
        DS 40H		;����Ӳ����ջ�Ĵ�С

OSStkStart IDATA OSStack-1
;---------------- �����ջ�ռ� ----------------


	
; ---------------- OSStartHighRdy() ----------------
RSEG  ?PR?OSStartHighRdy?OS_MCU_A_ASM
OSStartHighRdy:

        USING 0		;�ϵ��51�Զ����жϣ��˴�������CLR EAָ���Ϊ���˴���δ���жϣ��������˳��󣬿��ж�

		; ----- ��д -----
		
		MOV  DPTR, #task_stk_idle + 0	; ָ��task_stk_idle[0] 
		MOVX A,@DPTR					; ��task_stk_idle[0]��ֵ����A(��ջ����)
		MOV  R5,A		;R5=�û���ջ����


		; ----- ��д -----
   
   
   
   
   
		
;		; 1����ջָ���ó��� �ŵ���R0
;		MOV DPTR,#tcb_Runing
;		MOVX  A,  @DPTR
;		MOV   R0, A
;		INC   DPTR
;		MOVX  A,  @DPTR
;		MOV   DPL, A
;		MOV   DPH, R0
;		; 2�������ջ�����ó���(��ʵ���Ƕ�ջ����ĵ�0��Ԫ��)
;		;*UserStkPtr ==> R5 �û���ջ��ʼ��ַ����(���û���ջ���ȷ��ڴ˴�)   
;        MOVX A,@DPTR	;�û���ջ����unsigned char��������
;        MOV  R5,A		;R5=�û���ջ����
;		; 3���ָ��ֳ���ջ����
;		MOV  R0,#OSStkStart
;		; 4���ָ���ջָ��SP
;restore_stack:
;        INC  DPTR
;        INC  R0
;        MOVX A,@DPTR
;        MOV  @R0,A
;        DJNZ R5,restore_stack
;        MOV  SP,R0
;		
;		; 5���ָ������ջָ��?C_XBP  
;        INC  DPTR
;        MOVX A,@DPTR
;        MOV  ?C_XBP,A	;?C_XBP �����ջָ���8λ
;        INC  DPTR
;        MOVX A,@DPTR
;        MOV  ?C_XBP+1,A	;?C_XBP �����ջָ���8λ		
;		; 6������ջ

;        POPALL
;		; 7�����ж�
;		SETB EA		;���ж�
RETI
; ---------------- OSStartHighRdy() ----------------


;		MOV  DPTR, #task_stk_idle + 0	; ָ��task_stk_idle[0] 
;		MOVX A,@DPTR					; ��task_stk_idle[0]��ֵ����A
;		MOV A,#0X55						; Ȼ���A��ֵһ��
;		MOVX @DPTR,A					; ��A��ֵ����task_stc_idle[0]��



;MOV DPTR,#tcb_Runing
;MOVX  A,  @DPTR
;MOV   R0, A
;INC   DPTR
;MOVX  A,  @DPTR
;MOV   DPL, A
;MOV   DPH, R0
;;-- ���Ͼ��õ���tcb_Runing�ĵ�ַ

;-----------------
END
;-----------------