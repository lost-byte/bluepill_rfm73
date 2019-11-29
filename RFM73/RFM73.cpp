#include "RFM73.h"

// External SPI IO fn
char *(*SPI_1byte_IO)(char *b);

// SPI IO Functions
void RFM73::SPI_transfer(char *dest, char *src, uint8_t len)
{
  //digitalWrite(SPI2_NSS_PIN, LOW); 
  SPI_NSS_set(0);
  for (uint8_t i=0;i<len;i++){
    *dest++ = SPI_1byte_IO(*src++); 
  }
  //digitalWrite(SPI2_NSS_PIN, HIGH);
  SPI_NSS_set(1);
}

void RFM73::SPI_write(char *src, uint8_t len)
{
  //digitalWrite(SPI2_NSS_PIN, LOW); 
  SPI_NSS_set(0);
  for (uint8_t i=0;i<len;i++){
    SPI_1byte_IO(*src++); 
  }
  //digitalWrite(SPI2_NSS_PIN, HIGH);
  SPI_NSS_set(1);
}

void RFM73::SPI_cmd_read(char cmd, char *dest, uint8_t len)
{
  //digitalWrite(SPI2_NSS_PIN, LOW); 
  SPI_NSS_set(0);
  SPI_1byte_IO(cmd);
  for (uint8_t i=0;i<len;i++){
    *dest++=SPI_1byte_IO(0); 
  }
  //digitalWrite(SPI2_NSS_PIN, HIGH);
  SPI_NSS_set(1);
}

void RFM73::SPI_cmd_write(char cmd, char *src, uint8_t len)
{
  //digitalWrite(SPI2_NSS_PIN, LOW); 
  SPI_NSS_set(0);
  SPI_1byte_IO(cmd);
  for (uint8_t i=0;i<len;i++){
    SPI_1byte_IO(*src++); 
  }
  //digitalWrite(SPI2_NSS_PIN, HIGH);
  SPI_NSS_set(1);
}

// RFM73 IO Functions
uint8_t RFM73::reg_read(uint8_t r_address){
  char dest[2];
  char src[2];
  uint8_t r_addr =r_address&0x1F;  // Только 5 бит адреса

  src[0]=RFM73_CMD_R_REG|r_addr;
  src[1]=0;

  SPI_transfer(dest,src,2);
  return dest[1];
}

void RFM73::reg_write(uint8_t r_address, uint8_t val){
  char dest[2];
  char src[2];
  uint8_t r_addr =r_address&0x1F;  // Только 5 бит адреса

  src[0]=RFM73_CMD_W_REG|r_addr;
  src[1]=val;

  SPI_transfer(dest,src,2);
}

uint8_t RFM73::reg_modify(uint8_t r_address, uint8_t set_bits, uint8_t reset_bits){
  uint8_t reg = reg_read(r_address);
  reg&=(~reset_bits);
  reg|=(set_bits);
  reg_write(r_address,reg);
}

// FIFO workout
void RFM73::set_tx_pl(char *pl, uint8_t len){
  SPI_cmd_write(RFM73_CMD_W_TX_PAYLOAD,pl,len);
}

void RFM73::set_tx_pl_no_ack(char *pl, uint8_t len){
  SPI_cmd_write(RFM73_CMD_W_TX_PL_NOACK,pl,len);
}

uint8_t RFM73::get_rxpl_len(){
  char dest[2];
  char src[2];

  src[0]=RFM73_CMD_R_RX_PL_WID;
  src[1]=0;

  SPI_transfer(dest,src,2);
  return dest[1];
}

void RFM73::activate(){
  char dest[2];
  char src[2];

  src[0]=RFM73_CMD_ACTIVATE;
  src[1]=0x73;

  SPI_transfer(dest,src,2);
}

void RFM73::get_rx_pl(char *dest, uint8_t len){
  SPI_cmd_read(RFM73_CMD_R_RX_PAYLOAD,dest,len);  
}

