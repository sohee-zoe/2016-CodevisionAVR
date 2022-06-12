// C : LED
// D : LCD
// E4, E5 : 인터럽트
// F0 : 전압 조절  F1 : CDS 

#include <stdio.h>
#include <mega128.h>
#include <delay.h>

////////////////////////////////// LCD define //////////////////////////////////////

#define FUNCSET                0x28         // Function Set
#define ENTMODE               0x06         // Entry Mode Set
#define ALLCLR                  0x01         //  All Clear
#define DISPON                 0x0C         //  Display On
#define DISPOFF               0x08         //  Display OFF
#define HOME                    0x02         //  Cursor Home
#define LINE1                    0x80        //  2nd Line Move
#define LINE2                    0xC0        //  2nd Line Move
#define RSHIFT                  0x1C         //  Display Right Shift

////////////////////////////////////////////////////////////////////////////

/////////////////////// 함수 원형 ////////////////////////////////////////////


void LCD_init(void);
void LCD_String(char flash *);
void LCD_printf(char *);
void Command(unsigned char);
void Data(unsigned char);
void Busy(void);       
void LCD_pos(unsigned , unsigned );

void External_Interrupt_Initilalize(void);

void what_sw(void);

void sw1_1(void);
void sw1_2(void);
void sw1_3(void);
void sw2_1(void);
void sw2_2(void);
void sw2_3(void);
void sw2_4(void);
void sw_reset(void);


////////////////////////////////////////////////////////////////////////////


/////////////////////// 전역 함수 ////////////////////////////////////////////

unsigned char led = 0x00;
unsigned int cnt1, cnt2;       // 인터럽트 발생 횟수   
int btn=0;                         // 무슨 버튼 눌렀나
unsigned char t[1];         // 무슨 버튼 눌렀다 LCD
unsigned char c1[1];       // 1번 버튼 횟수  LCD
unsigned char c2[1];       // 2번 버튼 횟수    LCD
  

////////////////////////////////////////////////////////////////////////////


//////////////////////////////// main /////////////////////////////////////////////

void main(void)
{
        LCD_init();
      
        // 포트 초기화
        DDRC = 0xFF;                // 포트 C 출력 설정
        DDRE = 0b00000010;     // 포트 E 입력(PE1 출력)

        PORTC = ~led;
        
         //외부 인터럽트 요구 초기화 함수
      External_Interrupt_Initilalize();          
        
        cnt1 = 0;
        cnt2 = 0;    
 
while(1)
        {            
            if(btn==1 && cnt1==1)        sw1_1();
            else if(btn==1 && cnt1==2) sw1_2();

            
            else if(btn==2 && cnt2==1) sw2_1();          
            else if(btn==2 && cnt2==2){LCD_init(); sw2_2(); }       
            else if (cnt1==3 || cnt2==3)  sw_reset();
        }

}

/////////////////////////////////////////////////////////////////////////




//////////////////// LCD /////////////////////////////////////////////////////////////////////////////////////////

//LCD 초기화
void LCD_init(void)            
{
    DDRD = 0xFF;            // 포트 D 출력 설정
    PORTD &= 0xFB;      // E=0;                
                            
    delay_ms(15);
    Command(0x20);
    delay_ms(5);
    Command(0x20);
    delay_us(100);
    Command(0x20);
    Command(FUNCSET);
    Command(DISPON);
    Command(ALLCLR);
    Command(ENTMODE);
}


// 문자열 출력 함수
void LCD_String(char flash *str)
{
    char flash *pStr=0;
    
    pStr = str;
    while(*pStr)
        Data(*pStr++);
}    



//변수 출력 함수
void LCD_printf(char *str)
{
        char *pStr=0;
        
        pStr = str;
        while(*pStr) Data(*pStr++);
}


