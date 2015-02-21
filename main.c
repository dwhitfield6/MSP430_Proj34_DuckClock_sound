#include <msp430.h>
#include "sounddata.h"


#define LED0 BIT3
#define LED1 BIT4
#define CS BIT0  //2.0 is SPI CS
#define MOSI BIT2  //1.7 is SPI MOSI
#define SCLK BIT4  //1.5 is SPI clock
#define alarm BIT2
#define IRrec BIT0
#define IRrec1 BIT1
#define audioON BIT1
#define debug BIT5

int MSB =0;
int LSB =0;
int count =0;
unsigned int sample =0;
int i=0;
int bit =0;
int one =0;
int two =0;
int three =0;
int four =0;
int five =0;
int six =0;
int seven =0;
int eight =0;
int loopcount =0;
int loopcountMAX =0;
int odd =0;
int resetcount =990;
int start =0;
int soundtime=0;

unsigned long realdata =0;
unsigned long rxdata =0;
unsigned long ledcount =0;




void play(void);

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

	  BCSCTL1 = CALBC1_16MHZ;
	  DCOCTL = CALDCO_16MHZ;

    P2DIR |= (LED0 + LED1 + CS + audioON + debug);

    P2OUT |= (CS + audioON);
    _delay_cycles(200000);
    P2OUT &= ~(LED0 + LED1);


    P1IE |= IRrec; // P1.3 interrupt enabled
    P1IES &= ~IRrec; // P1.3 interrupt enabled
    P1IFG &= ~IRrec; // P1.3 IFG cleared
    P1IE |= IRrec1; // P1.3 interrupt enabled
    P1IES |= IRrec1; // P1.3 interrupt enabled
    P1IFG &= ~IRrec1; // P1.3 IFG cleared

    TA0CCTL0 |= CCIE;                             // CCR0 interrupt enabled
    TA0CTL = TASSEL_2 + MC_1 + ID_3;           // SMCLK/8, upmode
    TA0CCR0 =  400;

    P1SEL |= MOSI + SCLK;
    P1SEL2 |= MOSI + SCLK;

    UCA0CTL1 = UCSWRST;
    UCA0CTL0 |= UCMSB + UCMST + UCSYNC + UCCKPH; // 4-pin, 8-bit SPI master
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 1;                          // /2
    UCA0BR1 = 0;                              //
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

    __enable_interrupt(); // enable all interrupts


    while(1)
      {
    	_delay_cycles(100);
    			soundtime=0;
    			if(start != 2)
    			{
            for(i=4;i>=0;i--)
                 {
               	    if((P2IN & alarm) == alarm)
               	    {
               	    	soundtime++;
               	    }
                 }

if(soundtime >3)
{

	start =1;
}

    			}
    			if(resetcount == 200)
            {
              loopcountMAX=loopcount;
              loopcount =0;
              realdata = rxdata >> 1;

            }
            if(realdata != 0) //gun 1 code
                {
            	if(start == 2)
            	{
            		start=0;
            	}
                }
if(start == 1)
{
	start++;
	P2OUT &= ~audioON;
	_delay_cycles(5000);
}
else if(start ==2)
{
	play();
	_delay_cycles(4000000);
}
else
{
	P2OUT |= audioON;
	_delay_cycles(40);
}

ledcount++;
if(ledcount ==1)
{
	P2OUT |= (LED0);
	P2OUT &= ~(LED1);
}
else if(ledcount ==10000)
{
	P2OUT |= (LED1);
	P2OUT &= ~(LED0);
}
else if(ledcount == 30000)
{
	P2OUT &= ~(LED0 + LED1);
}
else if(ledcount>50000)
{
	ledcount = 50000;
}

                    resetcount++;
                if(resetcount >1000)
                {
                  resetcount =999;
                }
                if(count >1000)
                {
                  count =999;
                }

      }
    }

void play(void)
{

	sample =0;
	for(i = NUM_ELEMENTS;i>0;i--)
	{
	MSB = 0x70 | ((data[sample] & 0XF0) >> 4);
	LSB = ((data[sample] & 0X0F) << 4);
	P2OUT &= ~CS;
	UCA0TXBUF = MSB ;
	while (UCA0STAT & UCBUSY);
	UCA0TXBUF = LSB ;
	while (UCA0STAT & UCBUSY);
	P2OUT |= CS;
	_delay_cycles(785);
	P2OUT ^= debug;
	sample++;
	}
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
  TA0CTL |= TACLR;

  if(loopcount == 0)
  {
    rxdata =0;
  }
  if (loopcount == 50)
  {
    one = count;
  }
  else if (loopcount ==51)
  {
    two = count;
  }
  else if(loopcount ==52)
  {
    three = count;
  }
  else if(loopcount ==53)
  {
    four = count;
  }
  else if(loopcount == 54)
  {
    five = count;

  }
  else if(loopcount == 55)
  {
    six = count;

  }
  else if(loopcount == 56)
  {
    seven = count;

  }
  else if(loopcount == 57)
  {
    eight = count;

  }

  odd = (loopcount & 0x1);
  if(odd == 0)
  {
        if(count >13 && count < 24)
            {
                bit = 1;
            }
        else
        {
            bit = 0;
        }

                rxdata += bit;
                rxdata<<=1;
  }
start =0;
loopcount++;
  count=0;
  resetcount=0;
  ledcount=0;
  P1IFG &= ~IRrec; // P1.3 IFG cleared
  P1IFG &= ~IRrec1; // P1.3 IFG cleared
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
  count++;
}
