// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <assert.h>
#include "proc.h"
#include <stdlib.h>  
#include "importTableInject.h"

HANDLE hThread;
pFunc Hook = (pFunc)(0x0047AA60);




void attachDllEx() {
	
	char newFile[MAX_PATH];
	char* path= "ALLEG40.dll";

	sprintf(newFile, "%s.bak", path);
	CopyFileA(path, newFile, 0);
	
	importTableInject(newFile, "chars\\kfm\\MugenHelper.dll");

	rename(path, "ALLEG40_old.dll");
	rename(newFile, path);

}
/*
	ʹ��Ms��detours���޸�dll�����Ĺ���
	�޸�zlib.dll�������ʱ�Զ�����MugenHelper.dll�������һ�κ�����������Ч
*/


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DWORD threadID;
		UINT level;
		//HANDLE hThread;
		level= loadCodes(hModule); //���ش���
		if (level >4) {
			attachDllEx();
			//attachDll(); //�´γ�������ʱֱ�Ӽ��ش���
			
		}
				
		
		// MessageBoxW(0, L"������Ϣ��", L"����", 0);
		//hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, &threadID); // �����߳�
		break;
	case DLL_THREAD_ATTACH:

		break;
	case DLL_THREAD_DETACH:

		break;
	case DLL_PROCESS_DETACH:
		
		break;
	}
	return TRUE;
}

void hookTest(DWORD para1, DWORD para2) {
	
	MessageBoxA(NULL, "hookTest!", "INFO", MB_OK);
	Hook(para1, para2);
	
}