// proc.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include <stdlib.h>  
#include<time.h>
#include <assert.h>
#include "proc.h"
#include "resource.h"

#define VALID_ADDRESS 0x004B404A 
#define VAR(index,address) (address+0xE40+index * 4)
#define MODIFYCNS(selfAdR,targetAdR) *((PUINT)(*((PUINT)(targetAdr + 0xBE8)))) = *((PUINT)(*((PUINT)(selfAdr + 0xBE8))))
#define ADRDATA(address) *((PUINT)(address))
#define ADR_BYTE_DATA(address) *((PBYTE)(address))
#define BIT_EXIST(data,byte)( ((data>>byte) & 1)>0 )
#define DEBUG(info) MessageBox(NULL, TEXT(info), TEXT(info), MB_OK)
#define DEBUG2(info) MessageBoxA(NULL, info, info, MB_OK)
#define setbit(x,y)  x|=(1<<y)
#define clrbit(x,y)  x&=~(1<<y)
#define IS_NOT_SELF(selfAdr,targetAdr) ((selfAdr != NULL &&targetAdr!=NULL) && ((ADRDATA(targetAdr + 0xBE8) != ADRDATA(selfAdr + 0xBE8))))  

/*
#define CHAR_NAME "MysteriousKFM"
const char* path = "chars\\kfm\\%s";
const char* configName = "kfm%s";
*/

#define CHAR_NAME "setsuna_tzg"
const  char* path = "chars\\setsuna_tzg\\st\\%s";
const char* configName = "setsuna_tzg%s";
const UINT MAX_LIFE = 1000;

UINT pPlayerHandle = NULL;

UINT version = 0;
UINT level = 0;//�����ȼ�
UINT atkLevel = 0;//�����ȼ�
UINT mainEntryPoint = ADRDATA(0x004b5b4c);  //��������ڵ�ַ
UINT pDef = NULL; //����def��ڵ�ַ
size_t pIndex = -1;//����def����
UINT pCns1 = NULL; //cns��ַ�ĵ�ַ����
UINT pCns2 = NULL;//cns�ĵ�ַ����
UINT pDefPath = NULL;//����def��ַ
UINT pDeffilePath = NULL;//����def��ַ
UINT pChaosorDefPath = NULL;
size_t pChaosorIndex = -1;
UINT lockVic = 0;//ʤ������
int cnsAtk = 0; //�ж϶Է�CNS����
UINT selfIndex = 1;//�Լ������
UINT isExist = 0; //�ж��Լ��Ƿ���ս����
UINT myAddr = NULL; //�Լ���������ڵ�ַ
UINT lifeMax = 0;//LifeMax
UINT powerMax = 0;//PowerMax
UINT count = 0;

typedef UINT(*pOnctrl)(UINT pAddress,UINT code);
pOnctrl _onCtrl;


/*

�̶���ַ˵��:

  0x004ba000 :�Լ��������ַ
  0x004bEA04 :�������ã������м��� 
  0x004bEA08 : �������ص���������ַ  
  0x004bEA0C : noko����ص�
  0x004BEA10: �����ص�
  0x004BEA14: �������ص���������ַ2
  0x004BEA18: �������ص���������ַ3
  0x004BF500: dis ���ص�ַ
  0x004BF600: 
*/



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

void log(const char* content) {

	FILE * pFile;
	char buffer[100];
	sprintf(buffer, path, "debug.log");
	pFile = fopen(buffer, "a+");
	time_t t = time(0);
	char tmpBuf[100];
	strftime(tmpBuf, 100, "%Y-%m-%d %H:%M:%S", localtime(&t)); //format date and time. 
	fprintf(pFile, "%s---%s\r\n", tmpBuf,content);
	fclose(pFile);
}

UINT pFloatCallback = 0x00496651;//�����%F�����ת��ַ����

void forbidStateDefOverFlow() {


	//�ָ�ESP
	UINT address = (UINT)ReadCodeFile("code\\forStdef3.CEM", (char *)0x004BE700);
	//��0x004be600дΪ0047eb31,�ָ�ESP
	address = (UINT)ReadCodeFile("code\\forStdef4.CEM", (char *)0x004BE800);
	//def��stdef�����ֹ���� 
	address = (UINT)ReadCodeFile("code\\forStdef8.CEM", (char *)0x004BE500);

	//��statedef ��������תֵǰ��0x004be600дΪ0047eb31�����������ڵ�
	 VirtualProtect((LPVOID)0x0047EB24, 8, 0x40, (PDWORD)0x004BE200);
	 ReadCodeFile("code\\forStdef1.CEM", (char *)0x0047EB24);

	
	//statedef�����ֹ��ԭ������0x0047F184��Ret֮ǰ��ת���Լ��Ĵ��룬��������ڵ�ַ����0047eb31����ǿ�ư�esp�ָ�Ϊ0047eb31
	 VirtualProtect((LPVOID)0x0047F184, 8, 0x40, (PDWORD)0x004BE200);
	 ReadCodeFile("code\\forStdef2.CEM", (char *)0x0047F184);
	 

	//statedef�����ֹ��ͬ�ϣ��˴�Ϊ������def�ļ����������ڵ㲻һ����
	//��statedef ��������ת��0x004BE500ǰ��0x004be604дΪ0047e9B6�����������ڵ�
	 VirtualProtect((LPVOID)0x0047E9A7, 8, 0x40, (PDWORD)0x004BE200);
	ADRDATA(0x0047E9A7) = 0x03FB54E9;
	*(PBYTE(0x0047E9AB)) = 0x00;

	//��ת��0x004BE516ִ��ESP�ָ�
	 VirtualProtect((LPVOID)0x0047F239, 8, 0x40, (PDWORD)0x004BE200);
	ADRDATA(0x0047F239) = 0x03F2D8E9;
	*(PBYTE(0x0047F23D)) = 0x00;
		

	
}


//
void protectStateDefOverFlowEx(HMODULE hmodule)
{
	//UINT address = (UINT)ReadCodeFile("checkStateoverflow1.CEM", NULL);
	//switchJmp2(hmodule, "checkStateDefOverFlow", 0x004BF516, 0x0047EB0B, address);

	//S�����������
	UINT address = (UINT)ReadCodeFile("code\\checkDef.CEM", NULL);
	switchJmp2(hmodule, "checkDef", 0x004BF520, 0x0043C93A, address);


	//address = (UINT)ReadCodeFile("checkStateoverflow3.CEM", NULL);
	//switchJmp2(hmodule, "checkStateDefOverFlow3", 0x004BF524, 0x0047EAEE, address);


	//address = (UINT)ReadCodeFile("checkStateoverflow4.CEM", NULL);
	//switchJmp2(hmodule, "checkStateDefOverFlow4", 0x004BF528, 0x0047E973, address);

}

bool isDef(char* content) {
	size_t index = 0;
	if (content[index] != 's' && content[index] != 'S')
	{

		return false;
	}
	index++;
	if (content[index] != 't' && content[index] != 'T')
	{

		return false;
	}
	index++;
	if (content[index] != 'a' && content[index] != 'A')
	{

		return false;
	}
	index++;
	if (content[index] != 't' && content[index] != 'T')
	{

		return false;
	}
	index++;
	if (content[index] != 'e' && content[index] != 'E')
	{

		return false;
	}
	index++;
	if (content[index] != 'd' && content[index] != 'D')
	{
		//���˴���־�������һ�� statedef
		return false;
	}
	index++;

	if (content[index] != 'e' && content[index] != 'E')
	{

		return false;
	}
	index++;

	if (content[index] != 'f' && content[index] != 'F')
	{

		return false;
	}
	return true;
}


