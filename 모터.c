  
// C : LED            // D : LCD         // E4, E5 : ���ͷ�Ʈ             
// F0 : ���� ����  F1 : CDS                    // H : Enable A,       ���� B4 : Input1        B7 : Input2

#include <stdio.h>
#include <mega128.h>
#include <delay.h>

////////////////////////////////// ��ũ�� //////////////////////////////////////////

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
#define RIGHT   0x80       // �ð����
#define LEFT    0x10        // �ݽð����
#define STOP    0x00        // ���� ����

/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////// �Լ� ���� ////////////////////////////////////////////

//------------ LCD --------------------------//
void LCD_init(void);
void LCD_String(char flash *);
void LCD_printf(char *);
void Command(unsigned char);
void Data(unsigned char);
void Busy(void);       
void LCD_pos(unsigned , unsigned );

//----------- ���ͷ�Ʈ -----------------------//
void External_Interrupt_Initilalize(void);

//--------- Ÿ�̸�/ī���� --------------//
void CTC_Setting(void);
void fast_PWM_Setting(void);

//------------ ����ġ --------------------------//
void what_sw(void);
void sw1_1(void);
void sw1_2(void);
void sw2_1(void);
void sw2_2(void);

void sw_reset(void);

//------------------ ��� �Լ� ----------------------//
void CDS_VALUE(void);
void VOLTAGE_VALUE(void);
void CDS_PD(void);
int Distance(void);

void Guest_Number(void); 
void Call_Number(void);
void Wait_Number(void);

int Motor_speed_up(unsigned int C);    // ���� 6�ܰ� ���� �Լ�  ����
int Motor_speed_down(unsigned int C);                                // ����
void Motor_direction_speed(int DR, int speed);  // ���� ����, �ӵ� ���� �Լ� 
int Motor1_2(int speed);

void LED1_1(unsigned int V_speed);
void LED1_2(unsigned int V_speed);
void LED2_2(unsigned int V_speed);

////////////////////////////////////////////////////////////////////////////


/////////////////////// ���� ���� ////////////////////////////////////////////

unsigned char led = 0x00;
int led_cnt=0;
int i=0;
unsigned char l[1];
unsigned int cnt1=0, cnt2=0;       // ���ͷ�Ʈ �߻� Ƚ��   
int btn=0;                         // ���� ��ư ������
unsigned char t[1];         // ���� ��ư ������ LCD
unsigned char c1[1];       // 1�� ��ư Ƚ��  LCD
unsigned char c2[1];       // 2�� ��ư Ƚ��    LCD
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
    // Ÿ�̸�/ī���� ���� ��������                           
    TCCR0 =  (1<<WGM01) | (0<<WGM00);   // CTC mode
    TCCRO |= (1<<COM01) | (0<<COM00) ;   // ����� �� ��� ���
    TCCRO |= (1<<CS02) |  (0<<CS02) | (0<<CS02) ;  // ���ֺ� 
    TCCR2 = TCCR0;  
    
     // Ÿ�̸�/ī���� ���ͷ�Ʈ ����ũ ��������
    TIMSK = (1<<OCIE0) | (1<<OCIE2);     // ��� �� ��ġ ���ͷ�Ʈ �ο��̺�   
    
    OCR0 = 0;     // ��� �� ��������
}

void fast_PWM_Setting(void)
{                            
    TCCR0 =  (1<<WGM01) | (1<<WGM00);   // fast PWM mode
    TCCRO |= (1<<COM01) | (0<<COM00) ;   // ����� �� ��� ���
    TCCRO |= (1<<CS02) |  (0<<CS02) | (0<<CS02) ;  // ���ֺ� 64    
    TCCR2 = TCCR0;
    
    TIMSK = (1<<OCIE0) | (1<<OCIE2);  
    
    OCR2 = 0;       
}

// Ÿ�̸�0 ��ġ ���ͷ�Ʈ
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

// TCCR0    Ÿ�̸�/ī����0 ���������Ϸ�
//      7            6            5              4                3             2             1           0
// FOC0     WGM00    COM01     COM00       WGM01      CS02       CS01     CS00
//                  0                                                0                                                 : �Ϲ� ���
//                  1                                                0                                                 : Phase Correct PWM
//                  0                                                1                                                 : CTC  (�� ��ġ�ÿ� Ÿ�̸� Ŭ����)
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


 // Ÿ�̸�2 ��ġ ���ͷ�Ʈ
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
TIMSK=(1<<OCIE0) | (1<<OCIE2);   //comp_mode (Ÿ�̸�/ī����0,2 ��� �� ���)

ETIMSK=0x00;

