#include "stdafx.h"
#include "util.h"



UINT protectOverFlow() {

	goto END;
BEGIN:
	__asm {
		
		PUSHAD;
		PUSHFD;
		PUSH EAX;
		PUSH ECX;
		PUSH EBX;
		CALL DWORD PTR DS : [0x4BF520] ;
		MOV DWORD PTR DS : [0x4BF640] , EAX;
		POPFD;
		POPAD;
		CALL DWORD PTR DS : [0x4BF630] ;
		ADD ESP, 0x1C;
		CMP EAX, 1;
		JNE _end;
		MOV EAX, DWORD PTR DS : [0x4BF640] ;
		_end:
		JMP DWORD PTR DS : [0x4BF600] ;

	}

END:
	//ȷ�����뷶Χ
	UINT begin, end;
	__asm
	{
		mov eax, BEGIN;
		mov begin, eax;
		mov eax, END;
		mov end, eax;
	}
	return copyAsmCode(begin, (end - begin));
}

UINT mainHandle() {

	goto END;
BEGIN:
	__asm {
		CALL DWORD PTR DS : [0x4BF700] ;
		MOV EAX, 0x4829AA;
		JMP EAX;
	}

END:
	//ȷ�����뷶Χ
	UINT begin, end;
	__asm
	{
		mov eax, BEGIN;
		mov begin, eax;
		mov eax, END;
		mov end, eax;
	}
	return copyAsmCode(begin, (end - begin));
}

//st: ��statedef ��������תֵǰ,���������ڵ�
UINT saveEsp1() {

	goto END;
BEGIN:
	__asm {
		MOV DWORD PTR DS : [0x4BE600] , 0x0047EB31;
		PUSH EDX;
		LEA EAX, [ESP + 0x1C];
		mov  ecx, 0x0047EB29;
		jmp ecx;
	}

END:
	//ȷ�����뷶Χ
	UINT begin, end;
	__asm
	{
		mov eax, BEGIN;
		mov begin, eax;
		mov eax, END;
		mov end, eax;
	}
	return copyAsmCode(begin, (end - begin));
}
//st:�ָ�S�����ESP
UINT restoreEsp1() {

	goto END;
BEGIN:
	__asm {
		add esp, 0x90;
		CMP DWORD PTR DS : [0x4BE600] , 0x0047EB31
			jne  _end;
		mov  DWORD PTR DS : [esp] , 0x0047EB31;
		mov  DWORD PTR DS : [0x4BE600] , 0;
	_end:
		retn;

	}

END:
	//ȷ�����뷶Χ
	UINT begin, end;
	__asm
	{
		mov eax, BEGIN;
		mov begin, eax;
		mov eax, END;
		mov end, eax;
	}

	return copyAsmCode(begin, (end - begin));
}
//cmd:��statedef ��������תֵǰ,���������ڵ�
UINT saveEsp2() {

	goto END;
BEGIN:
	__asm {
		LEA EDX, [ESP + 0x1C];
		PUSH ECX;
		MOV DWORD PTR DS : [0x004BE604] , 0x0047E9B6;
		MOV EBX, 0x0047E9AC;
		JMP EBX;
	}

END:
	//ȷ�����뷶Χ
	UINT begin, end;
	__asm
	{
		mov eax, BEGIN;
		mov begin, eax;
		mov eax, END;
		mov end, eax;
	}
	return copyAsmCode(begin, (end - begin));
}


//cmd:�ָ�S�����ESP
UINT restoreEsp2() {

	goto END;
BEGIN:
	__asm {
		add esp, 0x90;
		CMP DWORD PTR DS : [0x4BE604] , 0x0047E9B6
			jne  _end;
		mov  DWORD PTR DS : [esp] , 0x0047E9B6;
		mov  DWORD PTR DS : [0x4BE604] , 0;
	_end:
		retn;

	}

END:
	//ȷ�����뷶Χ
	UINT begin, end;
	__asm
	{
		mov eax, BEGIN;
		mov begin, eax;
		mov eax, END;
		mov end, eax;
	}

	return copyAsmCode(begin, (end - begin));
}


/*

�������ص�����1
*/
UINT changeController1() {

	goto END;
BEGIN:
	__asm {
		
		
		                                                               
		MOV EAX, DWORD PTR SS : [ESP + 0x10A0];
		PUSH EDI;
		PUSH ESI;
		PUSH ECX;
		PUSH EDX;
		PUSH EBX;
		PUSH EAX;
		CALL DWORD PTR DS : [0x4BEA08];
		MOV EBX, EAX;
		POP EDX;
		POP ECX;
		POP ESI;
		POP EDI;
		JMP DWORD PTR DS : [EBX * 4 + 0x471644];


	}

END:
	//ȷ�����뷶Χ
	UINT begin, end;
	__asm
	{
		mov eax, BEGIN;
		mov begin, eax;
		mov eax, END;
		mov end, eax;
	}

	return copyAsmCode(begin, (end - begin));
}

