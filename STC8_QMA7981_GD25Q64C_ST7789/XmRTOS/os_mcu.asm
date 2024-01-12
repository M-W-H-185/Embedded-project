NAME	OS_CPU_A_ASM	; ����һ������Ϊ ��OS_CPU_A_ASM������

?PR?timeIsp_handle?OS_CPU_A_ASM  	SEGMENT CODE	; ���ú�����ŵ�code��
?PR?OSCtxSw?OS_CPU_A_ASM  	SEGMENT CODE	; ���ú�����ŵ�code��

EXTRN CODE  (_?time0_handle)
EXTRN CODE  (_?os_taskSwtich)



PUBLIC	OSCtxSw			; ������ǽ����OSCtxSw����������ȥ


;����ѹջ��
PUSHALL 	MACRO  
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
POPALL MACRO   
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




; ---------------- OSCtxSw �����л����� ----------------
RSEG  ?PR?OSCtxSw?OS_CPU_A_ASM
OSCtxSw:

	CLR EA 		; ���ж�
	PUSHALL		; ѹջ

	LCALL _?os_taskSwtich	; �����жϴ�����

	POPALL		; ��ջ
	SETB EA		; ���ж�
RETI;
; ---------------- OSCtxSw �����л����� ----------------





; ---------------- ��ʱ��0 ��ص��� ----------------
; --- ��ʱ��0 �жϷ������ ---
RSEG  ?PR?timeIsp_handle?OS_CPU_A_ASM
timeIsp_handle:     
	CLR EA 		; ���ж�
	PUSHALL		; ѹջ
	
	LCALL _?time0_handle	; �����жϴ�����
	
	POPALL		; ��ջ
	SETB EA		; ���ж�
	
RETI
; --- ��ʱ��0 �жϷ������ ---


CSEG AT 000BH	;��ʱ��T0�ж�
 LJMP timeIsp_handle

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
