#include <reg51.h>
#include <stdio.h>


#define uchar unsigned char
#define ulint unsigned long int
#define L 5.0 //���������⴫������ľ���


//----------------------------------------------------------------------------------------------------------------------------
//���Ŷ���
//----------------------------------------------------------------------------------------------------------------------------


//���ܺ��⴫��ģ������źŵ�����
sbit key1=P3^2;//ģ��1,int0�½����ж�

sbit key2=P3^3;//ģ��2,int1�½����ж�
sbit key3=P2^2;//ģ��3,io��P2.2����Ӧ������ʱģ������͵�ƽ������Ϊ�ߵ�ƽ

//--------------------------------------------------------------------------------------------------------------------

//LED�ƣ�ÿ������ģ����Ӧʱ��Ӧ��ָʾ����

//blue lights
sbit led11=P1^7;//11
sbit led12=P1^6;//12

//yellow lights
sbit led21=P1^5;//21
sbit led22=P1^4;//22

//green lights
sbit led31=P1^3;//31
sbit led32=P1^2;//32


//--------------------------------------------------------------------------------------------------------------------

//LCD���ӿڶ���
sbit RS=P1^0;//�Ĵ���ѡ��λ��1Ϊѡ�����ݼĴ�����0Ϊѡ��ָ��Ĵ���
sbit RW=P1^1;//��д�ź�λ��1Ϊ��������0Ϊд����
sbit EN=P2^5;//ʹ��λ���ߵ�ƽʹ��


//----------------------------------------------------------------------------------------------------------------------------
//��������
//----------------------------------------------------------------------------------------------------------------------------


//��һ��ʱ��time0���ڶ���ʱ��time1
static ulint time0,time1;

//��ʱ��0����ʱ��1�������count0��count1
static ulint count0=0,count1=0;

//�����Ӧģ����Ӧ��־������Ӧ���������ģ��״̬��־��1
static	ulint state1,state2,state3;


//----------------------------------------------------------------------------------------------------------------------------
//��������
//----------------------------------------------------------------------------------------------------------------------------


void delay(ulint n);//��ʱ

void int0initial(void);//int0��ʼ��

void int1initial(void);//int1��ʼ��

void time0initial(void);//time0��ʼ��

void time1initial(void);//time1��ʼ��

void lcdwritedata(uchar dat);//LCD��д����

void lcdwritecmd(uchar cmd);//LCD��дָ��

void lcdinitial(void);//LCD����ʼ��

void lcddisplay(uchar x, uchar y, uchar *str);//LCD���ۺ���ʾ����

void time0reset(void);//time0��λ
	
void time1reset(void);//time1��λ


//----------------------------------------------------------------------------------------------------------------------------
//��������
//----------------------------------------------------------------------------------------------------------------------------


//��ʱ����,n=1��ʱ1ms
void delay(ulint n)
{
	ulint i,j;
	for(i=n;i>0;i--)
	{
		for(j=110;j>0;j--);
	}
}



//�ⲿ�ж�int0��ʼ��
void int0initial(void)
{
	IT0=1;//�½��ش���
	EX0=1;//�ⲿ�ж�0����
	EA=1;//�����ж�
	PX0=0;//INT0�����ȼ�
}



//�ⲿ�ж�int1��ʼ��
void int1initial(void)
{
	IT1=1;
	EX1=1;
	EA=1;
	PX1=1;//INT1�����ȼ�
}



//time0��ʼ��
void time0initial()
{
	TMOD=0x11;//��ʱ��0��1��Ϊ������ʽ1
	TH0=0;
	TL0=0;//TH,TL��ֵ��Ϊ0
	TR0=0;//��ʱ���ر�
	EA=1;//�����ж�
	ET0=1;//����time0�ж�
	PT0=0;//T0�����ȼ�
}



//time1��ʼ��
void time1initial()
{
	TMOD=0x11;//��ʱ��0��1��Ϊ������ʽ1
	TH1=0;
	TL1=0;
	TR1=0;
	EA=1;
	ET1=1;
	PT1=1;//T1�����ȼ�
}



