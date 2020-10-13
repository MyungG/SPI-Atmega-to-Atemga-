#include <mega128.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "SPI_Class.h"

#define char_send  0x22
#define str_send  0x23

unsigned char TextString[] = "AVR communicating via the SPI";
unsigned char str[] = "Master Send : ";
static unsigned char data = 0;
static unsigned char pos = 0;
char *PtrToStrChar;

void PD0_Press()
{
    Init_Master();
    
    LCD_Clear();
    LCD_Pos(0,0);
    LCD_Str(str);
        
    Master_Send(char_send); // 한 문자씩 보내겠다고 알려 주는 구분자
    
    data ='A';
    Master_Send(data);
    
    LCD_Pos(1,0);
    LCD_Char(data);
}

void PD1_Press()
{
    Init_Master();
    
    LCD_Clear();
    LCD_Pos(0,0);
    LCD_Str(str);         
        
    for(data=0x41; data<0x5B; data++)
    {        
        delay_ms(100);
        Master_Send(char_send); // 한 문자씩 보내겠다고 알려 주는 구분자
        Master_Send(data);
        LCD_Pos(1,0);
        LCD_Char(data);
    }
}

void PD2_Press()
{
    static unsigned char len = 0;    

    Init_Master();
    len = strlen(TextString);
    
    Master_Send(char_send); // 한 문자씩 보내겠다고 알려 주는 구분자
     
    for(pos = 0; pos<len; pos++)
    {
        if(pos<20)
        {
            delay_ms(50);
            Master_Send(TextString[pos]);
            
            //LCD 출력 라인 초기화
            if(pos == 0)
            {
                LCD_Pos(1,0);
                LCD_Str("                    ");
            }
            else;
            
            LCD_Pos(1,pos);
            LCD_Char(TextString[pos]);
        }
        else if(pos>=20)
        {
            delay_ms(50);
            Master_Send(TextString[pos]); 
            
            //LCD 출력 라인 초기화
            if(pos-20 == 0)
            {
                LCD_Pos(1,0);
                LCD_Str("                    ");
            }
            else;
            
             //LCD 행에 출력 되는 최대 글자수 20글자를 넘어 그 이후 부터 다시 표기 위치를 0 자리 부터 출력 하기 위해
            LCD_Pos(1,pos-20); 
            LCD_Char(TextString[pos]);
        }
    }
}

void PD3_Press()
{
    Init_Master_IntContr();
    
    PtrToStrChar = TextString; // Set Pointer to beginning of String   
    
    SPI_CS = 0;                // Slave enable  
    delay_us(10);   // Slave가 원만한 인식을 위한 지연 시간
    
    SPDR = 0x23;           // 문자열로 보내겠다고 알려 주는 구분자
}

interrupt [SPI_STC] void spi_isr(void) 
{
  
    if(*PtrToStrChar!=0)
    {
        //delay_us(4);           //필요시! 지연 발생
        SPDR = *PtrToStrChar++;//현재 배열 주소 위치의 값을 전송                
    }
    else
        SPI_CS = 1;      //Slave disable       
}


void main()
{
    unsigned char key=0;
    unsigned char state=0;
    
    //DDRD = 0x00;
    
    LCD_Init();
    LCD_Pos(0,0);
    LCD_Str(str);   
    
    
    Init_Master();
        
    while(1)
    {
        key = (PIND&0xff);
        switch(key)
        {
            case 0xFE:
                state = 1;
                break;
            
            case 0xFD:
                state = 2;
                break;
            
            case 0xFB:
                state = 3;
                break;
            
            case 0xF7:
                state = 4;
                break;
            
            default:
                break;
        }
        switch(state)
        {
            case 1:
                PD0_Press();
                state = 0;
                break;
            case 2:
                PD1_Press();
                state = 0;
                break;
            
            case 3:
                PD2_Press();
                state = 0;
                break;
            
            case 4:
                PD3_Press();
                state = 0;
                break;
                
            default:
                break;
        }
                
        delay_ms(100);
    }
}
        