
#include <mega128.h>
#include <delay.h>

#define FUNCSET                0x28         // Function Set
#define ENTMODE               0x06         // Entry Mode Set
#define ALLCLR                  0x01         //  All Clear
#define DISPON                 0x0C         //  Display On
#define DISPOFF               0x08         //  Display OFF
#define HOME                    0x02         //  Cursor Home
#define LINE1                    0x80        //  2nd Line Move
#define LINE2                    0xC0        //  2nd Line Move
#define RSHIFT                  0x1C         //  Display Right Shift



void LCD_init(void);
void LCD_String(char flash *);
void Command(unsigned char);
void Data(unsigned char);
void Busy(void);
// void LCD_location(unsigned char x, unsigned char y);               
void LCD_pos(unsigned , unsigned );

unsigned char led = 0xFE;

void main(void)
{
   LCD_init();

    while(1)
    {  /*        
         Command(HOME);
         LCD_String("20141828");
        
       //  Command(LINE2);
         LCD_location(1, 7);
         LCD_String("Lim Sohee");
         delay_ms(500);  
        */
        

        LCD_pos(0, 0);
        LCD_String("20141828");
        
        LCD_pos(1, 7);
        LCD_String("Lim sohee");
        delay_ms(500);   
        
        
        // 포트 초기화
        DDRC = 0xFF;
        DDRE = 0b00000010;
        
        PORTC = led;
        
        EICRB = 0b00001010;     // 외부 인터럽트 4, 5 : 하강에지
        EIMSK = 0b00110000;    // 외부 인터럷트 4, 5 인에이블
        SREG = 0x80;                 // 전역 인터럽트 인에이블 비트 셋
        
        while(1);
                 
    }
}




/****************************************** LCD  시작 *************************************************************/

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


/*
//글자 위치
void LCD_location(unsigned char x, unsigned char y)
{
 y = (x << 6) + y; // '<<' 해주는 이유는 시프트를 시켜줌으로써 그다음 계산에서 줄을 몇칸 띄울것 인가를결정 ex) x=1 이면 한칸 띄우고 둘째줄부터 출력

 y |= 0x80;     // 비트 7을 셋함, DDR주소에 쓸 때 해 야함 0x80 부터가 LCD 첫재쭐 출력 이고 그다음 줄 은 0xC0이다. y=는 칸수, 즉 열을 결정한다.

 Command(y);
}
*/



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

/****************************************** LCD 끝 *************************************************************/




/****************************************** 인터럽트 시작 *************************************************************/

// 외부 인터럽트4 서비스 루틴  
interrupt [EXT_INT4] void external_int4(void)
{
    led <<= 1;
    led |= 0b00000001;
    if(led == 0xFF) led = 0xFE;
    PORTC = led;
}

// 외부 인터럽트5 서비스 루틴  
interrupt [EXT_INT5] void external_int5(void)
{
    led <<= 2;
    led |= 0b00000011;
    if(led == 0xFF) led = 0xFE;
    PORTC = led;
}


/****************************************** 인터럽트 끝 *************************************************************/