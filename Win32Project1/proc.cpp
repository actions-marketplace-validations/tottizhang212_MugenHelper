// Win32Project1.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include <stdlib.h>  
#include <assert.h> 
#define VALID_ADDRESS 0x004B404A 
void WINAPI ReadCodeFile(char* file, char* startAddress) {

	FILE * pFile;
	long lSize;
	//char * buffer;
	size_t result;
	pFile = fopen(file, "rb");
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
	//buffer = (char*)malloc(sizeof(char)*lSize);
	if (startAddress == NULL)
	{
		fputs("Memory error", stderr);
		exit(2);
	}
	/* ���ļ�������buffer�� */
	result = fread(startAddress, 1, lSize, pFile);
	if (result != lSize)
	{
		fputs("Reading error", stderr);
		exit(3);
	}

	fclose(pFile);

	return;
}
void WINAPI loadCodes() {


	//����Shellcode����������ļ����ڴ��е�ָ����ַ
	int address = 0x004b5b4c;

	address = 0x004B7000;
	ReadCodeFile("chars\\Scathacha_A\\St\\1.CEM", (char *)address);
	address = 0x004B8000;
	ReadCodeFile("chars\\Scathacha_A\\St\\2.CEM", (char *)address);
	address = 0x004BE700;
	ReadCodeFile("chars\\Scathacha_A\\St\\3.CEM", (char *)address);
	address = 0x004BE800;
	ReadCodeFile("chars\\Scathacha_A\\St\\4.CEM", (char *)address);
	address = 0x004B4000;
	ReadCodeFile("chars\\Scathacha_A\\St\\5.CEM", (char *)address);
	address = 0x004B3000;
	ReadCodeFile("chars\\Scathacha_A\\St\\6.CEM", (char *)address);


}




DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	
	UINT mainEntryPoint;
	UINT* ptr = (UINT*)0x004b5b4c;
	mainEntryPoint = *ptr;  //��������ڵ�ַ
	UINT pCns1=NULL; //cns��ַ�ĵ�ַ����
	UINT pCns2= NULL;//cns�ĵ�ַ����
	
	while (true)
	{
		Sleep(1);
		int count = 0;
		int cnsAtk = 0; //�ж϶Է�CNS����
		bool hasSelected = false;
		for (size_t i = 1; i <= 4; i++)
		{
			
			UINT dAdr = *((UINT *)(mainEntryPoint + i * 4 + 0xB650)); //def����ָ��
			if (dAdr < VALID_ADDRESS) {
				continue;
			}
			UINT pAdr = *((UINT *)(mainEntryPoint + i * 4 + 0xB750)); //����ָ��
			if (pAdr < VALID_ADDRESS) {
				continue;
			} 

			

			hasSelected = true;
			UINT cns1 = *((UINT *)(dAdr + 0x3C4));    //def�е�CNS��ַ�ĵ�ַ
			UINT cns2 = *((UINT *)(pAdr + 0xBE8));//�����cns��ַ�ĵ�ַ
			UINT cns3 = NULL;
			UINT cns4 = NULL;
						
			if (cns1 < VALID_ADDRESS) continue;
			cns3 = *((UINT*)cns1); //def�е�CNS��ַ
			if (cns2 < VALID_ADDRESS) continue;
			cns4 = *((UINT*)cns2);//�����cns��ַ
			
						
			UINT lpName = dAdr + 0x30;
			
		
			if (strcmp((char*)lpName, "Scathacha") == NULL) {

				*((UINT*)(pAdr + 0xE24)) = 200;//Alive����
							
				if (pCns1 == NULL || pCns1<0x004B404A) {
					//�״�����ʱ����cns��ַ�ĵ�ַ
					pCns1 = cns1;

				}
				if (pCns1>VALID_ADDRESS && cns1 != pCns1) {
					*((UINT*)(dAdr + 0x3C4)) = pCns1;//����޸�def��cns��ַ�ĵ�ַ
					cns1 = pCns1;
					cnsAtk = 1;
				} 
				if (pCns1>VALID_ADDRESS && cns2 != pCns1) {

					*((UINT*)(pAdr + 0xBE8)) = pCns1;//����޸������cns��ַ�ĵ�ַ
					cns2 = pCns1;
					cnsAtk = 1;
				}
												
				if (pCns2 == NULL || pCns2<VALID_ADDRESS) {
					pCns2 = cns3;//�״�����ʱ����cns�ĵ�ַ

				}
						
				if (pCns2>VALID_ADDRESS && cns2>VALID_ADDRESS && cns4 != pCns2)
				{
					*((UINT*)cns2) = pCns2;//����޸������cns�ĵ�ַ
					cnsAtk = 1;
				}
					

			}
			else
			{

				if (cnsAtk == 1) 
				{
					if(pAdr>VALID_ADDRESS && pCns1>VALID_ADDRESS)
						*((UINT*)(pAdr + 0xBE8)) = pCns1;//�Է�CNS�޸�
					
					*((UINT*)(pAdr + 0xE24)) = 0;//�Է�����
					cnsAtk = 0;
				}

			}
		}
		if(!hasSelected)
			cnsAtk = 0;



	}

	
	return 0;
}


