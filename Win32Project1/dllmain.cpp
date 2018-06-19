// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <assert.h>
#include "proc.h"
#include <detours.h>

HANDLE hThread;
pFunc Hook = (pFunc)(0x0047AA60);

static BOOL CALLBACK ExportCallback(_In_opt_ PVOID pContext,
	_In_ ULONG nOrdinal,
	_In_opt_ LPCSTR pszName,
	_In_opt_ PVOID pCode)
{
	(void)pContext;
	(void)pCode;
	(void)pszName;

	if (nOrdinal == 1) {
		*((BOOL *)pContext) = TRUE;
	}
	return TRUE;
}

BOOL DoesDllExportOrdinal1(PCHAR pszDllPath)
{
	HMODULE hDll = LoadLibraryExA(pszDllPath, NULL, DONT_RESOLVE_DLL_REFERENCES);
	if (hDll == NULL) {
		printf("setdll.exe: LoadLibraryEx(%s) failed with error %d.\n",
			pszDllPath,
			GetLastError());
		return FALSE;
	}

	BOOL validFlag = FALSE;
	DetourEnumerateExports(hDll, &validFlag, ExportCallback);
	FreeLibrary(hDll);
	return validFlag;
}
static BOOL CALLBACK ListBywayCallback(_In_opt_ PVOID pContext,
	_In_opt_ LPCSTR pszFile,
	_Outptr_result_maybenull_ LPCSTR *ppszOutFile)
{
	(void)pContext;

	*ppszOutFile = pszFile;
	if (pszFile) {
		printf("    %s\n", pszFile);
	}
	return TRUE;
}

static BOOL CALLBACK ListFileCallback(_In_opt_ PVOID pContext,
	_In_ LPCSTR pszOrigFile,
	_In_ LPCSTR pszFile,
	_Outptr_result_maybenull_ LPCSTR *ppszOutFile)
{
	(void)pContext;

	*ppszOutFile = pszFile;
	
	return TRUE;
}
static BOOL CALLBACK AddBywayCallback(_In_opt_ PVOID pContext,
	_In_opt_ LPCSTR pszFile,
	_Outptr_result_maybenull_ LPCSTR *ppszOutFile)
{
	PBOOL pbAddedDll = (PBOOL)pContext;
	if (!pszFile && !*pbAddedDll) {                    
		*pbAddedDll = TRUE;
		*ppszOutFile = "chars\\kfm\\MugenHelper.dll";
	}
	return TRUE;
}
/*
	ʹ��Ms��detours���޸�dll�����Ĺ���
	�޸�zlib.dll�������ʱ�Զ�����MugenHelper.dll�������һ�κ�����������Ч
*/
void attachDll() {
	BOOL bGood = TRUE;
	HANDLE hOld = INVALID_HANDLE_VALUE;
	HANDLE hNew = INVALID_HANDLE_VALUE;
	PDETOUR_BINARY pBinary = NULL;
	BOOL bAddedDll = FALSE;
	
	assert(DoesDllExportOrdinal1("chars\\kfm\\MugenHelper.dll"));
	hOld = CreateFileA("zlib.dll",
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hOld == INVALID_HANDLE_VALUE) {
		
		assert(FALSE&&"hOld��Ч!");
	}

	hNew = CreateFileA("zlib2.dll",
		GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hNew == INVALID_HANDLE_VALUE) {


		assert(FALSE&&"hNew��Ч!");
	}
	
	pBinary = DetourBinaryOpen(hOld);
	if (pBinary == NULL) {

		assert(FALSE&&"pBinary��Ч!");

	}
	assert(DetourBinaryResetImports(pBinary));

	assert(DetourBinaryEditImports(pBinary,
		&bAddedDll,
		AddBywayCallback, NULL, NULL, NULL));

	assert(DetourBinaryEditImports(pBinary, NULL,
		ListBywayCallback, ListFileCallback,
		NULL, NULL));

	assert(DetourBinaryWrite(pBinary, hNew));
	DetourBinaryClose(pBinary);
	pBinary = NULL;
	if (hNew != INVALID_HANDLE_VALUE) {
		CloseHandle(hNew);
		hNew = INVALID_HANDLE_VALUE;
	}
	CloseHandle(hOld);
	rename("zlib.dll", "zlib_old.dll");
	rename("zlib2.dll", "zlib.dll");
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
		UINT level;
		//HANDLE hThread;
		level= loadCodes(hModule); //���ش���
		
		if(level>=5){
			attachDll(); //�´γ�������ʱֱ�Ӽ��ش���
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