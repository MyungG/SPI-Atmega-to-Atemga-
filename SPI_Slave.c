#include <mega128.h>
#include <stdio.h>
#include <string.h>
#include <delay.h>
#include "lcd.h"
#include "SPI_Class.h"

#define char_send   0x22
#define str_send    0x23
#define enable  1
#define disable 0

unsigned char print_buf[30] = {0,};
unsigned char rx_buf = 0;
unsigned char pos = 0;

bit Rx_Start_Flg =0; //SPI ���� �غ� Ȯ�� �÷���
bit Rec_flag =0;     //SPI ���� �÷���
bit Rx_str_Flg =0;   //���ڿ� ��� �÷���

unsigned char str[] = "Rec : ";

interrupt [SPI_STC] void spi_isr(void) 
{
   
   rx_buf = SPDR;
   
   if((rx_buf == char_send) | (rx_buf == str_send)) 
   {              
        memset(print_buf,0,30);
        pos = 0; 
        
        Rx_Start_Flg = enable;
        
        //���� �� ���ڿ� ���� �˸� �ڵ忡 ���� LCD ��� ��� ����
        if(rx_buf == str_send)
            Rx_str_Flg = enable; //���ڿ��� LCD ���
        else
            Rx_str_Flg = disable;//�� ���ھ� LCD ���    
   }
   else
   {
        print_buf[pos++] = rx_buf; 
        Rec_flag = enable;
   }   
}        

void main()
{
    LCD_Init();
    LCD_Pos(0,0);
    LCD_Str(str);
    
    //���� �Լ��� ��ü ���ͷ�Ʈ �㰡 SREG |= 0x80 ��ɾ ���� �Ǿ� �ֱ� ������ �ݺ��� ������ ���� �ϵ��� ��ġ�ؾ� ��.
    Init_Slave_Int_Contr();
    
    while(1)
    {   
        if(Rx_Start_Flg)
        {
            LCD_Clear();
            LCD_Pos(0,0);
            LCD_Str(str); 
            LCD_Pos(1,0);
            
            Rx_Start_Flg = disable; 
        }   
                
        else if(Rec_flag)
        {
            if(Rx_str_Flg)
            {                
                // ��� �ڸ��� �ִ�ġ�� �Ǿ����� �ʱ�ȭ �ϴ� �Լ�, lcd.h �߰� 
                // ���ڿ� ��� ��
                LCD_Str_1Row_20L_Over(print_buf);
                
                LCD_Pos(0,6);
                LCD_Char(0x30 |((pos)/10));
                LCD_Char(0x30 |((pos)%10));
                LCD_Str("byte");                                           
            }
            else
            { 
                // �� ���ھ� ����ϴ� ��
                // LCD ��� �ִ� �ڸ����� �Ǹ� �ش� ���� ����� ó������ �ٽ� ��� �ϴ� �Լ�         
                if(pos == 20) 
                {
                    LCD_Pos(1,0);
                    LCD_Str("                    ");
                    LCD_Pos(1,0);
                }
                else;
                
                //SPI ���ͷ�Ʈ ���� ������ �迭 ��ġ�� ���� ��ġ�� ���� �Ǿ��� ������ ���� ���� ���� ���� -1�� ��ġ ����
                LCD_Char(print_buf[pos-1]); 
            }            
                        
            Rec_flag = disable;   
        }
        else; 
    }
} 
    



