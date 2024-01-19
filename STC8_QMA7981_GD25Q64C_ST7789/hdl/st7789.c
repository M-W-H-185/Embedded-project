#include "st7789.h"
#include "delay.h"
//#include "font.h"

/* ����ʵ���ɫ�ͱ���ɫ */
unsigned int  POINT_COLOR = 0X0000;
unsigned int  BACK_COLOR = 0XFFFF;

/* ��ָ��ļĴ���д����  ����val����Ҫд������� */
void LCD_WR_DATA(unsigned int val)
{  
	LCD_RS=1;  //RS=1;
	LCD_CS=0;	//CS=0;
	P2=val;					
	LCD_WR=0;	//WR=0;
	LCD_WR=1;	//WR=1;
	LCD_CS=1;	//CS=1;	
}

/* ָ��Һ����ĳ���Ĵ���  ����reg�ǼĴ������� */
void LCD_WR_REG(unsigned int reg)		
{	
  LCD_RS=0;	//RS=0;
	LCD_CS=0;	//CS=0;	
	P2=reg;	
	LCD_WR=0;	//WR=0;
	LCD_WR=1;	//WR=1;
	LCD_CS=1;	//CS=1;		
}

/* Һ������ʼ������ */
void LCD_Init()
{
	delay_ms(120);                

	LCD_WR_REG(0x11);   // Sleep Out           
	delay_ms(120); 

	LCD_WR_REG(0x36);			
	LCD_WR_DATA(0x60);

	LCD_WR_REG(0x3A);			
	LCD_WR_DATA(0x05);



	LCD_WR_REG(0xB2);			
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x0C); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x33); 
	LCD_WR_DATA(0x33); 			

	LCD_WR_REG(0xB7);			
	LCD_WR_DATA(0x35);

	LCD_WR_REG(0xBB);			
	LCD_WR_DATA(0x29); //32 Vcom=1.35V
															
	LCD_WR_REG(0xC2);			
	LCD_WR_DATA(0x01);

	LCD_WR_REG(0xC3);			
	LCD_WR_DATA(0x19); //GVDD=4.8V 
															
	LCD_WR_REG(0xC4);			
	LCD_WR_DATA(0x20); //VDV, 0x20:0v

	LCD_WR_REG(0xC5);			
	LCD_WR_DATA(0x1A);//VCOM Offset Set

	LCD_WR_REG(0xC6);			
	LCD_WR_DATA(0x1F); //0x0F:60Hz        	

	LCD_WR_REG(0xD0);			
	LCD_WR_DATA(0xA4);
	LCD_WR_DATA(0xA1); 											  												  																								
				
		
	LCD_WR_REG( 0xE0);     
	LCD_WR_DATA( 0xD0);   
	LCD_WR_DATA( 0x08);   
	LCD_WR_DATA( 0x0E);   
	LCD_WR_DATA( 0x09);   
	LCD_WR_DATA( 0x09);   
	LCD_WR_DATA( 0x05);   
	LCD_WR_DATA( 0x31);   
	LCD_WR_DATA( 0x33);   
	LCD_WR_DATA( 0x48);   
	LCD_WR_DATA( 0x17);   
	LCD_WR_DATA( 0x14);   
	LCD_WR_DATA( 0x15);   
	LCD_WR_DATA( 0x31);   
	LCD_WR_DATA( 0x34);   

	LCD_WR_REG( 0xE1);     
	LCD_WR_DATA( 0xD0);   
	LCD_WR_DATA( 0x08);   
	LCD_WR_DATA( 0x0E);   
	LCD_WR_DATA( 0x09);   
	LCD_WR_DATA( 0x09);   
	LCD_WR_DATA( 0x15);   
	LCD_WR_DATA( 0x31);   
	LCD_WR_DATA( 0x33);   
	LCD_WR_DATA( 0x48);   
	LCD_WR_DATA( 0x17);   
	LCD_WR_DATA( 0x14);   
	LCD_WR_DATA( 0x15);   
	LCD_WR_DATA( 0x31);   
	LCD_WR_DATA( 0x34);
	 

	LCD_WR_REG(0x21);

	LCD_WR_REG(0x29);
	TFT_BL = 0; // ��Һ���������
	

}

