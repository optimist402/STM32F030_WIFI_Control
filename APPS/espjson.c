//********************************************************************
//
//      ���빦�ܣ�ESP8266 JSON��������
//      4DClass
//      Copyright(C)  2017
//      All rights reserved
//***********************************************************************

#include "includes.h"


uint8_t   FLAG_CJSON_SEND = 0;     // ��ʱ������cJson���Ʊ���

uint8_t   js_swout_status[2]="l";      //����״ֵ̬
uint8_t   js_vout_status[2]="l";       //ad����ĺ�һ�����״ֵ̬
uint8_t   js_mode[2];      //ģʽ����
uint8_t   js_time[6];      //��ǰʱ���ת������ ÿ�ο��Ƶ�ʱ�򶼶������ʱ��У��
uint8_t   js_ad_value[4];  //ADC������ֵ
uint8_t   js_repeat_t1[6];  //repeat��t1����ֵ
uint8_t   js_repeat_t2[6];  //repeat��t2����ֵ
uint8_t   js_timetask_t1[6];  //timetask��t1����ֵ
uint8_t   js_timetask_t2[6];  //timetask��t2����ֵ
uint8_t   js_timetask_week[2];  //timetask��ѭ�������� ��һ������
uint8_t   js_timetask_num[3];  //timetask������ֵ  �ڼ�������ֵ
uint8_t   js_timetask_week_remove[3];  //������������ʹ��
uint8_t   mode_status_value=0;     //ģʽ��ֵ
uint16_t  adc_status_value;      //ǰһ��ADCֵ
uint16_t  set_adc_value;         //ADC��������ֵ
uint8_t   adtask_vout_value=0;     //ADC��vout״̬
uint32_t  repeat_t1_value = 0;
uint32_t  repeat_t2_value = 0;
uint8_t   timetask_num_value = 0;  //�ڼ�������
uint32_t  FirstRECVTime = 0;     //��һ�ν��ܵĻ�����Ƿ�������ֵ��������ھͱ��� ��ǰʱ��
uint8_t   FirstRECVWeek = 0;     //��һ�ν��ܵĻ�����Ƿ�������ֵ��������ھͱ��� ����
uint8_t   ADC_CP_status=0;    //ADC�򿪹ر�����ģʽ
#define  OneDimensional 10  //һά�� 50��
#define  TwoDimensional 4 //��λ�� 4��


uint32_t  timetaskbuf[OneDimensional][TwoDimensional]; //t1/t2/cycle/num �����Դ���50������



char json_get_temp[200] = "";  //����Json�洢 ����
char json_temp[70] = "";       //����Json�洢 ����

