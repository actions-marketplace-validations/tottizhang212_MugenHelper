
#include "stdafx.h"
#include <stdlib.h>  
#include<time.h>
#include <assert.h>
#include "proc.h"
#include "util.h"
#include "resource.h"




/*

��cem�ļ��ж�ȡ shellcode���뵽�ڴ��ָ����ַ��
*/
char* WINAPI ReadCodeFile(char* file, char* startAddress) {

	FILE * pFile;
	long lSize;
	char buffer[100];
	sprintf(buffer, path, file);
	//char * buffer;
	size_t result;
	pFile = fopen(buffer, "rb");
	if (pFile == NULL)
	{
		fputs("File error", stderr);
		exit(1);
	}

	/* ��ȡ�ļ���С */
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);
	/* �����ڴ�洢�����ļ� */
	//buffer 
	if (startAddress == NULL)
	{
		startAddress = (char*)malloc(sizeof(char)*lSize);
	}
	/* ���ļ�������buffer�� */
	result = fread(startAddress, 1, lSize, pFile);
	if (result != lSize)
	{
		fputs("Reading error", stderr);
		exit(3);
	}

	fclose(pFile);

	return startAddress;
}
void switchJmp(HMODULE hmodule, LPCSTR funName, UINT funAdr, UINT startAdr, UINT relCode) {

	ADRDATA(funAdr) = (UINT)GetProcAddress(hmodule, funName);
	VirtualProtect((LPVOID)startAdr, 16, 0x40, (PDWORD)0x004BE200);
	ADRDATA(startAdr) = relCode;
	startAdr += 4;
	ADR_BYTE_DATA(startAdr) = 0;


}

void switchJmp2(HMODULE hmodule, LPCSTR funName, UINT funAdr, UINT startAdr, UINT writeAdr) {

	ADRDATA(funAdr) = (UINT)GetProcAddress(hmodule, funName);

	
	VirtualProtect((LPVOID)startAdr, 16, 0x40, (PDWORD)0x004BE200);
	UINT rav = writeAdr - startAdr - 5;


	UINT relCode = 0xE9 | (rav << 8);
	ADRDATA(startAdr) = relCode;
	startAdr += 4;
	relCode = 0 | (rav >> 24);
	ADR_BYTE_DATA(startAdr) = relCode;


}

void switchJmp3(UINT jumpAdr, UINT targetAdr) {

	
	VirtualProtect((LPVOID)jumpAdr, 16, 0x40, (PDWORD)0x004BE200);
	UINT rav = targetAdr - jumpAdr - 5;
	
	UINT relCode = 0xE9 | (rav << 8);
	ADRDATA(jumpAdr) = relCode;
	jumpAdr += 4;
	relCode = 0 | (rav >> 24);
	ADR_BYTE_DATA(jumpAdr) = relCode;


}


UINT copyAsmCode(UINT begin, int len) {

	char* startAddress = (char*)malloc(sizeof(char)*len);
	memcpy(startAddress, (void*)begin, len);
	//���ֽڶ���
	int fill = (len - len % 4) % 4;
	while (fill--)startAddress[len + fill] = 0x90;
	return (UINT)startAddress;

}

void log(const char* content) {

	FILE * pFile;
	char buffer[100];
	sprintf(buffer, path, "debug.log");
	pFile = fopen(buffer, "a+");
	time_t t = time(0);
	char tmpBuf[100];
	strftime(tmpBuf, 100, "%Y-%m-%d %H:%M:%S", localtime(&t)); //format date and time. 
	fprintf(pFile, "%s---%s\r\n", tmpBuf, content);
	fclose(pFile);
}


int isFileExist(const char* file) {

	
	long  handle; //���ڲ��ҵľ��
	struct _finddata_t fileinfo; //�ļ���Ϣ�Ľṹ��

	handle = _findfirst(file, &fileinfo);
	if (-1 == handle) return 0;		
	else return 1;
	

}

char* trim(const char* str)
{

	
	unsigned int uLen = strlen(str);

	if (0 == uLen)
	{
		return '\0';
	}
	char* strRet = (char*)malloc(uLen + 1);
	memset(strRet, 0, uLen + 1);

	unsigned int i = 0, j = 0;
	for (i = 0; i < uLen + 1; i++)
	{
		if (str[i] != ' ')
		{
			strRet[j++] = str[i];
		}
	}
	strRet[j] = '\0';

	return strRet;
}