/* ������ʾ���� ���������� */
void LCD_SetArea(unsigned int stx,unsigned int sty,unsigned int endx,unsigned int endy)
{
	LCD_WR_REG(0x2A);  
	LCD_WR_DATA(stx>>8);    
	LCD_WR_DATA(stx&0xff);    	
	LCD_WR_DATA(endx>>8); 
	LCD_WR_DATA(endx&0xff);	

	LCD_WR_REG(0x2B);  
	LCD_WR_DATA(sty>>8); 
	LCD_WR_DATA(sty&0xff);	
	LCD_WR_DATA(endy>>8); 
	LCD_WR_DATA(endy&0xff);	
}

/* ���㺯�� ��������ɫ */
void LcdWirteColorData(unsigned int color)
{
	LCD_RS=1;  //RS=1;
	LCD_CS=0;	//CS=0;
 	P2=color>>8;						
	LCD_WR=0;	//WR=0;
	LCD_WR=1;	//WR=1;
	P2=color;					
	LCD_WR=0;	//WR=0;
	LCD_WR=1;	//WR=1;
	LCD_CS=1;	//CS=1;	
}

/* �������� ��������ɫ */
void LCD_Clear(unsigned int color)
{  
	xdata unsigned int  i;
	xdata unsigned int  j;
	

	LCD_SetArea(0,0,319,239);
	LCD_WR_REG(0x2C);
	for(i=0;i<240;i++)
	{
		for(j=0;j<320;j++)
		{
			LcdWirteColorData(color);
		}
	}
}

///* ��ʾ16*24����Ӣ���ַ� ����xy������ ����zifu����font�����еĵڼ��� */
//void LCD_ShowFont(unsigned int x, unsigned int y, unsigned char num)
//{
//	unsigned int i,j;
//	unsigned char temp;
//	
//	LCD_SetArea(x,y,x+15,y+23);
//  LCD_WR_REG(0x2C);
//	
//	for(i=0;i<48;i++)
//	{
//		temp = font[num][i];
//		for(j=0;j<8;j++)
//		{
//			if(temp&0x80)
//				LcdWirteColorData(POINT_COLOR);
//			else
//				LcdWirteColorData(BACK_COLOR);
//			temp=temp<<1;
//		}
//	}
//}

/* ��һ���� ����xy�ǵ������ */
void LCD_DrawPoint(unsigned int x, unsigned int y)
{
	LCD_SetArea(x,y,x+1,y+1);
	LCD_WR_REG(0x2C);
	LcdWirteColorData(POINT_COLOR);
}

/* ���ߺ��� ����(x1,y1)(x2,y2)���ߵ������˵����� */
void LCD_DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	xdata unsigned int i; 
	
	xdata int Error_X = 0, Error_Y = 0, Delta_X, Delta_Y, Distance; 
	xdata int Increase_X, Increase_Y, usX_Current, usY_Current;	
	
	Delta_X = x2 - x1; //������������ 
	Delta_Y = y2 - y1; 
	
	usX_Current = x1; 
	usY_Current = y1; 
	
	
	if ( Delta_X > 0 ) Increase_X = 1; //���õ������� 
	else if ( Delta_X == 0 ) Increase_X = 0;//��ֱ�� 
	else { Increase_X = -1;Delta_X = - Delta_X;} 
	
	if ( Delta_Y > 0 ) Increase_Y = 1; 
	else if ( Delta_Y == 0 ) Increase_Y = 0;//ˮƽ�� 
	else {Increase_Y = -1;Delta_Y = - Delta_Y;} 
	
	if (  Delta_X > Delta_Y ) Distance = Delta_X; //ѡȡ�������������� 
	else Distance = Delta_Y; 
	
	for ( i = 0; i <= Distance + 1; i++ )//������� 
	{  	
		
		LCD_DrawPoint(usX_Current, usY_Current);
		Error_X += Delta_X ; 
		Error_Y += Delta_Y ; 
		
		if ( Error_X > Distance ) 
		{ 
			Error_X -= Distance; 
			usX_Current += Increase_X; 
		}  
		if ( Error_Y > Distance ) 
		{ 
			Error_Y -= Distance; 
			usY_Current += Increase_Y; 
		} 		
	}  
}

/* ������ ����(x1,y1)(x2,y2)�Ǿ��εĶԽ����� */
void LCD_DrawRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

/* ������� */
void LCD_Fill(unsigned int stx, unsigned int sty, unsigned int endx, unsigned int endy, unsigned int color)
{          
	unsigned int i,j;

	LCD_SetArea(stx, sty, endx, endy);
	LCD_WR_REG(0x2C);
	for(i=0;i<endx+1;i++)
	{
		for(j=0;j<endy+1;j++)
		{
			LcdWirteColorData(color);
		}
	}
}

