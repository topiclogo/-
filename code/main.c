/*******************************************************
**	CPU: STC11L08XE
**	晶振：22.1184MHZ
**	波特率：9600 bit/S
**  口令模式： 即每次识别时都需要说“小杰”这个口令 ，才能够进行下一级的识别
/*********************************************************/


#include "config.h"
/************************************************************************************/
//	nAsrStatus 用来在main主程序中表示程序运行的状态，不是LD3320芯片内部的状态寄存器
//	LD_ASR_NONE:		表示没有在作ASR识别
//	LD_ASR_RUNING：		表示LD3320正在作ASR识别中
//	LD_ASR_FOUNDOK:		表示一次识别流程结束后，有一个识别结果
//	LD_ASR_FOUNDZERO:	表示一次识别流程结束后，没有识别结果
//	LD_ASR_ERROR:		表示一次识别流程中LD3320芯片内部出现不正确的状态
/***********************************************************************************/
uint8 idata nAsrStatus = 0;
void MCU_init();
void ProcessInt0(); //识别处理函数
void delay(unsigned long uldata);
void User_handle(uint8 dat);//用户执行操作函数
void Delay200ms();
void Led_test(void);//单片机工作指示
uint8_t G0_flag = DISABLE; //运行标志，ENABLE:运行。DISABLE:禁止运行
sbit LED = P4 ^ 2; //信号指示灯

sbit SRD1 = P1 ^ 7;
sbit SRD2 = P1 ^ 6;
sbit SRD3 = P1 ^ 5;
sbit SRD4 = P1 ^ 4;


/***********************************************************
* 名    称： void  main(void)
* 功    能： 主函数	程序入口
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
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
	UartIni(); /*串口初始化*/
	nAsrStatus = LD_ASR_NONE;		//	初始状态：没有在作ASR
	PrintCom("<G>欢迎使用");
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
			if (RunASR() == 0)	/*	启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算*/
			{
				nAsrStatus = LD_ASR_ERROR;
			}
			break;
		}
		case LD_ASR_FOUNDOK: /*	一次ASR识别流程结束，去取ASR识别结果*/
		{
			nAsrRes = LD_GetResult();		/*获取结果*/
			User_handle(nAsrRes);//用户执行函数
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
* 名    称： 	 LED灯测试
* 功    能： 单片机是否工作指示
* 入口参数： 无
* 出口参数：无
* 说    明：
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
* 名    称： void MCU_init()
* 功    能： 单片机初始化
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
**********************************************************/
void MCU_init()
{
	P0 = 0xff;
	P1 = 0x00;
	P2 = 0xff;
	P3 = 0xff;
	P4 = 0xff;


	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x28;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;

	LD_MODE = 0;		//	设置MD管脚为低，并行模式读写
	IE0 = 1;
	EX0 = 1;
	EA = 1;
	WDT_CONTR = 0x3D;
}
/***********************************************************
* 名    称：	延时函数
* 功    能：
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
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
* 名    称： 中断处理函数
* 功    能：
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
**********************************************************/
void ExtInt0Handler(void) interrupt 0
{
	ProcessInt0();
}
/***********************************************************
* 名    称：用户执行函数
* 功    能：识别成功后，执行动作可在此进行修改
* 入口参数： 无
* 出口参数：无
* 说    明：
**********************************************************/
void 	User_handle(uint8 dat)
{
	if(0 == dat)
	{
		G0_flag = ENABLE;
		PrintCom("<G>你好，主人");
		LED = 0;
	}
	else if(ENABLE == G0_flag)
	{
		G0_flag = DISABLE;
		LED = 1;
		switch(dat)
		{
		case CODE_1:	 /*命令“开灯”*/
			SRD1 = 1;
			PrintCom("<G>灯已打开");
			break;
		case CODE_2:	 /*命令“关灯”*/
			SRD1 = 0;
			PrintCom("<G>灯已关闭\r\n");
			break;
		case CODE_3:		/*命令“打开电视”*/
			SRD2 = 1;
			PrintCom("<G>电视机已打开\r\n");
			break;
		case CODE_4:		/*命令“关闭电视”*/
			SRD2 = 0;
			PrintCom("<G>电视机已关闭\r\n");
			break;
		case CODE_5:		/*命令“打开冰箱”*/
			SRD3 = 1;
			PrintCom("<G>冰箱已打开\r\n");
			break;
		case CODE_6:		/*命令“关闭冰箱”*/
			SRD3 = 0;
			PrintCom("<G>冰箱已关闭\r\n");
			break;
		case CODE_7:		/*命令“打开空调”*/
			SRD4 = 1;
			PrintCom("<G>空条已打开\r\n");
			break;
		case CODE_8:		/*命令“关闭空调”*/
			SRD4 = 0;
			PrintCom("<G>空条已关闭\r\n");
			break;
		case CODE_9:		/*命令“全部打开”*/
			SRD1 = 1;
			SRD2 = 1;
			SRD3 = 1;
			SRD4 = 1;
			PrintCom("<G>已全部打开\r\n");
			break;
		case CODE_10:		/*命令“全部关闭”*/
			SRD1 = 0;
			SRD2 = 0;
			SRD3 = 0;
			SRD4 = 0;
			PrintCom("<G>已全部关闭\r\n");
			break;
		case CODE_11:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_12:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_13:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_14:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_15:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_16:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_17:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_18:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_19:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_20:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_21:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_22:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_23:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_24:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_25:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_26:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_27:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_28:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_29:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_30:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_31:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_32:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_33:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_34:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_35:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_36:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_37:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_38:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_39:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_40:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_41:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_42:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_43:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_44:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_45:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_46:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_47:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_48:		/*命令“.....”*/

			PrintCom("");
			break;
		case CODE_49:		/*命令“.....”*/

			PrintCom("");
			break;
		default:/*text.....*/
			break;
		}
	}
	else
	{
		//PrintCom("请说出一级口令\r\n"); /*text.....*/
	}
}

void tm0_isr() interrupt 1
{
    TL0 = 0x00;		//设置定时初值
	TH0 = 0x28;		//设置定时初值
	WDT_CONTR=0x3D;
		
}
