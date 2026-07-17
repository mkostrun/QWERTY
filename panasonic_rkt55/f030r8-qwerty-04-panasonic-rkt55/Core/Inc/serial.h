#ifndef __serial_H
#define __serial_H

extern uint8_t usart2_rx_dma_buffer[USART2_RX_DMA_BUFFER_SIZE];
extern char    usart2_rx_cmd_buffer[USART2_RX_CMD_BUFFER_SIZE];

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;

extern __IO ITStatus UART_TX_Completed;

extern void HAL_UART_MspDeInit(UART_HandleTypeDef* huart);

extern void MX_USART2_UART_Init(void);

extern void HAL_UART_MspInit(UART_HandleTypeDef* huart);

extern void print_serial(char * msg);

extern int32_t __io_putchar(char *);

extern int32_t __io_getchar(void);

extern void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle);

extern void USART2_IRQHandler(void);


#endif
