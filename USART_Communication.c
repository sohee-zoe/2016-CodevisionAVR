// C : LED            // D : LCD         // E4, E5 : 인터럽트             
// F0 : 전압 조절  F1 : CDS                    // H : Enable A,       모터 B4 : Input1        B7 : Input2
//  BAUD=9600   DataBit=8bit    StopBit=1bit    Parity=None

#include <stdio.h>
#include <mega128.h>
#include <delay.h>
////////////////////////////////// 매크로 //////////////////////////////////////////

//-------------------- LCD -----------------------------------------//
#define FUNCSET                0x28         // Function Set
#define ENTMODE               0x06         // Entry Mode Set
#define ALLCLR                  0x01         //  All Clear
#define DISPON                 0x0C         //  Display On
#define DISPOFF               0x08         //  Display OFF
#define HOME                    0x02         //  Cursor Home
#define LINE1                    0x80        //  2nd Line Move
#define LINE2                    0xC0        //  2nd Line Move
#define RSHIFT                  0x1C         //  Display Right Shift

//------------------------- MOTOR --------------------------------------//
#define RIGHT   0x80       // 시계방향
#define LEFT    0x10        // 반시계방향
#define STOP    0x00        // 구동 정지

/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////// 함수 원형 ////////////////////////////////////////////

//------------ LCD --------------------------//
void LCD_init(void);
void LCD_String(char flash *);
void LCD_printf(char *);
void Command(unsigned char);
void Data(unsigned char);
void Busy(void);       
void LCD_pos(unsigned , unsigned );

//----------- 인터럽트 -----------------------//
void External_Interrupt_Initilalize(void);

//--------- 타이머/카운터 --------------//
void fast_PWM_Setting(void);

//------------------ 기능 함수 ----------------------//
void CDS_VALUE(void);
void VOLTAGE_VALUE(void);
void put_a(void);
void put_b(void);

//------------- 통신 ------------------------//
void USART_init(void);
void Putch(char);   // 1바이트 송신
char Getch(void);   // 1바이트 수신

////////////////////////////////////////////////////////////////////////////

 
/////////////////////// 전역 변수 ////////////////////////////////////////////

unsigned char led = 0x00;
char str[30];
int ADC_I;
float ADC_F;

 ////////////////////////////////////////////////////////////////////////////                  
 
 
//////////////////////////////// main /////////////////////////////////////////////
      
void main(void)
{
        LCD_init();
        USART_init();
        
        // 포트 초기화
        DDRC = 0xFF;                // 포트 C 출력 설정  
        PORTC = ~led;
        DDRE = 0b00000010;     // 포트 E 입력(PE1 출력)    
        DDRF&=0xFE; 
        DDRB = 0xFF;                // MOTOR PB4, PB7 출력
        
        ADMUX=0b00000000;       //use AREF, adc channel 0, 단극성 전압 입력,  우측 정렬
        ADCSRA=0xE7;               //adc enable, start, prescaler 128
                    
        External_Interrupt_Initilalize(); //외부 인터럽트 요구 초기화 함수         
        fast_PWM_Setting();   // 타이머/카운터 fast PWM, 64분주      
                       
        while(1)
        {            
            unsigned char temp;
            temp=Getch(); 

            switch(temp)
            {
                case 'a' : 
                put_a();
                break;

                case 'b' : 
                put_b();             
                break;
            }
        }            
      
}

/////////////////////////////////////////////////////////////////////////



//////////////////// LCD /////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////// 타이머/ 카운터 초기화 //////////////////////////////////////////////////////////

void fast_PWM_Setting(void)
{
     // 타이머/카운터 인터럽트 마스크 레지스터
//    TIMSK = (1<<OCIE0) | (1<<OCIE2);         // 출력 비교 매치 인터럽트 인에이블   
//   TIMSK = (1<<TOIE1) | (1<<TOIE2);        // 오버플로우 인터럽트 인에이블 
    TIMSK = (1<<OCIE0); 
       
    // 타이머/카운터 제어 레지스터                            
    TCCR0 =  (1<<WGM01) | (1<<WGM00)   // fast PWM mode
                | (1<<COM01) | (0<<COM00)    // 비반전 비교 출력 모드
                | (0<<CS02) |  (1<<CS02) | (1<<CS02) ;  // 분주비 64    
    TCCR2 = (1<<WGM01) | (1<<WGM00)   // fast PWM mode
                | (1<<COM01) | (0<<COM00)    // 비반전 비교 출력 모드
                | (1<<CS02) |  (0<<CS02) | (0<<CS02) ;  // 분주비 64       
                           
    OCR0 = 0;            // 출력 비교 레지스터      역방향
    OCR2 = 0;            // 출력 비교 레지스터     정방향
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////// 인터럽트 ///////////////////////////////////////////////////////////////////////

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
 
}

// 외부 인터럽트5 서비스 루틴  
interrupt [EXT_INT5] void external_int5(void)
{                      

}

interrupt [TIM0_COMP] void timer_comp0_isr(void)    // 카운터 내부 인터럽트
{
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 
/////////////////////////////////// 기능 함수 ///////////////////////////////////////////////////////

//-------------ADC ------------------------------//
void CDS_VALUE(void)   //조도량
{
      ADC_I=ADCW;
      //ADC_F=(float)ADC_I * 5.0/1024.0;
      sprintf(str,"ADC = %4d", ADC_I); 
      LCD_pos(0, 0); 
      LCD_printf(str);  
  }

void VOLTAGE_VALUE(void)  // 볼트값
{
      ADC_I=ADCW;
      ADC_F=(float)ADC_I * 5.0/1024.0;    
      sprintf(str,"Volt = %4.3f V", ADC_F); 
      LCD_pos(1, 0);
      LCD_printf(str);   
  }       
    
void put_a(void)
{    
    led = 0xFF;
    PORTC = ~led;
    CDS_VALUE();
    VOLTAGE_VALUE();
    OCR2 = 200;      
    delay_ms(500);
}
  
void put_b(void)
{
    led = 0x00;
    PORTC = ~led;
    LCD_init();
    OCR2 = 0;      
    delay_ms(500);
}
  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////// 통신 ///////////////////////////////////////////////////////////////

void USART_init(void)    // USART 초기화
{
    UCSR0A = 0x0;
    UCSR0B = (1<<RXEN0) | (1<<TXEN0); // 수신기, 송신기 인에이블
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // Bit6 : 0 비동기, Bit 5,4 : 00 패리티 모드 사용 x , Bit 3 : 0 정지비트길이 1비트, Bit 2, 1 : 11 8비트 Bit 0 : 0 송신 상승, 수신 하강에지
    UBRR0H = 0;     // X-TAL = 16MHz 일 때, BAUD = 9600
    UBRR0L = 103;  //0x67;                                  
}    
    
void Putch(char data)     // 1바이트 송신      Tx      MCU -> PC
{
    while(!(UCSR0A & 0x20));    //UDRE0 = 1?    같아지면 멈춤
    UDR0 = data;    // USART I/O 레지스터에 데이터 전송
} 

char Getch(void)     // 1바이트 수신              Rx      MCU <- PC
{
    while(!(UCSR0A & 0x80));    // RXC = 1?    같아지면 멈춤
    return UDR0;    // USART I/O 레지스터 리턴
}
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////