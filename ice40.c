//******************************************************************************
// 2024 Written by Gary Sinde, Flagler Engineering LLC
// Feel free to use and modify as you need
//******************************************************************************
#include "ice40.h"

void Delay(int msec);

//******************************************************************************
// The FPGA must be booted with its firmware before it can be used
// 1) After exiting the Power-On Reset (POR) state or when CRESET_B returns High 
//    after being held Low, the iCE40 device samples the logical value on its 
//    SPI_SS_B pin. The SPI_SS_B pin has an internal pull-up resistor.
// 2)  If the SPI_SS_B pin is sampled as (High), then read from Serial Flash
// 3)  If the SPI_SS_B pin is sampled as (Low), then we boot it over SPI
//******************************************************************************
void FPGA_BOOT(unsigned char *bin, int size)
{
    uint8_t txbuff;
    int i;
    
    // Place ICE40 in Reset
    FPGA_RESET_LOW();
    
    // Activate the SPI Pins
    FPGA_SPI_ACTIVE();
    
    // Make sure ICE40 Chip Select is LOW (aka Master SPI Program Mode)
    FPGA_CS_LOW();
    
    // Make sure the ICE 40 Clock is HIGH
    FPGA_CLK_HIGH();

    // Release the ICE40 out of Reset
    Delay(50);
    FPGA_RESET_HIGH();
    Delay(10);
   
    // Boot the Image
    while (size)
    {
        txbuff = *bin;
        for( i = 0; i < 8; i++ )
        {
            // Lower the Clock
            FPGA_CLK_LOW();
            
            // Set or Clear the Output Bit
            if (txbuff & 0x80)
                FPGA_MOSI_HIGH();
            else
                FPGA_MOSI_LOW();

            // Raise the Clock
            FPGA_CLK_HIGH();

            txbuff <<= 1;
        }
        size--;
        bin++;
    }
    
    // Send 49 dummy bits
    size = 49;
    while (size)
    {
        // Lower the Clock
        FPGA_CLK_LOW();

        // Raise the Clock
        FPGA_CLK_HIGH();

        size--;
    }
     
    Delay(10);
    
    // Deactivate the FPGA SPI Bus
    FPGA_CS_HIGH();
    
    FPGA_SPI_INACTIVE();    
}