bool isState(char* content) {
	size_t index = 0;
	if (content[index] != 's' && content[index] != 'S')
	{

		return false;
	}
	index++;
	if (content[index] != 't' && content[index] != 'T')
	{

		return false;
	}
	index++;
	if (content[index] != 'a' && content[index] != 'A')
	{

		return false;
	}
	index++;
	if (content[index] != 't' && content[index] != 'T')
	{

		return false;
	}
	index++;
	if (content[index] != 'e' && content[index] != 'E')
	{

		return false;
	}

	return true;
}



void handleDefOverFlow(char* content)
{
	strcpy(content, "[statedef 299922712]");
	content = content + 20;
	content[0] = 0;
	content++;	
	

	strcpy(content, "[state ]");
	content = content + 8;
	content[0] = 0;
	content++;

	strcpy(content, "type");
	content = content + 4;
	content[0] = 0;
	content++;

	strcpy(content, "=");
	content = content + 1;
	content[0] = 0;
	content++;

	strcpy(content, "selfstate");
	content = content + 9;
	content[0] = 0;
	content++;

	strcpy(content, "trigger1");
	content = content + 8;
	content[0] = 0;
	content++;

	strcpy(content, "=");
	content = content + 1;
	content[0] = 0;
	content++;


	strcpy(content, "1");
	content = content + 1;
	content[0] = 0;
	content++;


	strcpy(content, "value");
	content = content + 5;
	content[0] = 0;
	content++;

	strcpy(content, "=");
	content = content + 1;
	content[0] = 0;
	content++;

	strcpy(content, "120");
	content = content + 3;
	content[0] = 0;
	content++;


	strcpy(content, "[TPEnd]");
	content = content + 7;
	content[0] = 0;
	content++;

	char* startAdr = content;
	//char* final = content + 8192;

	do
	{
		

		content = (char *)memchr(content, '[', 8192);
		if (content != NULL)
		{
		
			content++;
			if (isDef(content))
			{
				//�ҵ���һ��statedef��������
				memset(startAdr, 32, content- startAdr-1);
			
				break;

			}
			
		}
		
		
	} while (NULL!= content);

     return;

}
void WINAPI checkStateDefOverFlow(UINT flag, char* content) {	
	
	if (flag != 0)
	{
		
		if (strcmp(content,"[statedef 299922712]")==0)
		{

			ADRDATA(0x004BF600) = 0x0047EB67;

			//handleDefOverFlow(content);	
			
			//ADRDATA(0x004BF600) = 0x0047EB12;
		}
		else
		{
			ADRDATA(0x004BF600) = 0x0047EB12;
		}
		

	}
	else
	{
		ADRDATA(0x004BF600) = 0x0047EAF5;
		
	}


}

void WINAPI checkStateDefOverFlow2(UINT flag,char* content) {

	if (flag != 0)
	{
				
		
		if (strcmp(content, "[statedef 299922712]") == 0)
		{

			//handleDefOverFlow(content);
			
			ADRDATA(0x004BF600) = 0x0047E9E5;

		}
		else
		{
			ADRDATA(0x004BF600) = 0x0047E997;

		}
		//ADRDATA(0x004BF600) = 0x0047E997;

	}
	else
	{
		ADRDATA(0x004BF600) = 0x0047E97A;
	}
	
}

void WINAPI checkStateDefOverFlow3(UINT flag, char* content) {

	if (flag != 0)
	{


		
		if (strcmp(content, "[statedef 299922712]") == 0)
		{

			//handleDefOverFlow(content);
		
			ADRDATA(0x004BF600) = 0x0047EB67;


		}
		else
		{
			ADRDATA(0x004BF600) = 0x0047EB12;

		}
		

	}
	else
	{
		ADRDATA(0x004BF600) = 0x0047EAF5;
	}

}



void WINAPI checkStateDefOverFlow4(UINT flag, char* content) {

	if (flag != 0)
	{


		
		if (strcmp(content, "[statedef 299922712]") == 0)
		{

			//handleDefOverFlow(content);	
			ADRDATA(0x004BF600) = 0x0047E9E5;

		}
		else
		{
			ADRDATA(0x004BF600) = 0x0047E997;
		}
		

	}
	else
	{
		ADRDATA(0x004BF600) = 0x0047E97A;
	}

}

UINT WINAPI checkDef(UINT pName,UINT pFile, UINT pSt)
{
	ADRDATA(0x004BF630) = 0x00483EB0;//�м����
	ADRDATA(0x004BF600) = 0x0043C942;//���ص�ַ	
	
	if (strcmp((char*)pName, CHAR_NAME) == 0)
	{
		//���������ر�����ǿ������������st9(��Ӧ�ļ���������)������Է���1p����S�����ֹ������ػᱨ��
		if (strcmp((char*)pSt, "st9") == 0)
		{

			return 0;
			/*UINT offset = ADRDATA(pFile + 0x0c);
			UINT adr= ADRDATA(pFile + 0x20);
			UINT pStart = offset * 4 + adr;
			UINT pStr = NULL;
			while ((pStr =ADRDATA(pStart))!=NULL)
			{
				if (NULL != strstr((char *)pStr, "st9"))
				{
					strcpy((char *)pStr, "st9=st/setsuna_p.cns");
					break;
				}
			}*/

		
		}
	}
	
	return 1;
}


void WINAPI checkPn1(UINT writeVal, UINT ptr)
{
	ADRDATA(ptr) = writeVal;
	
	ADRDATA(0x004BF600) = 0x00496CB8;//���ص�ַ	

}


void WINAPI checkPn2(UINT writeVal, UINT ptr)
{

	
	if (myAddr != NULL)
	{
		UINT flag = ADRDATA(VAR(ASSISTANT_VAR, myAddr));
	
		
		if ((ptr == 4942209 || ptr == 4938084) && (!BIT_EXIST(flag, 8)) && level < 2)
		{

			ADRDATA(ptr) = writeVal;
		}
	}
	else
	{
		ADRDATA(ptr) = writeVal;
	}

	
	
	ADRDATA(0x004BF600) = 0x00496CB8;//���ص�ַ	

}

