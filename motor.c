  
// C : LED            // D : LCD         // E4, E5 : 인터럽트             
// F0 : 전압 조절  F1 : CDS                    // H : Enable A,       모터 B4 : Input1        B7 : Input2

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
void CTC_Setting(void);
void fast_PWM_Setting(void);

//------------ 스위치 --------------------------//
void what_sw(void);
void sw1_1(void);
void sw1_2(void);
void sw2_1(void);
void sw2_2(void);

void sw_reset(void);

//------------------ 기능 함수 ----------------------//
void CDS_VALUE(void);
void VOLTAGE_VALUE(void);
void CDS_PD(void);
int Distance(void);

void Guest_Number(void); 
void Call_Number(void);
void Wait_Number(void);

int Motor_speed_up(unsigned int C);    // 모터 6단계 동작 함수  가속
int Motor_speed_down(unsigned int C);                                // 감속
void Motor_direction_speed(int DR, int speed);  // 모터 방향, 속도 조절 함수 
int Motor1_2(int speed);

void LED1_1(unsigned int V_speed);
void LED1_2(unsigned int V_speed);
void LED2_2(unsigned int V_speed);

////////////////////////////////////////////////////////////////////////////


/////////////////////// 전역 변수 ////////////////////////////////////////////

unsigned char led = 0x00;
int led_cnt=0;
int i=0;
unsigned char l[1];
unsigned int cnt1=0, cnt2=0;       // 인터럽트 발생 횟수   
int btn=0;                         // 무슨 버튼 눌렀나
unsigned char t[1];         // 무슨 버튼 눌렀다 LCD
unsigned char c1[1];       // 1번 버튼 횟수  LCD
unsigned char c2[1];       // 2번 버튼 횟수    LCD
unsigned char Guest[1]; 
 
char str[30];
int ADC_I;
float ADC_F;
int PRE_ADC=0;
int ADC_Err=0;
float Kp=1;
float Kd=1;
float PD=0;

unsigned int C=0;
unsigned int V_speed=0;

////////////////////////////////////////////////////////////////////////////


/*

void CTC_Setting(void)
{    
    // 타이머/카운터 제어 레지스터                           
    TCCR0 =  (1<<WGM01) | (0<<WGM00);   // CTC mode
    TCCRO |= (1<<COM01) | (0<<COM00) ;   // 비반전 비교 출력 모드
    TCCRO |= (1<<CS02) |  (0<<CS02) | (0<<CS02) ;  // 분주비 
    TCCR2 = TCCR0;  
    
     // 타이머/카운터 인터럽트 마스크 레지스터
    TIMSK = (1<<OCIE0) | (1<<OCIE2);     // 출력 비교 매치 인터럽트 인에이블   
    
    OCR0 = 0;     // 출력 비교 레지스터
}

void fast_PWM_Setting(void)
{                            
    TCCR0 =  (1<<WGM01) | (1<<WGM00);   // fast PWM mode
    TCCRO |= (1<<COM01) | (0<<COM00) ;   // 비반전 비교 출력 모드
    TCCRO |= (1<<CS02) |  (0<<CS02) | (0<<CS02) ;  // 분주비 64    
    TCCR2 = TCCR0;
    
    TIMSK = (1<<OCIE0) | (1<<OCIE2);  
    
    OCR2 = 0;       
}

// 타이머0 매치 인터럽트
    TCCR0=0x0B; OCR0= 28;  TIMSK=2;  //16000000/  32/(1+ 28)=17241.37931Hz=58us
    TCCR0=0x0B; OCR0= 49;  TIMSK=2;  //16000000/  32/(1+ 49)=10000Hz=100us
    TCCR0=0x0B; OCR0= 99;  TIMSK=2;  //16000000/  32/(1+ 99)= 5000Hz=200us    
    
    TCCR0=0x0C; OCR0= 74;  TIMSK=2;  //16000000/  64/(1+ 74)= 3333.3Hz=300us
    TCCR0=0x0C; OCR0=124;  TIMSK=2;  //16000000/  64/(1+124)= 2000Hz=500us
    TCCR0=0x0C; OCR0=249;  TIMSK=2;  //16000000/  64/(1+249)= 1000Hz=1ms    
    
    TCCR0=0x0D; OCR0=124;  TIMSK=2;  //16000000/ 128/(1+124)= 1000Hz=1ms
    TCCR0=0x0D; OCR0=249;  TIMSK=2;  //16000000/ 128/(1+249)=  500Hz=2ms
    TCCR0=0x0E; OCR0=249;  TIMSK=2;  //16000000/ 256/(1+249)=  250Hz=4ms   
    
    TCCR0=0x0F; OCR0= 71;  TIMSK=2;  //14745600/1024/(1+ 71)=  200Hz=5ms
    TCCR0=0x0F; OCR0=155;  TIMSK=2;  //16000000/1024/(1+155)= 100.16Hz=9.984ms
    TCCR0=0x0F; OCR0=249;  TIMSK=2;  //16000000/1024/(1+249)= 62.5Hz=16ms

// TCCR0    타이머/카운터0 프리스케일러
//      7            6            5              4                3             2             1           0
// FOC0     WGM00    COM01     COM00       WGM01      CS02       CS01     CS00
//                  0                                                0                                                 : 일반 모드
//                  1                                                0                                                 : Phase Correct PWM
//                  0                                                1                                                 : CTC  (비교 일치시에 타이머 클리어)
//                  1                                                1                                                 :  Fast PWM
       

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0 output: Disconnected
ASSR=0x00;
TCCR0=0x00;
TCNT0=0x00;
OCR0=0x00;


 // 타이머2 매치 인터럽트
    TCCR2=0x0B; OCR2= 24;  TIMSK=0x80; //16000000/64/(1+ 24)=10000Hz=100us
    TCCR2=0x0B; OCR2=249;  TIMSK=0x80; //16000000/64/(1+249)=1000Hz=1ms


// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;


// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(1<<OCIE0) | (1<<OCIE2);   //comp_mode (타이머/카운터0,2 출력 비교 모드)

ETIMSK=0x00;

*/



