/*
*********************************************************************************************************
*
*                                           MASTER INCLUDES
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM32F030 Evaluation Board
*
* Filename      : includes.h
* Version       : V1.10
* Programmer(s) : BAN
*************************************************************************************************/

#ifndef __INCLUDES_H
#define __INCLUDES_H

#include "stm32f0xx.h"



extern char  *macbufs;             //mac ��ַ����

extern uint8_t Dis_Number;         //��ʼ���Լ��������̿��Ʊ���
extern uint8_t NetNumber;          //����Airkiss�������̱���
extern uint8_t Control_Flag_Mode;  // ����ģ��ģʽ�л�
extern uint8_t FLAG_CJSON_SEND;    // ��ʱ������cJson���Ʊ���
extern uint8_t FLAG_delay_s;       //�����ϴ����ݱ���


extern uint32_t  FirstRECVTime;     //��һ�ν��ܵĻ�����Ƿ�������ֵ��������ھͱ���
extern uint8_t   FirstRECVWeek;     //��һ�ν��ܵĻ�����Ƿ�������ֵ��������ھͱ���
extern uint8_t   NewWeekNum;         //��ǰ����
extern uint8_t   mode_status_value; //ģʽ��ֵ
extern uint32_t  repeat_t1_value;
extern uint32_t  repeat_t2_value;
extern uint8_t   ADC_CP_status;     //ADC�򿪹ر�����ģʽ
/*
 *����ͷ�ļ�
 */
#include "delay.h"

/*
 *wifi����ͷ�ļ�
 */
#include "websocket.h"
#include "esp8266.h"
#include "espjson.h"
#include "Airkiss.h"

/*
 *������ͷ�ļ�
 */
#include "USART1.h"
#include "timer2.h"
#include "timer3.h"
#include "timer14.h"
#include "led.h"
#include "key.h"
#include "ADC1.h"

/*
 *C���Կ�ͷ�ļ�
 */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"
#include "stdbool.h"


#endif
/**********************************     END     **********************************/









