NAME	OS_CPU_A_ASM	; ����һ������Ϊ ��OS_CPU_A_ASM������

?PR?test?OS_CPU_A_ASM 			SEGMENT CODE ; ���Ӧ���� �� OS_CPU_A_ASM��������һ��test������������code����
?PR?OSCtxSw?OS_CPU_A_ASM 			SEGMENT CODE ; ���Ӧ���� �� OS_CPU_A_ASM��������һ��test������������code����
?PR?TIMER0_ISR?OS_CPU_A_ASM  	SEGMENT CODE
	
	
EXTRN	DATA 	(OS_TCB_SIZE)	; ������ƽṹ��Ĵ�С
EXTRN	IDATA 	(tcb_list)		; �����б�
EXTRN	DATA 	(task_id)		; ��ǰ������

EXTRN CODE  (_?time0_handle)


PUBLIC	test			; ������ǽ����test����������ȥ
PUBLIC	OSCtxSw			; ������ǽ����OSCtxSw����������ȥ


;����ѹջ��
PUSHALL    MACRO
        USING 0     ;�����Ĵ���0  AR0~AR7 ��Ӧ00~07H��ַ
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
;�����ջ��
POPALL    MACRO
        USING 0     ;�����Ĵ���0  AR0~AR7 ��Ӧ00~07H��ַ
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

; ---------------- ���Ժ��� ----------------
RSEG  ?PR?test?OS_CPU_A_ASM
test:
	MOV A,task_id
	MOV B,OS_TCB_SIZE
	MUL AB ; �˷�����.��λ�ŵ�A ��λ�ŵ�B ����ȡAֵ��������ƫ����
	MOV R0 ,A	; ������ƫ�����浽R0
	
	
	MOV R1 ,#tcb_list	; tcb_list����ĵ�ַ
	
	MOV A,R0	; �ְ� r0����ƫ��������A
	ADD A,R1	; A + R1 A���ǵ�λ������
	MOV R1,A	; R1���������������ƫ�ƺ�ĵ�ַ
	; R1 ����ָ��sp�� MOV @R1, #0x66	; �����������ṹ������ĵ�һ������tcb_list[].sp = 0x66


	
RETI
; ---------------- ���Ժ��� ----------------

; ---------------- OSCtxSw() �����Զ�����cpu��Դ ��ʱ�õ��� ----------------
RSEG  ?PR?OSCtxSw?OS_CPU_A_ASM
OSCtxSw:
	; ----------------------------- �����л� -----------------------------
	MOV A,task_id
	MOV B,OS_TCB_SIZE
	MUL AB ; �˷�����.��λ�ŵ�A ��λ�ŵ�B ����ȡAֵ��������ƫ����
	MOV R0 ,A	; ������ƫ�����浽R0
	
	
	MOV R1 ,#tcb_list	; tcb_list����ĵ�ַ
	
	MOV A,R0	; �ְ� r0����ƫ��������A
	ADD A,R1	; A + R1 A���ǵ�λ������
	MOV R1,A	; R1���������������ƫ�ƺ�ĵ�ַ
	; R1 ����ָ��sp�� MOV @R1, #0x66	; �����������ṹ������ĵ�һ������tcb_list[].sp = 0x66
	;�ָ���ջָ��SP
	MOV A,@R1	; R1���������������ƫ�ƺ�ĵ�ַ

	MOV  SP,A
	; ----------------------------- �����л� -----------------------------
RETI;
; ---------------- OSCtxSw() �����л����� ----------------



; ---------------- ��ʱ��0 ��ص��� ----------------
; --- ��ʱ��0 �жϷ������ ---
RSEG  ?PR?TIMER0_ISR?OS_CPU_A_ASM
TIMER0_ISR:     
	CLR EA 

	;PUSHALL

	LCALL _?time0_handle
	;POPALL

	; ----------------------------- �����л� -----------------------------
	MOV A,task_id
	MOV B,OS_TCB_SIZE
	MUL AB ; �˷�����.��λ�ŵ�A ��λ�ŵ�B ����ȡAֵ��������ƫ����
	MOV R0 ,A	; ������ƫ�����浽R0
	
	
	MOV R1 ,#tcb_list	; tcb_list����ĵ�ַ
	
	MOV A,R0	; �ְ� r0����ƫ��������A
	ADD A,R1	; A + R1 A���ǵ�λ������
	MOV R1,A	; R1���������������ƫ�ƺ�ĵ�ַ
	; R1 ����ָ��sp�� MOV @R1, #0x66	; �����������ṹ������ĵ�һ������tcb_list[].sp = 0x66
	;�ָ���ջָ��SP
	MOV A,@R1	; R1���������������ƫ�ƺ�ĵ�ַ

	MOV  SP,A
	; ----------------------------- �����л� -----------------------------

	SETB EA

;	
RETI
; --- ��ʱ��0 �жϷ������ ---


CSEG AT 000BH	;��ʱ��T0�ж�
LJMP TIMER0_ISR

; ---------------- ��ʱ��0 ��ص��� ----------------

END
	
	
	
	
	
;	;-- �������ʼ��ַ����ָ���һ�������ĵ�һ���ṹ���µı���
;    MOV R0, #tcb_list ; ��������ƿ��������ʼ��ַ���ص� R0
;	MOV @R0, #0x44	; tcb_list[0].sp = 0x44;
;	MOV R0,#tcb_list+1;	sp ��uint8_t ������ƫ����+1������һ��tcb_list[0].delay_tick = 0x00000066
;	MOV @R0,#0x00000066;	
;	MOV R0,#tcb_list+1+4;	delay_tick ��uint32_t ������ƫ����+4������һ��tcb_list[0].os_status_type = 0x77;
;	MOV @R0,#0x77;	
;	MOV R0,#tcb_list+1+4+1;	os_status_type ��uint8_t ������ƫ����+1������һ��tcb_list[0].test_value = 0x88;
;	MOV @R0,#0x88;