//////////////////////////////// main /////////////////////////////////////////////
      
void main(void)
{
        LCD_init();
      
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
            if(EIFR) break;    
            if (btn==1 && cnt1==1)        sw1_1();
            else if (btn==1 && cnt1==2) sw1_2();
          
            else if (btn==2 && cnt2==1) sw2_1();          
            else if (btn==2 && cnt2==2) sw2_2();      
          
            else if (cnt1==3 || cnt2==3) sw_reset();              
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
    /*
void CTC_Setting(void)
{    
    // 타이머/카운터 제어 레지스터                           
    TCCR0 =  (1<<WGM01) | (0<<WGM00)   // CTC mode
                | (1<<COM01) | (0<<COM00)   // 비반전 비교 출력 모드
                | (1<<CS02) |  (0<<CS02) | (0<<CS02) ;  // 분주비 
    TCCR2 = TCCR0;  
    
     // 타이머/카운터 인터럽트 마스크 레지스터
   // TIMSK = (1<<OCIE0) | (1<<OCIE2);     // 출력 비교 매치 인터럽트 인에이블   
   //   TIMSK = (1<<TOIE1);        // 오버플로우 인터럽트 인에이블  
   
    OCR0 = 0;     // 출력 비교 레지스터  
    OCR2 = 0;            // 출력 비교 레지스터
}
*/
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
    btn=1;      cnt1++;     Guest_Number();      Wait_Number();    
}

// 외부 인터럽트5 서비스 루틴  
interrupt [EXT_INT5] void external_int5(void)
{                      
    btn=2;      cnt2++;     Call_Number();      Wait_Number();       
}

  
interrupt [TIM0_COMP] void timer_comp0_isr(void)    // 카운터 내부 인터럽트
{ 
//        C++;    // 모터 속도 제어 변수       
//        if(C==61)  C=0;    // 초기화
}  
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    


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



/////////////////////////////////// 기능 함수 ///////////////////////////////////////////////////////

//-------------ADC ------------------------------//
void CDS_VALUE(void)   //조도량
{
      ADC_I=ADCW;
      //ADC_F=(float)ADC_I * 5.0/1024.0;
      sprintf(str,"%4d", ADC_I); 
      LCD_pos(0, 0); 
      LCD_printf(str);  
      delay_ms(500);  
  }

