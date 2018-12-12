/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.3 Standard
Automatic Program Generator
© Copyright 1998-2011 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 5/30/2018
Author  : msi
Company : 
Comments: 


Chip type               : ATmega32
Program type            : Application
AVR Core Clock frequency: 1.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*****************************************************/

#include <mega32.h>
#include <stdlib.h>
#include <delay.h>
int year=0 , mounth=0 , day=0 , hour=0 , minute=0 , second=0;
int display[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
char key,c;
char comparematch;


#ifndef RXB8
#define RXB8 1
#endif

#ifndef TXB8
#define TXB8 0
#endif

#ifndef UPE
#define UPE 2
#endif

#ifndef DOR
#define DOR 3
#endif

#ifndef FE
#define FE 4
#endif

#ifndef UDRE
#define UDRE 5
#endif

#ifndef RXC
#define RXC 7
#endif

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

// USART Receiver buffer
#define RX_BUFFER_SIZE 8
char rx_buffer[RX_BUFFER_SIZE];

#if RX_BUFFER_SIZE <= 256
unsigned char rx_wr_index,rx_rd_index,rx_counter;
#else
unsigned int rx_wr_index,rx_rd_index,rx_counter;
#endif

// This flag is set on USART Receiver buffer overflow
bit rx_buffer_overflow;

// USART Receiver interrupt service routine
interrupt [USART_RXC] void usart_rx_isr(void)
{
char status,data;
status=UCSRA;
data=UDR;
key=data;

if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer[rx_wr_index++]=data;
#if RX_BUFFER_SIZE == 256
   // special case for receiver buffer size=256
   if (++rx_counter == 0) rx_buffer_overflow=1;
#else
   if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
   if (++rx_counter == RX_BUFFER_SIZE)
      {
      rx_counter=0;
      rx_buffer_overflow=1;
      }
#endif
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void)
{
char data;
while (rx_counter==0);
data=rx_buffer[rx_rd_index++];
#if RX_BUFFER_SIZE != 256
if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0;
#endif
#asm("cli")
--rx_counter;
#asm("sei")
return data;
}
#pragma used-
#endif

// USART Transmitter buffer
#define TX_BUFFER_SIZE 8
char tx_buffer[TX_BUFFER_SIZE];

#if TX_BUFFER_SIZE <= 256
unsigned char tx_wr_index,tx_rd_index,tx_counter;
#else
unsigned int tx_wr_index,tx_rd_index,tx_counter;
#endif

// USART Transmitter interrupt service routine
interrupt [USART_TXC] void usart_tx_isr(void)
{
if (tx_counter)
   {
   --tx_counter;
   UDR=tx_buffer[tx_rd_index++];
#if TX_BUFFER_SIZE != 256
   if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;
#endif
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c)
{
while (tx_counter == TX_BUFFER_SIZE);
#asm("cli")
if (tx_counter || ((UCSRA & DATA_REGISTER_EMPTY)==0))
   {
   tx_buffer[tx_wr_index++]=c;
#if TX_BUFFER_SIZE != 256
   if (tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0;
#endif
   ++tx_counter;
   }
else
   UDR=c;
#asm("sei")
}
#pragma used-
#endif

// Standard Input/Output functions
#include <stdio.h>

// Timer 0 output compare interrupt service routine
interrupt [TIM0_COMP] void timer0_comp_isr(void)
{

comparematch++;

 if(comparematch==4){
        TCNT0=0X00;
        second++;
        comparematch=0;
        if(key=='s'){
        key=getchar();
        if(second==60){second=0; minute++;} 
        if(minute==60){minute=0; hour++;}
        if(hour==24){hour=0;day++;}
        if(day==31){day=0; mounth++;} 
        if(mounth==12){mounth=0; year++;}
        //show temp
        printf("temprature is: %d" , c);
        putchar('\n\r');
    
        //show Date     
         putchar('\n\r'); 
         putchar('\n\r'); 
        
        putsf("Date:");
        printf("year=%d",year); 
        putchar('/'); 
        
        printf("month=%d",mounth); 
        putchar('/'); 
         
        printf("day=%d",day); 
        putchar('\n\r'); 
        
        //show Time
        putsf("Time:");
        printf("hour=%d",hour); 
        putchar(':'); 

        printf("minute=%d",minute); 
        putchar(':'); 
               
        printf("second=%d",second); 
        putchar('\n\r');  
        putchar('\n\r'); 
        key='h';   }}
}  

#define ADC_VREF_TYPE 0xC0

// Read the 8 most significant bits
// of the AD conversion result
unsigned char read_adc(unsigned char adc_input)
{
ADMUX=adc_input | (ADC_VREF_TYPE & 0xff);
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=0x40;
// Wait for the AD conversion to complete
while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCW;
}

// Declare your global variables here

void main(void)
{
int temp[3]={0,0,0};
float n;
int T;
int k=0,i=0,j=0,l=0,p=0;
char pd,v,A,B;

// Declare your local variables here

// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0x00;
DDRA=0x00;

// Port B initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=1 State6=1 State5=1 State4=1 State3=1 State2=1 State1=1 State0=1 
PORTB=0xFF;
DDRB=0xFF;

// Port C initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTC=0x00;
DDRC=0xFF;

// Port D initialization
// Func7=In Func6=In Func5=Out Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=0 State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x20;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 0.977 kHz
// Mode: Normal top=0xFF
// OC0 output: Disconnected
TCCR0=0x05;
TCNT0=0x00;
OCR0=0xD1;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 0.977 kHz
// Mode: Fast PWM top=ICR1
// OC1A output: Non-Inv.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x82;
TCCR1B=0x1D;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x5A;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x02;

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 4800
UCSRA=0x00;
UCSRB=0xD8;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x0C;

// ADC initialization
// ADC Clock frequency: 500.000 kHz
// ADC Voltage Reference: Int., cap. on AREF
// Only the 8 most significant bits of
// the AD conversion result are used
ADMUX=ADC_VREF_TYPE & 0xff;
ADCSRA=0x81;

// Global enable interrupts
#asm("sei")

while (1)
      {
      n=(float)read_adc(0)/4;   //n is a float variable
      c=read_adc(0)/4;   //c is a character variable  
      if(k==0){
      OCR1AL=c ; } 
      
      

      
      if(l==0){
      T=(n*10);}
      temp[0]=T%10;
      temp[1]=(T/10)%10;
      temp[2]=(T/100)%10; 
      
      
      pd=0x01; 
      for(i=0;i<3;i++){
      PORTB=~pd;  
      PORTC=display[temp[i]];
      if(i==1){PORTC+=0x80;}
      pd=pd<<1;
      delay_ms(5); 
       }     
       
      if(p==0){ 
      putsf("to enter motors' settings press v");
      putchar('\n\r');
      putsf("to enter time settings press t");
      putchar('\n\r');   
      putsf("to enter 7segments's settings press g");
      putchar('\n\r');
      putchar('\n\r');  
      p++;}
       
      //motors velocity
      if(key=='v'){
      key=getchar();
	  putsf("motor's settings:");
      putchar('\n\r');
      putsf("set motors velocity or control it by temprature? press a or b");
      putchar('\n\r');
      key=getchar(); 
      
      if(key=='a'){
      putsf("velocity:");
      A=getchar();
      B=getchar();
      A=A-48;
      B=B-48;
      v=(A*10)+B;
      printf("%d",v);
      putchar('\n\r');  
      OCR1AH=0x00;
      OCR1AL=v ;}
      
      if(key=='b'){
      OCR1AH=0x00;
      OCR1AL=c ;
      putsf("temperature control ok");
      putchar('\n\r');} 
      
      k++;

      }

       
       if(key=='t'){
       getchar();
	   putsf("time settings"); 
         year=0;
        putsf("  year:");
         for(j=0;j<4;j++){
         A=getchar();
         A=A-48;
         year=10*year+A ;
         }        
         printf("%d",year);

         putsf("  MONTH:");
         A=getchar();
         B=getchar();
         A=A-48;
         B=B-48;
         mounth=(A*10)+B;
         printf("%d",mounth); 
         
         while(mounth>12){
         putsf("  MONTH:");
         A=getchar();
         B=getchar();
         A=A-48;
         B=B-48;
         mounth=(A*10)+B;
         printf("%d",mounth);}
         
         putsf("  day:");             
         A=getchar();
         B=getchar();
         A=A-48;
         B=B-48;
         day=(A*10)+B;
         
         while(day>31){
         putsf("  day:");             
         A=getchar();
         B=getchar();
         A=A-48;
         B=B-48;
         day=(A*10)+B;} 
         
         printf("%d",day); 
                 
         putsf("  hour:");
	   A=getchar();
         B=getchar();
         A=A-48;
         B=B-48;
         hour=(A*10)+B; 
         
      while(hour>24){ 
         putsf("  hour:");
	   A=getchar();
         B=getchar();
         A=A-48;
         B=B-48;}
         printf("%d",hour);
                 
         putsf("  minute:");
         A=getchar();
         B=getchar();
         A=A-48;
         B=B-48;
         minute=(A*10)+B;
         printf("%d",minute);
         
         while(minute>60){
         putsf("  minute:");
         A=getchar();
         B=getchar();
         A=A-48;
         B=B-48;
         minute=(A*10)+B;
         printf("%d",minute);}
              
         putsf("  second:");
         A=getchar();
         B=getchar();
         A=A-48;
         B=B-48;
          second=(A*10)+B;
         printf("%d", second);
         
         while(second>60){  
         putsf("  second:");
         A=getchar();
         B=getchar();
         A=A-48;
         B=B-48;
          second=(A*10)+B;
         printf("%d", second);}  
         putchar('\n\r');
         putchar('\n\r');
         putsf("to show the time,date and temprature press s");
         putchar('\n\r');
         putchar('\n\r');           
	    }

      if(key=='g'){
      key=getchar();
	  putsf("7segment's settings:");
      putchar('\n\r');
      putsf("Would you like to show an arbitrary number or the sensors temperature on 7 segment ? press a or b");
      putchar('\n\r');
      key=getchar(); 
      
      if(key=='a'){
      putsf("enter the number");
      
      A=getchar();
      B=getchar();
      A=A-48;
      B=B-48;
      T=(A*10)+B; 
      printf("%d",T);  
      putchar('\n\r');
      T=T*10;
          }
      
      if(key=='b'){
      T=(n*10);
      putsf("sensor temperature ok");
      putchar('\n\r');} 
      
      l++;}
      
      }
}
