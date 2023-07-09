/*******************************************************
**	CPU: STC11L08XE
**	����22.1184MHZ
**	�����ʣ�9600 bit/S
**  ����ģʽ�� ��ÿ��ʶ��ʱ����Ҫ˵��С�ܡ�������� �����ܹ�������һ����ʶ��
/*********************************************************/


#include "config.h"
/************************************************************************************/
//	nAsrStatus ������main�������б�ʾ�������е�״̬������LD3320оƬ�ڲ���״̬�Ĵ���
//	LD_ASR_NONE:		��ʾû������ASRʶ��
//	LD_ASR_RUNING��		��ʾLD3320������ASRʶ����
//	LD_ASR_FOUNDOK:		��ʾһ��ʶ�����̽�������һ��ʶ����
//	LD_ASR_FOUNDZERO:	��ʾһ��ʶ�����̽�����û��ʶ����
//	LD_ASR_ERROR:		��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬
/***********************************************************************************/
uint8 idata nAsrStatus = 0;
void MCU_init();
void ProcessInt0(); //ʶ������
void delay(unsigned long uldata);
void User_handle(uint8 dat);//�û�ִ�в�������
void Delay200ms();
void Led_test(void);//��Ƭ������ָʾ
uint8_t G0_flag = DISABLE; //���б�־��ENABLE:���С�DISABLE:��ֹ����
sbit LED = P4 ^ 2; //�ź�ָʾ��

sbit SRD1 = P1 ^ 7;
sbit SRD2 = P1 ^ 6;
sbit SRD3 = P1 ^ 5;
sbit SRD4 = P1 ^ 4;


/***********************************************************
* ��    �ƣ� void  main(void)
* ��    �ܣ� ������	�������
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
**********************************************************/
void  main(void)
{
	uint8 idata nAsrRes;
	uint8 i = 0;
	P1M0 = 0xFF;
	P1M1 = 0x00;
	SRD1 = SRD2 = SRD3 = SRD4 = 0;
	Led_test();
	MCU_init();
	LD_Reset();
	UartIni(); /*���ڳ�ʼ��*/
	nAsrStatus = LD_ASR_NONE;		//	��ʼ״̬��û������ASR
	PrintCom("<G>��ӭʹ��");
	while(1)
	{
		switch(nAsrStatus)
		{
		case LD_ASR_RUNING:
		case LD_ASR_ERROR:
			break;
		case LD_ASR_NONE:
		{
			nAsrStatus = LD_ASR_RUNING;
			if (RunASR() == 0)	/*	����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����*/
			{
				nAsrStatus = LD_ASR_ERROR;
			}
			break;
		}
		case LD_ASR_FOUNDOK: /*	һ��ASRʶ�����̽�����ȥȡASRʶ����*/
		{
			nAsrRes = LD_GetResult();		/*��ȡ���*/
			User_handle(nAsrRes);//�û�ִ�к���
			nAsrStatus = LD_ASR_NONE;
			break;
		}
		case LD_ASR_FOUNDZERO:
		default:
		{
			nAsrStatus = LD_ASR_NONE;
			break;
		}
		}// switch
	}// while

}
/***********************************************************
* ��    �ƣ� 	 LED�Ʋ���
* ��    �ܣ� ��Ƭ���Ƿ���ָʾ
* ��ڲ����� ��
* ���ڲ�������
* ˵    ����
**********************************************************/
void Led_test(void)
{
	LED = ~ LED;
	Delay200ms();
	LED = ~ LED;
	Delay200ms();
	LED = ~ LED;
	Delay200ms();
	LED = ~ LED;
	Delay200ms();
	LED = ~ LED;
	Delay200ms();
	LED = ~ LED;
}
/***********************************************************
* ��    �ƣ� void MCU_init()
* ��    �ܣ� ��Ƭ����ʼ��
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
**********************************************************/
void MCU_init()
{
	P0 = 0xff;
	P1 = 0x00;
	P2 = 0xff;
	P3 = 0xff;
	P4 = 0xff;


	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD |= 0x01;		//���ö�ʱ��ģʽ
	TL0 = 0x00;		//���ö�ʱ��ֵ
	TH0 = 0x28;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;

	LD_MODE = 0;		//	����MD�ܽ�Ϊ�ͣ�����ģʽ��д
	IE0 = 1;
	EX0 = 1;
	EA = 1;
	WDT_CONTR = 0x3D;
}
/***********************************************************
* ��    �ƣ�	��ʱ����
* ��    �ܣ�
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
**********************************************************/
void Delay200us()		//@22.1184MHz
{
	unsigned char i, j;
	_nop_();
	_nop_();
	i = 5;
	j = 73;
	do
	{
		while (--j);
	}
	while (--i);
}

