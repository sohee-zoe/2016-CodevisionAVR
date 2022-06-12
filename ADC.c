// C : LED
// D : LCD
// E4, E5 : ���ͷ�Ʈ
// F0 : ���� ����  F1 : CDS 
// H : Enable A, B4 : Input1 B7:Input2 ����

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

/////////////////////// �Լ� ���� ////////////////////////////////////////////


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

void CDS_VALUE(void);
void VOLTAGE_VALUE(void);
void VOLTAGE_VALUE2(void);

////////////////////////////////////////////////////////////////////////////


/////////////////////// ���� �Լ� ////////////////////////////////////////////

unsigned char led = 0x00;
int led_cnt=0;
unsigned char l[1];
unsigned int cnt1, cnt2;       // ���ͷ�Ʈ �߻� Ƚ��   
int btn=0;                         // ���� ��ư ������
unsigned char t[1];         // ���� ��ư ������ LCD
unsigned char c1[1];       // 1�� ��ư Ƚ��  LCD
unsigned char c2[1];       // 2�� ��ư Ƚ��    LCD
  
char str[30];
int ADC_I;
float ADC_F;

////////////////////////////////////////////////////////////////////////////
                          /*   
ADMUX
7,6 ���� : �������� ���� 
0,0 AREF, ���� Vref�� ���ܵ�  
1,1 AREF �ɿ� �ܺ� ĳ�н��Ͱ� �ִ� ���� 2.56V ��������

5 ���� : ADC��� ���� 1: �������� ���� 0: �������� ����

ADC ��ȯ�� 10��Ʈ
��ȯ�� �����ϴ� �������� ADCL/ADCH 16��Ʈ�� ��/��������          

ADC = 5V*1024/VREF    
                          */

//////////////////////////////// main /////////////////////////////////////////////
      
void main(void)
{
        LCD_init();
      
        // ��Ʈ �ʱ�ȭ
        DDRC = 0xFF;                // ��Ʈ C ��� ����
        DDRE = 0b00000010;     // ��Ʈ E �Է�(PE1 ���)    
        DDRF&=0xfe;
        ADMUX=0b00000000;       //use AREF, adc channel 0, �ܱؼ� ���� �Է�,  ���� ����
        ADCSRA=0xE7;               //adc enable, start, prescaler 128

        PORTC = ~led;
        
     //�ܺ� ���ͷ�Ʈ �䱸 �ʱ�ȭ �Լ�
      External_Interrupt_Initilalize();          
        
        cnt1 = 0;
        cnt2 = 0;  
        

while(1)
        {            
            if (btn==1 && cnt1==1)        sw1_1();
            else if (btn==1 && cnt1==2) sw1_2();
          
            else if (btn==2 && cnt2==1) sw2_1();          
            else if (btn==2 && cnt2==2) sw2_2();      
            
            else if (cnt1==3 || cnt2==3) sw_reset();           
        }
      
}

/////////////////////////////////////////////////////////////////////////




//////////////////// LCD /////////////////////////////////////////////////////////////////////////////////////////

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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////// ���ͷ�Ʈ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
   cnt1++;   btn=1;
}

// �ܺ� ���ͷ�Ʈ5 ���� ��ƾ  
interrupt [EXT_INT5] void external_int5(void)
{                      
    cnt2++;  btn=2;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
//////////////////////////////////////////////////////////////////////////////////////////////////


////////////// ����ġ�� �Լ� //////////////////////////////////////////////////////////////////////////////

void sw1_1(void)   //�Ѳ� �ݺ�, ������(0~1023)
{  
         LCD_init();
         led = 0x00; 
         CDS_VALUE();  
         delay_ms(500);
            
}

void CDS_VALUE(void)   //������
{
      ADC_I=1023-ADCW;
      //ADC_F=(float)ADC_I * 5.0/1024.0;
      sprintf(str,"%4d", ADC_I); 
      LCD_pos(0, 0); 
      LCD_printf(str); 
  }


void sw1_2(void)  //���� ����, ��� ����, ��Ʈ��
{         
       
        VOLTAGE_VALUE();   
         delay_ms(500);                   
}
 
void VOLTAGE_VALUE(void)  // ��Ʈ��
{
      ADC_I=ADCW;
      ADC_F=(float)ADC_I * 5.0/1024.0;    
      sprintf(str,"Volt = %4.3f V", ADC_F); 
      LCD_pos(0, 0);
      LCD_printf(str);   
      LCD_pos(1, 0);
      LCD_String("                ");
  }
  
void sw2_1(void)   // ��⿡ ���� ���� ����, LED ����
{ 
        LCD_init();
         ADC_I=1023-ADCW; 
         
         if(ADC_I <114)       {led=0b00000000; led_cnt=0; }
         else if(ADC_I<228) {led=0b10000000; led_cnt=1; }
         else if(ADC_I<342) {led=0b11000000; led_cnt=2; }
         else if(ADC_I<456) {led=0b11100000; led_cnt=3; }
         else if(ADC_I<570) {led=0b11110000; led_cnt=4; }
         else if(ADC_I<684) {led=0b11111000; led_cnt=5; }
         else if(ADC_I<798) {led=0b11111100; led_cnt=6; }
         else if(ADC_I<912) {led=0b11111110; led_cnt=7; }
         else if(ADC_I<1024) {led=0b11111111; led_cnt=8; }  
          
         PORTC = ~led;
         LCD_pos(0, 0);  
         sprintf(l, "%4d", led_cnt);
         LCD_printf(l);
         delay_ms(500);    
          
}

void sw2_2(void)     // ��⿡ ���� ���� ���Ͽ� ����, Miracle sw2-2 ��Ʈ��
{      
        int i;
         ADC_I=1023-ADCW; 
         
         if(ADC_I <342) 
         { 
            led = 0b10000000;  
            for(i=0 ; i<8 ; i++)   
            {
                PORTC = ~led;
                delay_ms(500); 
                VOLTAGE_VALUE2();
                led |= (led >> 1);  
                if(i==8) { led = 0b00000000; i=0;}  
            }
         }
         else if(ADC_I<684) 
         {
            led = 0b11000000; 
            for(i=0 ; i<4 ; i++)
            { 
                PORTC = ~led;
                delay_ms(500);
                VOLTAGE_VALUE2();
                led |= (led >>2);  
                if(i==4) { led = 0b00000000;   i=0;}  
            }
         }
         else if(ADC_I<1024) 
         {
            led = 0b11100000;     
            for(i=0 ; i< 3 ; i++)
            {
                PORTC = ~led;
                delay_ms(500); 
                VOLTAGE_VALUE2();
                led |= (led >> 3);  
                if(i==3) { led = 0b00000000;  i=0;}    
            }
         }
          

             
}

void VOLTAGE_VALUE2(void)  // ��Ʈ��
{
        ADC_I=ADCW;
        ADC_F=(float)ADC_I * 5.0/1024.0;  
        LCD_pos(0, 0);
        LCD_String("    Miracle    ");
        LCD_pos(1,0);
        LCD_String("sw2-2");  
        sprintf(str,"%4.3f V", ADC_F); 
        LCD_pos(1, 9);
        LCD_printf(str); 
  }

void sw_reset(void)
{
            LCD_init();
            PORTC=0xff;
            
            cnt1=0;
            cnt2=0;      
}