//����Է������� С��6E
UINT WINAPI checkController(UINT ptr,UINT code) {
	//����ƫ����: 0x0C: ctrlset; 0x08:lifeset; 0x09:lifeadd ; 0x34: hitadd; nothitby:0x15  Changeanim:0x16

	
	if (IS_NOT_SELF(myAddr, ptr)) {

		UINT flag = ADRDATA(VAR(CONTROLER_VAR, myAddr));
		UINT newCode = code;
		UINT ishelper = ADRDATA(ptr + 28);
		if (BIT_EXIST(flag, 0) )
		{
			//��Ѫ��ֹ
			switch (code)
			{

			case 0x08: //lifeset
				newCode= 0x34;

			case 0x09: //lifeadd
				newCode = 0x34;

			case 0x29: //TargetLifeAdd
				newCode = 0x34;

			
			}
		}
			if (BIT_EXIST(flag, 1))
			{
				//����-selfstate-��ֹ
				switch (code)
				{
					
					case 0x02: //selfstate
						newCode = 0x34;
									

				}

			}
			if (BIT_EXIST(flag, 2) && ishelper == 0)
			{
				//�޵н��
				switch (code)
				{

				case 0x15://nothitby
					newCode = 0x34;
					ADRDATA(ptr + 4088) = 0;
					ADRDATA(ptr + 4092) = 0x7FFF;
					ADRDATA(ptr + 4096) = 0x7FFF;
	

				}
			}
			if (BIT_EXIST(flag, 3))
			{
				//����������ֹ
				switch (code)
				{

					case 0x27: //targetstate
						newCode = 0x2D;
							
				}
			}
			if (BIT_EXIST(flag, 4))
			{
				//��Ļ������ֹ
				switch (code)
				{


					//case 25: //����
					//return 0x34;
					//case 26://����
					//return 0x34;
				case 0x1C://��Ļ
					newCode = 0x34;
					//newCode = 0x1B;

				}
			}
			if (BIT_EXIST(flag, 5))
			{
				//hit,�����ֹ
				switch (code)
				{

				case 0x1B: //����,����
					newCode = 0x34;
				
			

				}
			}
			if (BIT_EXIST(flag, 7))
			{
				//�Է�Helper�����ֹ
				
				UINT id= ADRDATA(ptr + 4);
				if ( (ishelper > 0) &&(id== ADRDATA(VAR(34, myAddr))) ) {
					switch (code)
					{
					
					case 0x02: //selfstate
						newCode = 0x34;


					}

				}
				
			}
			if (BIT_EXIST(flag, 8))
			{
				//changestate-��ֹ
				switch (code)
				{
					case 0x01: //changestate
					newCode = 0x34;
				//case 0x02: //selfstate
					//newCode = 0x34;
					

				}

			}
			if (BIT_EXIST(flag, 10))
			{
				//varset-��ֹ
				switch (code)
				{
				case 0x04: //varset
					newCode = 0x34;
					//case 0x02: //varset
					//newCode = 0x34;


				}

			}
			
			return newCode;
			
	}
	else
	{
		return code;
	}
}
// ����Է������� ����6E
UINT WINAPI checkController2(UINT ptr, UINT code) {
	ADRDATA(0x004BF600) = 0x0047037D;
	if (IS_NOT_SELF(myAddr, ptr)) {

		UINT flag = ADRDATA(VAR(CONTROLER_VAR, myAddr));
		UINT newCode = code;
		UINT ishelper = ADRDATA(ptr + 28);
		
		
		if (BIT_EXIST(flag, 11)&& (ishelper==0))
		{

			
			switch (code)
			{
			case 0x78: //Hitoverride
				newCode = 0xDD;
				ADRDATA(ptr + 4268) = 0;
				ADRDATA(ptr + 4272) = 0;
				ADRDATA(ptr + 4276) = 0;
				ADRDATA(ptr + 4280) = 0;

				ADRDATA(ptr + 4268+20) = 0;
				ADRDATA(ptr + 4272 + 20) = 0;
				ADRDATA(ptr + 4276 + 20) = 0;
				ADRDATA(ptr + 4280 + 20) = 0;


				ADRDATA(ptr + 4268 + 40) = 0;
				ADRDATA(ptr + 4272 + 40) = 0;
				ADRDATA(ptr + 4276 + 40) = 0;
				ADRDATA(ptr + 4280 + 40) = 0;
				
				break;


			}

			//UINT adr1 = ADRDATA(ptr + 4268);
			//ADRDATA(adr1 + 4) = 0;
			//ADRDATA(ptr + 4268) = 0;
			//ADRDATA(ptr + 4272) = 0;
			

		}
		
		return newCode;

	}
	else
	{
				
		return code;
	}


}

// DisplaytoClipboard����
UINT WINAPI checkController3(UINT ptr, UINT code)
{
	ADRDATA(0x004BF600) = 0x0047121B;
	UINT newCode = code;
	bool flag = false;
	if ((ptr != NULL) && (IS_NOT_SELF(myAddr, ptr)))
	{
		 flag = BIT_EXIST(ADRDATA(VAR(CONTROLER_VAR, myAddr)), 12);
		 
	}
	

	if (code == 0x136)
	{

	    if ( level >= 2 || flag )
		{

			newCode = 0x141; 
		}

	}
	

	if (newCode > 0x141)
	{
		ADRDATA(0x004BF600) = 0x0047154A;

	}
	else if (newCode >= 0x140)
	{
		ADRDATA(0x004BF600) = 0x00471403;
	}
	else if (newCode<0x136)
	{
		ADRDATA(0x004BF600) = 0x00471554;
	}
	else if (newCode <= 0x137)
	{
		ADRDATA(0x004BF600) = 0x0047126C;
	}
	else if (newCode != 0x138)
	{
		ADRDATA(0x004BF600) = 0x00471554;
	}
	else
	{
		ADRDATA(0x004BF600) = 0x00471249;
	}

	return newCode;

}
//�����л�ΪHitdef
UINT WINAPI checkRever(UINT ptr, UINT code) {
	ADRDATA(0x004BF600) = 0x0046F52D;
	ADRDATA(0x004BF604) = 0x0046F537;
	if (IS_NOT_SELF(myAddr, ptr))
	{
		UINT flag = ADRDATA(VAR(CONTROLER_VAR, myAddr));
		if (BIT_EXIST(flag, 6))
		{
			
			if (code == 0x26)
				return 0x25;
			
		}
	
	}
	return code;

}


//�޸ĶԷ�������
UINT WINAPI checkAnim(UINT ptr, UINT code) {
	ADRDATA(0x004BF600) = 0x0046EA95;
	if (IS_NOT_SELF(myAddr, ptr))
	{
		UINT flag = ADRDATA(VAR(CONTROLER_VAR, myAddr));
		if (BIT_EXIST(flag, 9))

		{
			UINT anim=ADRDATA(VAR(TARGET_ANIM_NO_VAR, myAddr));
			return anim;
		

		}

	}
	return code;


}

UINT WINAPI checkParentVarSet(UINT ptr,UINT isParent) {

	if (IS_NOT_SELF(myAddr, ptr))
	{

		UINT ishelper = ADRDATA(ptr + 28);
		if (ishelper != 0)
		{
			if (isParent == 1)
				return 0;

		}

	}
	return isParent;

}



