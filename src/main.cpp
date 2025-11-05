#include <avr/io.h>
#include <drivers/gpio_driver.hpp>  

int main() {

    GpioDriver gpioDriver;

    PinDescription led(&PORTB, 5u); //imi da eroare cu const la port, cu volatile nu!!!!

    GpioStatus status = gpioDriver.SetPinDirection(led, PinDirection::OUT);

    if(!status.IsSuccess()) 
    {
        return -1;
    } 

    gpioDriver.SetPinValue(led, PinValue::HIGH);


    //UartStatus myStatus = myUartObject.ReceiveByte();
 //if(myStatus.IsError()) 
 //{
  // Close connection;  
 //} 
 //else 
 //{
    //uint8_t receivedData = myStatus.GetReceivedByte();
 //}

    return 0; 
}   