void VOLTAGE_VALUE(void)  // 볼트값
{
       ADC_F=(float)ADC_I * 5.0/1024.0;    
      sprintf(str,"Volt = %4.3f V", ADC_F); 
      LCD_pos(0, 0);
      LCD_printf(str);   
      LCD_pos(1, 0);
      LCD_String("                ");           
      delay_ms(500);  
  }      
  
 void CDS_PD(void)        // 거리 PD 제어
 { 
    ADC_I=ADCW;
    ADC_Err = 5 - ((float)ADC_I*5/1023.0);       // 에러 = 목표-현재값   
    PRE_ADC = ADC_Err;    // 현재값을 이전값에 저장               
    PD = Kp*(float)ADC_Err + Kd*(float)(ADC_Err - PRE_ADC);           // PD 제어값        
    sprintf(str,"  %4.0f cm  ", PD); 
    LCD_pos(0, 0);
    LCD_printf(str);   
    LCD_pos(1, 0);
    LCD_String("                ");           
    delay_ms(500);          
 }  
 
 int Distance(void)        // PD 제어된 거리        
 {    /*
    ADC_I=ADCW;
    ADC_Err = 1023 - ADC_I;       // 에러 = 목표-현재값       
    PRE_ADC = ADC_Err;    // 현재값을 이전값에 저장 
    PD = Kp*(float)ADC_Err + Kd*(float)(ADC_Err - PRE_ADC);           // PD 제어값
      */
    ADC_I=ADCW;
    ADC_Err = 5 - ((float)ADC_I*5/1023.0);       // 에러 = 목표-현재값   
    PRE_ADC = ADC_Err;    // 현재값을 이전값에 저장               
    PD = Kp*(float)ADC_Err + Kd*(float)(ADC_Err - PRE_ADC);           // PD 제어값     
    return PD;
 }    

//------------- 대기자 손님 LCD 출력 -------------------// 
// 1번 스위치와 2번 스위치 몇번 눌렀는지 각각 다른 변수에 저장해서
// 배열에 저장한 다음 출력한다 배열 다른 스위치 카운트로  업데이트 해도 변수에 카운트 저장되어있으므로 데이터 날라가지 않는다
// 남은 대기자는 1번 스위치 누른 횟수 - 2번 스위치 누른 횟수로 계산하여 배열에 저장해서 출력 
 void Guest_Number(void) // 대기번호
 {   
        sprintf(Guest,"%d", cnt1); 
        LCD_pos(0, 0);
        LCD_String("Number"); 
        LCD_pos(0, 7);
        LCD_printf(Guest);  
        delay_ms(500);    
 }
 
 void Call_Number(void)   // 대기자 call
 { 
        sprintf(Guest,"%d", cnt2); 
        LCD_pos(1, 0);
        LCD_String("Call"); 
        LCD_pos(1, 5);
        LCD_printf(Guest);  
        delay_ms(500);   
 }   
 
 void Wait_Number(void)   // 남은 대기자 수          
 {
        i=cnt1-cnt2; 
        if(i<0) i=0;
        sprintf(Guest,"%d", i); 
        LCD_pos(1, 9);
        LCD_String("Wait"); 
        LCD_pos(1, 14);
        LCD_printf(Guest);  
        delay_ms(500);   
 }      
  
 
//------------------모터 테스트 ---------------------------------------// 

int Motor_speed_up(unsigned int C)    // 시간에 따라 모터 3단계 동작
{
    if(C<20) OCR2 =60;
    else if(C<30) OCR2 = 100;
    else if(C<60) OCR2 = 200;     
    
    return OCR2;
}   // OCR로 전압에 변화를 주어 속도 제어

int Motor_speed_down(unsigned int C)    // 시간에 따라 모터 3단계 감속
{
    if(C<20) OCR2 =200;
    else if(C<40) OCR2 = 100;
    else if(C<60) OCR2 = 60; 

    return OCR2;
}   // OCR로 전압에 변화를 주어 속도 제어



void Motor_direction_speed(int DR, int speed)       // 모터 방향, 속도 설정
{
    if(DR <= 512)   // CW
    {   OCR0 = 0;   OCR2 = speed;    }
    else // CCW
    {   OCR0 = speed, OCR2 = 0;    }
}

int Motor1_2(int speed)       // 모터 시계방향, 속도 설정
{
    OCR0 = 0;   OCR2 = speed;         
    
    return speed;
}


//-------------------- LED --------------------------------------------//