char asciibuf[] = {0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

char weekbuf[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

/**
 * ���յ�����1��ESPP8266����
 * �ж��Ƿ�ΪJSON������ִ����Ӧ�Ĺ���
 */
void Apps_Analysis_Data(void)
{
    uint8_t jsFirstTime[7];  //��һ�ν��յ�ʱ��
    uint8_t jsFirstWeek[2];  //��һ�ν��յ�����

    if (USART1_RX_STA & 0x8000) {
        if(cJson_Pars_data() == 0x01) { //���ݽ���  ����
            json_get_ItemObjectFirst((char *)USART1_RX_BUF,"welcome",json_get_temp);
            if (json_check(json_get_temp)  == 1) { //�Ϸ� Ϊ0  ���Ϸ�Ϊ1
                json_get_value((char *)json_get_temp,"time",(char *)jsFirstTime);
                json_get_value((char *)json_get_temp,"week",(char *)jsFirstWeek);
                FirstRECVTime = stringtoint((char *)jsFirstTime);//ת�������������Ժ��ص�ʱ�����
                FirstRECVWeek = (uint8_t)(jsFirstWeek[0]&0x0f);//ת�������������Ժ��ص��ܲ���
                //FirstRECVWeek =1~7
                FirstRECVWeek = weekbuf[FirstRECVWeek-1];
                NewWeekNum = FirstRECVWeek; //���õ�ǰ����
            }
            memset(USART1_RX_BUF,0,300);
        }
        USART1_RX_STA=0;
    }
}

/**
 * ���յ�����1��ESPP8266����
 * �ж��Ƿ�ΪJSON������ִ����Ӧ�Ĺ���
 * {"id":"SWTZ_Cantilever_a0:20:a6:29:a7:dc","to":"user","services":{"strength1":"0.01","strength2":"0.02"}}
*/
char cJson_Pars_data(void)
{
    uint32_t  Remove_timeweek_status=0;

    memset(json_get_temp,0,200);//������� д0 ��С1024Byte
    memset(json_temp,0,70);//������� д0 ��С1024Byte
    //��ȡ��json
    json_get_ItemObject((char *)USART1_RX_BUF,"services",(char *)json_get_temp); //��ȡ��json

    if (json_check(json_get_temp)  == 1) { //�Ϸ� Ϊ1  ���Ϸ�Ϊ0
        json_get_value( (char *)json_get_temp, "mode", (char *)js_mode);   //ģʽ�ļ�ֵ
        json_get_value( (char *)json_get_temp, "swout", (char *)js_swout_status); //���صļ�ֵ
        json_get_value( (char *)json_get_temp, "time", (char *)js_time);  //��ǰʱ���ת������ ÿ�ο��Ƶ�ʱ�򶼶������ʱ��У��

        ////����adtask
        json_get_obj( (char *)json_get_temp, "adtask", (char *)json_temp);  //��ȡAD�����µ�json
        json_get_value( (char *)json_temp, "value", (char *)js_ad_value);   //��ȡAD���õ�valueֵ
        json_get_value( (char *)json_temp, "vout", (char *)js_vout_status);     //��ȡAD���õ�����ߵ͵�ƽ
        ////����repeat
        json_get_obj( (char *)json_get_temp, "repeat", (char *)json_temp);  //��ȡAD�����µ�json
        json_get_value( (char *)json_temp, "t1", (char *)js_repeat_t1);   //��ȡAD���õ�valueֵ
        json_get_value( (char *)json_temp, "t2", (char *)js_repeat_t2);     //��ȡAD���õ�����ߵ͵�ƽ
        ////����timetask
        json_get_obj( (char *)json_get_temp, "timetask", (char *)json_temp);  //��ȡAD�����µ�json
        json_get_value( (char *)json_temp, "t1", (char *)js_timetask_t1);   //��ȡAD���õ�valueֵ
        json_get_value( (char *)json_temp, "t2", (char *)js_timetask_t2);     //��ȡAD���õ�����ߵ͵�ƽ
        json_get_value( (char *)json_temp, "week", (char *)js_timetask_week);   //��ȡAD���õ�valueֵ
        json_get_value( (char *)json_temp, "num", (char *)js_timetask_num);     //��ȡAD���õ�����ߵ͵�ƽ

        ////�ֶ���ֱ�ӿ���
        if( json_check_value( (char *)js_swout_status,"h" ) == 1 )  //����Ǹ�
            OUTH;  //��һ�������
        else
            OUTL;  //��һ�������

        //��ȡģʽ��ֵ
        mode_status_value = *js_mode;          //��ȡģʽֵ
        //��ȡ��ǰʱ�� ���ڸ��µ�ǰʱ��
        FirstRECVTime = stringtoint((char *)js_time);  //����ʱ��ֵ

        //adtask�ļ�ֵ����
        set_adc_value = (uint16_t)((float)(stringtoint((char *)js_ad_value))*30); //��ȡADC����ֵ
        if( json_check_value( (char *)js_vout_status,"h" ) == 1 )  //����Ǹ�
            adtask_vout_value = 1;//ADC����ֵ���״̬��H��
        else
            adtask_vout_value = 0;//ADC����ֵ���״̬��L��

        //repeat�ļ�ֵ����
        repeat_t1_value = stringtoint((char *)js_repeat_t1);  //PWM����ģʽ��ֵ  ����ʱ��
        repeat_t2_value = stringtoint((char *)js_repeat_t2);  //PWM����ģʽ��ֵ  �ر�ʱ��

        //timetask��ֵ����
        timetask_num_value = stringtoint((char *)js_timetask_num); //�����
        //���ά�����ֵ
        if(json_check_value( (char *)js_timetask_week, "-1") != 1) {	//û�йر�
            timetaskbuf[timetask_num_value-1][0] = timetask_num_value;  //�����
            if(js_timetask_week[0] <= 0x40) {
                Remove_timeweek_status = (uint32_t)((js_timetask_week[0] & 0x0f) << 4);
            } else {
                Remove_timeweek_status = (uint32_t)(asciibuf[(js_timetask_week[0] & 0x0f)-1] << 4);
            }
            if(js_timetask_week[1] <= 0x40) {
                Remove_timeweek_status |= (uint32_t)(js_timetask_week[1] & 0X0F);
            } else {
                Remove_timeweek_status |= (uint32_t)(asciibuf[(js_timetask_week[1] & 0X0F)-1]);
            }
            timetaskbuf[timetask_num_value-1][1] = Remove_timeweek_status;
            timetaskbuf[timetask_num_value-1][2] = stringtoint((char *)js_timetask_t1);   //��ʱ����Ĺر�ʱ��
            timetaskbuf[timetask_num_value-1][3] = stringtoint((char *)js_timetask_t2);   //��ʱ���������ʱ��
        } else {
            if(timetask_num_value == 0) timetask_num_value=1;
            timetaskbuf[timetask_num_value-1][0] = 0; //�����
            timetaskbuf[timetask_num_value-1][1] = 0; //����
            timetaskbuf[timetask_num_value-1][2] = 0; //��ʱ����Ĺر�ʱ��
            timetaskbuf[timetask_num_value-1][3] = 0; //��ʱ���������ʱ��
        }
        //��������Ժ�ֱ�ӷ������ݵ�������
        send_data_user();  //����json��������
        return 0;
    } else {
        if(checkJSON("Connection: close")) {
            TIM_Cmd(TIM2, DISABLE);//���������
            Control_Flag_Mode=0;
            Dis_Number = 10;  //��������ͷ
        }
        //��Ч��+
        GPIO_ResetBits(GPIOA, GPIO_Pin_3); //1���1
        delay_ms(40);
        GPIO_SetBits(GPIOA, GPIO_Pin_3);//1���1
        delay_ms(40);
        GPIO_ResetBits(GPIOA, GPIO_Pin_3); //1���1
        delay_ms(40);
        GPIO_SetBits(GPIOA, GPIO_Pin_3);//1���1
        delay_ms(40);
        return 1;
    }
}

/**
 * �����ռ���������
 * ���̿���
 */
void ControlProcessTime(uint8_t m)
{
    if(timetaskbuf[m][0] != 0) { //����������
        if((timetaskbuf[m][1] & NewWeekNum)||(timetaskbuf[m][1] & 0X80)) { //�жϵ�ǰ���Ƿ����
            if((timetaskbuf[m][2] <= FirstRECVTime)&&(timetaskbuf[m][3]>= FirstRECVTime)) { //��ʼʱ��
                if(ADC_CP_status == 1)
                    OUTH;  //��һ�������
            } else if((timetaskbuf[m][3] <= FirstRECVTime)&&((timetaskbuf[m][3]+10) >= FirstRECVTime)) { //����ʱ��
                if(ADC_CP_status == 1) {
                    OUTL;  //��һ�������
                    if(timetaskbuf[m][1] & 0X80) {
                        timetaskbuf[m][0] = 0;
                        mode_status_value &=0x7f;
                    }
                }
            }
        }
    }
}

void ControlProcessADC(void)
{
    adc_status_value = ADC_GetValue();  //��ȡAD��ֵ
    if(adtask_vout_value == 1) { //�����
        if(adc_status_value >= set_adc_value) {
            if((mode_status_value&0x04)||(mode_status_value&0x02)) { //�Ƿ�����ظ�������߶�ʱ����
                ADC_CP_status=1;  //������
            } else {
                OUTH;             //��һ�������
            }
        } else {
            OUTL;  //��һ�������
            ADC_CP_status=0;//�ر�����
        }
    } else {
        if(adc_status_value >= set_adc_value) {
            OUTL;  //��һ�������
            ADC_CP_status=0;//�ر�����
        } else {
            if((mode_status_value&0x04)||(mode_status_value&0x02)) { //�Ƿ�����ظ�������߶�ʱ����
                ADC_CP_status=1;//������
            } else
                OUTH;  //��һ�������
        }
    }
}

/**
 * �������ݵ�������
 * ���͵�{\"id\":\"SWTZ_WifiModule_a0:20:a6:29:a7:dc\",\"to\":\"user\",\"services\":{\"switch\":\"off\",\"adc\":\"4096\"}}
 * ���յ���
 */
void send_data_user(void)
{
    char PRemove[32];
    uint8_t buffs_num=0;
    uint8_t adc_mode_value=0;
    uint16_t ADC_send_value=0;
    uint16_t ADC_send_set=0;
    uint8_t packingData[300];        //���800�ֽ�

    strcpy((char *) packingData, "{\"id\":\"SWTZ_WifiModule_");
    strcat((char *) packingData, macbufs);
    strcat((char *) packingData, "\",\"to\":\"user\",");

    strcat((char *) packingData, "\"debug\":{\"variable\":\"");
    sprintf((char *)PRemove,"%d,%d,%d",NewWeekNum,NetNumber,Dis_Number);
    strcat((char *) packingData, PRemove);
    strcat((char *) packingData, "\"},");
    strcat((char *) packingData, "\"services\":{\"swout\":\"");
    if((json_check_value((char *)js_swout_status,"h") !=1)&&(json_check_value((char *)js_swout_status,"l") !=1))   //L=l
        sprintf((char *)js_swout_status,"%s","l");
    if(Read_pin == SET ) //���״̬Ϊ��
        sprintf((char *)js_swout_status,"%s","h");
    else
        sprintf((char *)js_swout_status,"%s","l");
    strcat((char *) packingData, (char *)js_swout_status);  //�Զ����ƿ���on/off.;

    strcat((char *) packingData, "\",\"adc\":\"");
    ADC_send_value = ADC_GetValue()/30;
    if(ADC_send_value >= 100) ADC_send_value=100;
    if(mode_status_value&0x01) adc_mode_value=1;
    else adc_mode_value=0;
    ADC_send_set = set_adc_value/30;
    sprintf(PRemove, "%d,%d,%d,%d", adc_mode_value, ADC_send_value,adtask_vout_value,ADC_send_set);   // ADCֵ7
    strcat((char *) packingData, PRemove);

    if(mode_status_value&0x04)
        strcat((char *) packingData, "\",\"repeat\":\"1,");
    else  strcat((char *) packingData, "\",\"repeat\":\"0,");

    sprintf(PRemove, "%d,%d", repeat_t1_value, repeat_t2_value);   // ADCֵ
    strcat((char *) packingData, PRemove);

    strcat((char *) packingData, "\",\"timetask\":\"");
    buffs_num =(timetaskbuf[0][0] != 0)? 1:0;
    sprintf(PRemove,"%d,%2d,%d,%d#",buffs_num,timetaskbuf[0][1],timetaskbuf[0][2],timetaskbuf[0][3]);
    strcat((char *) packingData, PRemove);
    buffs_num =(timetaskbuf[1][0] != 0)? 1:0;
    sprintf(PRemove,"%d,%2d,%d,%d#",buffs_num,timetaskbuf[1][1],timetaskbuf[1][2],timetaskbuf[1][3]);
    strcat((char *) packingData, PRemove);
    buffs_num =(timetaskbuf[2][0] != 0)? 1:0;
    sprintf(PRemove,"%d,%2d,%d,%d#",buffs_num,timetaskbuf[2][1],timetaskbuf[2][2],timetaskbuf[2][3]);
    strcat((char *) packingData, PRemove);
    buffs_num =(timetaskbuf[3][0] != 0)? 1:0;
    sprintf(PRemove,"%d,%2d,%d,%d#",buffs_num,timetaskbuf[3][1],timetaskbuf[3][2],timetaskbuf[3][3]);
    strcat((char *) packingData, PRemove);
    buffs_num =(timetaskbuf[4][0] != 0)? 1:0;
    sprintf(PRemove,"%d,%2d,%d,%d#",buffs_num,timetaskbuf[4][1],timetaskbuf[4][2],timetaskbuf[4][3]);
    strcat((char *) packingData, PRemove);
    buffs_num =(timetaskbuf[5][0] != 0)? 1:0;
    sprintf(PRemove,"%d,%2d,%d,%d#",buffs_num,timetaskbuf[5][1],timetaskbuf[5][2],timetaskbuf[5][3]);
    strcat((char *) packingData, PRemove);
    buffs_num =(timetaskbuf[6][0] != 0)? 1:0;
    sprintf(PRemove,"%d,%2d,%d,%d#",buffs_num,timetaskbuf[6][1],timetaskbuf[6][2],timetaskbuf[6][3]);
    strcat((char *) packingData, PRemove);
    buffs_num =(timetaskbuf[7][0] != 0)? 1:0;
    sprintf(PRemove,"%d,%2d,%d,%d#",buffs_num,timetaskbuf[7][1],timetaskbuf[7][2],timetaskbuf[7][3]);
    strcat((char *) packingData, PRemove);
    buffs_num =(timetaskbuf[8][0] != 0)? 1:0;
    sprintf(PRemove,"%d,%2d,%d,%d#",buffs_num,timetaskbuf[8][1],timetaskbuf[8][2],timetaskbuf[8][3]);
    strcat((char *) packingData, PRemove);
    buffs_num =(timetaskbuf[9][0] != 0)? 1:0;
    sprintf(PRemove,"%d,%2d,%d,%d",buffs_num,timetaskbuf[9][1],timetaskbuf[9][2],timetaskbuf[9][3]);
    strcat((char *) packingData, PRemove);

    strcat((char *) packingData, "\"}}");
    sendMessagePacket((char *)packingData);
}


////////////********************** JSON  ���ݴ��� ***************************////////////////

/***************************************
 name:       JSON У�麯��
 input:      �ַ���
 output:     �Ϸ�JAON ����1 ���Ϸ�JSON ����0
 description:
 ***************************************/
char json_check(char *str)
{
    uint16_t str_length;//�����ַ����ȱ���
    str_length = strlen(str);//�����ַ�����
    if (str[0] == '{' && str[str_length - 1] == '}') { //ͨ����β�������ж��Ƿ�ΪJSON
        return 1;//����ַ���Ϊ�Ϸ�JSON ����1
    } else {
        return 0;//����ַ���Ϊ�Ϸ�JSON ����0
    }
}

/***************************************
 name:       JSON ��ȡ��ֵ����
 input:      JSON�ַ��� Ҫ��ȡ�ļ��� ��ȡ��ֵ���ַ���
 output:     �����ȡ�ɹ�����1 ��ȡʧ�ܷ���0
 description:
 ***************************************/
char json_get_value(char *json, char *json_key, char *json_value)
{
    char *json_key_start;//���������ʼ��λ��
    char *json_key_end;//�������������λ��
    char json_key_length;//�����������
    char *json_value_start;//�����ֵ��ʼ��λ��
    char *json_value_end;//�����ֵ������λ��
    char json_value_length;//�����ֵ����

    json_key_start = strstr(json, json_key);//��ȡ������ʼ��λ��
    json_key_length = strlen(json_key);//��ȡ�����ĳ���
    json_key_end = json_key_start + json_key_length;//��ȡ����������λ��
    if (json_key_start != 0 && *(json_key_start - 1) == '\"'
        && *(json_key_end) == '\"' && *(json_key_end + 1) == ':'
        && *(json_key_end + 2) == '\"') {
        json_value_start = json_key_end + 3;//��ȡ��ֵ��ʼ��λ��
        json_value_end = strstr(json_value_start, "\""); //��ȡ��ֵ������λ��
        json_value_length = json_value_end - json_value_start;//��ȡ��ֵ�ĳ���
        strncpy(json_value, json_value_start, json_value_length);//����ֵ����ָ������
        json_value[json_value_length] = '\0';//ָ�����һλ���ַ��������� \0
        return 1;//�ɹ���ȡ��ֵ ����1
    } else {
        json_value[0] = '\0';
        return 0;//ʧ�ܻ�ȡ��ֵ ����0
    }
}

/***************************************
 name:      JSON ��ȡ��JSON��ֵ��ֵ��һ����ȡ��
 input:     JSON�ַ��� Ҫ��ȡ�ļ��� ��ȡ��ֵ��ֵ
 output:    ��
 description:
 ***************************************/
void json_get_ItemObject(char *json, char *json_key, char *json_value)
{
    int  json_len;
    uint16_t jsoncls_0=0;
    char *json_get_Item_obj;
    char *ptr_obj;

    while(json[jsoncls_0++] != '\0');
    json[jsoncls_0-1] = 0x30;
    json_get_Item_obj = strstr(json, json_key);
    json_get_Item_obj = strstr(json_get_Item_obj, "{");
    ptr_obj = strrchr(json_get_Item_obj, ',');
    json_len = strlen(json_get_Item_obj) - strlen(ptr_obj);
    strncpy(json_value, json_get_Item_obj, json_len);
    json_value[json_len] = '\0';
}

/***************************************
 name:      JSON ��ȡ��JSON��ֵ��ֵ
 input:     JSON�ַ��� Ҫ��ȡ�ļ��� ��ȡ��ֵ��ֵ
 output:    ��
 description:
 ***************************************/
void json_get_ItemObjectFirst(char *json, char *json_key, char *json_value)
{
    int  json_len_Fir;
    char *json_get_Item_obj_Fir;
    char *ptr_obj_Fir;

    json_get_Item_obj_Fir = strstr(json, json_key);
    json_get_Item_obj_Fir = strstr(json_get_Item_obj_Fir, "{");
    ptr_obj_Fir = strrchr(json_get_Item_obj_Fir, '}');
    json_len_Fir = strlen(json_get_Item_obj_Fir) - strlen(ptr_obj_Fir);
    strncpy(json_value, json_get_Item_obj_Fir, json_len_Fir);
    json_value[json_len_Fir] = '\0';
}


/***************************************
 name:      JSON ��ȡ��JSON(������ȡ)
 input:     JSON�ַ��� Ҫ��ȡ�ļ��� ��ȡ��json�Ĵ�
 output:    ��
 description:
 ***************************************/
void json_get_obj(char *json, char *json_key,char *json_objectItem)
{
    int json_len_t;
    char *json_get_temp_obj_t;
    char *ptr_t;

    json_get_temp_obj_t = strstr(json,json_key);
    json_get_temp_obj_t = strstr(json_get_temp_obj_t,"{");
    ptr_t = strchr(json_get_temp_obj_t,'}');
    json_len_t =strlen(json_get_temp_obj_t)-strlen(ptr_t)+1;
    strncpy(json_objectItem, json_get_temp_obj_t, json_len_t);
    json_objectItem[json_len_t] = '\0';
}


/***************************************
 name:       JSON ��ֵ�ԱȺ���
 input:      JSON ��ֵ Ҫƥ����ַ�
 output:     ���ƥ��ɹ�����1 ʧ�ܷ���0
 description:
 ***************************************/
char json_check_value(char *str1, char *str2)
{
    if (strcmp(str1, str2) == 0) {
        return 1;//ƥ��ɹ�����1
    } else {
        return 0;//ƥ��ɹ�����0
    }
}


/***************************************
 name:       string����תint����
 input:      �洢string�Ĳ���
 output:     ���ת���ɹ�������ֵ ʧ�ܷ���0
 description:
 ***************************************/
uint32_t stringtoint(char *a)
{
    uint32_t val=0;
    uint8_t i=0;
    while(1) {
        if( *(a+i) == '\0' )break;
        else {
            if(*(a+i)<='9'&&*(a+i)>='0')
                val=(val*10)+(*(a+(i++))&0x0F);
            else
                return 0;
        }
    }
    return val;
}

//
char* checkJSON(char *str)
{
    char *strx=0;
    strx=strstr((const char*)USART1_RX_BUF,(const char*)str);
    return (char*)strx;
}

