#pragma once

#include "stdafx.h"

/*
---------------�����������壬�⼸��������Ϊ����ר�ã�����Ը����Լ�������޸�
*/

const UINT SWITCH_VAR = 2;//�����־
const UINT PRIMARY_LEVEL_VAR = 18; //��������ȼ�
//const UINT AI_LEVEL_VAR = 56; //ǿ��AI�ȼ�
const UINT TARGET_HELPER_VAR = 25;//��¼����Helper��ID
const UINT ATTAACK_VAR = 28;  //���빥���ÿ��Ʊ���
const UINT ASSISTANT_VAR = 39; //���븨���ÿ��Ʊ���
const UINT CONTROLER_VAR = 31; //�����������ÿ��Ʊ���
const UINT TARGET_STATUS_VAR = 43;//��¼���ƶԷ�״̬��
const UINT TARGET_ANIM_NO_VAR = 29;//��¼���ƶԷ�������
const UINT TARGET_LIFE_VAR = 26;//��¼���ƶԷ�����ֵ��
const UINT GOD_HELPER_VAR = 6;//����helperID
const UINT SPY_CTL_HELPER_VAR = 0;//���߿���helperID
const UINT SPY_HELPER_VAR = 1;//����helperID

//----------------------------------------------------------------------------------
extern const  char* path ;
extern const char* configName ;

typedef void(WINAPI *pFunc)(DWORD, DWORD);
 UINT WINAPI loadCodes(HMODULE hmodule);
 void WINAPI playerHandle();
 void log( char* info);