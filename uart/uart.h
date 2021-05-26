#ifndef UART_H
#define UART_H

#include <picoUART.h>
#include <pu_print.h>

#define printc putx

#if DEBUG
    #include <stdio.h>
    #define print printf
#else
    #define print
#endif

#endif
