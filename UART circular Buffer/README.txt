General code for UART circular buffers (RX and TX), running in half duplex mode (single wire) using interrupts.
It was developed to run on any STM32 platform. Change on "UART_circular_buffers.h" the UART Handler on the UART_HNDLR definition. 
Both the UART and its pins configuration should be done on the Application using this code. 
The callback functions should be called in the Interrupt Service Routine.
