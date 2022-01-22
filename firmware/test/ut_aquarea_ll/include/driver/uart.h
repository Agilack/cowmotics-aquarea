#ifndef DRIVER_UART_H
#define DRIVER_UART_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define UART_NUM_2 2

#define UART_PIN_NO_CHANGE      (-1)

/**
 * @brief UART word length constants
 */
typedef enum {
    UART_DATA_5_BITS   = 0x0,    /*!< word length: 5bits*/
    UART_DATA_6_BITS   = 0x1,    /*!< word length: 6bits*/
    UART_DATA_7_BITS   = 0x2,    /*!< word length: 7bits*/
    UART_DATA_8_BITS   = 0x3,    /*!< word length: 8bits*/
    UART_DATA_BITS_MAX = 0x4,
} uart_word_length_t;

/**
 * @brief UART stop bits number
 */
typedef enum {
    UART_STOP_BITS_1   = 0x1,  /*!< stop bit: 1bit*/
    UART_STOP_BITS_1_5 = 0x2,  /*!< stop bit: 1.5bits*/
    UART_STOP_BITS_2   = 0x3,  /*!< stop bit: 2bits*/
    UART_STOP_BITS_MAX = 0x4,
} uart_stop_bits_t;

/**
 * @brief UART parity constants
 */
typedef enum {
    UART_PARITY_DISABLE  = 0x0,  /*!< Disable UART parity*/
    UART_PARITY_EVEN     = 0x2,  /*!< Enable UART even parity*/
    UART_PARITY_ODD      = 0x3   /*!< Enable UART odd parity*/
} uart_parity_t;

/**
 * @brief UART hardware flow control modes
 */
typedef enum {
    UART_HW_FLOWCTRL_DISABLE = 0x0,   /*!< disable hardware flow control*/
    UART_HW_FLOWCTRL_RTS     = 0x1,   /*!< enable RX hardware flow control (rts)*/
    UART_HW_FLOWCTRL_CTS     = 0x2,   /*!< enable TX hardware flow control (cts)*/
    UART_HW_FLOWCTRL_CTS_RTS = 0x3,   /*!< enable hardware flow control*/
    UART_HW_FLOWCTRL_MAX     = 0x4,
} uart_hw_flowcontrol_t;

/**
 * @brief UART source clock
 */
typedef enum {
    UART_SCLK_APB = 0x0,            /*!< UART source clock from APB*/
    UART_SCLK_RTC = 0x1,            /*!< UART source clock from RTC*/
    UART_SCLK_XTAL = 0x2,           /*!< UART source clock from XTAL*/
    UART_SCLK_REF_TICK = 0x3,       /*!< UART source clock from REF_TICK*/

} uart_sclk_t;

/**
 * @brief UART configuration parameters for uart_param_config function
 */
typedef struct {
    int baud_rate;                      /*!< UART baud rate*/
    uart_word_length_t data_bits;       /*!< UART byte size*/
    uart_parity_t parity;               /*!< UART parity mode*/
    uart_stop_bits_t stop_bits;         /*!< UART stop bits*/
    uart_hw_flowcontrol_t flow_ctrl;    /*!< UART HW flow control mode (cts/rts)*/
    uint8_t rx_flow_ctrl_thresh;        /*!< UART HW RTS threshold*/
    union {
        uart_sclk_t source_clk;         /*!< UART source clock selection */
        bool use_ref_tick  __attribute__((deprecated)); /*!< Deprecated method to select ref tick clock source, set source_clk field instead */
    };
} uart_config_t;

/* ========================================================================== */

int uart_driver_install(int uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, void *uart_queue, int intr_alloc_flags);
int uart_get_buffered_data_len(int uart_num, size_t* size);
int uart_param_config(int uart_num, const uart_config_t *uart_config);
int uart_read_bytes(int uart_num, void* buf, uint32_t length, int ticks_to_wait);
int uart_set_pin(int uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num);
#endif
