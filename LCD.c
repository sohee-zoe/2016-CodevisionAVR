
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
        
        
        // ��Ʈ �ʱ�ȭ
        DDRC = 0xFF;
        DDRE = 0b00000010;
        
        PORTC = led;
        
        EICRB = 0b00001010;     // �ܺ� ���ͷ�Ʈ 4, 5 : �ϰ�����
        EIMSK = 0b00110000;    // �ܺ� ���͎�Ʈ 4, 5 �ο��̺�
        SREG = 0x80;                 // ���� ���ͷ�Ʈ �ο��̺� ��Ʈ ��
        
        while(1);
                 
    }
}




/****************************************** LCD  ���� *************************************************************/

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


/*
//���� ��ġ
void LCD_location(unsigned char x, unsigned char y)
{
 y = (x << 6) + y; // '<<' ���ִ� ������ ����Ʈ�� ���������ν� �״��� ��꿡�� ���� ��ĭ ���� �ΰ������� ex) x=1 �̸� ��ĭ ���� ��°�ٺ��� ���

 y |= 0x80;     // ��Ʈ 7�� ����, DDR�ּҿ� �� �� �� ���� 0x80 ���Ͱ� LCD ù���� ��� �̰� �״��� �� �� 0xC0�̴�. y=�� ĭ��, �� ���� �����Ѵ�.

 Command(y);
}
*/



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

/****************************************** LCD �� *************************************************************/




/****************************************** ���ͷ�Ʈ ���� *************************************************************/

// �ܺ� ���ͷ�Ʈ4 ���� ��ƾ  
interrupt [EXT_INT4] void external_int4(void)
{
    led <<= 1;
    led |= 0b00000001;
    if(led == 0xFF) led = 0xFE;
    PORTC = led;
}

// �ܺ� ���ͷ�Ʈ5 ���� ��ƾ  
interrupt [EXT_INT5] void external_int5(void)
{
    led <<= 2;
    led |= 0b00000011;
    if(led == 0xFF) led = 0xFE;
    PORTC = led;
}


/****************************************** ���ͷ�Ʈ �� *************************************************************/