// 인스트럭션 쓰기 함수
void Command(unsigned char byte)
{
    Busy();
    
    // 인스트럭션 상위 바이트
    PORTD = (byte & 0xF0);      // 데이터
    PORTD &= 0xFE;               // RS = 0 명렁어
    PORTD &= 0xFD;              // RW = 0 Write
    delay_us(1);
    PORTD |= 0x04;                // E = 1  LCD 동작
    delay_us(1);
    PORTD &= 0xFB;              // E = 0;
    
    // 인스트럭션 하위 바이트
    PORTD = ((byte<<4) & 0xF0);         //데이터
    PORTD &= 0xFE;                         // RS = 0 명렁어
    PORTD &= 0xFD;                        // RW = 0 Write
    delay_us(1);
    PORTD |= 0x04;                          // E = 1 LCD 동작
    delay_us(1);
    PORTD &= 0xFB;                       // E = 0;
 }
 

// 데이터 쓰기 함수
void Data(unsigned char byte)
{
    Busy();
    
    // 데이터  상위 바이트
    PORTD = (byte & 0xF0);      // 데이터
    PORTD |= 0x01;               // RS = 1 데이터
    PORTD &= 0xFD;              // RW = 0 Write
    delay_us(1);
    PORTD |= 0x04;                // E = 1 LCD 동작
    delay_us(1);
    PORTD &= 0xFB;              // E = 0;
    
    // 데이터  하위 바이트
    PORTD = ((byte<<4) & 0xF0);         //데이터  좌로 4 Shift
    PORTD |= 0x01;                         // RS = 1;
    PORTD &= 0xFD;                        // RW = 0;
    delay_us(1);
    PORTD |= 0x04;                          // E = 1;
    delay_us(1);
    PORTD &= 0xFB;                       // E = 0;
 }           


// 표시 위치 설정
void LCD_pos(unsigned col, unsigned row)
{
    Command( 0x80 | ( row + col*0x40 ) );           // col 문자열, row 문자행
}
// 표시 위치와 DDRAM 어드레스 번지와의 관계 생각할 것
  



// Busy Flag Check -> 일반적인 BF 체크하는 것이 아니라 일정한 시간 지연 이용
void Busy(void)
{
    delay_ms(2);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////// 인터럽트 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//외부 인터럽트 요구 초기화 함수
void External_Interrupt_Initilalize(void)
{
        EICRB = 0b00001111;     // 외부 인터럽트 4, 5 : 상승에지
        EIMSK = 0b00110000;    // 외부 인터럽트 4, 5 인에이블
        EIFR = 0xFF;                  // 외부 인터럽트 플래그
        SREG = 0x80;                 // 전역 인터럽트 인에이블 비트 셋      // sei();
}

// 외부 인터럽트4 서비스 루틴  
interrupt [EXT_INT4] void external_int4(void)
{      
   cnt1++;   btn=1;
}

// 외부 인터럽트5 서비스 루틴  
interrupt [EXT_INT5] void external_int5(void)
{                      
    cnt2++;  btn=2;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////// 스위치 확인   /////////////////////////////////////////
 
void what_sw(void)     // 어떤 스위치 몇 번 눌렀나
{   
            LCD_pos(0, 12);
            LCD_String("    ");         
            LCD_pos(1, 12);
            LCD_String("    ");

            if(btn==1)
            { 
            LCD_pos(0, 12);
            LCD_String("S"); 
            LCD_pos(0, 13);  
            sprintf(t, "%d", btn);
            LCD_printf(t);             
            LCD_pos(0, 15);  
            sprintf(c1, "%d", cnt1); 
            LCD_printf(c1);
           }
           
           if(btn==2)
           { 
            LCD_pos(1, 12);
            LCD_String("S"); 
            LCD_pos(1, 13);  
            sprintf(t, "%d", btn);
            LCD_printf(t);  
            LCD_pos(1, 15);
            sprintf(c2, "%d", cnt2);  
            LCD_printf(c2);    
            }  
}
//////////////////////////////////////////////////////////////////////////////////////////////////


////////////// 스위치별 함수 //////////////////////////////////////////////////////////////////////////////

void sw1_1(void)
{

     
     while(1)
    {       
         
         break; 
    }              
}

void sw1_2(void)
{

     
     while(1)
    {       
         
         break; 
    }              
}

void sw2_1(void)
{

     
     while(1)
    {       
         
         break; 
    }              
}

void sw2_2(void)
{

     
     while(1)
    {       
         
         break; 
    }              
}

void sw_reset(void)
{
            LCD_init();
            PORTC=0xff;
            
            cnt1=0;
            cnt2=0;      
}
