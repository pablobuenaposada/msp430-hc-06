#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "resources.h"

char command[30];
int cmdPos=0;
int cmdRdy=0;

void sendString(const char *string){
	int index;
    for(index=0; index < strlen(string); index++){
    	UCA0TXBUF = string[index];
    	while (!(UCA0IFG & UCTXIFG));  // USCI_A0 TX buffer ready?
    }
}

int char2Int(char c){
	int number = c - '0';
	number = number + 0;
	return number;
}

int main(void){

  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  P7SEL |= 0x03;                            // Port select XT1

  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    __delay_cycles(100000);                 // Delay for Osc to stabilize
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

  P1OUT = 0x000;                            // P1.0/1 setup for LED output
  P3SEL |= BIT4+BIT5;                       // P3.4,5 UART option select

  UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
  UCA0CTL1 |= UCSSEL_2;                     // CLK = ACLK
  UCA0BR0 = 52;                           // 32k/9600 - 3.41
  UCA0BR1 = 0;                           //
  UCA0MCTL =UCBRS0;                          // Modulation
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UCA0IE |= UCRXIE;                // Enable USCI_A0 TX/RX interrupt

  __bis_SR_register(SCG0);       // Enter LPM3 w/ interrupts enabled
  _enable_interrupt();
  __no_operation();                         // For debugger

  while(1){
	  if (cmdRdy == 1){

		  if(command[0] == 'N'){ //the device notify us that it established a new connection
		  }

		  else if(command[0] == 'S'){
			  if(command[1] == 'D'){
				  if(command[2] == 'O'){ //SETUP DIGITAL OUTPUT
					  int port = char2Int(command[3]);
					  int pin = char2Int(command[4]);
					  setupDigitalOutput(port,pin);
				  }
				  else if(command[2] == 'I'){ //SETUP DIGITAL INPUT
					  int port = char2Int(command[3]);
					  int pin = char2Int(command[4]);
					  setupDigitalInput(port,pin);
				  }
			  }

			  else if(command[1] == 'A'){
				  if(command[2] == 'I'){ //SETUP ANALOG INPUT
					  int port = char2Int(command[3]);
			  		  int pin = char2Int(command[4]);
			  		  setupAnalogInput(port,pin);
				  }
			  }

			  else if (command[1] == 'P' & command[2] == 'W' & command[3] == 'M'){ //SETUP PWM
				  int port = char2Int(command[4]);
				  int pin = char2Int(command[5]);
				  int lenPeriod = char2Int(command[6]);
				  int lenDuty = char2Int(command[7]);
				  int i=0;
				  int period=0;
				  int duty=0;

				  for(i=0; i<lenPeriod; i++){
					  period += (char2Int(command[8+i])*pow(10,lenPeriod-1-i));
				  }
				  for(i=0; i<lenDuty; i++){
					  duty += (char2Int(command[8+lenPeriod+i])*pow(10,lenDuty-1-i));
				  }

				  setupPWM(port,pin,period,duty);
			  }

			  else if(command[1] == 'O' & command[2] == 'T'){
				  if(command[3] == 'D' & command[4] == 'I'){ //SET OFFLINE TASK DIGITAL INPUT
					  int port = char2Int(command[5]);
					  int pin = char2Int(command[6]);

					  int countLimit = 0;
					  int i=0;
					  for(i=0; i<5; i++){
						  countLimit += (char2Int(command[7+i])*pow(10,5-1-i));
					  }

					  int offlineSize=0;
					  for(i=0; i<4; i++){
						  offlineSize += (char2Int(command[12+i])*pow(10,4-1-i));
					  }

					  setupOfflineTask('D',port,pin,countLimit,offlineSize);

				  }
				  else if(command[3] == 'A' & command[4] == 'I'){ //SET OFFLINE TASK ANALOG INPUT
					  int port = char2Int(command[5]);
					  int pin = char2Int(command[6]);

					  int countLimit = 0;
					  int i=0;
					  for(i=0; i<5; i++){
						  countLimit += (char2Int(command[7+i])*pow(10,5-1-i));
					  }

					  int offlineSize=0;
					  for(i=0; i<4; i++){
						  offlineSize += (char2Int(command[12+i])*pow(10,4-1-i));
					  }

					  setupOfflineTask('I',port,pin,countLimit,offlineSize);
				  }
			  }
		  }

		  else if(command[0] == 'D'){
			  if(command[1] == 'O'){ //SET DIGITAL OUTPUT VALUE
				  int port = char2Int(command[2]);
				  int pin = char2Int(command[3]);

				  setDigitalOutput(port,pin,command[4]);
			  }
			  else if(command[1] == 'I'){ //GET DIGITAL INPUT VALUE
				  int port = char2Int(command[2]);
				  int pin = char2Int(command[3]);

				  int value = getDigitalInput(port,pin);
				  if (value == 1){
					  sendString("1");
				  }
				  else{
					  sendString("0");
				  }
			  }
		  }

		  else if(command[0] == 'A'){ //GET ANALOG INPUT VALUE
			  if(command[1] == 'I'){
				  int port = char2Int(command[2]);
				  int pin = char2Int(command[3]);

				  char adcValue[4];
				  sprintf(adcValue,"%d",getAnalogInput(port,pin));
				  sendString(adcValue);
			  }
		  }

		  else if(command[0] == 'P' & command[1] == 'W' & command[2] == 'M'){ //SET PERIOD OR DUTY CYCLE OF PWM
			  int port = char2Int(command[3]);
			  int pin = char2Int(command[4]);
			  int lenValue = char2Int(command[6]);
			  int value = 0;
			  int i;

			  for(i=0; i<lenValue; i++){
				  value += (char2Int(command[7+i])*pow(10,lenValue-1-i));
			  }

			  if (command[5] == 'P'){
				  setPWMPeriod(port,pin,value);
			  }
			  else if(command[5] == 'D'){
				  setPWMDuty(port,pin,value);
			  }
		  }

		  else if(command[0] == 'O' & command[1] == 'T'){ //GET OFFLINE TASK RESULTS

			  int samples = getOfflineTaskSize();
			  int *offlineArray = getOfflineTask();
			  int i;
			  for(i=0; i < samples; i++){
				  char c[4];
				  sprintf(c, "%d",offlineArray[i]);
				  sendString(c);
				  sendString(".");
			  }
		  }

		  memset(command, 0, 30); //clean command because it has been processed
		  cmdPos=0; //starting point of the command pointer
		  cmdRdy=0; //no command ready to be processed
		  sendString("/"); //end of output message
	  }


  } //end while

} //end main





#pragma vector=TIMER0_A0_VECTOR
	__interrupt void Timer0_A0 (void) {		// Timer0 A0 interrupt service routine
		doOfflineTask();
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){
	//TA0CCTL0 = ~CCIE;
    switch(__even_in_range(UCA0IV,4)){
		case 0: break;                          // Vector 0 - no interrupt
		case 2:

			if(UCA0RXBUF == 'N'){
				cmdPos=0;
				command[cmdPos]=UCA0RXBUF;
				cmdRdy=1;
			}
			else if (UCA0RXBUF != '/'){
				command[cmdPos]=UCA0RXBUF;
				cmdPos++;
			}
			else{
				cmdPos=0;
				cmdRdy=1;
			}

			break;
		case 4: break;
		default: break;
    }
    //TA0CCTL0 = CCIE;
}
