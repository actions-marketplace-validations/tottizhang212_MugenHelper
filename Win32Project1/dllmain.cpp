// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <assert.h>
#include "proc.h"
#include <stdlib.h>  


HANDLE hThread;
HMODULE hDll;





DWORD WINAPI proc(LPVOID lpParam) {

	Sleep(8000L);


	
	UINT level = loadCodes(hDll); //���ش���
	

	return 0;

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	UINT level = 0;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DWORD threadID;
		
		hDll = hModule;
		level = loadCodes(hDll);
		//hThread = CreateThread(NULL, 0, proc, NULL, 0, &threadID); // �����߳�
		
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

