#include "uart.h"

#if DEBUG

static int uart_putchar(char c, FILE *stream);

#ifdef __cplusplus
extern "C" {
    FILE * uart_stdout;
}
#endif

static int uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
        uart_putchar('\r', stream);

    putx(c);
    return 0;
}

__attribute((naked, used, section (".init8")))
void uart_debug_init()
{
    uart_stdout = fdevopen(uart_putchar, NULL);
    stdout = uart_stdout;
}

#endif