/*

�������ص�����2
*/
UINT changeController2() {

	goto END;
BEGIN:
	__asm {
	
		MOV DWORD PTR DS : [0x4BE200],EBX;
		MOV EBX, DWORD PTR SS : [ESP + 0x10A0];
		PUSH ECX;
		PUSH EDX;
		PUSH ESI;
		PUSH EDI;
		PUSH EAX;
		PUSH EBX;
		CALL DWORD PTR DS : [0x4BEA14] ;
		POP EDI;
		POP ESI;
		POP EDX;
		POP ECX;
		MOV EBX, DWORD PTR DS : [0x4BE200];
		CMP EAX, 0x0DC;
		JMP DWORD PTR DS : [0x4BF600];

	}

END:
	//ȷ�����뷶Χ
	UINT begin, end;
	__asm
	{
		mov eax, BEGIN;
		mov begin, eax;
		mov eax, END;
		mov end, eax;
	}

	return copyAsmCode(begin, (end - begin));
}

/*

�������ص�����3
*/
UINT changeController3() {

	goto END;
BEGIN:
	__asm {
				
		PUSH ECX;
		PUSH EDX;
		PUSH ESI;
		PUSH EDI;
		PUSH EAX;
		PUSH EBX;
		CALL DWORD PTR DS : [0x4BEA18];
		POP EDI;
		POP ESI;
		POP EDX;
		POP ECX;
		JMP DWORD PTR DS : [0x4BF600];


	}

END:
	//ȷ�����뷶Χ
	UINT begin, end;
	__asm
	{
		mov eax, BEGIN;
		mov begin, eax;
		mov eax, END;
		mov end, eax;
	}

	return copyAsmCode(begin, (end - begin));
}
/**

����ص�����
*/
UINT changeRever() {

	goto END;
BEGIN:
	__asm {

				                                               
		MOV ESI,EAX;
		PUSH ESI;
		PUSH EDI;
		CALL DWORD PTR DS : [0x4BEA0C];
		CMP EAX, 0x26;
		JNE _end;
		JMP DWORD PTR DS : [0x4BF600];
		_end :
		JMP DWORD PTR DS : [0x4BF604];

	}

END:
	//ȷ�����뷶Χ
	UINT begin, end;
	__asm
	{
		mov eax, BEGIN;
		mov begin, eax;
		mov eax, END;
		mov end, eax;
	}

	return copyAsmCode(begin, (end - begin));


}

/**
* 
�л������ص�����
*/
UINT changeAnim() {

	goto END;
BEGIN:
	__asm {

	
		ADD ESP, 0x18;
		MOV ESI, EAX;
		PUSH ESI;
		PUSH EDI;
		CALL DWORD PTR DS : [0x4BEA10] ;
		MOV ESI, EAX;
		JMP DWORD PTR DS : [0x4BF600] ;


	}

END:
	//ȷ�����뷶Χ
	UINT begin, end;
	__asm
	{
		mov eax, BEGIN;
		mov begin, eax;
		mov eax, END;
		mov end, eax;
	}

	return copyAsmCode(begin, (end - begin));


}

/**
����ʤ�����룺/0x0041f8bb Ϊ�ж�ʤ���Ĵ���: edx!=0 && eax=0 ʱ 2p��ʤ; edx=0 && eax!=0 ʱ 1p���ж�ʤ ;edx=0 && eax=0 ʱ ����
*/
UINT changeVictory() {

	goto END;
BEGIN:
	__asm {

		CMP DWORD PTR DS : [0x4BEA00] , 0;
		JE SHORT _code3;
		CMP DWORD PTR DS : [0x4BEA00] , 1;
		JNE SHORT _code2;
		TEST EDX, EDX;
		JE SHORT _code3;
		MOV EDX, 0;
		JMP SHORT _code3;
	_code2:
		TEST EAX, EAX;
		JE SHORT _code3;
		MOV EAX, 0;
		JMP SHORT _code3;

	_code3:		
		TEST EDX, EDX;
		JE SHORT _jmp1;
		TEST EAX, EAX;
		MOV DWORD PTR DS : [0x4BF600] , 0x0041F8DD;
		MOV DWORD PTR DS : [0x4BF604] , 0x0041F8C3;
		JE SHORT _jmp2;
		JMP  DWORD PTR DS : [0x4BF604];
     
	_jmp1:
		MOV DWORD PTR DS : [0x4BF600] , 0x0041F8F8;
		JMP  DWORD PTR DS : [0x4BF600];

	_jmp2:
		JMP  DWORD PTR DS : [0x4BF600] ;
	}

END:
	//ȷ�����뷶Χ
	UINT begin, end;
	__asm
	{
		mov eax, BEGIN;
		mov begin, eax;
		mov eax, END;
		mov end, eax;
	}

	return copyAsmCode(begin, (end - begin));


}