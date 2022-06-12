#include <mega128.h>
#include <delay.h>
    
void main(void)
{
    signed char led1[3] = {0xFF, 0xAA, 0x55};
    signed char led2 = 0xDB; 
    signed char led3[3] = {0x7B, 0xBD, 0xDE};
       
    int i, j = 0;         
    int cnt=0;

    DDRC = 0xFF;  
 
         for(cnt=0 ; cnt<12 ; cnt++)
         {
             if(cnt % 2 == 0)
            {           
                PORTC = led1[i];
                delay_ms(1500);    

                i++;
                if(i == 3)
                    i=0;                 
            }    
            
                if(cnt % 2 != 0)
            {    
                if(cnt >= 7)
                {
                 PORTC = led3[j];
                delay_ms(1500);    

                j++;
                if(j == 3)
                    j=0; 
                }
              
                 if(cnt < 7)
               {            
                PORTC = led2;
                delay_ms(1500);  
                led2 >>= 1;
                }  
            }                
         }  
        
  

}