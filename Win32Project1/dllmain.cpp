// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <assert.h>
#include "proc.h"
#include <stdlib.h>  
#include "importTableInject.h"

HANDLE hThread;
HMODULE hDll;




/*

�޸�ALLEG40.dll�������ʱ�Զ�����MugenHelper.dll�������һ�κ�����������Ч
*/
void attachDllEx() {

	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(L"ALLEG40_old.dll", &wfd);
	if (INVALID_HANDLE_VALUE != hFind) return;
	
	char newFile[MAX_PATH];
	char* path= "ALLEG40.dll";

	sprintf(newFile, "%s.bak", path);
	CopyFileA(path, newFile, 0);
	
	importTableInject(newFile, "chars\\Scathacha_A\\St\\MugenHelper.dll");

	rename(path, "ALLEG40_old.dll");
	rename(newFile, path);


}

DWORD WINAPI proc(LPVOID lpParam) {

	Sleep(500L);


	UINT level;
	level = loadCodes(hDll); //���ش���
	if (level >4) {
		attachDllEx();


	}

	return 0;

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DWORD threadID;
		
		hDll = hModule;
		
		
		hThread = CreateThread(NULL, 0, proc, NULL, 0, &threadID); // �����߳�
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

