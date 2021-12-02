#include<REG51.h>
#include<LCD.h>
#include<UART.h>
#include<ADC.h>
#include<GSM.h>
#include<string.h>
#define EOM 0X1A
extern char S_VAL[4];
sbit KEY1= P3^2;
sbit KEY2= P3^3;
sbit REL1= P3^4;
void main()
	{
	unsigned int i,j,DATA1,DATA2;
	unsigned char TX_DATA[10],TMP[4],HBT[4];
	REL1=0;
	REL1=0;
	LCD_INIT();
	UART_INIT();
	DIS_LCD("PATIENT MONTORNG");
	REL1=1;
	DELAY(250);
	INIT_GSM_SMS();
	REL1=0;
	LCD_CMD(0xC0);
	DIS_LCD("   SYSTEM");
	DELAY(500);DELAY(500);
	while(1)
		{
		LCD_CMD(0x01);
		DATA1=ADC(0);
		DATA1=DATA1*1.8;
		CONVERT_DAT(DATA1);
		for(i=0;i<3;i++)
			{
			TX_DATA[i]= S_VAL[i];
			}
		TX_DATA[i]='*';
		strcpy(TMP,S_VAL);
		DIS_LCD("BODY TMP : ");
		LCD_CMD(0x8A);
		DIS_LCD(S_VAL);
		DATA2=ADC(1);
		CONVERT_DAT(DATA2);
		j=4;
		for(i=0;i<3;i++)
			{
			TX_DATA[j]= S_VAL[i];
			j++;
			}
		TX_DATA[j]='\0';
		strcpy(HBT,S_VAL);
		LCD_CMD(0xC0);
		DIS_LCD("HEART BT :");
		LCD_CMD(0xCA);
		DIS_LCD(S_VAL);
		if(DATA2>80)
			{
			LCD_CMD(0x80);
			DIS_LCD("CALL THE DOCTOR");
			if(KEY1==0)
				{
				REL1=1;
				DELAY(250);
				CALL();
				REL1=0;
				DELAY(250);
				}
			}
		if(DATA2<70)
			{
			LCD_CMD(0x80);
			DIS_LCD("CALL THE DOCTOR");
			if(KEY1==0)
				{
				REL1=1;
				DELAY(250);
				CALL();
				REL1=0;
				DELAY(250);
				}
			}
		if(KEY2==0)
			{
			REL1=1;
			DELAY(250);
			TR1=1; 
			SEND_SMS();
			SEND_STRING_UART("BODY TEMP: ");
			SEND_STRING_UART(TMP);
			SEND_STRING_UART("HEART BEAT: ");
			SEND_STRING_UART(HBT);
			SEND_CRLF(EOM);
			REL1=0;
			DELAY(250);
			}
		DELAY(500);
		SEND_STRING_UART(TX_DATA);
		}
	}

Code for LCD display
#include<reg51.h>
#include<LCD.h>
unsigned char CMD_ARRAY[]={0x20,0x28,0x28,0x0e,0x00,0x01,0x00,0x06,0x00,0x80},X;
void LCD_INIT()
	{
	for(X=0;X<9;X++)
		{
		LCD_CMD(CMD_ARRAY[X]);
		}
	}
void LCD_CMD(unsigned int CMD)
	{
	unsigned int CMD1,j=0;
			
	CMD1=((CMD&0xf0)>>2);
	P1=CMD1;
	RS=0;
	EN=1;
	EN=0;
	CMD1=((CMD&0x0f)<<2);
	P1=CMD1;
	RS=0;
	EN=1;
	
	EN=0;
	DELAY(1);
//	for(j=0;j<5;j++);
	}
void DIS_LCD(unsigned char *dat)
	{
	unsigned char i;
	for(i=0;*dat!='\0';i++)
		{
		unsigned int dta1;
		if(i==16)
		LCD_CMD(0xc0); //stepping to 2nd line
		if(i>=16)
		LCD_CMD(0x06); //to increment the cursor
		dta1=((*dat&0xf0)>>2);
		P1=dta1;
		RS=1;
		EN=1;
		DELAY(10);
		EN=0;
		dta1=((*dat&0x0f)<<2);
		P1=dta1;
		RS=1;
		EN=1;
		DELAY(10);
		EN=0;
		DELAY(10);
		dat++;
		}
	}
