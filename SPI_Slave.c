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

bit Rx_Start_Flg =0; //SPI 수신 준비 확인 플래그
bit Rec_flag =0;     //SPI 수신 플래그
bit Rx_str_Flg =0;   //문자열 출력 플래그

unsigned char str[] = "Rec : ";

interrupt [SPI_STC] void spi_isr(void) 
{
   
   rx_buf = SPDR;
   
   if((rx_buf == char_send) | (rx_buf == str_send)) 
   {              
        memset(print_buf,0,30);
        pos = 0; 
        
        Rx_Start_Flg = enable;
        
        //문자 및 문자열 전송 알림 코드에 따라 LCD 출력 방식 결정
        if(rx_buf == str_send)
            Rx_str_Flg = enable; //문자열로 LCD 출력
        else
            Rx_str_Flg = disable;//한 문자씩 LCD 출력    
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
    
    //하위 함수는 전체 인터럽트 허가 SREG |= 0x80 명령어가 포함 되어 있기 때문에 반복문 직전에 실행 하도록 위치해야 함.
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
                // 출력 자리가 최대치가 되었을때 초기화 하는 함수, lcd.h 추가 
                // 문자열 출력 함
                LCD_Str_1Row_20L_Over(print_buf);
                
                LCD_Pos(0,6);
                LCD_Char(0x30 |((pos)/10));
                LCD_Char(0x30 |((pos)%10));
                LCD_Str("byte");                                           
            }
            else
            { 
                // 한 문자씩 출력하는 구
                // LCD 출력 최대 자리수가 되면 해당 라인 지우고 처음부터 다시 출력 하는 함수         
                if(pos == 20) 
                {
                    LCD_Pos(1,0);
                    LCD_Str("                    ");
                    LCD_Pos(1,0);
                }
                else;
                
                //SPI 인터럽트 수신 버퍼의 배열 위치가 다음 위치로 설정 되었기 때문에 현재 수신 값을 위해 -1로 위치 감소
                LCD_Char(print_buf[pos-1]); 
            }            
                        
            Rec_flag = disable;   
        }
        else; 
    }
} 
    



