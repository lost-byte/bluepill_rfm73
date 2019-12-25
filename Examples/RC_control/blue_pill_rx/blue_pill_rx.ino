#include <SPI.h>
#include <RFM73.h>
#include <Servo.h>

//#define DEBUG

// Blue Pill PINS
#define SPI2_NSS_PIN  PB12    // NSS SPI
#define RFM73_CE      PA9     // CE платы rfm73 "1" - работа "0" - Ожидание
#define LED           PC13
#define STEER_PIN     PB9
#define ACCBR_PIN     PB8

//SPI
#define SPI_SPEED 80000

SPIClass SPI_2(2); //Create an instance of the SPI Class called SPI_2 that uses the 2nd SPI Port

// RXTX - settings
#define RXTX_ADDR 0x03
#define RXTX_CHAN 0x01

// RF module
RFM73 rfm73; 

// Servo
Servo steer;
Servo accbr;

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

// Max tryes to wait rc packet in FIFO
#define WAIT_RX_MAXCNT 200

void setup() {

  // Serial line - debug and settings/trim
  Serial.begin(115200);

  // SPI2
  SPI_2.begin();
  SPI_2.setBitOrder(MSBFIRST); // Set the SPI_2 bit order
  SPI_2.setDataMode(SPI_MODE0); //Set the  SPI_2 data mode 0
  SPI_2.setClockDivider(SPI_CLOCK_DIV16);  // Use a different speed to SPI 1

  // Blue pill pins
  pinMode(SPI2_NSS_PIN, OUTPUT);
  digitalWrite(SPI2_NSS_PIN,HIGH);
  pinMode(RFM73_CE,OUTPUT);
  digitalWrite(RFM73_CE,HIGH);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);   // Led lights when pin is LOW
  
  // Read radio channel
  //set_channel();

  // RFM73 module init
  rfm73.SPI_1byte_IO = spi_io;
  rfm73.SPI_NSS_set = spi_nss_set;
  rfm73.CE_set = rfm73_ce_set;
  rfm73.init(1,RC_PACKET_LEN);

  // Servo init
  steer.attach(STEER_PIN);
  accbr.attach(ACCBR_PIN);
}

void controls_neitral(){
  // set up servos
  steer.write(170/2);
  accbr.write(170/2);
}

void rc_packet_apply(){
    
#ifdef DEBUG
  // debug - show channel values 
  Serial.print("CH1:");
  Serial.println(rc_packet[0],DEC);
  Serial.print("CH2:");
  Serial.println(rc_packet[1],DEC);
#endif

  // set up servos
  steer.write(rc_packet[0]);
  accbr.write(rc_packet[1]);
}

void loop() {
  
  uint8_t pl_len=0;   // rx payload len
  uint8_t wait_rx_cnt = WAIT_RX_MAXCNT;

  // wait for any byte on rx FIFO
  while ((pl_len<RC_PACKET_LEN)&&wait_rx_cnt){
    pl_len = rfm73.get_rxpl_len();
    delay(1);
    wait_rx_cnt--;
  }

  if (wait_rx_cnt){
    // rc packet has been received less then WAIT_RX_MAXCNT tryes

    // indicate that rc_packet has been recieved
    digitalWrite(LED,LOW);
    delay(10);
    digitalWrite(LED,HIGH);

    // get payload
    rfm73.get_rx_pl((char *)rc_packet,RC_PACKET_LEN);
    
    // set up servos
    rc_packet_apply();

    // clear packet buffer
    memset(rc_packet,0,RC_PACKET_LEN);
    
  }else{
    // rc packet not received in WAIT_RX_MAXCNT tryes
    // tx lost

    // Set all controls to neitral
    controls_neitral();

    // flush tx FIFO
    rfm73.flush_rx_fifo();

    // clear all interrups on rfm73
    rfm73.cli();
  }
    
  //delay(200);
}