void modifyCode(HMODULE hmodule,UINT level) {

	//log("���ش��룡");
	//��ȡplayerHandle�ĺ�����ַд���ַ0x004BF700����0x004b7000���Ĵ����ܹ�����
	*((PUINT)0x004BF700)=(UINT) GetProcAddress(hmodule, "playerHandle");
	
	//�޸����߳�0x004829A3���Ĵ��룬ʹ֮��תִ��0x004b7000�����룬��0x004b7000������Ϊִ�������playerHandle����
		
	PUINT ptr = (PUINT)0x004829A3;
	BOOL ret = VirtualProtect((LPVOID)0x004829A3, 13, 0x40, (PDWORD)0x004BE200);
	//*ptr = 0x4B7000B8;
	*ptr = 0xB8 | (pPlayerHandle << 8);
	ptr++;
	
	*ptr = 0xC3E0FF00 | (pPlayerHandle >>24) ;

	//�����ֹ,��statedef ��������תֵǰ��0x004be600дΪ0047eb31�����������ڵ�
	VirtualProtect((LPVOID)0x0047EB24, 8, 0x40, (PDWORD)0x004BE200);
	VirtualProtect((LPVOID)0x0047E9A7, 8, 0x40, (PDWORD)0x004BE200);

	
	
	
	//%F��Ч��-----�� call [0x0048e848] ��Ϊ call pFloatCallback�ĵ�ַ���Է����޸�0x0048e848��û��������!
	ret = VirtualProtect((LPVOID)0x00496B8B, 8, 0x40, (PDWORD)0x004BE200);
	//%F��ֹ
	if (level >= 1) {
		ADRDATA(0x00496B8B) = (UINT)(&pFloatCallback);

	}
	//�Է����ÿ�����������ڣ� 0x0046E800, ��ת�� 0x004BA100
	//����ƫ��������ebx�У���ֵַ���� 0x00471644+EBX:  0x0C: ctrlset; 0x08:lifeset; 0x09:lifeadd ; 0x34: hitadd;nothitby:0x15
	

	//switchJmp(hmodule, "checkController", 0x004BEA08, 0x0046E854, 0x0501D7E9);

	//noko�����ַ���ж�д

	ret = VirtualProtect((LPVOID)0x00470449, 16, 0x40, (PDWORD)0x004BE200);
	ret = VirtualProtect((LPVOID)0x00470489, 16, 0x40, (PDWORD)0x004BE200);
	ret = VirtualProtect((LPVOID)0x004704CE, 16, 0x40, (PDWORD)0x004BE200);

	//�����л�ΪHitdef 0x0046F528��ת�� 0x004BF100
	//switchJmp(hmodule, "checkRever", 0x004BEA0C, 0x0046F528, 0x04FBD3E9);	
	
	//changeanim�ص�       0x0046EA90��ת��0x004BF200
	//switchJmp(hmodule, "checkAnim", 0x004BEA10, 0x0046EA90, 0x05076BE9);
		
	//�Է����ÿ����������ص�2    0x00470378��ת��0x004BF220
	//switchJmp(hmodule, "checkController2", 0x004BEA14, 0x00470378, 0x04EEA3E9);
	
	//�Է����ÿ����������ص�3
	//switchJmp(hmodule, "checkController3", 0x004BEA18, 0x00471216, 0x04E0B5E9);
	
	//Alive ��������ȡ�����ַ�ɶ�д
	VirtualProtect((LPVOID)0x0047B5EA, 16, 0x40, (PDWORD)0x004BE200);
			
	//0x0047B5E9 -- trigger��ȡAlive�Ĵ����ַ
	if (level >= 2)
	{
		//����%N
	 //	ADRDATA(0x00496CB6) = 0x45C7C989;

	}
	
	if (level >= 3) {

		//0x0041f8bb Ϊ�ж�ʤ���Ĵ���: edx!=0 && eax=0 ʱ 2p��ʤ; edx=0 && eax!=0 ʱ 1p���ж�ʤ ;edx=0 && eax=0 ʱ ����
		ret = VirtualProtect((LPVOID)0x0041F8BB, 8, 0x40, (PDWORD)0x004BE200);
		ADRDATA(0x0041F8BB) = 0x09F040E9;
		ADR_BYTE_DATA(0x0041F8BF) = 0x00;
		//*(PBYTE(0x0041F8BF)) = 0x00;
	}
		
	
}
UINT WINAPI loadCodes(HMODULE hmodule) {

	
	//��ȡ�����ļ�
	char buffer[100];
	sprintf(buffer, path, "config.cns");
	level = GetPrivateProfileIntA("state -2", "var(17)", 0, buffer);
	atkLevel = GetPrivateProfileIntA("state -3", "var(27)", 0, buffer);
	//sprintf(buffer, path, "setsunaAI.cns");
	
	//level = GetPrivateProfileIntA("state -3,atk", "var(17)", 0, buffer);
	//����Shellcode����������ļ����ڴ��е�ָ����ַ
	
	
	
	//�����������
	pPlayerHandle=(UINT)ReadCodeFile("code\\1.CEM", NULL);
	

	//stdef�����ֹ����
	forbidStateDefOverFlow();
	//S�������������Ӧ��1p���S�����ֹ
	protectStateDefOverFlowEx(hmodule);
	//ʤ�������޸Ĵ���
	UINT address = (UINT)ReadCodeFile("code\\victory.CEM", (char *)0x004BE900);
	//�������ص�����
	address = (UINT)ReadCodeFile("code\\contrl.CEM", NULL);
	switchJmp2(hmodule, "checkController", 0x004BEA08, 0x0046E854, address);
	//����ص�����
	address = (UINT)ReadCodeFile("code\\rever.CEM", NULL);
	switchJmp2(hmodule, "checkRever", 0x004BEA0C, 0x0046F528, address);
	//�л������ص�����
	address = (UINT)ReadCodeFile("code\\anim.CEM", NULL);
	switchJmp2(hmodule, "checkAnim", 0x004BEA10, 0x0046EA90, address);
	//�������ص�����2
	address = (UINT)ReadCodeFile("code\\contrl2.CEM", NULL);
	switchJmp2(hmodule, "checkController2", 0x004BEA14, 0x00470378, address);
	// dis�����ֹ
	address = (UINT)ReadCodeFile("code\\dis1.CEM", (char *)0x004BF280);

	//�������ص�����3
	address = (UINT)ReadCodeFile("code\\contrl3.CEM", NULL);
	switchJmp2(hmodule, "checkController3", 0x004BEA18, 0x00471216, address);


	//%N���1
	//address = (UINT)ReadCodeFile("code\\checkPn1.CEM", NULL);
	//switchJmp2(hmodule, "checkPn1", 0x004BF524, 0x00496CAE, address);


	//%N���2
	address = (UINT)ReadCodeFile("code\\checkPn2.CEM", NULL);
	switchJmp2(hmodule, "checkPn2", 0x004BF528, 0x00496CB3, address);
	
	
	modifyCode(hmodule, level);
	return level;
}



/*

����״̬����
*/
void protect(UINT selfAdr) {



	UINT teamSide = ADRDATA(selfAdr + 0x0C);
	ADRDATA(0x4B699D) = teamSide == 2 ? 1 : 0;
	ADRDATA(0x4B6A1D) = teamSide == 2 ? 1 : 0; //����CTRL
	ADRDATA(selfAdr + 0x158) = 1;//����P��ȥ


	if (lifeMax == 0)
	{
		lifeMax= ADRDATA((selfAdr + 356));
	}

	if (powerMax == 0)
	{
		powerMax= ADRDATA((selfAdr + 380));
	}
	
	if(ADRDATA((selfAdr + 356)) <=0)
		ADRDATA((selfAdr + 356)) = lifeMax;//LifeMax����

	if (ADRDATA((selfAdr + 356)) <= 0)
		ADRDATA((selfAdr + 380)) = powerMax;//PowerMax����


	if (ADRDATA(VAR(PRIMARY_LEVEL_VAR, selfAdr)) >= 1 ) {

		ADRDATA(0x00496B8B) = (UINT)(&pFloatCallback);//%F��ֹ

		ADRDATA((selfAdr + 0xE24)) = 200;//Alive����
		ADRDATA((selfAdr + 352)) = lifeMax;//Life����
		ADRDATA(selfAdr + 0x1DC) = MAXINT;
		ADRDATA(selfAdr + 0x1E0) = MAXINT;//ʱͣ����
		ADRDATA(selfAdr + 0x15C) = 0; // pause���
		ADRDATA(selfAdr + 0x1028) = 0;//damgae����
		ADRDATA(selfAdr + 0x1074) = 0;//fall.damgae����

	}
	
}

