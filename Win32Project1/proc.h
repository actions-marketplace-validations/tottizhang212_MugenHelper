#pragma once

#include "stdafx.h"

/*
---------------�����������壬�⼸��������Ϊ����ר�ã�����Ը����Լ�������޸�
*/
const UINT SWITCH_VAR = 2;//�����־
const UINT PRIMARY_LEVEL_VAR = 18; //��������ȼ�
const UINT TARGET_HELPER_VAR = 25;//��¼����Helper��ID
const UINT ATTAACK_VAR = 22;  //���빥���ÿ��Ʊ���
const UINT ASSISTANT_VAR = 39; //���븨���ÿ��Ʊ���
const UINT CONTROLER_VAR = 31; //�����������ÿ��Ʊ���
const UINT TARGET_STATUS_VAR = 43;//��¼���ƶԷ�״̬��
const UINT TARGET_ANIM_NO_VAR = 14;//��¼���ƶԷ�������
const UINT TARGET_LIFE_VAR = 26;//��¼���ƶԷ�����ֵ��

//----------------------------------------------------------------------------------
extern const  char* path ;
extern const char* configName ;

typedef void(WINAPI *pFunc)(DWORD, DWORD);
 UINT WINAPI loadCodes(HMODULE hmodule);
 void WINAPI playerHandle();
 void log( char* info);