#ifndef RFM73_H
#define RFN73_H

#include "Arduino.h"

// RFM73 Commands
#define RFM73_CMD_R_REG         0x00
#define RFM73_CMD_W_REG         0x20
#define RFM73_CMD_R_RX_PAYLOAD  0x61
#define RFM73_CMD_W_TX_PAYLOAD  0xA0
#define RFM73_CMD_FLUSH_TX      0xE1
#define RFM73_CMD_FLUSH_RX      0xE2
#define RFM73_CMD_REUSE_TX_PL   0xE3
#define RFM73_CMD_ACTIVATE      0x50
#define RFM73_CMD_R_RX_PL_WID   0x60
#define RFM73_CMD_W_ACK_PAYLOAD 0xA8
#define RFM73_CMD_W_TX_PL_NOACK 0xB0
#define RFM73_CMD_NOP           0xFF

// RFM73 Registers
#define RFM73_RG_CONFIG            0x00
#define RFM73_RG_EN_AA             0x01
#define RFM73_RG_RX_ADDR           0x02
#define RFM73_RG_SETUP_AW          0x03
#define RFM73_RG_SETUP_RETR        0x04
#define RFM73_RG_RF_CH             0x05
#define RFM73_RG_RF_SETUP          0x06
#define RFM73_RG_STATUS            0x07
#define RFM73_RG_OBSERVE_TX        0x08
#define RFM73_RG_CD                0x09
#define RFM73_RG_RX_ADDR_P0        0x0A
#define RFM73_RG_RX_ADDR_P1        0x0B
#define RFM73_RG_RX_ADDR_P2        0x0C
#define RFM73_RG_RX_ADDR_P3        0x0D
#define RFM73_RG_RX_ADDR_P4        0x0E
#define RFM73_RG_RX_ADDR_P5        0x0F
#define RFM73_RG_TX_ADDR           0x10
#define RFM73_RG_RX_PW_P0          0x11
#define RFM73_RG_RX_PW_P1          0x12
#define RFM73_RG_RX_PW_P2          0x13
#define RFM73_RG_RX_PW_P3          0x14
#define RFM73_RG_RX_PW_P4          0x15
#define RFM73_RG_RX_PW_P5          0x16
#define RFM73_RG_FIFO_STATUS       0x17
#define RFM73_RG_DINPD             0x1C
#define RFM73_RG_FEATURE           0x1D

// RFM73 Registers bits

// RFM73_CONFIG
#define MASK_RX_DR              6
#define MASK_TX_DS              5
#define MASK_MAX_RT             4
#define EN_CRC                  3
#define CRCCO                   2
#define PWR_UP                  1
#define PRIM_RX                 0

// RFM73_STATUS
#define RBANK                   7
#define RX_DR                   6
#define TX_DS                   5
#define MAX_RT                  4
#define RX_P_NO                 1
#define TX_FULL                 0

// RFM73_FIFO_STATUS
#define TX_REUSE                6
#define TX_FULL                 5
#define TX_EMPTY                4
#define RX_FULL                 1
#define RX_EMPTY                0

// RFM73_FIFO_STATUS
#define TX_REUSE                6
#define TX_FULL                 5
#define TX_EMPTY                4
#define RX_FULL                 1
#define RX_EMPTY                0


// RFM73 Operation modes

// addr_width
#define RFM73_AW3               0x00
#define RFM73_AW4               0x02
#define RFM73_AW5               0x03

// data rates
#define RFM73_DR_1M             0x00
#define RFM73_DR_2M             0x01
#define RFM73_DR_250K           0x04
#define RFM73_DR_2M_2           0x05

class RFM73 {

	
private:
	// SPI IO Functions
	void SPI_transfer(char *dest, char *src, uint8_t len);
	void SPI_write(char *src, uint8_t len);
	void SPI_cmd_read(char cmd, char *dest, uint8_t len);
	void SPI_cmd_write(char cmd, char *src, uint8_t len);
	
	uint8_t reg_read(uint8_t r_address);
	void reg_write(uint8_t r_address, uint8_t val);
	
	
public:
	// External SPI IO fn
	char (*SPI_1byte_IO)(char b);
	void (*SPI_NSS_set)(char b);
	void (*CE_set)(char b);

	// FIXME! should be moved to private
	uint8_t reg_modify(uint8_t r_address, uint8_t set_bits, uint8_t reset_bits);

	// FIFO workout
	void set_tx_pl(char *pl, uint8_t len);

	void set_tx_pl_no_ack(char *pl, uint8_t len);

	void flush_tx_buf();

	uint8_t get_rxpl_len();

	void activate();

	void get_rx_pl(char *dest, uint8_t len);

	void switch_bank();

	void init_b1_regs();
// RFM73 LVL2 API - based on LVL1

	void pwrup(char rx_mode);

	void init(char rx_mode, uint8_t pl_width);

	bool is_rx_fifo_full();

	bool is_tx_fifo_empty();

	void read_rx_blocked(char *dest);

};
#endif