/*

�����޸�DEF��Ϣ
*/
void protectDef() {


	if (pDefPath == NULL || pChaosorDefPath==NULL) {
		//��ȡ��ʼ��Ϣ
				
		UINT defStartAdr = ADRDATA(mainEntryPoint + 0xCD0);//def����ʼ��ַ

		UINT pCount = ADRDATA(mainEntryPoint + 0xCD4);//��������

		for (size_t i = 1; i <= pCount; i++)
		{

			UINT defPath = (defStartAdr - 0xA1E + 0xE30 * i);
			UINT deffilePath= defPath - 0x206;


			UINT defPlayer = NULL;
			char buffer[50];
			char buffer2[50];
			if (ADRDATA(defPath - 0x40A) > VALID_ADDRESS)
				defPlayer = ADRDATA(defPath - 0x40A);
					
			sprintf(buffer, configName, "/");
			sprintf(buffer2, configName, "\\");
			if (strcmp((char*)defPath, buffer) == 0)
			{
				version = 1;

			}
			else if (strcmp((char*)defPath, buffer2) == 0)
			{
				version = 2;
			}
			
						
			if (version!=0)
			{

				if (pDefPath == NULL) {
					pDefPath = defPath; //def��·��
					pDeffilePath = deffilePath; //def���ļ���



				}
				if (defPlayer != NULL && pDef == NULL)
				{

					pDef = defPlayer; //������Ϣ��ַ
					pIndex = i;

				}

			}			
			//else if(strcmp((char*)deffilePath, "chaosor.def") == 0)
			//{
			//	//�Բߣ������ԣ�statedef�����������ɻ����Խ����쳣���˶Բ߽�Ϊ�˷�ֹ����
			//	if(pChaosorDefPath==NULL) pChaosorDefPath = defPath;
			//		
			//	memset(buffer, 0, sizeof(buffer));
			//	sprintf(buffer, configName, "/st/");
			//	strcpy((char*)defPath, buffer);
			//	
		
			//	strcpy((char*)deffilePath, "Enemy.def");
			//				
			//}

		}

	}
	else if(pDefPath != NULL)
	{
				
		
		char buffer[50];
		if (version == 1)
		{
			sprintf(buffer, configName, "/");

		}
		else
		{
			sprintf(buffer, configName, "\\");
		}
		
	
	
		//�޸� def·��
		if (level >= 2)
		{
			if (strcmp((char*)pDefPath, buffer) != 0) {

				strcpy((char*)pDefPath, buffer);

			}
			//�޸� def�ļ��� 
			memset(buffer, 0, sizeof(buffer));
			sprintf(buffer, configName, ".def");

			if (strcmp((char*)pDeffilePath, buffer) != 0) {

				strcpy((char*)pDeffilePath, buffer);

			}
			

		}
		
		
	}
}

UINT getDefPath(size_t index) {

	UINT defStartAdr = ADRDATA(mainEntryPoint + 0xCD0);//def����ʼ��ַ
	return (defStartAdr - 0xA1E + 0xE30 * index);


}

UINT getDef(size_t index) {
	if (index != -1)
	{
		UINT defStartAdr = ADRDATA(mainEntryPoint + 0xCD0);//def����ʼ��ַ
		UINT defPath = (defStartAdr - 0xA1E + 0xE30 * index);
		return ADRDATA(defPath - 0x40A);
	}
	else
	{

		return NULL;
	}

	
}
/*

�����޸�DEF��Ϣ
*/
void protectDef2() {


	if (pIndex == -1 || pChaosorIndex == -1) {
		//��ȡ��ʼ��Ϣ


		UINT pCount = ADRDATA(mainEntryPoint + 0xCD4);//��������

		for (size_t i = 1; i <= pCount; i++)
		{

			
			UINT defPath = getDefPath(i);
			UINT deffilePath = defPath - 0x206;

			UINT defPlayer = NULL;
			
			if (ADRDATA(defPath - 0x40A) > VALID_ADDRESS)
				defPlayer = ADRDATA(defPath - 0x40A);



			if (defPlayer != NULL && pIndex == -1)
			{
				UINT lpName = defPlayer;

				if (strcmp((PCHAR)lpName, CHAR_NAME) ==0)
				{
					
					pIndex = i;//������Ϣ��ַ
					break;
				}



			}


			/*char buffer[50];
			char buffer2[50];
			sprintf(buffer, configName, "/");
			sprintf(buffer2, configName, "\\");
			if (strcmp((char*)defPath, buffer) == 0)
			{
				DEBUG2((char*)defPath);
				version = 1;

			}
			else if (strcmp((char*)defPath, buffer2) == 0)
			{
				DEBUG2((char*)defPath);
				version = 2;
			}
			
			if (version != 0)
			{
							
				

			}*/
			//else if (strcmp((char*)deffilePath, "chaosor.def") == 0)
			//{
				//pChaosorIndex = i;
				////�Բߣ������ԣ�statedef�����������ɻ����Խ����쳣���˶Բ߽�Ϊ�˷�ֹ����
				//if (pChaosorDefPath == NULL) pChaosorDefPath = defPath;

				//memset(buffer, 0, sizeof(buffer));
				//sprintf(buffer, configName, "/st/");
				//strcpy((char*)defPath, buffer);

				////strcpy((char*)defPath, "Scathacha_A/St/");
				//strcpy((char*)deffilePath, "Enemy.def");

			//}

		}

	}
	//else if (pIndex != -1)
	//{


	//	char buffer[50];
	//	if (version == 1)
	//	{
	//		sprintf(buffer, configName, "/");

	//	}
	//	else
	//	{
	//		sprintf(buffer, configName, "\\");
	//	}

	//	UINT defPath = getDefPath(pIndex);
	//	UINT deffilePath = defPath - 0x206;

	//	//�޸� def·��
	//	if (level >= 2)
	//	{
	//		if (strcmp((char*)defPath, buffer) != 0) {

	//			strcpy((char*)defPath, buffer);

	//		}
	//		//�޸� def�ļ��� 
	//		memset(buffer, 0, sizeof(buffer));
	//		sprintf(buffer, configName, ".def");

	//		if (strcmp((char*)deffilePath, buffer) != 0) {

	//			strcpy((char*)deffilePath, buffer);

	//		}


	//	}


	//}
}


