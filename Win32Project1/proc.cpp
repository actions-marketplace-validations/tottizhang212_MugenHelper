// Win32Project1.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include <stdlib.h>  
#include <assert.h> 
#include "proc.h"
#define VALID_ADDRESS 0x004B404A 
#define VAR(index,address) (address+0xE40+index * 4)
#define MODIFYCNS(selfAdR,targetAdR) *((PUINT)(*((PUINT)(targetAdr + 0xBE8)))) = *((PUINT)(*((PUINT)(selfAdr + 0xBE8))))
#define ADRDATA(address) *((PUINT)(address))
#define BIT_EXIST(data,byte)( ((data>>byte) & 1)>0 )



/*

��cem�ļ��ж�ȡ shellcode���뵽�ڴ��ָ����ַ��
*/
char* WINAPI ReadCodeFile(char* file, char* startAddress) {

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
UINT pFloatCallback = 0x00496651;//�����%F�����ת��ַ����
void modifyCode(HMODULE hmodule) {

	
	
	//��ȡplayerHandle�ĺ�����ַд���ַ0x004BF300����0x004b7000���Ĵ����ܹ�����
	*((PUINT)0x004BF700)=(UINT) GetProcAddress(hmodule, "playerHandle");


	//�޸����߳�0x004829A3���Ĵ��룬ʹ֮��תִ��0x004b7000�����룬��0x004b7000������Ϊִ�������playerHandle����
	PUINT ptr = (PUINT)0x004829A3;
	BOOL ret = VirtualProtect((LPVOID)0x004829A3, 13, 0x40, (PDWORD)0x004BE200);
	*ptr = 0x4B7000B8;
	ptr++;
	*ptr = 0xC3E0FF00;
	assert(ret);

	// %n��Ч��---��0x00496CB6���� mov [eax],ecx��Ϊ mov ecx,ecx,��д���ڴ���Ч��
	 ret = VirtualProtect((LPVOID)0x00496CB6, 8, 0x40, (PDWORD)0x004BE200);
	//ADRDATA(0x00496CB6) = 0x45C7C989;
	 //ADRDATA(0x00496CB6) = 0x45C70889;
	//%F��Ч��-----�� call [0x0048e848] ��Ϊ call pFloatCallback�ĵ�ַ���Է����޸�0x0048e848��û��������!
	ret = VirtualProtect((LPVOID)0x00496B8B, 8, 0x40, (PDWORD)0x004BE200);
	ADRDATA(0x00496B8B) = (UINT)(&pFloatCallback);

}
void WINAPI loadCodes(HMODULE hmodule) {


	//����Shellcode����������ļ����ڴ��е�ָ����ַ
	int address = 0x004b5b4c;

	address = 0x004B7000; //��ת��playerHandle
	ReadCodeFile("chars\\Scathacha_A\\St\\1.CEM", (char *)address);
	/*
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
	
	*/

	modifyCode(hmodule);
}


UINT mainEntryPoint = ADRDATA(0x004b5b4c);  //��������ڵ�ַ
UINT pCns1 = NULL; //cns��ַ�ĵ�ַ����
UINT pCns2 = NULL;//cns�ĵ�ַ����
int cnsAtk = 0; //�ж϶Է�CNS����
/*
���������ʼ��
*/
void initial(UINT dAdr, UINT pAdr) {
	

	/*
	
	if (ADRDATA(VAR(2, pAdr)) < VALID_ADDRESS) {

		//�������ַ���õ�var(2)
		ADRDATA((VAR(2, pAdr))) = mainEntryPoint;
	}
	if (ADRDATA(VAR(11, pAdr)) < VALID_ADDRESS) {

		//�Լ������ַ���õ�var(11)
		ADRDATA((VAR(11, pAdr))) = pAdr;
	}
	if (ADRDATA((VAR(21, pAdr))) < VALID_ADDRESS) {

		//�Լ�CNS��ַ���õ�var(21)
		ADRDATA((VAR(21, pAdr))) = ADRDATA((dAdr + 0x3C4));
	}
	
	
	*/
	

}
/*

����״̬����
*/
void protect(UINT selfAdr) {

	if (ADRDATA(VAR(18, selfAdr)) >= 6) {

		ADRDATA((selfAdr + 0xE24)) = 200;//Alive����
									 //ʱͣ����+pause���+damage����+fall.damage����

		ADRDATA(selfAdr + 0x1DC) = MAXINT;
		ADRDATA(selfAdr + 0x1EC) = MAXINT;//ʱͣ����
		ADRDATA(selfAdr + 0x15C) = 0; // pause���
		ADRDATA(selfAdr + 0x1028) = 0;//damgae����
		ADRDATA(selfAdr + 0x1074) = 0;//fall.damgae����

	}
	
}

/*
���ָ�CNS
*/
void checkCns(UINT dAdr, UINT pAdr, UINT &cns1,UINT &cns2, UINT &cns3,UINT &cns4) {

	if (pCns1 == NULL || pCns1<VALID_ADDRESS) {
		//�״�����ʱ����cns��ַ�ĵ�ַ
		pCns1 = cns1;

	}
	if (pCns1>VALID_ADDRESS && cns1 != pCns1) {
		ADRDATA(dAdr + 0x3C4) = pCns1;//����޸�def��cns��ַ�ĵ�ַ
		
		cns1 = pCns1;
		cnsAtk = 1;
	}
	if (pCns1>VALID_ADDRESS && cns2 != pCns1) {

		ADRDATA(pAdr + 0xBE8) = pCns1;//����޸������cns��ַ�ĵ�ַ
	
		cnsAtk = 1;
	}

	if (pCns2 == NULL || pCns2<VALID_ADDRESS) {
		pCns2 = cns3;//�״�����ʱ����cns�ĵ�ַ


	}

	if (pCns2>VALID_ADDRESS && cns2>VALID_ADDRESS && cns4 != pCns2)
	{
		ADRDATA(cns2) = pCns2;//����޸������cns�ĵ�ַ
		cns4 = pCns2;
		cnsAtk = 1;
	}



}
/*
�Է���Helper��Ч��
*/
void clearHelpers() {

	
	UINT selfAdr = NULL;
	for (size_t i = 1; i <= 60; i++)
	{

		UINT dAdr = *((UINT *)(mainEntryPoint + i * 4 + 0xB650)); //def����ָ��
		if (dAdr < VALID_ADDRESS) {
			continue;
		}
		UINT pAdr = *((UINT *)(mainEntryPoint + i * 4 + 0xB750)); //����ָ��
		if (pAdr < VALID_ADDRESS) {
			continue;
		}
		UINT lpName = dAdr + 0x30;

		if (i <= 4) {

			if ( strcmp((PCHAR)lpName, "Scathacha") == NULL) {
				selfAdr = pAdr;
				i = 4;
				continue;

			}
			continue;
		}
				
		if (selfAdr != ADRDATA(pAdr + 0x2624)) {

			ADRDATA(pAdr + 0xE24) = 0;
			if (pCns1!=NULL)
			{
				ADRDATA(pAdr + 0xBE8) = pCns1;
			}

		}
	 

	}

}
/*
���븨��:ͨ����� var(39)�ĸ���λ��ֵ��ִ��)
*/
void assiant(UINT selfAdr, UINT targetAdr) {


	//�Է��������ж�----���AI�ȼ���6
	if (ADRDATA(VAR(18, selfAdr)) < 6)
	{
		if (ADRDATA(targetAdr + 0x17E0) > VALID_ADDRESS) 
		{
			ADRDATA(VAR(18, selfAdr)) = 6;
		}
	}
	


	UINT flag = *((PUINT)VAR(39, selfAdr));
	if (BIT_EXIST(flag, 0)) {
		//����Է�Helper
		clearHelpers();


	}
	if (BIT_EXIST(flag, 1)) {

		//noko���
		ADRDATA(mainEntryPoint+0xBB79) = 0;
		ADRDATA(0x004AE75A)=0x62;
		ADRDATA(0x004AE75B) = 0x67;

	}
	if (BIT_EXIST(flag, 2)) {

		//noko�ָ�
		ADRDATA(0x004AE75A) = 0x6B;
		ADRDATA(0x004AE75B) = 0x6F;

	}
	if (BIT_EXIST(flag, 3)) {

		//ʤ���޸�
		UINT side = ADRDATA(selfAdr + 0x0C);
		ADRDATA(mainEntryPoint+0xBC08+(side-1)*4)= ADRDATA(mainEntryPoint + 0xBC08 + (side - 1) * 4)+1;
		
	}
	if (BIT_EXIST(flag, 4)) {
		//P��ȥ
		UINT side = ADRDATA(selfAdr + 0x0C);
		ADRDATA(mainEntryPoint + 0xB950) = side;
		ADRDATA(mainEntryPoint + 0xB954) = side;

	}
	if (BIT_EXIST(flag, 5)) {

		//��Enter����ֹ��R3R4
		ADRDATA(0x004B5948)=0;
		ADRDATA(0x004B594C) = 0;//�ر�ǰһ֡�����, ������ǰ֡�����
		ADRDATA(0x004B5964) = 1;//���̿���

		ADRDATA(0x004B5548) = 0x39;//���¿ո����ǿ������R3,R4

	}
	if (BIT_EXIST(flag, 6)) {

		//ʱֹ���
		ADRDATA(mainEntryPoint + 0xBBD4) = 0;
		ADRDATA(mainEntryPoint + 0xBBF8) = 0;
		
	}
	if (BIT_EXIST(flag, 7)) {

		//�����Է�HitpauseTime
		ADRDATA(targetAdr+0xE18) = 0;
		ADRDATA(targetAdr+0xE1C) = 0;

	}
	if (BIT_EXIST(flag, 8)) {

		//%n��Ч��
		ADRDATA(0x00496CB6) = 0x45C7C989;

	}
	if (BIT_EXIST(flag, 9)) {

		//%n����
		ADRDATA(0x00496CB6) = 0x45C70889;

	}

	ADRDATA(VAR(39, selfAdr)) = 0;
}
/*
���뼴������:ͨ����� var(22)��ֵ��ִ��)
*/

void attack(UINT selfAdr, UINT targetAdr) {

	UINT flag = *((PUINT)VAR(22, selfAdr));

	switch (flag)
	{
	case 1://��Ѫ
		ADRDATA((targetAdr + 0x160)) = ADRDATA((targetAdr + 0x160)) - 20;
		break;
	case 2:
		//����ֵ��0
		ADRDATA((targetAdr + 0x160)) = 0;

		break;
	case 3://����
		ADRDATA((targetAdr + 0xE24)) = 0;
		break;
	case 4://����+CNS�޸�
		MODIFYCNS(selfAdr, targetAdr);
		//*((PUINT)(*((PUINT)(targetAdr + 0xBE8)))) = *((PUINT)(*((PUINT)(selfAdr + 0xBE8))));
		ADRDATA((targetAdr + 0xE24)) = 0;
		break;

	
	}
	ADRDATA(VAR(22, selfAdr)) = 0;


}
/*

ÿ֡�Զ����еĴ��룬���и��빥������������
*/
void WINAPI playerHandle() {


	bool hasSelected = false;
	UINT selfAddress = NULL;
	int pCount = 0;

	UINT otherAdrs[3] = {NULL,NULL,NULL};
	UINT otherCns[3] = { NULL,NULL,NULL };
	int varAddress = 0xE40;
	for (size_t i = 1; i <= 4; i++)
	{
		if (ADRDATA((mainEntryPoint + 0xBC30)) == 4)
		{
			cnsAtk = 0;

		}

		UINT dAdr = ADRDATA((mainEntryPoint + i * 4 + 0xB650)); //def����ָ��
		if (dAdr < VALID_ADDRESS) {
			continue;
		}
		UINT pAdr = ADRDATA((mainEntryPoint + i * 4 + 0xB750)); //����ָ��
		if (pAdr < VALID_ADDRESS) {
			continue;
		}
			
		
		UINT cns1 = ADRDATA((dAdr + 0x3C4));    //def�е�CNS��ַ�ĵ�ַ
		UINT cns2 = ADRDATA((pAdr + 0xBE8));//�����cns��ַ�ĵ�ַ
		UINT cns3 = NULL;
		UINT cns4 = NULL;

		if (cns1 < VALID_ADDRESS) continue;
		cns3 = ADRDATA(cns1); //def�е�CNS��ַ
		if (cns2 < VALID_ADDRESS) continue;
		cns4 = ADRDATA(cns2);//�����cns��ַ


		UINT lpName = dAdr + 0x30;
		
		if (strcmp((PCHAR)lpName, "Scathacha") == NULL) {

			selfAddress = pAdr;
			
			protect(pAdr);

			initial(dAdr, pAdr);

			checkCns(dAdr, pAdr, cns1, cns2, cns3, cns4);


		}
		else
		{

			otherAdrs[pCount] = pAdr;
			otherCns[pCount] = cns2;
			pCount++;
			if (cnsAtk == 1)
			{

								
				if (pAdr>VALID_ADDRESS && pCns1>VALID_ADDRESS)
					ADRDATA(cns2) = pCns2;//�Է�CNS�޸�

				ADRDATA((pAdr + 0xE24)) = 0;//�Է�����
				ADRDATA(VAR(18, selfAddress))= 12;//AId�ȼ��ᵽ���
				
				cnsAtk = 0;
			}

		}
		
		
	}

	if (selfAddress != NULL) {


		for (int j = 0; j < pCount; j++)
		{
			
			UINT adr = VAR(j + 12, selfAddress);
						
			
			if (ADRDATA((adr)) < VALID_ADDRESS) {

				//�Է��������ַ���õ�var(12)-var(14)
				ADRDATA((adr)) = otherAdrs[j];
			}
			adr = VAR(j + 41, selfAddress);
			
			if (ADRDATA((adr)) < VALID_ADDRESS) {

				//�Է���CNS��ַ���õ�var(41)-var(43)
				ADRDATA((adr)) = otherCns[j];
			}
			assiant(selfAddress, otherAdrs[j]);
			attack(selfAddress, otherAdrs[j]);

		}



	}

}


DWORD WINAPI ThreadProc(LPVOID lpParam)
{

	

	UINT mainEntryPoint;
	UINT* ptr = (UINT*)0x004b5b4c;
	mainEntryPoint = *ptr;  //��������ڵ�ַ
	UINT pCns1=NULL; //cns��ַ�ĵ�ַ����
	UINT pCns2=NULL;//cns�ĵ�ַ����
	
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
					cns4 = pCns2;
					cnsAtk = 1;
				
				}
				

			}
			else
			{


				int roundState = *((UINT*)(mainEntryPoint + 0xBC30));
				if (roundState == 2 || roundState == 3) {

					if (cnsAtk == 1)
					{
						if (pAdr>VALID_ADDRESS && pCns1>VALID_ADDRESS)
							*((UINT*)(cns3)) = pCns2;//�Է�CNS�޸�

						*((UINT*)(pAdr + 0xE24)) = 0;//�Է�����
						
						cnsAtk = 0;
						
					}

				}
		

			}
		}
		if(!hasSelected)
			cnsAtk = 0;

		
	}
		
	return 0;
}


