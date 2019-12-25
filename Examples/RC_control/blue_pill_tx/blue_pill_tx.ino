#include <SPI.h>
#include <RFM73.h>

//#define DEBUG

// Blue Pill PINS
#define SPI2_NSS_PIN  PB12    // NSS SPI
#define RFM73_CE      PA9     // CE rfm73 module "1" - rx/tx "0" - Standby
#define LED           PC13

//SPI
#define SPI_SPEED 80000

SPIClass SPI_2(2); //Create an instance of the SPI Class called SPI_2 that uses the 2nd SPI Port

// RXTX - settings
#define RXTX_ADDR 0x03
#define RXTX_CHAN 0x01

// RF module
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

// rc data packet 4 bytes: |ch1|ch2|reserved|reserved|
#define RC_PACKET_LEN 4
uint8_t rc_packet[RC_PACKET_LEN];

void setup() {

  Serial.begin(115200);

  // SPI initialisation
  SPI_2.begin();
  SPI_2.setBitOrder(MSBFIRST); // Set the SPI_2 bit order
  SPI_2.setDataMode(SPI_MODE0); //Set the  SPI_2 data mode 0
  SPI_2.setClockDivider(SPI_CLOCK_DIV16);  // Use a different speed to SPI 1

  // PINS initiaation
  // Digital
  pinMode(SPI2_NSS_PIN, OUTPUT);
  digitalWrite(SPI2_NSS_PIN,HIGH);
  pinMode(RFM73_CE,OUTPUT);
  digitalWrite(RFM73_CE,HIGH);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);   // LED Lights when pin LOW
  // Analog
  pinMode(PA0,INPUT_ANALOG);
  pinMode(PA1,INPUT_ANALOG);
  
  // Выбор канала - джампер
  //set_channel();

  // RFM73 initialisation
  rfm73.SPI_1byte_IO = spi_io;
  rfm73.SPI_NSS_set = spi_nss_set;
  rfm73.CE_set = rfm73_ce_set;
  rfm73.init(0,RC_PACKET_LEN);
}

void loop() {
  uint8_t reg=0;
  //  Опрашиваем кнопки, резисторы, энкодеры
  rc_packet[0]=map(analogRead(PA0),0,4095,0,170);
  rc_packet[1]=map(analogRead(PA1),0,4095,0,170);

  // debug
 #ifdef DEBUG
  Serial.print("CH1:");
  Serial.println(rc_packet[0],DEC);
  Serial.print("CH2:");
  Serial.println(rc_packet[1],DEC);
 #endif
  
  // Отправляем пакет в канал
  
  rfm73.send((char *)rc_packet,RC_PACKET_LEN);
  

  uint16_t cnt=10;
  while ((!rfm73.is_data_sent())&&cnt){
    cnt--;
    delay(1);
  }

  // if got ASK from rx - blink led
  if (cnt){
    digitalWrite(LED,LOW);
    delay(10);
    digitalWrite(LED,HIGH);
  }

  // debug
#ifdef DEBUG
  Serial.print("CNT:");
  Serial.println(cnt,DEC);
#endif
  
  //delay(200);
  
}