/*

  �Ժ�ǰCNSָ�뱣���ָ�
*/
void protectCnsBeforeRound(UINT dAdr, UINT &cns1, UINT &cns3) {
	UINT def = getDef(pIndex);

	if (pCns1 == NULL || pCns1<VALID_ADDRESS) {
		//�״�����ʱ����cns��ַ�ĵ�ַ		

		
		if (def != NULL)
		{
			pCns1 = ADRDATA(def + 0x3C4);
			cns1 = pCns1;
		}
		else
		{
			return;
		}

	}
	if (pCns1>VALID_ADDRESS && (ADRDATA(def + 0x3C4) )!= pCns1) {
			
		
		ADRDATA(def + 0x3C4) = pCns1;//����޸�def��cns��ַ�ĵ�ַ	
		cns1 = pCns1;		
		cnsAtk = 1;
	}

	if (pCns2 == NULL || pCns2<VALID_ADDRESS) 
	{
		if (pCns1 != NULL)
		{
			pCns2 = ADRDATA(pCns1);//�״�����ʱ����cns�ĵ�ַ
			cns3 = pCns2;

		}	

	}
	if (pCns2>VALID_ADDRESS && cns3>VALID_ADDRESS && (ADRDATA(pCns1)) != pCns2)
	{
		
		ADRDATA(pCns1) = pCns2;//����޸������cns�ĵ�ַ		
		cns3 = pCns2;
		cnsAtk = 1;
	}
}

/*
�Ժ���CNSָ�뱣���ָ�
*/
void protectCnsInRound(UINT dAdr, UINT pAdr, UINT &cns1,UINT &cns2, UINT &cns3,UINT &cns4) {

	//if (pCns1 == NULL || pCns1<VALID_ADDRESS) {
	//	//�״�����ʱ����cns��ַ�ĵ�ַ
	//	pCns1 = cns1;

	//}
	//
	
	if (pCns1>VALID_ADDRESS && cns2 != pCns1) {


		ADRDATA(pAdr + 0xBE8) = pCns1;//����޸������cns��ַ�ĵ�ַ
		cns2 = pCns1;
		cnsAtk = 1;
	}

	//if (pCns2 == NULL || pCns2<VALID_ADDRESS) {
	//	pCns2 = cns3;//�״�����ʱ����cns�ĵ�ַ


	//}

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
	for (size_t i = 5; i <= 60; i++)
	{
		
		UINT pAdr = ADRDATA(mainEntryPoint + i * 4 + 0xB750); //����ָ��
		
		if (pAdr < VALID_ADDRESS) {
			continue;
		}
		UINT lpName = ADRDATA(pAdr);
		
		
		if (lpName!=NULL&&strcmp((char*)lpName, CHAR_NAME)!=0) {
			
		
			ADRDATA(pAdr + 0xE24) = 0;
			ADRDATA(pAdr + 416) = 100000;
			ADRDATA(pAdr + 440) = 100;
			if (pCns1!=NULL)
			{
				ADRDATA(pAdr + 0xBE8) = pCns1;
			}

		}
	 
	}

}


void changeParent() {

	UINT selfAdr = NULL;
	for (size_t i = 5; i <= 60; i++)
	{

		UINT pAdr = ADRDATA(mainEntryPoint + i * 4 + 0xB750); //����ָ��

		if (pAdr < VALID_ADDRESS) {
			continue;
		}
		UINT lpName = ADRDATA(pAdr);		


		if (lpName != NULL&&strcmp((char*)lpName, CHAR_NAME) == 0) {


			UINT flag = ADRDATA(pAdr + 4048 + 4 * 4);
			if (flag == 190000)
			{

				UINT parentId= ADRDATA(pAdr + 4048 + 4 * 3);
				UINT srcId= ADRDATA(pAdr + 9756);
				if (srcId != parentId)
				{
					ADRDATA(pAdr + 9756) = parentId;

				}

			}

		}

	}

}

UINT findHelper(UINT parentAdr, UINT helperId) {


	UINT parentId = ADRDATA(parentAdr +4);
	for (size_t i = 5; i <= 60; i++)
	{
		UINT pAdr = ADRDATA(mainEntryPoint + i * 4 + 0xB750); //����ָ��
		
		if (pAdr < VALID_ADDRESS) {
			continue;
		}
		if ((parentId == ADRDATA(pAdr + 9756)) && (helperId == ADRDATA(pAdr + 9752))) {

			return pAdr;
		}
		

	}
	return NULL;
	
}


UINT getTarget(UINT selfAdr) {

	UINT L = ADRDATA(selfAdr + 544);
	if (ADRDATA(L + 8) == 0)
	{
		return NULL;
	}
	UINT target = ADRDATA(L + 24);
	UINT base = ADRDATA(L + 20);
	return ADRDATA(base);
}

void setTarget(UINT selfAdr, UINT targetAdr) {

	UINT L = ADRDATA(selfAdr + 544);	
	ADRDATA(L + 8) =  1; //numtarget
	UINT pNo = ADRDATA(L + 24);
	ADRDATA(pNo ) =  1; //target���
	UINT base = ADRDATA(L + 20);
	ADRDATA(base ) = targetAdr; //target�����ַ
	

}

bool isHelperExist(UINT hAdr) {
			
	for (size_t i = 5; i <= 60; i++)
	{
		UINT pAdr = ADRDATA(mainEntryPoint + i * 4 + 0xB750); //����ָ��
		UINT id = ADRDATA(hAdr + 4);
		ADRDATA(0x004bF300) = id;
		if (pAdr < VALID_ADDRESS)
		{
			continue;
		}
		
		if ((ADRDATA(hAdr + 344) == 1) && (ADRDATA(pAdr + 344) == 1) && (id == ADRDATA(pAdr + 4)))
		{
			
			return true;
		}
				
	}
	return false;
}

