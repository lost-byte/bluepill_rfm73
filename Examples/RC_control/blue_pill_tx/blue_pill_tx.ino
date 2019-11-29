#include <SPI.h>
#include <RFM73.h>

// Ноги Blue Pill
#define SPI2_NSS_PIN  PB12    // NSS SPI
#define RFM73_CE      PA9     // CE платы rfm73 "1" - работа "0" - Ожидание


//SPI
#define SPI_SPEED 80000

SPIClass SPI_2(2); //Create an instance of the SPI Class called SPI_2 that uses the 2nd SPI Port

// RXTX - settings
#define RXTX_ADDR 0x03
#define RXTX_CHAN 0x01


RFM73 rfm73; 

/// SPI IO for RFM73
char spi_io(char b){
  return (char)SPI_2.transfer(b);
}

void spi_nss_set(char b){
  digitalWrite(SPI2_NSS_PIN,b?HIGH:LOW);
}

void rfm73_ce_set(char b){
  digitalWrite(RFM73_CE,b?HIGH:LOW);
}

void setup() {

  // Инит ножек
  // Инит периферии
  Serial.begin(115200);

  // SPI2
  SPI_2.begin();
  SPI_2.setBitOrder(MSBFIRST); // Set the SPI_2 bit order
  SPI_2.setDataMode(SPI_MODE0); //Set the  SPI_2 data mode 0
  SPI_2.setClockDivider(SPI_CLOCK_DIV16);  // Use a different speed to SPI 1

  // GPIO
  pinMode(SPI2_NSS_PIN, OUTPUT);
  digitalWrite(SPI2_NSS_PIN,HIGH);
  pinMode(RFM73_CE,OUTPUT);
  digitalWrite(RFM73_CE,HIGH);
  // Выбор канала - джампер
  //set_channel();

  // Инициализация модуля RFM73
  rfm73.SPI_1byte_IO = spi_io;
  rfm73.SPI_NSS_set = spi_nss_set;
  rfm73.CE_set = rfm73_ce_set;
  rfm73.init(0);
  
}

void loop() {
  uint8_t reg=0;
  //  Опрашиваем кнопки, резисторы, энкодеры
  
  // Отправляем пакет в канал
  /*
  Serial.println("-------------------------");
  reg = rfm73.RFM73_reg_read(0);
  Serial.print("CONFIG:");
  Serial.println(reg,HEX);

  reg = rfm73.RFM73_reg_read(1);
  Serial.print("EN_AA:");
  Serial.println(reg,HEX);

  reg = rfm73.RFM73_reg_read(2);
  Serial.print("EN_RXADDR:");
  Serial.println(reg,HEX);

  reg = rfm73.RFM73_reg_read(3);
  Serial.print("SETUP_AW:");
  Serial.println(reg,HEX);

  reg = rfm73.RFM73_reg_read(4);
  Serial.print("SETUP_RETR:");
  Serial.println(reg,HEX);

  reg = rfm73.RFM73_reg_read(5);
  Serial.print("RF_CH:");
  Serial.println(reg,HEX);

  reg = rfm73.RFM73_reg_read(6);
  Serial.print("RF_SETUP:");
  Serial.println(reg,HEX);

  reg = rfm73.RFM73_reg_read(7);
  Serial.print("STATUS:");
  Serial.println(reg,HEX);

  reg = rfm73.RFM73_reg_read(8);
  Serial.print("OBSERVE_TX:");
  Serial.println(reg,HEX);
  */
  char mess[] = "Hello rfm73!Hello rfm73!.......";
  Serial.println("\n>");
  
  
  rfm73.reg_modify(RFM73_RG_STATUS,(1<<RX_DR)|(1<TX_DS)|(1<<MAX_RT),0); // Без этой фичи не передает.
  rfm73.set_tx_pl(mess,32);
  

  int cnt=10;
  while ((!rfm73.is_tx_fifo_empty())&&cnt){
    cnt--;
    delay(100);
  }
    
  Serial.print("CNT:");
  Serial.println(cnt,DEC);
  
  delay(100);
  
}
