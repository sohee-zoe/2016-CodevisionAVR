// C : LED            // D : LCD         // E4, E5 : ���ͷ�Ʈ             
// F0 : ���� ����  F1 : CDS                    // H : Enable A,       ���� B4 : Input1        B7 : Input2
//  BAUD=9600   DataBit=8bit    StopBit=1bit    Parity=None

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
void fast_PWM_Setting(void);

//------------------ ��� �Լ� ----------------------//
void CDS_VALUE(void);
void VOLTAGE_VALUE(void);
void put_a(void);
void put_b(void);

//------------- ��� ------------------------//
void USART_init(void);
void Putch(char);   // 1����Ʈ �۽�
char Getch(void);   // 1����Ʈ ����

////////////////////////////////////////////////////////////////////////////

 
/////////////////////// ���� ���� ////////////////////////////////////////////

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
 
}

// �ܺ� ���ͷ�Ʈ5 ���� ��ƾ  
interrupt [EXT_INT5] void external_int5(void)
{                      

}

interrupt [TIM0_COMP] void timer_comp0_isr(void)    // ī���� ���� ���ͷ�Ʈ
{
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 
/////////////////////////////////// ��� �Լ� ///////////////////////////////////////////////////////

//-------------ADC ------------------------------//
void CDS_VALUE(void)   //������
{
      ADC_I=ADCW;
      //ADC_F=(float)ADC_I * 5.0/1024.0;
      sprintf(str,"ADC = %4d", ADC_I); 
      LCD_pos(0, 0); 
      LCD_printf(str);  
  }

void VOLTAGE_VALUE(void)  // ��Ʈ��
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

//////////////////////////////////// ��� ///////////////////////////////////////////////////////////////

void USART_init(void)    // USART �ʱ�ȭ
{
    UCSR0A = 0x0;
    UCSR0B = (1<<RXEN0) | (1<<TXEN0); // ���ű�, �۽ű� �ο��̺�
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // Bit6 : 0 �񵿱�, Bit 5,4 : 00 �и�Ƽ ��� ��� x , Bit 3 : 0 ������Ʈ���� 1��Ʈ, Bit 2, 1 : 11 8��Ʈ Bit 0 : 0 �۽� ���, ���� �ϰ�����
    UBRR0H = 0;     // X-TAL = 16MHz �� ��, BAUD = 9600
    UBRR0L = 103;  //0x67;                                  
}    
    
void Putch(char data)     // 1����Ʈ �۽�      Tx      MCU -> PC
{
    while(!(UCSR0A & 0x20));    //UDRE0 = 1?    �������� ����
    UDR0 = data;    // USART I/O �������Ϳ� ������ ����
} 

char Getch(void)     // 1����Ʈ ����              Rx      MCU <- PC
{
    while(!(UCSR0A & 0x80));    // RXC = 1?    �������� ����
    return UDR0;    // USART I/O �������� ����
}
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////