void LED1_1(unsigned int V_speed)   // 모터 속도에 따라 개수 다르게 점등 오른쪽
{
    if(V_speed <=70)
    {      /*
            led = 0b00000001;  
            for(i=0 ; i<8 ; i++)   
            {
                PORTC = ~led;
                delay_ms(600); 
                led |= (led << 1);  
                if(i==8) { led = 0b00000000; i=0;       delay_ms(600);}  
            }   */
            
            led=0x01;       PORTC = ~ led;      delay_ms(600);
            led=0x03;       PORTC = ~ led;      delay_ms(600);
            led=0x07;       PORTC = ~ led;      delay_ms(600);
            led=0x0F;       PORTC = ~ led;      delay_ms(600);
            led=0x1F;       PORTC = ~ led;      delay_ms(600);
            led=0x3F;       PORTC = ~ led;      delay_ms(600);
            led=0x7F;       PORTC = ~ led;      delay_ms(600);
            led=0xFF;       PORTC = ~ led;      delay_ms(600);
            led=0x00;       PORTC = ~ led;      delay_ms(600);
            
    }  
   
    else if(V_speed <=110)
    {      /*
            led = 0b00000011; 
            for(i=0 ; i<4 ; i++)
            { 
                PORTC = ~led;
                delay_ms(600);
                led |= (led <<2);  
                if(i==4) { led = 0b00000000;   i=0;     delay_ms(600);}  
            }       */                
            
            led=0x03;       PORTC = ~ led;      delay_ms(600);
            led=0x0F;       PORTC = ~ led;      delay_ms(600);
            led=0x3F;       PORTC = ~ led;      delay_ms(600);
            led=0xFF;       PORTC = ~ led;      delay_ms(600); 
            led=0x00;       PORTC = ~ led;      delay_ms(600);
    }     
      
    else  if(V_speed <= 250)   
    {    /*
            led = 0b00001111;     
            for(i=0 ; i< 2 ; i++)
            {
                PORTC = ~led;
                delay_ms(600); 
                led |= (led << 4);  
                if(i==2) { led = 0b00000000;  i=0;      delay_ms(600);}    
            }   */             
            led=0x0F;       PORTC = ~ led;      delay_ms(600);
            led=0xFF;       PORTC = ~ led;      delay_ms(600);       
            led=0x00;       PORTC = ~ led;      delay_ms(600);
    }   
}     
void LED1_2(unsigned int V_speed)   // 모터 속도에 따라 shift 되는 속도 조절
{
     if(i==8) { led = 0b00000001; i=0;}   
     if(V_speed > 70)   
    {
        led = 0b00000001;  
    
        for(i=0 ; i<8 ; i++)   
        {
            PORTC = ~led;
            delay_ms(60);
            led = led << 1;   
        }
    }
    
     else
     { 
      led = 0b00000001;  
    
        for(i=0 ; i<8 ; i++)   
        {
            PORTC = ~led;
            delay_ms(600);
            led = led << 1;   
        }
    } 
    
}

void LED2_2(unsigned int V_speed)   // 모터 속도에 따라 개수 다르게 점등   왼쪽
{
    if(V_speed <= 250)   
    {
            led = 0b11110000;     
            for(i=0 ; i< 2 ; i++)
            {
                PORTC = ~led;
                delay_ms(600); 

                led |= (led >> 4);  
                if(i==2) { led = 0b00000000;  i=0;      delay_ms(600);}    
            }
    }  
   
    else if(V_speed <=110)
    {
            led = 0b11000000; 
            for(i=0 ; i<4 ; i++)
            { 
                PORTC = ~led;
                delay_ms(600);
                led |= (led >>2);  
                if(i==4) { led = 0b00000000;   i=0;     delay_ms(600);}  
            }
    }     
      
    else if(V_speed <=70)
    { 
            led = 0b10000000;  
            for(i=0 ; i<8 ; i++)   
            {
                PORTC = ~led;
                delay_ms(600); 
                led |= (led >> 1);  
                if(i==8) { led = 0b00000000; i=0;       delay_ms(600);}  
            }
    } 
} 

 
 

//////////////////////////////////////////////////////////////////////////////////////////////////


////////////// 스위치별 함수 //////////////////////////////////////////////////////////////////////////////