void  delay(unsigned long uldata)
{
	unsigned int j  =  0;
	unsigned int g  =  0;
	while(uldata--)
		Delay200us();
}

void Delay200ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	i = 17;
	j = 208;
	k = 27;
	do
	{
		do
		{
			while (--k);
		}
		while (--j);
	}
	while (--i);
}

/***********************************************************
* ��    �ƣ� �жϴ�����
* ��    �ܣ�
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
**********************************************************/
void ExtInt0Handler(void) interrupt 0
{
	ProcessInt0();
}
/***********************************************************
* ��    �ƣ��û�ִ�к���
* ��    �ܣ�ʶ��ɹ���ִ�ж������ڴ˽����޸�
* ��ڲ����� ��
* ���ڲ�������
* ˵    ����
**********************************************************/
void 	User_handle(uint8 dat)
{
	if(0 == dat)
	{
		G0_flag = ENABLE;
		PrintCom("<G>��ã�����");
		LED = 0;
	}
	else if(ENABLE == G0_flag)
	{
		G0_flag = DISABLE;
		LED = 1;
		switch(dat)
		{
		case CODE_1:	 /*������ơ�*/
			SRD1 = 1;
			PrintCom("<G>���Ѵ�");
			break;
		case CODE_2:	 /*����صơ�*/
			SRD1 = 0;
			PrintCom("<G>���ѹر�\r\n");
			break;
		case CODE_3:		/*����򿪵��ӡ�*/
			SRD2 = 1;
			PrintCom("<G>���ӻ��Ѵ�\r\n");
			break;
		case CODE_4:		/*����رյ��ӡ�*/
			SRD2 = 0;
			PrintCom("<G>���ӻ��ѹر�\r\n");
			break;
		case CODE_5:		/*����򿪱��䡱*/
			SRD3 = 1;
			PrintCom("<G>�����Ѵ�\r\n");
			break;
		case CODE_6:		/*����رձ��䡱*/
			SRD3 = 0;
			PrintCom("<G>�����ѹر�\r\n");
			break;
		case CODE_7:		/*����򿪿յ���*/
			SRD4 = 1;
			PrintCom("<G>�����Ѵ�\r\n");
			break;
		case CODE_8:		/*����رտյ���*/
			SRD4 = 0;
			PrintCom("<G>�����ѹر�\r\n");
			break;
		case CODE_9:		/*���ȫ���򿪡�*/
			SRD1 = 1;
			SRD2 = 1;
			SRD3 = 1;
			SRD4 = 1;
			PrintCom("<G>��ȫ����\r\n");
			break;
		case CODE_10:		/*���ȫ���رա�*/
			SRD1 = 0;
			SRD2 = 0;
			SRD3 = 0;
			SRD4 = 0;
			PrintCom("<G>��ȫ���ر�\r\n");
			break;
		case CODE_11:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_12:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_13:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_14:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_15:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_16:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_17:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_18:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_19:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_20:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_21:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_22:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_23:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_24:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_25:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_26:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_27:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_28:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_29:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_30:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_31:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_32:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_33:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_34:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_35:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_36:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_37:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_38:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_39:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_40:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_41:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_42:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_43:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_44:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_45:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_46:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_47:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_48:		/*���.....��*/

			PrintCom("");
			break;
		case CODE_49:		/*���.....��*/

			PrintCom("");
			break;
		default:/*text.....*/
			break;
		}
	}
	else
	{
		//PrintCom("��˵��һ������\r\n"); /*text.....*/
	}
}

void tm0_isr() interrupt 1
{
    TL0 = 0x00;		//���ö�ʱ��ֵ
	TH0 = 0x28;		//���ö�ʱ��ֵ
	WDT_CONTR=0x3D;
		
}