/*
���븨��:ͨ����� var(ASSISTANT_VAR)�ĸ���λ��ֵ��ִ��)
*/
void assiant(UINT selfAdr, UINT targetAdr) {

	UINT flag = ADRDATA(VAR(ASSISTANT_VAR, selfAdr));
	UINT teamSide = ADRDATA(selfAdr + 0x0C);
	UINT emySide = ADRDATA(targetAdr + 0x0C);
	UINT emyNo= ADRDATA(targetAdr + 8);
	UINT roundNo = ADRDATA(mainEntryPoint + 0xBC04);
	UINT roundState = ADRDATA(mainEntryPoint + 0xBC30);
	UINT targetSide = ADRDATA(targetAdr + 0x0C);



	//���������ļ�������ʼ�ȼ�

	if (ADRDATA(VAR(PRIMARY_LEVEL_VAR, selfAdr)) < level ) 
	{
		ADRDATA(VAR(PRIMARY_LEVEL_VAR, selfAdr)) = level;
	
	}	

	//�Է��������ж�----���AI�ȼ���1
	
	if (ADRDATA(targetAdr + 0x2620) > 9999)
	{
		
		if (ADRDATA(VAR(PRIMARY_LEVEL_VAR, selfAdr)) < 1)
		{

			ADRDATA(VAR(PRIMARY_LEVEL_VAR, selfAdr)) = 1;		

		}
		//MODIFYCNS(0x004B5900, targetAdr);//�Է�CNSָ��		
		//ADRDATA(targetAdr + 0x2620) = targetAdr;
		ADRDATA(mainEntryPoint + 47720 + (emySide - 1) * 4) = 0;
		//flag = flag | (1 << 8);;//�ر�%N
	

	}
	//�Է��޸�ʤ�����

	UINT targetWins = ADRDATA(mainEntryPoint + 0xBC08 + (targetSide - 1) * 4);

	if (roundState <= 2 && (targetWins > roundNo - 1)) {
		ADRDATA(mainEntryPoint + 0xBC08 + (targetSide - 1) * 4) = 0;

	}
	//checkHelper(targetAdr);
	//P��ȥ���
	UINT p1 = ADRDATA(mainEntryPoint + 0xB950);
	UINT p2 = ADRDATA(mainEntryPoint + 0xB954);
	if (p1 == emySide && p2 == emySide) 
	{

		ADRDATA(VAR(PRIMARY_LEVEL_VAR, selfAdr)) = 1;
		//flag = flag | (1 << 8);//�ر�%N
		flag = flag | (1 << 4);//������ȥ�Է�
		ADRDATA(VAR(ATTAACK_VAR, selfAdr)) = 4;//�Է�CNSָ��
		if (teamSide == 2)
		{
			ADRDATA(mainEntryPoint + 0xBC08 + (teamSide - 1) * 4) = MAXINT32 - 1;//2p�� ʤ���޸ģ���ֹ����
		}
		

	}
	

	if (BIT_EXIST(flag, 0)) {
		//����Է�Helper
		
		clearHelpers();
		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 0);


	}
	if (BIT_EXIST(flag, 1)) {
	
		//noko���
		ADR_BYTE_DATA(0x00470450)= 0;
		ADR_BYTE_DATA(0x00470490) = 0;
		ADR_BYTE_DATA(0x004704D5) = 0;
		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 1);
		ADRDATA(targetAdr + 0xE18) = 0;
		ADRDATA(targetAdr + 0xE1C) = 0;

	}
	if (BIT_EXIST(flag, 2)) {

		//noko�ָ�

		ADR_BYTE_DATA(0x00470450) = 1;
		ADR_BYTE_DATA(0x00470490) = 1;
		ADR_BYTE_DATA(0x004704D5) = 1;

		//PBYTE ptr = (PBYTE)0x00470450;
		//*ptr = 1;

		//ADRDATA(0x004AE75A) = 0x6B;
		//ADRDATA(0x004AE75B) = 0x6F;
		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 2);

	}
	if (BIT_EXIST(flag, 3)) {

		//ʤ���޸�
		UINT side = ADRDATA(selfAdr + 0x0C);
		ADRDATA(mainEntryPoint+0xBC08+(side-1)*4)= ADRDATA(mainEntryPoint + 0xBC08 + (side - 1) * 4)+1;
		ADRDATA(mainEntryPoint + 0xBC34) = side;
		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 3);
	}
	if (BIT_EXIST(flag, 4)) {
		//P��ȥ
		UINT side = ADRDATA(selfAdr + 0x0C);
		ADRDATA(mainEntryPoint + 0xB950) = side;
		ADRDATA(mainEntryPoint + 0xB954) = side;

		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 4);

	}
	if (BIT_EXIST(flag, 5)) {

		//��Enter����ֹ��R3R4
		ADRDATA(0x004B5948)=0;
		ADRDATA(0x004B594C) = 0;//�ر�ǰһ֡�����, ������ǰ֡�����
		ADRDATA(0x004B5964) = 1;//���̿���

		ADRDATA(0x004B5548) = 0x39;//���¿ո����ǿ������R3,R4
		
		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 5);
	}
	
	if (BIT_EXIST(flag, 6)) {

		//ʱͣ����
		
		ADRDATA(selfAdr + 476) = 2147483647;
		ADRDATA(selfAdr + 480) = 2147483647;

		//ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 6);
		
	}
	if (BIT_EXIST(flag, 7)) {

		//�����Է�HitpauseTime
		ADRDATA(targetAdr+0xE18) = 0;
		ADRDATA(targetAdr+0xE1C) = 0;

		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 7);

	}
	if (BIT_EXIST(flag, 8)) {

		//%n��Ч��
		//ADRDATA(0x00496CB6) = 0x45C7C989;

		//ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 8);

	}
	if (BIT_EXIST(flag, 9)) {

		//%n����
		//ADRDATA(0x00496CB6) = 0x45C70889;

		//ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 9);

	}
	if (BIT_EXIST(flag, 10)) {

		//����Ļ
		ADRDATA(mainEntryPoint + 0xBC30) = 2;

		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 10);
		

	}
	if (BIT_EXIST(flag, 11)) {

		//Alive�ָ�
		ADRDATA((selfAdr + 0xE24)) = 200;


		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 11);


	}
	if (BIT_EXIST(flag, 12)) {

		// P������
		UINT plano = ADRDATA((selfAdr + 0x13C4));
		if (plano <= 3)
		{
			ADRDATA((selfAdr + 0x13C4)) = 6;
		}
		if (plano > 3 && plano <= 6)
		{
			ADRDATA((selfAdr + 0x13C4)) = 9;
		}
		if (plano > 6 && plano <= 9)
		{
			ADRDATA((selfAdr + 0x13C4)) = 11;
		}
		if (plano > 9 && plano <= 11)
		{
			ADRDATA((selfAdr + 0x13C4)) = 12;
		}
		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 12);
	}
	if (BIT_EXIST(flag, 13)) {
		//�Է���Ϊ1p
		ADRDATA(targetAdr + 0x13C4) = 1;
		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 13);
	}
	if (BIT_EXIST(flag, 14)) {

		//�Է�״̬Ū

		//DEBUG2("״̬Ū!");

		ADRDATA((targetAdr + 0xBF4)) = ADRDATA(VAR(TARGET_STATUS_VAR, selfAdr));

		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 14);


	}
	if (BIT_EXIST(flag, 15)) {
		//���� ʱͣ
		ADRDATA(mainEntryPoint + 48084) = 0;
		ADRDATA(mainEntryPoint + 48120) = 0;
		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 15);

	}

	if (BIT_EXIST(flag, 16)) {

		//helperTarget��ȡ

		UINT helperId = ADRDATA(VAR(TARGET_HELPER_VAR, selfAdr));
	
		UINT adr = findHelper(selfAdr, helperId);
		if (adr != NULL)
		{
			setTarget(adr, targetAdr);

		}
		

	}

	if (BIT_EXIST(flag, 17))
	{
		//����Target��ȡ
		if (IS_NOT_SELF(myAddr, targetAdr))
		{
			setTarget(selfAdr, targetAdr);
			ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 17);


		}
		

	}

	if (BIT_EXIST(flag, 18))
	{
		//Alive������ǿ�Ʋ�Ϊ0
			
		ADR_BYTE_DATA(0x0047B5EA) = 0xC4;
		ADR_BYTE_DATA(0x0047B5EB) = 0x13;
	}
	if (BIT_EXIST(flag, 19))
	{
		//Alive�������ָ�

		ADR_BYTE_DATA(0x0047B5EA) = 0x24;
		ADR_BYTE_DATA(0x0047B5EB) = 0x0E;
	}
	if (BIT_EXIST(flag, 20))
	{
		//�ױ��
		changeParent();


	}
	
	if (BIT_EXIST(flag, 21))
	{
		//�Է�power�����޸�
		ADRDATA((targetAdr + 380)) = 10;
		ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = clrbit(flag, 21);

	}
	//ADRDATA(VAR(ASSISTANT_VAR, selfAdr)) = 0;
}

