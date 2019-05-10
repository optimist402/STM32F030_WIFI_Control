/****************************************************************************
*       Copyright (C), ������ά���ǽ����Ƽ����޹�˾
*
*       WIFIģ��
*       V0.1
*       2017-06-27
*       Woody
*< STM32F030F4P6 >
			FLASH SIZE:16Kbytes
			RAM SIZE:4Kbytes
			Package:TSSOP20
* ����
			FLASH:15.616Kbytes
			RAM:  2.808Kbytes
*****************************************************************************/
//TX  RX  RST  GPIO0

#include "includes.h"


uint8_t TIMETASK_FLAG = 0; //��ʱ����ļ���ֵ



int main(void)
{
    uint8_t ADC_task_delay=0XFF;

    delay_init();
    TIM3_Init(200,7199);  //������100��100msʱ��
    TIM2_Init(50,7199);	   //������50��50msʱ��
    TIM14_Init(1000,7199);//������1000��1sʱ��
    usart1_Init(115200);  //����1��ʼ��������ESP8266��������
    ADC1_Init();  //ADC��ʼ��
    LED_Init();   //LED�Ƴ�ʼ��
    KeyInit();    //������ʼ��

    while (1)
    {
        KEY_SCAN();
        if(FLAG_delay_s == 1)
        {
            if(FLAG_CJSON_SEND == 1)
            {
                FLAG_CJSON_SEND = 0;
                send_data_user();    //����json��������
            }
        }
        if(mode_status_value!= 0)
        {
            if(mode_status_value&0x01) //������ADC����
            {
                ADC_task_delay--;
                if(ADC_task_delay == 0)
                {
                    ADC_task_delay=0xFF;
                    ControlProcessADC();
                }
            }
            if(mode_status_value&0x02)  //�����˶�ʱ������
            {
                ControlProcessTime(TIMETASK_FLAG);
                TIMETASK_FLAG++;
                if(TIMETASK_FLAG == 10) TIMETASK_FLAG=0;
            }
        }
        switch(Control_Flag_Mode)
        {
        case 0:                      //�״ν���
        {
            FLAG_delay_s=0;
            TIM_Cmd(TIM2, DISABLE);  //��ʱ���ر�
            Distribution_Network();  //�ϵ��Ժ���룬�ж����������Լ��Ƿ����Airkissģʽ
            break;
        }
        case 1:                      //���ݽ��շ���
        {
            FLAG_delay_s=1;
            Apps_Analysis_Data();    //����Airkissģʽ�Ժ� ���ݽ��շ��ʹ���
            break;
        }
        case 2:
        {
            FLAG_delay_s=0;
            TIM_Cmd(TIM2, DISABLE);  //��ʱ���ر�
            Configuration_Network(); //���������Ժ����Airkissģʽ���˳����ӽ���Airkiss
            break;
        }
        }
    }
}


/*****END OF FILE****/












