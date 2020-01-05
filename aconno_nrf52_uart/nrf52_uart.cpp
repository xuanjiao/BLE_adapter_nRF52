/** Made by: Karlo Milicevic, 2017. 
 * aconno.de
 */
 
#include "nrf52_uart.h"
#include "utilities.h"

NRF52_UART::NRF52_UART(PinName TX, PinName RX, Baudrate baudrate){
    setOutput(TX);
    NRF_UARTE0->PSEL.TXD = TX;
    
    setInput(RX);
    NRF_UARTE0->PSEL.RTS = RX;
    
    NRF_UARTE0 ->BAUDRATE = (int32_t)baudrate;
    NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Enabled;
}

bool NRF52_UART::send(uint8_t *buffer, uint8_t length)
{
    NRF_UARTE0->EVENTS_ENDTX = 0;
    NRF_UARTE0->TASKS_STOPTX = 0;

    NRF_UARTE0->TXD.PTR = (uint32_t)buffer;
    NRF_UARTE0->TXD.MAXCNT = (uint32_t)length;

    NRF_UARTE0->TASKS_STARTTX = 1;

    while((NRF_UARTE0->EVENTS_ENDTX == 0) &&
          (NRF_UARTE0->EVENTS_TXSTOPPED == 0)){
        // wait
    }

    NRF_UARTE0->EVENTS_ENDTX = 0;
    NRF_UARTE0->TASKS_STOPTX = 0;
    
    return NRF_UARTE0->TXD.AMOUNT == length;
}

bool NRF52_UART::receive(uint8_t *buffer, uint8_t length)
{
    return 0;
}