void RFM73::switch_bank(){
  char dest[2];
  char src[2];

  src[0]=RFM73_CMD_ACTIVATE;
  src[1]=0x53;

  SPI_write(src,2);
}

const char b1reg00[] = {0x40, 0x4B, 0x01, 0xE2};
const char b1reg01[] = {0xC0, 0x4B, 0x00, 0x00};
const char b1reg02[] = {0xD0, 0xFC, 0x8C, 0x02};
const char b1reg03[] = {0x99, 0x00, 0x39, 0x41};
const char b1reg04[] = {0xD9, 0x9E, 0x86, 0x0B};
const char b1reg05[] = {0x24, 0x06, 0x0F, 0xA6};
const char b1reg0C[] = {0x00, 0x12, 0x73, 0x00};
const char b1reg0D[] = {0x36, 0xb4, 0x80, 0x00};
const char b1reg0E[] = {0x41,0x20,0x08,0x04,0x81,0x20,0xCF,0xF7,0xFE,0xFF,0xFF};

void RFM73::init_b1_regs(){
  char src[12];
  char dest[12];
  
  switch_bank();
  src[0]=RFM73_CMD_W_REG|0x00;
  memcpy(src+1,b1reg00,4);
  SPI_transfer(dest,src,5);
  src[0]=RFM73_CMD_W_REG|0x01;
  memcpy(src+1,b1reg01,4);
  SPI_transfer(dest,src,5);
  src[0]=RFM73_CMD_W_REG|0x02;
  memcpy(src+1,b1reg02,4);
  SPI_transfer(dest,src,5);
  src[0]=RFM73_CMD_W_REG|0x03;
  memcpy(src+1,b1reg03,4);
  SPI_transfer(dest,src,5);
  src[0]=RFM73_CMD_W_REG|0x04;
  memcpy(src+1,b1reg04,4);
  SPI_transfer(dest,src,5);
  src[0]=RFM73_CMD_W_REG|0x05;
  memcpy(src+1,b1reg05,4);
  SPI_transfer(dest,src,5);
  src[0]=RFM73_CMD_W_REG|0x0C;
  memcpy(src+1,b1reg0C,4);
  SPI_transfer(dest,src,5);
  src[0]=RFM73_CMD_W_REG|0x0D;
  memcpy(src+1,b1reg0D,4);
  SPI_transfer(dest,src,5);
  src[0]=RFM73_CMD_W_REG|0x0E;
  memcpy(src+1,b1reg0E,11);
  SPI_transfer(dest,src,5);
  switch_bank();
}
// RFM73 LVL2 API - based on LVL1

void RFM73::pwrup(char rx_mode){
  //digitalWrite(RFM73_CE,LOW);
  CE_set(0);
  if (rx_mode){
    reg_modify(RFM73_RG_CONFIG,(1<<PRIM_RX)|(1<<PWR_UP),0);
  }else{
    reg_modify(RFM73_RG_CONFIG,(1<<PWR_UP),(1<<PWR_UP));
  }
  //digitalWrite(CE,HIGH);
  CE_set(1);
}

void RFM73::init(char rx_mode){
  pwrup(rx_mode);
  init_b1_regs();
  activate();
  reg_write(RFM73_RG_RX_PW_P0,32);
}

bool RFM73::is_rx_fifo_full(){
  uint8_t r_fifo_status = reg_read(RFM73_RG_FIFO_STATUS);
  return (r_fifo_status&(1<RX_FULL));
}

bool RFM73::is_tx_fifo_empty(){
  uint8_t r_fifo_status = reg_read(RFM73_RG_FIFO_STATUS);
  return (r_fifo_status&(1<TX_EMPTY));
}

void RFM73::read_rx_blocked(char *dest){
  while (!is_rx_fifo_full()){
    delay(100);
  }
  get_rx_pl(dest,32);
}