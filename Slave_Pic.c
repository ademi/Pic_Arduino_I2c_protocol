/*****************************
 
 * I2C PIC slave Code
 * Mohammed Al-ademi

 * Demonstrates usage of I2c Protocol in PIC 18F microcotrollers
 *
 * sends the message "Hello World" in ASCII format to master when requeste
 * Created 5 Nov 29=015
 * 
 * This example code is in the public domain.
*****************************/
#include <xc.h>

#pragma config OSC = HS 
#pragma config WDT = OFF
#pragma config PWRT= ON
 
#define _XTAL_FREQ          12000000 
#define slave_address       0x18

unsigned short recieved ;
unsigned char send[] = {0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,0x72,0x6c,0x64};
unsigned char counter =0;             // dummy counter for counting elements sent to I2C

void init_i2c(void){
    TRISC |=0x18;           // rc3 and rc4 inputs
    
    SSPADD = slave_address;
    SSPCON1 = 0x36;       // SSPEN: Synchronous Serial Port Enable bit - Enables the serial port and configures the SDA and SCL pins as the serial port pins
                          // CKP: SCK Release Control bit              - Release clock
                          // SSPM3:SSPM0: SSP Mode Select bits         - 0110 = I2C Slave mode, 7-bit address    
    SSPCON2=0x01;         // DISABLE GENERAL CALL ENABLE BIT
    SSPSTAT=0x00;
    PIE1bits.SSPIE = 1;            // Enable SSP interrupt
    IPR1bits.SSPIP = 0x01;         // Set SSP interrupt priority to high
    PIR1bits.SSPIF = 0;

}


// This is the actual SSP ISR
void i2c_isr(void) {
    unsigned char temp , discard ;
    unsigned char size = sizeof(send)/sizeof(send[0]);
    temp = SSPSTAT & 0x2d;
    if ((temp ^ 0x09) == 0x00) {            // write request, last byte was address
        // Nothing to do in this case for this project, just empty the buffer.
        discard = SSPBUF ;
        
    } else if ((temp ^ 0x29) == 0x00) {     // write request, last byte was data
        // Nothing to do in this case for this project, just empty the buffer.
        discard = SSPBUF ;
       
    } else if ((temp ^ 0x0c) == 0x00) {     // read request, last byte was address == SEND MESSAGE FROM THE BEGINNING
        counter = 0;       // reset the counter     
        SSPCON1bits.WCOL = 0;
        discard = SSPBUF;                   //clear the buffer
        SSPBUF = send[counter++];           //Send the first element of the message and increase the counter
        
        while(!SSPSTATbits.BF);             // hold on till the buffer is empty
        if (SSPSTATbits.BF) discard = SSPBUF; // just to ensure no collision happens, empty the buffer
       
    } else if ((temp ^ 0x2c) == 0x00) {     // read request , last byte was data
        if (SSPSTATbits.BF) discard = SSPBUF;    // Clear BF
        SSPCON1bits.WCOL = 0;
        if(counter >= size)counter = 0;         // ensure the counter did not go over the message limit
        SSPBUF = send[counter++];   
    } else {                                // slave logic reset by NACK from master
        discard = SSPBUF ;
    }
    SSPCON1bits.CKP = 1;                     // release the clock line
    LATDbits.LATD1 = ~LATDbits.LATD1;        // indicate any I2c operation by flashing an LED
}
void interrupt high_isr(void)             // High priority interrupt
{
    if (PIR1bits.SSPIF && PIE1bits.SSPIE) {        // Check for SSP interrupt
        i2c_isr();            // It is an SSP interrupt, call the SSP ISR
        PIR1bits.SSPIF = 0;  // Clear the interrupt flag
    }
}

void interrupt low_priority   LowIsr(void)    //Low priority interrupt
{
}

void main(void)
{
   TRISD=0x00;
    LATDbits.LATD3 = 1;
    LATDbits.LATD2 = 1;
    LATDbits.LATD1 = 0;

    init_i2c();
    INTCONbits.PEIE = 1;    // PERIPHERRAL INTERRUPT ENABLED
    RCONbits.IPEN   = 1;    // INTERRUPT PRIORITY ENABLED
    INTCONbits.GIE = 1;     // GLOBAL INTERRUPT ENABLED
    
    while (1);
}
