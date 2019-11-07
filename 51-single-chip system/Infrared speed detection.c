#include <reg51.h>
#include <stdio.h>


#define uchar unsigned char
#define ulint unsigned long int
#define L 5.0 //相邻两红外传感器间的距离


//----------------------------------------------------------------------------------------------------------------------------
//引脚定义
//----------------------------------------------------------------------------------------------------------------------------


//接受红外传感模块输出信号的引脚
sbit key1=P3^2;//模块1,int0下降沿中断
sbit key2=P3^3;//模块2,int1下降沿中断
sbit key3=P2^2;//模块3,io口P2.2，感应到物体时模块输出低电平，否则为高电平

//--------------------------------------------------------------------------------------------------------------------

//LED灯，每个红外模块响应时对应的指示灯亮

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

//LCD屏接口定义
sbit RS=P1^0;//寄存器选择位，1为选择数据寄存器，0为选择指令寄存器
sbit RW=P1^1;//读写信号位，1为读操作，0为写操作
sbit EN=P2^5;//使能位，高电平使能


//蜂鸣器引脚
sbit beep=P2^3;//蜂鸣器

//----------------------------------------------------------------------------------------------------------------------------
//变量定义
//----------------------------------------------------------------------------------------------------------------------------


//第一段时间time0，第二段时间time1
static ulint time0,time1;

//计时器0，计时器1溢出次数count0，count1
static ulint count0=0,count1=0;

//红外感应模块响应标志，若感应到物体则该模块状态标志置1
static	ulint state1,state2,state3;


//----------------------------------------------------------------------------------------------------------------------------
//函数声明
//----------------------------------------------------------------------------------------------------------------------------


void delay(ulint n);//延时

void int0initial(void);//int0初始化

void int1initial(void);//int1初始化

void time0initial(void);//time0初始化

void time1initial(void);//time1初始化

void lcdwritedata(uchar dat);//LCD屏写数据

void lcdwritecmd(uchar cmd);//LCD屏写指令

void lcdinitial(void);//LCD屏初始化

void lcddisplay(uchar x, uchar y, uchar *str);//LCD屏综合显示函数

void time0reset(void);//time0复位
	
void time1reset(void);//time1复位


//----------------------------------------------------------------------------------------------------------------------------
//函数定义
//----------------------------------------------------------------------------------------------------------------------------


//延时函数,n=1延时1ms
void delay(ulint n)
{
	ulint i,j;
	for(i=n;i>0;i--)
	{
		for(j=110;j>0;j--);
	}
}



//外部中断int0初始化
void int0initial(void)
{
	IT0=1;//下降沿触发
	EX0=1;//外部中断0允许
	EA=1;//开总中断
	PX0=0;//INT0低优先级
}



//外部中断int1初始化
void int1initial(void)
{
	IT1=1;
	EX1=1;
	EA=1;
	PX1=1;//INT1高优先级
}



//time0初始化
void time0initial()
{
	TMOD=0x11;//定时器0和1都为工作方式1
	TH0=0;
	TL0=0;//TH,TL初值都为0
	TR0=0;//计时器关闭
	EA=1;//开总中断
	ET0=1;//允许time0中断
	PT0=0;//T0低优先级
}



//time1初始化
void time1initial()
{
	TMOD=0x11;//定时器0和1都为工作方式1
	TH1=0;
	TL1=0;
	TR1=0;
	EA=1;
	ET1=1;
	PT1=1;//T1高优先级
}



//LCD屏写数据
void lcdwritedata(uchar dat)
{
	RS=1;//选择数据寄存器
	P0=dat;//P0 IO口接LCD屏数据口
	delay(2);
	EN=1;
	delay(2);
	EN=0;
}



//LCD屏写指令
void lcdwritecmd(uchar cmd)
{
	RS=0;//选择指令寄存器
	P0=cmd;
	delay(2);
	EN=1;
	delay(2);
	EN=0;
}



//LCD屏初始化
void lcdinitial(void)
{
	RW=0;//写操作
	lcdwritecmd(0x38);//屏幕初始化
	lcdwritecmd(0x0c);//打开显示，关闭光标
	lcdwritecmd(0x06);//每读或写一个字符，指针后移一位
	lcdwritecmd(0x01);//清屏
	lcdwritecmd(0x80);//设置字符显示位置
}



//LCD屏综合显示函数
//从第y行，第x列开始写str字符串
void lcddisplay(uchar x, uchar y, uchar* str)
{
	uchar addr;
	
	if(y==0)
	{
		addr=0x00+x;//第一行的x位置开始显示
	}
	else
	{
		addr=0x40+x;//第二行的x位置开始显示
	}
	lcdwritecmd(addr+0x80);
	while(*str!='\0')
	{
		lcdwritedata(*str++);
	}
}



//定时器0复位函数
void time0reset(void)
{
	TH0=0;
	TL0=0;//TH,TL初值置0
	count0=0;//溢出次数清0
}



//定时器1复位函数
void time1reset(void)
{
	TH1=0;
	TL1=0;
	count1=0;
}



//time0中断
void time0stop() interrupt 1
{
	count0++;//中断一次溢出次数加一
	TH0=0;
	TL0=0;//重新赋初值,TH,TL=0;
}



//time1中断
void time1stop() interrupt 3
{
	count1++;
	TH1=0;
	TL1=0;
}



//外部中断INT0
void int0() interrupt 0
{
		state1=1;//红外模块1接收到信号
		TR0=1;//计时器0开始
		led11=led12=0;//指示灯亮		
}



//外部中断INT1
void int1() interrupt 2
{
	if(state1==1)//如果模块1已经有物体通过，才继续执行，否则不响应
	{
		state2=1;
		TR0=0;//time0 stop
		TR1=1;//time1 begin
		time0=(count0*65536+TH0*256+TL0);	//单位us
		led11=led12=1;
		led21=led22=0;
	}
	else
	{
		led21=led22=1;
	}	
}



//----------------------------------------------------------------------------------------------------------------------------
//主函数
//----------------------------------------------------------------------------------------------------------------------------


main()
{
	
	uchar str[10];
	float v1=0.0,v2=0.0,v=0.0;
	
	
	//定时器初始化
	time0initial();
	time1initial();
	
	//INT0初始化
	int0initial();
	
	//INT1初始化
	int1initial();
	
	//LCD屏初始化
	lcdinitial();
	
	
	while(1)
	{
				
		if(key3==0&&state2==1)//当前两个模块都有物体经过，且模块3感应到物体时执行
		{
			state3=1;
			TR1=0;//计时器1结束
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
			

			//浮点数转字符串
			sprintf(str,"%f",v);
			
			lcddisplay(0,0,"Average Velocity");
			lcddisplay(0,1,str);
			lcddisplay(12,1,"cm/s");
			
			//如果速度大于设定值，报警器响
			if(v>=60.0)
			{
				beep=0;
				delay(500);
				beep=1;
			}
			
			state1=state2=state3=0;
			
			time0reset();
			time1reset();
		}
	}			
}

