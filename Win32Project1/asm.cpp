#include "stdafx.h"
#include "util.h"

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