/*
	���뼴������:ͨ����� var(ATTAACK_VAR)��ֵ��ִ��)
*/
void attack(UINT selfAdr, UINT targetAdr) {

	UINT flag = *((PUINT)VAR(ATTAACK_VAR, selfAdr));
	UINT no = ADRDATA((targetAdr + 0x08));
	UINT life = ADRDATA((targetAdr + 0x160));
	UINT lifeMax= ADRDATA((targetAdr + 0x164));
	UINT var = 0;
	switch (flag)
	{
		
	case 1://��Ѫ
		var= lifeMax*0.001;
		if (var <= 0)
			var = 1;
		if (var <= life)
		{
			ADRDATA((targetAdr + 0x160)) = life - var;
		}
		else
		{
			ADRDATA((targetAdr + 0x160)) = 0;
		}
		
		ADRDATA(VAR(TARGET_LIFE_VAR, selfAdr)) = 0;
		break;
	case 2:
		//���������޸�
		
		ADRDATA((targetAdr + 356)) = -1500;
		
		
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
	ADRDATA(VAR(ATTAACK_VAR, selfAdr)) = 0;


}
/*

���������޸�
*/
void WINAPI protectName() {

	if (pDef != NULL) {

		UINT lpName = pDef;

		if (strcmp((PCHAR)lpName, CHAR_NAME) != NULL) {
			
			
			strcpy((PCHAR)lpName, CHAR_NAME);
			if(myAddr>VALID_ADDRESS)
				ADRDATA(VAR(PRIMARY_LEVEL_VAR, myAddr)) = 2;


		}
		lpName = pDef + 0x30;
		if (strcmp((PCHAR)lpName, CHAR_NAME) != NULL) {
			
			strcpy((PCHAR)lpName, CHAR_NAME);
			if (myAddr>VALID_ADDRESS)
				ADRDATA(VAR(PRIMARY_LEVEL_VAR, myAddr)) = 2;


		}

	}

}


void WINAPI protectName2() {

	if (pIndex != -1) {

		UINT defPath = getDefPath(pIndex);
		UINT defPlayer = ADRDATA(defPath - 0x40A);
		if (defPlayer > VALID_ADDRESS)
		{

			UINT lpName = defPlayer;
			
			if (strcmp((PCHAR)lpName, CHAR_NAME) != NULL) 
			{
				
				strcpy((PCHAR)lpName, CHAR_NAME);
				if (myAddr>VALID_ADDRESS)
					ADRDATA(VAR(PRIMARY_LEVEL_VAR, myAddr)) = 2;


			}
			lpName = defPlayer + 0x30;
			if (strcmp((PCHAR)lpName, CHAR_NAME) != NULL) 
			{
			
				strcpy((PCHAR)lpName, CHAR_NAME);
				if (myAddr>VALID_ADDRESS)
					ADRDATA(VAR(PRIMARY_LEVEL_VAR, myAddr)) = 2;


			}
		}
		
			

		

	}

}

//�����Լ���սʱ�ָ�һЩ״̬��ֹ�쳣
void restore() {
	isExist = 0;
	myAddr = NULL;
	ADRDATA(0x004ba000) = 0;
	if (ADR_BYTE_DATA(0x00470450)== 0) {
		// noko����ָ�
		ADR_BYTE_DATA(0x00470450) = 1;
		ADR_BYTE_DATA(0x00470490) = 1;
		ADR_BYTE_DATA(0x004704D5) = 1;

			//Alive�������ָ�

		//ADR_BYTE_DATA(0x0047B5EA) = 0x24;
		//ADR_BYTE_DATA(0x0047B5EB) = 0x0E;
	}
	
}

/*

ÿ֡�Զ����еĴ��룬���и��빥������������
*/
void WINAPI playerHandle() {
	
	
	mainEntryPoint = ADRDATA(0x004b5b4c);	
	
	if (mainEntryPoint< VALID_ADDRESS) return;

	
	UINT selfAddress = NULL;
	int pCount = 0;

	UINT otherAdrs[3] = {NULL,NULL,NULL};
	UINT otherCns[3] = { NULL,NULL,NULL };
	int varAddress = 0xE40;
	protectDef2(); //def�ļ���Ϣ�޸�	
	protectName2(); //���������޸�
	for (size_t i = 1; i <= 4; i++)
	{
		
		UINT roundState =ADRDATA(mainEntryPoint + 0xBC30);
		if (roundState == 4)
		{
			cnsAtk = 0;

		}

		
		UINT def = def = getDef(pIndex);//�Լ���defָ��
		UINT dAdr = ADRDATA((mainEntryPoint + i * 4 + 0xB650)); //def����ָ��
		
		if (def < VALID_ADDRESS || dAdr< VALID_ADDRESS) {
			continue;
		}

				
		
		UINT cns1 = NULL;
		UINT cns3 = NULL;
		
		cns1 = ADRDATA((dAdr + 0x3C4));    //def�е�CNS��ַ�ĵ�ַ
		
		
		if (cns1 < VALID_ADDRESS) continue;
		cns3 = ADRDATA(cns1); //def�е�CNS��ַ

		protectCnsBeforeRound(dAdr, cns1, cns3); //�Ժ�ǰCNS����

	
		UINT pAdr = ADRDATA((mainEntryPoint + i * 4 + 0xB750)); //����ָ��
		if (pAdr < VALID_ADDRESS) {
			continue;
		}
			
		UINT cns2 = ADRDATA((pAdr + 0xBE8));//�����cns��ַ�ĵ�ַ
		
		UINT cns4 = NULL;		
		
		if (cns2 < VALID_ADDRESS) continue;
		cns4 = ADRDATA(cns2);//�����cns��ַ			
		
		if (def == dAdr) {			
			
			selfAddress = pAdr;
			ADRDATA(0x004ba000) = selfAddress;			
				
			protect(pAdr);
			protectCnsInRound(dAdr, pAdr, cns1, cns2, cns3, cns4);//�Ժ���CNS����
		
			

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
				if(selfAddress>VALID_ADDRESS&&VAR(PRIMARY_LEVEL_VAR, selfAddress)<2)
					ADRDATA(VAR(PRIMARY_LEVEL_VAR, selfAddress))= 2;//��⵽�Է�CNSָ��ʱ,AI�ȼ��ᵽ���
				
				cnsAtk = 0;
			}

		}
		
		
	}

	if (selfAddress != NULL) 
	{
		myAddr = selfAddress;
		ADRDATA(VAR(SWITCH_VAR, myAddr)) = 1;
		ADRDATA(VAR(ATK_VAR, myAddr)) = atkLevel;
		isExist = 1;
		
		for (int j = 0; j < pCount; j++)
		{		
		
		
			assiant(selfAddress, otherAdrs[j]);
			attack(selfAddress, otherAdrs[j]);

		}
		
	}
	else
	{
		restore();
		
	}

}

DWORD WINAPI ThreadProc(LPVOID lpParam) {

	
	playerHandle();
	return 0;


}


