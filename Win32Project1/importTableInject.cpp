//�����ע��
#include "stdafx.h"
#include <stdlib.h>  
#include <assert.h>


DWORD rva2offset(LPVOID base, DWORD rva)
{
	IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)base;
	IMAGE_NT_HEADERS32* ntHeader = (IMAGE_NT_HEADERS32*)(dosHeader->e_lfanew + (DWORD)base);
	IMAGE_SECTION_HEADER* sectionHeader = (IMAGE_SECTION_HEADER*)((DWORD)ntHeader + sizeof(IMAGE_NT_HEADERS32));
	if (rva<ntHeader->OptionalHeader.SizeOfHeaders)
	{
		return rva;
	}
	for (DWORD i = 1; i <= ntHeader->FileHeader.NumberOfSections; i++)
	{
		//����������һ����ʱ,�Ϳ���ֱ�Ӽ�����,�������ͨ��ǰ���ͷ��VirtualAddressȷ�����ĸ�����
		if (i == ntHeader->FileHeader.NumberOfSections)
		{
			return rva - sectionHeader->VirtualAddress + sectionHeader->PointerToRawData;
		}
		else if (rva >= sectionHeader->VirtualAddress && rva < (sectionHeader + 1)->VirtualAddress)
		{
			return rva - sectionHeader->VirtualAddress + sectionHeader->PointerToRawData;
		}
		sectionHeader++;
	}
	return 0;
}
//�ļ������ڴ����
DWORD PEAlign(DWORD size, DWORD dwAlignTo)
{
	return(((size + dwAlignTo - 1) / dwAlignTo)*dwAlignTo);
}