void sw1_1(void)     // 모터 시간에 따라 가속, LED 모터 속도에 따라 개수 다르게 점등
{ 
//    V_speed = Motor_speed_up(C); 
//    LED1_1(V_speed); 

            OCR2 = 60;
            /*led = 0b10000000;  
            for(i=0 ; i<8 ; i++)   
            {
                PORTC = ~led;
                delay_ms(300); 
                led |= (led >> 1);  
                if(i==8) { led = 0b00000000; i=0;}  
            }*/
            led=0x01;       PORTC = ~ led;      delay_ms(400);
            led=0x03;       PORTC = ~ led;      delay_ms(400);
            led=0x07;       PORTC = ~ led;      delay_ms(400);
            led=0x0F;       PORTC = ~ led;      delay_ms(400);
            led=0x1F;       PORTC = ~ led;      delay_ms(400);
            led=0x3F;       PORTC = ~ led;      delay_ms(400);
            led=0x7F;       PORTC = ~ led;      delay_ms(400);
            led=0xFF;       PORTC = ~ led;      delay_ms(400);
            led=0x00;       PORTC = ~ led;      delay_ms(400);

            OCR2 = 150;
            /*led = 0b11000000; 
            for(i=0 ; i<4 ; i++)
            { 
                PORTC = ~led;
                delay_ms(300);
                led |= (led >>2);  
                if(i==4) { led = 0b00000000;   i=0;}       
            }   */
            led=0x03;       PORTC = ~ led;      delay_ms(400);
            led=0x0F;       PORTC = ~ led;      delay_ms(400);
            led=0x3F;       PORTC = ~ led;      delay_ms(400);
            led=0xFF;       PORTC = ~ led;      delay_ms(400); 
            led=0x00;       PORTC = ~ led;      delay_ms(400);

            OCR2 = 220;      
            /*
            led = 0b11100000;     
            for(i=0 ; i< 3 ; i++)
            {
                PORTC = ~led;
                delay_ms(300);
                led |= (led >> 3);  
                if(i==3) { led = 0b00000000;  i=0;}    
            } */
            led=0x0F;       PORTC = ~ led;      delay_ms(400);
            led=0xFF;       PORTC = ~ led;      delay_ms(400);       
            led=0x00;       PORTC = ~ led;      delay_ms(400);
}        

void sw1_2(void)  // ADC-> 거리 -> 모터 속도 -> LED 1개 Shift 되는 속도 조절
{       
    int d = Distance();  d*=20;
    V_speed = Motor1_2(d);
    LED1_2(V_speed);                                  
}

void sw2_1(void)    // 밝으면 LED 켜고 반시계, 어두우면 꺼지고, 시계  
{
    ADC_I = ADCW;
    if(ADC_I>200)   // CW
    {   OCR0 = 0;    OCR2 = 50;    PORTC=~(0x00); }
    else  // CCW
    {   OCR0 = 50,    OCR2 = 0;    PORTC=~(0xFF); }
    delay_ms(60);
}

void sw2_2(void)  // 모터 속도에 따라 감속 -> LED 개수 다르게 왼쪽으로 점등
{
//    V_speed = Motor_speed_down(C); 
//    LED2_2(V_speed);      

            OCR2 = 220;  
            /*
            led = 0b00000111;     
            for(i=0 ; i< 3 ; i++)
            {
                PORTC = ~led;
                delay_ms(300); 
                led |= (led << 3);  
                if(i==3) { led = 0b00000000;  i=0;}    
            }     */   
            led=0xF0;       PORTC = ~ led;      delay_ms(400);
            led=0xFF;       PORTC = ~ led;      delay_ms(400);       
            led=0x00;       PORTC = ~ led;      delay_ms(400);
            
            OCR2 = 150;    
            /*
            led = 0b00000011; 
            for(i=0 ; i<4 ; i++)
            { 
                PORTC = ~led;
                delay_ms(300);
                led |= (led <<2);  
                if(i==4) { led = 0b00000000;   i=0;}  
            }     */
            led=0xC0;       PORTC = ~ led;      delay_ms(400);
            led=0xF0;       PORTC = ~ led;      delay_ms(400);
            led=0xFC;       PORTC = ~ led;      delay_ms(400);
            led=0xFF;       PORTC = ~ led;      delay_ms(400); 
            led=0x00;       PORTC = ~ led;      delay_ms(400);
            
            OCR2 = 60;       
            /*
            led = 0b00000001;  
            for(i=0 ; i<8 ; i++)   
            {
                PORTC = ~led;
                delay_ms(300); 
                led |= (led << 1);  
                if(i==8) { led = 0b00000000; i=0;}  
            }       */
            led=0x80;       PORTC = ~ led;      delay_ms(400);
            led=0xC0;       PORTC = ~ led;      delay_ms(400);
            led=0xE0;       PORTC = ~ led;      delay_ms(400);
            led=0xF0;       PORTC = ~ led;      delay_ms(400);
            led=0xF8;       PORTC = ~ led;      delay_ms(400);
            led=0xFC;       PORTC = ~ led;      delay_ms(400);
            led=0xFE;       PORTC = ~ led;      delay_ms(400);
            led=0xFF;       PORTC = ~ led;      delay_ms(400);
            led=0x00;       PORTC = ~ led;      delay_ms(400);             
}

//////////////////////////////////////////////////////////////////////////////////////////////////


void sw_reset(void)
{
       //     LCD_init();
            PORTC=0xFF;
            OCR0=0; OCR2=0;
            
       //     cnt1=0;
       //     cnt2=0;      
}      

//////////////////////////////////////////////////////////////////////////////////////////////////
