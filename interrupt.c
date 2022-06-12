#include <stdio.h>
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

// C : LED
// D : LCD
// E4, E5 : ���ͷ�Ʈ

void LCD_init(void);
void LCD_String(char flash *);
void LCD_printf(char *);
void Command(unsigned char);
void Data(unsigned char);
void Busy(void);
// void LCD_location(unsigned char x, unsigned char y);               
void LCD_pos(unsigned , unsigned );
void External_Interrupt_Initilalize(void);
void what_sw(void);
void LCD_out1(void);
void LCD_out2(void);
void LCD_out3(void);

void sw1_1(void);
void sw1_2(void);
void sw1_3(void);
void sw2_1(void);
void sw2_2(void);
void sw2_3(void);
void sw2_4(void);



unsigned int led = 0x00;
unsigned char led1 = 0x10;
unsigned char led2 = 0x08;
unsigned char led3 = 0xFF;
unsigned int cnt1, cnt2;       // ���ͷ�Ʈ �߻� Ƚ��   
unsigned char led_num[16];
int temp=0;
unsigned char t[1];
unsigned char c1[1];
unsigned char c2[1];
int count0=0;
int count9=9999;
char cnt0[4];
char cnt9[4];    

void main(void)
{
        LCD_init();
      
        // ��Ʈ �ʱ�ȭ
        DDRC = 0xFF;                // ��Ʈ C ��� ����
        DDRE = 0b00000010;     // ��Ʈ E �Է�(PE1 ���)

        PORTC = ~led;
        
         //�ܺ� ���ͷ�Ʈ �䱸 �ʱ�ȭ �Լ�
      External_Interrupt_Initilalize();          
        
        cnt1 = 0;
        cnt2 = 0;    
 
while(1){            
            if(temp==1 && cnt1==1)        sw1_1();
            else if(temp==1 && cnt1==2) sw1_2();
            else if(temp==1 && cnt1==3) sw1_3();
            
            else if(temp==2 && cnt2==1) sw2_1();
            
            else if(temp==2 && cnt2==2){LCD_init(); sw2_2(); }
            else if(temp==2 && cnt2==3) sw2_3();
            else if(temp==2 && cnt2==4) sw2_4();   
         }

}




/////////////////////////////////////////////////////////////////// LCD  ���� ///////////////////////////////////////////////////////////////////

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
// ǥ�� ��ġ�� DDRAM ��巹�� �������� ���� ������ ��
void LCD_pos(unsigned col, unsigned row)
{
    Command( 0x80 | ( row + col*0x40 ) );           // col ���ڿ�, row ������
}

  
// Busy Flag Check -> �Ϲ����� BF üũ�ϴ� ���� �ƴ϶� ������ �ð� ���� �̿�
void Busy(void)
{
    delay_ms(2);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////���ͷ�Ʈ ���� ///////////////////////////////////////////////////////////////////


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
   cnt1++;   temp=1;
}




// �ܺ� ���ͷ�Ʈ5 ���� ��ƾ  
interrupt [EXT_INT5] void external_int5(void)
{                      
    cnt2++;  temp=2;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sw1_1(void)
{

     
     while(1)
    {       
         led = (led1 | led2); 
         PORTC = ~led; 

         LCD_out1();    
                 
          led1<<=1;
          led2>>=1;    
          
         if(led1==0x00 && led2==0x00)      // �ʱ�ȭ     
         {
           led1 = 0x10;
           led2 = 0x08;  
         }   
                            
         delay_ms(1000);          
         
         break; 
    }              
}

void sw1_2(void)
{

  
           while(1)
           {       
                  led1>>=1;
                  led2<<=1;                       
                  led = (led1 | led2); 
                  PORTC = ~led;       
                  
                  LCD_out1();    
            
                  delay_ms(1000);      
                                 
                if( led1==0x10 || led2==0x80)           // �ʱ�ȭ
                { 
                     what_sw();
                     
                     led1 = 0x80;
                     led2 = 0x01;
                     led = (led1 | led2); 
                     PORTC = ~led;   
                     
                     LCD_out1();  
                     
                     delay_ms(1000);         
                     break;
                }
          break;
           }

}

void sw1_3(void)
{

              
          while(1)
          {   
                led=led1 | led2;
                
                if(led==0b10000001)
                {
                    led=0b10011001;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);
                    
                    led=0b10111101;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);
                    
                    led=0b11111111;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);                    
                }
                
                if(led==0b01000010)
                {     
                    led=0b11000011;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);
                    
                    led=0b11011011;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);
                    
                    led=0b11111111;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000); 
                }
                
                if(led==0b00100100)
                {
                    led=0b01100110;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);
                    
                    led=0b11100111;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);
                    
                    led=0b11111111;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000); 
                }
                
                if(led==0b000110000)
                {
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000); 
                    
                    led=0b00111100;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);
                    
                    led=0b01111110;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);
                    
                    led=0b11111111;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);
                       
                }   
                

                {                  
                    led=0b00011000;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);
                    
                    led=0b00111100;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);
                    
                    led=0b01111110;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);
                    
                    led=0b11111111;
                    PORTC=~led;
                    LCD_out1();
                    delay_ms(1000);
                }     
                break; 
            }      
}