//LCD��д����
void lcdwritedata(uchar dat)
{
	RS=1;//ѡ�����ݼĴ���
	P0=dat;//P0 IO�ڽ�LCD�����ݿ�
	delay(2);
	EN=1;
	delay(2);
	EN=0;
}



//LCD��дָ��
void lcdwritecmd(uchar cmd)
{
	RS=0;//ѡ��ָ��Ĵ���
	P0=cmd;
	delay(2);
	EN=1;
	delay(2);
	EN=0;
}



//LCD����ʼ��
void lcdinitial(void)
{
	RW=0;//д����
	lcdwritecmd(0x38);//��Ļ��ʼ��
	lcdwritecmd(0x0c);//����ʾ���رչ��
	lcdwritecmd(0x06);//ÿ����дһ���ַ���ָ�����һλ
	lcdwritecmd(0x01);//����
	lcdwritecmd(0x80);//�����ַ���ʾλ��
}



//LCD���ۺ���ʾ����
//�ӵ�y�У���x�п�ʼдstr�ַ���
void lcddisplay(uchar x, uchar y, uchar* str)
{
	uchar addr;
	
	if(y==0)
	{
		addr=0x00+x;//��һ�е�xλ�ÿ�ʼ��ʾ
	}
	else
	{
		addr=0x40+x;//�ڶ��е�xλ�ÿ�ʼ��ʾ
	}
	lcdwritecmd(addr+0x80);
	while(*str!='\0')
	{
		lcdwritedata(*str++);
	}
}



//��ʱ��0��λ����
void time0reset(void)
{
	TH0=0;
	TL0=0;//TH,TL��ֵ��0
	count0=0;//���������0
}



//��ʱ��1��λ����
void time1reset(void)
{
	TH1=0;
	TL1=0;
	count1=0;
}



//time0�ж�
void time0stop() interrupt 1
{
	count0++;//�ж�һ�����������һ
	TH0=0;
	TL0=0;//���¸���ֵ,TH,TL=0;
}



//time1�ж�
void time1stop() interrupt 3
{
	count1++;
	TH1=0;
	TL1=0;
}



//�ⲿ�ж�INT0
void int0() interrupt 0
{
		state1=1;//����ģ��1���յ��ź�
		TR0=1;//��ʱ��0��ʼ
		led11=led12=0;//ָʾ����		
}



//�ⲿ�ж�INT1
void int1() interrupt 2
{
	if(state1==1)//���ģ��1�Ѿ�������ͨ�����ż���ִ�У�������Ӧ
	{
		state2=1;
		TR0=0;//time0 stop
		TR1=1;//time1 begin
		time0=(count0*65536+TH0*256+TL0);	//��λus
		led11=led12=1;
		led21=led22=0;
	}
	else
	{
		led21=led22=1;
	}	
}



//----------------------------------------------------------------------------------------------------------------------------
//������
//----------------------------------------------------------------------------------------------------------------------------


main()
{
	
	uchar str[10];
	float v1=0.0,v2=0.0,v=0.0;
	
	
	//��ʱ����ʼ��
	time0initial();
	time1initial();
	
	//INT0��ʼ��
	int0initial();
	
	//INT1��ʼ��
	int1initial();
	
	//LCD����ʼ��
	lcdinitial();
	
	
	while(1)
	{
				
		if(key3==0&&state2==1)//��ǰ����ģ�鶼�����徭������ģ��3��Ӧ������ʱִ��
		{
			state3=1;
			TR1=0;//��ʱ��1����
			time1=(count1*65536+TH1*256+TL1);
			led21=led22=1;
			led31=led32=0;	
		}
		else
		{
			led31=led32=1;
		}


	
		if(state3==1)
		{
			
			led11=led12=1;
			led21=led22=1;
			
			v1=L/time0;
			v2=L/time1;
			
			v=(float)(v1+v2)*1000000/2.0;

			//������ת�ַ���
			sprintf(str,"%f",v);
			
			lcddisplay(0,0,"Average Velocity");
			lcddisplay(0,1,str);
			lcddisplay(12,1,"cm/s");
			
			state1=state2=state3=0;
			
			time0reset();
			time1reset();
		}
	}			
}