DWORD importTableInject(char* modulepath, char* dllpath)//dllpath����dll������,������·��
{
	//�ȱ���Դ�ļ�
	//WCHAR newFile[MAX_PATH];

	//wsprintf(newFile, L"%s.bak", modulepath);
	//CopyFileW(modulepath, newFile, 0);
	HANDLE hFile = CreateFileA(modulepath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == 0 || hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	DWORD fileSize = GetFileSize(hFile, 0);
	HANDLE hMap = CreateFileMappingW(hFile, NULL, PAGE_READWRITE, 0, 0, 0);
	if (hMap <= 0)
	{
		CloseHandle(hFile);
		return 0;
	}
	LPVOID imagebase = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (imagebase == 0)
	{
		CloseHandle(hFile);
		CloseHandle(hMap);
		return 0;
	}
	PIMAGE_NT_HEADERS32 ntHeader = (PIMAGE_NT_HEADERS32)((DWORD)imagebase + ((PIMAGE_DOS_HEADER)(imagebase))->e_lfanew);
	if ((ntHeader->FileHeader.NumberOfSections + 1) * sizeof(IMAGE_SECTION_HEADER)>ntHeader->OptionalHeader.SizeOfHeaders)
	{
		CloseHandle(hFile);
		CloseHandle(hMap);
		return 0;
	}

	//��λ�����һ���������������ַ,����ntͷ����β��
	PIMAGE_SECTION_HEADER newSection = (PIMAGE_SECTION_HEADER)(ntHeader + 1) + ntHeader->FileHeader.NumberOfSections;

	//��ӽ���ͷ
	memcpy(newSection->Name, "freesec", 8); //����ͷ�������Ϊ8���ֽ�,������β��\0
	newSection->Characteristics = IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;
	newSection->Misc.VirtualSize =  //���һ��dll��Ϣ,������ԭ����С�Ļ����ϼ�һ���������������С��dll�����ַ�����С+4��IMAGE_THUNK_DATA32��С
		ntHeader->OptionalHeader.DataDirectory[1].Size + sizeof(IMAGE_IMPORT_DESCRIPTOR) + strlen(dllpath) + 1 + sizeof(IMAGE_THUNK_DATA32) * 4;
	newSection->NumberOfLinenumbers = 0;
	newSection->NumberOfRelocations = 0;
	newSection->PointerToLinenumbers = 0;
	newSection->PointerToRawData = (newSection - 1)->PointerToRawData + (newSection - 1)->SizeOfRawData;
	newSection->PointerToRelocations = 0;
	newSection->VirtualAddress = (newSection - 1)->VirtualAddress + PEAlign((newSection - 1)->SizeOfRawData, ntHeader->OptionalHeader.SectionAlignment);
	newSection->SizeOfRawData = PEAlign(newSection->Misc.VirtualSize, ntHeader->OptionalHeader.FileAlignment);
	DWORD sectionSize = newSection->SizeOfRawData;
	//��ӽڱ�
	SetFilePointer(hFile, 0, 0, FILE_END); //�ļ�ָ�����ļ�β��
	LPVOID content = malloc(newSection->SizeOfRawData);
	memset(content, 0, newSection->SizeOfRawData);
	char* p = (char*)content;
	memcpy(content, (LPVOID)((DWORD)imagebase + rva2offset(imagebase, ntHeader->OptionalHeader.DataDirectory[1].VirtualAddress)), ntHeader->OptionalHeader.DataDirectory[1].Size - sizeof(IMAGE_IMPORT_DESCRIPTOR));


	

	p = (char*)((DWORD)p + ntHeader->OptionalHeader.DataDirectory[1].Size - sizeof(IMAGE_IMPORT_DESCRIPTOR));
	((PIMAGE_IMPORT_DESCRIPTOR)p)->OriginalFirstThunk = newSection->VirtualAddress + ntHeader->OptionalHeader.DataDirectory[1].Size + sizeof(IMAGE_IMPORT_DESCRIPTOR) + strlen(dllpath) + 1;
	((PIMAGE_IMPORT_DESCRIPTOR)p)->FirstThunk = ((PIMAGE_IMPORT_DESCRIPTOR)p)->OriginalFirstThunk + sizeof(IMAGE_THUNK_DATA32) * 2;
	((PIMAGE_IMPORT_DESCRIPTOR)p)->ForwarderChain = 0;
	((PIMAGE_IMPORT_DESCRIPTOR)p)->Name = newSection->VirtualAddress + ntHeader->OptionalHeader.DataDirectory[1].Size + sizeof(IMAGE_IMPORT_DESCRIPTOR);
	((PIMAGE_IMPORT_DESCRIPTOR)p)->TimeDateStamp = 0;
	p += sizeof(IMAGE_IMPORT_DESCRIPTOR) * 2; //Խ�������ںͿս�β

											  //������������е�name�ֶ�,ע��2��֮��rva�Ĺ���
	memcpy(p, dllpath, strlen(dllpath) + 1);
	p += strlen(dllpath) + 1;
	//���ע���dll��iat��int.4��Ԫ��,ǰ2���Ǹ�iat��,��2����int�ľ���
	IMAGE_THUNK_DATA32 ixt[4] = { 0 };
	ixt[0].u1.AddressOfData |= 0x80000000; //�����λ��1,��ʾ������ŵ���
	ixt[0].u1.AddressOfData += 1;
	ixt[2].u1.AddressOfData |= 0x80000000; //�����λ��1,��ʾ������ŵ���
	ixt[2].u1.AddressOfData += 1;
	memcpy(p, ixt, 4 * sizeof(IMAGE_THUNK_DATA32));

	
	
	//�޸ı�Ҫ�ֶ�
	ntHeader->FileHeader.NumberOfSections++;
	ntHeader->OptionalHeader.SizeOfImage += newSection->SizeOfRawData;
	//�󶨵�����Ϊ0,����һЩ
	ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size = 0;
	ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress = 0;
	ntHeader->OptionalHeader.DataDirectory[1].VirtualAddress = newSection->VirtualAddress;
	ntHeader->OptionalHeader.DataDirectory[1].Size = newSection->Misc.VirtualSize;
	UnmapViewOfFile(imagebase); //���������Ҫ��writefile֮ǰ����

	DWORD size = 0;
	if (!WriteFile(hFile, content, sectionSize, &size, 0))
	{
		
		CloseHandle(hMap);
		CloseHandle(hFile);
		free(content);
		return 0;
	}

	CloseHandle(hMap);
	CloseHandle(hFile);
	free(content);
	return 1;
}
//end �����ע��