*/



//////////////////////////////// main /////////////////////////////////////////////
      
void main(void)
{
        LCD_init();
      
        // ��Ʈ �ʱ�ȭ
        DDRC = 0xFF;                // ��Ʈ C ��� ����  
        PORTC = ~led;
        DDRE = 0b00000010;     // ��Ʈ E �Է�(PE1 ���)    
        DDRF&=0xFE; 
        DDRB = 0xFF;                // MOTOR PB4, PB7 ���
        
        ADMUX=0b00000000;       //use AREF, adc channel 0, �ܱؼ� ���� �Է�,  ���� ����
        ADCSRA=0xE7;               //adc enable, start, prescaler 128
                    
        External_Interrupt_Initilalize(); //�ܺ� ���ͷ�Ʈ �䱸 �ʱ�ȭ �Լ�         
        fast_PWM_Setting();   // Ÿ�̸�/ī���� fast PWM, 64����      
                       
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

//LCD �ʱ�ȭ
void LCD_init(void)            
{
    DDRD = 0xFF;            // ��Ʈ D ��� ����
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


// ���ڿ� ��� �Լ�
void LCD_String(char flash *str)
{
    char flash *pStr=0;
    
    pStr = str;
    while(*pStr)
        Data(*pStr++);
}    



//���� ��� �Լ�
void LCD_printf(char *str)
{
        char *pStr=0;
        
        pStr = str;
        while(*pStr) Data(*pStr++);
}


// �ν�Ʈ���� ���� �Լ�
void Command(unsigned char byte)
{
    Busy();
    
    // �ν�Ʈ���� ���� ����Ʈ
    PORTD = (byte & 0xF0);      // ������
    PORTD &= 0xFE;               // RS = 0 ����
    PORTD &= 0xFD;              // RW = 0 Write
    delay_us(1);
    PORTD |= 0x04;                // E = 1  LCD ����
    delay_us(1);
    PORTD &= 0xFB;              // E = 0;
    
    // �ν�Ʈ���� ���� ����Ʈ
    PORTD = ((byte<<4) & 0xF0);         //������
    PORTD &= 0xFE;                         // RS = 0 ����
    PORTD &= 0xFD;                        // RW = 0 Write
    delay_us(1);
    PORTD |= 0x04;                          // E = 1 LCD ����
    delay_us(1);
    PORTD &= 0xFB;                       // E = 0;
 }
 

// ������ ���� �Լ�
void Data(unsigned char byte)
{
    Busy();
    
    // ������  ���� ����Ʈ
    PORTD = (byte & 0xF0);      // ������
    PORTD |= 0x01;               // RS = 1 ������
    PORTD &= 0xFD;              // RW = 0 Write
    delay_us(1);
    PORTD |= 0x04;                // E = 1 LCD ����
    delay_us(1);
    PORTD &= 0xFB;              // E = 0;
    
    // ������  ���� ����Ʈ
    PORTD = ((byte<<4) & 0xF0);         //������  �·� 4 Shift
    PORTD |= 0x01;                         // RS = 1;
    PORTD &= 0xFD;                        // RW = 0;
    delay_us(1);
    PORTD |= 0x04;                          // E = 1;
    delay_us(1);
    PORTD &= 0xFB;                       // E = 0;
 }           


// ǥ�� ��ġ ����
void LCD_pos(unsigned col, unsigned row)
{
    Command( 0x80 | ( row + col*0x40 ) );           // col ���ڿ�, row ������
}
// ǥ�� ��ġ�� DDRAM ��巹�� �������� ���� ������ ��
  



// Busy Flag Check -> �Ϲ����� BF üũ�ϴ� ���� �ƴ϶� ������ �ð� ���� �̿�
void Busy(void)
{
    delay_ms(2);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////// Ÿ�̸�/ ī���� �ʱ�ȭ //////////////////////////////////////////////////////////
    /*
void CTC_Setting(void)
{    
    // Ÿ�̸�/ī���� ���� ��������                           
    TCCR0 =  (1<<WGM01) | (0<<WGM00)   // CTC mode
                | (1<<COM01) | (0<<COM00)   // ����� �� ��� ���
                | (1<<CS02) |  (0<<CS02) | (0<<CS02) ;  // ���ֺ� 
    TCCR2 = TCCR0;  
    
     // Ÿ�̸�/ī���� ���ͷ�Ʈ ����ũ ��������
   // TIMSK = (1<<OCIE0) | (1<<OCIE2);     // ��� �� ��ġ ���ͷ�Ʈ �ο��̺�   
   //   TIMSK = (1<<TOIE1);        // �����÷ο� ���ͷ�Ʈ �ο��̺�  
   
    OCR0 = 0;     // ��� �� ��������  
    OCR2 = 0;            // ��� �� ��������
}
*/
void fast_PWM_Setting(void)
{
     // Ÿ�̸�/ī���� ���ͷ�Ʈ ����ũ ��������
//    TIMSK = (1<<OCIE0) | (1<<OCIE2);         // ��� �� ��ġ ���ͷ�Ʈ �ο��̺�   
//   TIMSK = (1<<TOIE1) | (1<<TOIE2);        // �����÷ο� ���ͷ�Ʈ �ο��̺� 
    TIMSK = (1<<OCIE0); 
       
    // Ÿ�̸�/ī���� ���� ��������                            
    TCCR0 =  (1<<WGM01) | (1<<WGM00)   // fast PWM mode
                | (1<<COM01) | (0<<COM00)    // ����� �� ��� ���
                | (0<<CS02) |  (1<<CS02) | (1<<CS02) ;  // ���ֺ� 64    
    TCCR2 = (1<<WGM01) | (1<<WGM00)   // fast PWM mode
                | (1<<COM01) | (0<<COM00)    // ����� �� ��� ���
                | (1<<CS02) |  (0<<CS02) | (0<<CS02) ;  // ���ֺ� 64       
                           
    OCR0 = 0;            // ��� �� ��������      ������
    OCR2 = 0;            // ��� �� ��������     ������
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////// ���ͷ�Ʈ ///////////////////////////////////////////////////////////////////////

//�ܺ� ���ͷ�Ʈ �䱸 �ʱ�ȭ �Լ�
void External_Interrupt_Initilalize(void)
{
        EICRB = 0b00001111;     // �ܺ� ���ͷ�Ʈ 4, 5 : ��¿���
        EIMSK = 0b00110000;    // �ܺ� ���ͷ�Ʈ 4, 5 �ο��̺�
        EIFR = 0xFF;                  // �ܺ� ���ͷ�Ʈ �÷���
        SREG = 0x80;                 // ���� ���ͷ�Ʈ �ο��̺� ��Ʈ ��      // sei();
}

// �ܺ� ���ͷ�Ʈ4 ���� ��ƾ  
interrupt [EXT_INT4] void external_int4(void)
{      
    btn=1;      cnt1++;     Guest_Number();      Wait_Number();    
}

// �ܺ� ���ͷ�Ʈ5 ���� ��ƾ  
interrupt [EXT_INT5] void external_int5(void)
{                      
    btn=2;      cnt2++;     Call_Number();      Wait_Number();       
}

  
interrupt [TIM0_COMP] void timer_comp0_isr(void)    // ī���� ���� ���ͷ�Ʈ
{ 
//        C++;    // ���� �ӵ� ���� ����       
//        if(C==61)  C=0;    // �ʱ�ȭ
}  
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    


////////////////////////////////////// ����ġ Ȯ��   /////////////////////////////////////////
 
void what_sw(void)     // � ����ġ �� �� ������
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



/////////////////////////////////// ��� �Լ� ///////////////////////////////////////////////////////

//-------------ADC ------------------------------//
void CDS_VALUE(void)   //������
{
      ADC_I=ADCW;
      //ADC_F=(float)ADC_I * 5.0/1024.0;
      sprintf(str,"%4d", ADC_I); 
      LCD_pos(0, 0); 
      LCD_printf(str);  
      delay_ms(500);  
  }

void VOLTAGE_VALUE(void)  // ��Ʈ��
{
       ADC_F=(float)ADC_I * 5.0/1024.0;    
      sprintf(str,"Volt = %4.3f V", ADC_F); 
      LCD_pos(0, 0);
      LCD_printf(str);   
      LCD_pos(1, 0);
      LCD_String("                ");           
      delay_ms(500);  
  }      
  
 void CDS_PD(void)        // �Ÿ� PD ����
 { 
    ADC_I=ADCW;
    ADC_Err = 5 - ((float)ADC_I*5/1023.0);       // ���� = ��ǥ-���簪   
    PRE_ADC = ADC_Err;    // ���簪�� �������� ����               
    PD = Kp*(float)ADC_Err + Kd*(float)(ADC_Err - PRE_ADC);           // PD ���        
    sprintf(str,"  %4.0f cm  ", PD); 
    LCD_pos(0, 0);
    LCD_printf(str);   
    LCD_pos(1, 0);
    LCD_String("                ");           
    delay_ms(500);          
 }  
 
 int Distance(void)        // PD ����� �Ÿ�        
 {    /*
    ADC_I=ADCW;
    ADC_Err = 1023 - ADC_I;       // ���� = ��ǥ-���簪       
    PRE_ADC = ADC_Err;    // ���簪�� �������� ���� 
    PD = Kp*(float)ADC_Err + Kd*(float)(ADC_Err - PRE_ADC);           // PD ���
      */
    ADC_I=ADCW;
    ADC_Err = 5 - ((float)ADC_I*5/1023.0);       // ���� = ��ǥ-���簪   
    PRE_ADC = ADC_Err;    // ���簪�� �������� ����               
    PD = Kp*(float)ADC_Err + Kd*(float)(ADC_Err - PRE_ADC);           // PD ���     
    return PD;
 }    

//------------- ����� �մ� LCD ��� -------------------// 
// 1�� ����ġ�� 2�� ����ġ ��� �������� ���� �ٸ� ������ �����ؼ�
// �迭�� ������ ���� ����Ѵ� �迭 �ٸ� ����ġ ī��Ʈ��  ������Ʈ �ص� ������ ī��Ʈ ����Ǿ������Ƿ� ������ ������ �ʴ´�
// ���� ����ڴ� 1�� ����ġ ���� Ƚ�� - 2�� ����ġ ���� Ƚ���� ����Ͽ� �迭�� �����ؼ� ��� 
 void Guest_Number(void) // ����ȣ
 {   
        sprintf(Guest,"%d", cnt1); 
        LCD_pos(0, 0);
        LCD_String("Number"); 
        LCD_pos(0, 7);
        LCD_printf(Guest);  
        delay_ms(500);    
 }
 
 void Call_Number(void)   // ����� call
 { 
        sprintf(Guest,"%d", cnt2); 
        LCD_pos(1, 0);
        LCD_String("Call"); 
        LCD_pos(1, 5);
        LCD_printf(Guest);  
        delay_ms(500);   
 }   
 
 void Wait_Number(void)   // ���� ����� ��          
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
  
 
//------------------���� �׽�Ʈ ---------------------------------------// 

int Motor_speed_up(unsigned int C)    // �ð��� ���� ���� 3�ܰ� ����
{
    if(C<20) OCR2 =60;
    else if(C<30) OCR2 = 100;
    else if(C<60) OCR2 = 200;     
    
    return OCR2;
}   // OCR�� ���п� ��ȭ�� �־� �ӵ� ����

int Motor_speed_down(unsigned int C)    // �ð��� ���� ���� 3�ܰ� ����
{
    if(C<20) OCR2 =200;
    else if(C<40) OCR2 = 100;
    else if(C<60) OCR2 = 60; 

    return OCR2;
}   // OCR�� ���п� ��ȭ�� �־� �ӵ� ����



void Motor_direction_speed(int DR, int speed)       // ���� ����, �ӵ� ����
{
    if(DR <= 512)   // CW
    {   OCR0 = 0;   OCR2 = speed;    }
    else // CCW
    {   OCR0 = speed, OCR2 = 0;    }
}

int Motor1_2(int speed)       // ���� �ð����, �ӵ� ����
{
    OCR0 = 0;   OCR2 = speed;         
    
    return speed;
}


//-------------------- LED --------------------------------------------//

void LED1_1(unsigned int V_speed)   // ���� �ӵ��� ���� ���� �ٸ��� ���� ������
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
void LED1_2(unsigned int V_speed)   // ���� �ӵ��� ���� shift �Ǵ� �ӵ� ����
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

void LED2_2(unsigned int V_speed)   // ���� �ӵ��� ���� ���� �ٸ��� ����   ����
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


////////////// ����ġ�� �Լ� //////////////////////////////////////////////////////////////////////////////

void sw1_1(void)     // ���� �ð��� ���� ����, LED ���� �ӵ��� ���� ���� �ٸ��� ����
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

void sw1_2(void)  // ADC-> �Ÿ� -> ���� �ӵ� -> LED 1�� Shift �Ǵ� �ӵ� ����
{       
    int d = Distance();  d*=20;
    V_speed = Motor1_2(d);
    LED1_2(V_speed);                                  
}

void sw2_1(void)    // ������ LED �Ѱ� �ݽð�, ��ο�� ������, �ð�  
{
    ADC_I = ADCW;
    if(ADC_I>200)   // CW
    {   OCR0 = 0;    OCR2 = 50;    PORTC=~(0x00); }
    else  // CCW
    {   OCR0 = 50,    OCR2 = 0;    PORTC=~(0xFF); }
    delay_ms(60);
}

void sw2_2(void)  // ���� �ӵ��� ���� ���� -> LED ���� �ٸ��� �������� ����
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