void DELAY(unsigned int n)
	{
	unsigned int i,j;
	for(i=0;i<=n;i++)
	for(j=0;j<=100;j++);
	}

Code for ADC:
#include<ADC.h>
#include<REG51.h> 
#include<LCD.h>

sbit a=P2^0;		  // channel select bits
sbit b=P2^1;
sbit c=P2^2;
sbit SC=P2^3;		 // start of conversion
sbit EOC=P2^4;		 // end of conversion 

//sbit OE=P3^5;
sfr adcdata=0x80;
sbit CLOCK=P2^5;//for adc
unsigned char S_VAL[4];
unsigned int ADC(unsigned int AD)
	{

	unsigned int value;
	TH0|=0XD2;	//timer settings(time period 100 micro sec)	
	IE=0X82;   // intrpt enable
	TR0=1;	

	EOC=1;
//	OE=0;
	SC=0;
	a=AD;
	b=0;
	c=0;
	DELAY(50);
	SC=1;
	DELAY(50);
	SC=0;
	while(EOC==1);
	while(EOC==0);
//	OE=1;
	DELAY(50);
	value=adcdata;
//	OE=0;
	return(value); 	
	} 
void timer(void) interrupt 1
	{						
	CLOCK = ~CLOCK;
	}
void CONVERT_DAT(unsigned int hex)
	{
	unsigned int msb1=0,msb2=0,lsb0=0;
	S_VAL[0]='\0';S_VAL[1]='\0';S_VAL[2]='\0';
 while(hex>=100)
  		{
hex=hex-100;
  		msb1++;
  		}
	while(hex>=10)
  		{
  		hex=hex-10;
  		msb2++;
  		} 

  	lsb0=hex;
 		
 	msb1=msb1+0x30;
	msb2=msb2+0x30;
	lsb0=lsb0+0x30;
	
  	S_VAL[0]=msb1;
  	S_VAL[1]=msb2;
	S_VAL[2]=lsb0;
	S_VAL[4]='\0';
 	}

Code for GSM:
#include<GSM.h>
#include<REG51.h>
#define  CR 0X0D
#define  LF 0X0D
 unsigned char code CMD_6[]="AT+CMGS=\"9739461441\"";
 unsigned char code CMD_7[]="AT+VGT=45";
 unsigned char code CMD_8[]="ATD+919739461441;";
unsigned char ASD;
void INIT_GSM_SMS()
	{
	
	unsigned char code CMD_1[]="AT";
	unsigned char code CMD_2[]="ATE0";
	//	unsigned char code CMD_3[]="AT&W";
	unsigned char code CMD_4[]="AT+CMGF=1";
	unsigned char code CMD_5[]="AT+CNMI=2,2,0,0,0";
	
	SEND_CMD(CMD_1,2);
	while((ASD=RECIEV_MSG())!='K');
        
	SEND_CMD(CMD_2,4);
	while((ASD=RECIEV_MSG())!='K');


	SEND_CMD(CMD_4,9);
	while((ASD=RECIEV_MSG())!='K');

	SEND_CMD(CMD_5,17);
	while((ASD=RECIEV_MSG())!='K');

//	RECIEV_MSG();
//	RECIEV_MSG();
	}

void SEND_CMD(unsigned char *BASE_ADD,unsigned char COUNT)
	{	
	unsigned char I;
	for(I=0;I<COUNT;I++)
		{
	    SBUF=*BASE_ADD;
		while(TI==0);
		TI=0;
		BASE_ADD++;
		}
	SEND_CRLF(CR);
	SEND_CRLF(LF);
	}
void SEND_CRLF(unsigned char b)
	{
	SBUF=b;
	while(TI==0);
	TI=0;
	}

unsigned char RECIEV_MSG()
	{
	unsigned char MESSAGE;
	RI=0;
	while(RI==0);
	MESSAGE=SBUF;
	return(MESSAGE);
	}	   
void SEND_SMS()
	{
	SEND_CMD(CMD_6,21);
//	while((ASD=RECIEV_MSG())!='>');
	}
void CALL()
	{
	//SEND_CMD(CMD_7,9);
	SEND_CMD(CMD_8,17);
	}