void sw2_1(void)
{

                        
        while(1)
        {
                           
             led=0b00111100;
             PORTC=~led;
             LCD_out1();
             delay_ms(1000);
             
             led=0b01100110;
             PORTC=~led;
             LCD_out1();
             delay_ms(1000);
             
             led=0b11000011;
             PORTC=~led;
             LCD_out1();
             delay_ms(1000);
             
             led=0b10011001;
             PORTC=~led;
             LCD_out1();
             delay_ms(1000);  
             
             break;
        
          }
}

void sw2_2(void)
{

         while(1)
         {  
             PORTC=~(0b10101010);
             LCD_out2();              
             delay_ms(1000);  
             
             PORTC=~led3;
             LCD_out2();              
             delay_ms(1000); 
             
             led3<<=1;                 
             
             PORTC=~(0b01010101);
             LCD_out2();              
             delay_ms(1000);  
             
             PORTC=~led3;
             LCD_out2();              
             delay_ms(1000); 
             
             led3<<=1; 
             
             if(led3==0x00) 
             {
             PORTC=~led3;
             LCD_out2();              
             delay_ms(1000); 
             led3 = 0xFF;
             }              

           break;
         }
         
}

void sw2_3(void)
{

         while(1)
         {
            PORTC=~(0b10101010);
             LCD_out3();              
             delay_ms(1000);  
             
             PORTC=~led3;
             LCD_out3();              
             delay_ms(1000); 
             
             led3<<=2;                 
             
             PORTC=~(0b01010101);
             LCD_out3();              
             delay_ms(1000);  
             
             PORTC=~led3;
             LCD_out3();              
             delay_ms(1000); 
             
             led3<<=2; 
             
             if(led3==0x00) 
             {
             PORTC=~led3;
             LCD_out3();              
             delay_ms(1000); 
             led3 = 0xFF;
             }    
              
             break;
         }   
}


void sw2_4(void)
{
            LCD_init();
            PORTC=0xff;
            
            cnt1=0;
            cnt2=0;      
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////// LCD  ��� ///////////////////////////////////

void LCD_out1(void)     // LED 16������ ǥ��
{
                LCD_pos(0,0);
                LCD_String("20141828");
                sprintf(led_num, "0x%x", led);
                LCD_pos(1, 6);
                LCD_printf(led_num);    
}

void LCD_out2(void)    // 0~9999 �� ī��Ʈ
{

                LCD_pos(0,4);
                LCD_String("Miracle");
                LCD_pos(1, 0);
                LCD_String("num");
                sprintf(cnt0, "%d", count0);
                LCD_pos(1, 5);
                LCD_printf(cnt0);
                count0++;   
                
                if(count0==9999) count0=0;
}

void LCD_out3(void)     // 9999~0 �ٿ� ī��Ʈ
{
                LCD_pos(0,4);
                LCD_String("Miracle");
                LCD_pos(1, 0);
                LCD_String("num");
                sprintf(cnt9, "%d", count9);
                LCD_pos(1, 5);
                LCD_printf(cnt9);
                count9--;  
                
                if(count9==0) count9=0;  
}

///////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////// ����ġ Ȯ��   /////////////////////////////////////////
 
void what_sw(void)     // � ����ġ �� �� ������
{   
            LCD_pos(0, 12);
            LCD_String("    ");         
            LCD_pos(1, 12);
            LCD_String("    ");

            if(temp==1)
            { 
            LCD_pos(0, 12);
            LCD_String("S"); 
            LCD_pos(0, 13);  
            sprintf(t, "%d", temp);
            LCD_printf(t);             
            LCD_pos(0, 15);  
            sprintf(c1, "%d", cnt1); 
            LCD_printf(c1);
           }
           
           if(temp==2)
           { 
            LCD_pos(1, 12);
            LCD_String("S"); 
            LCD_pos(1, 13);  
            sprintf(t, "%d", temp);
            LCD_printf(t);  
            LCD_pos(1, 15);
            sprintf(c2, "%d", cnt2);  
            LCD_printf(c2);    
            }  
}
//////////////////////////////////////////////////////////////////////////////////////////////////




