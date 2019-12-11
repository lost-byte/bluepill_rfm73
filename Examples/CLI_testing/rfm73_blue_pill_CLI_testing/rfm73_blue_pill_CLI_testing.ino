#include <SPI.h>
#include <RFM73.h>

//#define DEBUG

// Blue Pill PINS
#define SPI2_NSS_PIN  PB12    // NSS SPI
#define RFM73_CE      PA9     // CE платы rfm73 "1" - работа "0" - Ожидание
#define LED           PC13

//SPI
#define SPI_SPEED 80000

SPIClass SPI_2(2); //Create an instance of the SPI Class called SPI_2 that uses the 2nd SPI Port

// RF module
RFM73 rfm73; 

/// SPI IO functions for RFM73 lib
char spi_io(char b){
  return (char)SPI_2.transfer(b);
}

void spi_nss_set(char b){
  digitalWrite(SPI2_NSS_PIN,b?HIGH:LOW);
}

void rfm73_ce_set(char b){
  digitalWrite(RFM73_CE,b?HIGH:LOW);
}

// TX buffer, max 32 bytes
uint8_t tx_buf[32];

// RX buffer, max 32 bytes
uint8_t rx_buf[32];

// Future declaration of CLI command functions
void RFM73_reg_dump(char *args);
void RFM73_send(char *args);
void RFM73_send_nack(char *args);
void RFM73_set(char *args);

// CLI routlines (maybe sometime it will be library?)
#define CLI_PROMPT "rfm73#:"
#define CLI_MAX_IN 80

uint8_t cli_in_buf_lvl=0;   // level counter for CLI input buffer
char cli_in_buf[80];

struct cli_command{
  const char *command;                    // command name with space sym at the end
  void (*fn)(char*);                      // command function, should take line tail as argument
};

// Commands list
const struct cli_command cli_commad_list[]={
  {"help",  CLI_help},
  {"dump", RFM73_reg_dump},
  {"send", RFM73_send},
  {"send_nack", RFM73_send_nack},
  {"set", RFM73_set},
  NULL,
};

struct help_line{
  const char *line;
};

// Cli help list of strings
const struct help_line cli_help[]={
  {"RFM test utility, usage:"},
  {"help - this output"},
  {"dump - print RFM73 register states"},
  {"send - switch to TX-mode, send message and wait for ack"},
  {"send_nack - like 'send' but without ack"},
  {"set - set RFM73 option:"},
  {"     channel 1..8 - switch channel"},
  {"     plen 0..32 - lenght of payload, 0 - for dynamic length"},
  {"     pwr 0..4 - tx power lvl"},
  {"     ack_pl [string] - put message to ack payload"},
  NULL,
};

typedef struct cli_command_list cli_command_list_t;

void CLI_help(char *args){
  const struct help_line *next_line = cli_help;
  while(next_line->line!=NULL){
//    Serial.print((uint32_t)next_line,HEX);
//    Serial.print(':');
//    Serial.print((uint32_t)(next_line->line),HEX);
//    Serial.print(':');
    Serial.println(next_line->line);
    next_line++;
  }
}



void CLI_flush(){
  cli_in_buf_lvl = 0;
  memset(cli_in_buf,0,CLI_MAX_IN);
}

// CLI proces function
void CLI_process(uint16_t timeout_ms){ 
  char last_sym;
  char wait_cycles=0;
  
  // get CLI command, roll in while untill command collected, or wait exceeds timeout , or buffer space available
  while ((cli_in_buf_lvl<CLI_MAX_IN)&&(wait_cycles<timeout_ms)){
    if (Serial.available()){
      last_sym=Serial.read();
      if (last_sym=='\n'){                    // Is command entered?
        Serial.print(last_sym);                 // print echo
        CLI_command_process(cli_in_buf);    // yes, do it
        CLI_flush();                          // Clear input buffer
        //CLI_help(NULL);
        Serial.print("\n"CLI_PROMPT);             // Show prompt for new command
        return;
      }else{
        cli_in_buf[cli_in_buf_lvl]=last_sym;     // Not yet, collect sym
        cli_in_buf_lvl++;
        Serial.print(last_sym);                 // print echo
      }
    }else{
      delay(1);
      wait_cycles++;                      // Append wait counter
    }
  }

  // Let's process cycle exit conditions
  // When timeout exceeds - do nothing
  // When command collected -  function returns after command complete
  if (cli_in_buf_lvl==CLI_MAX_IN){        // too mach symbols at buffer, pre-overflow
    CLI_flush();                          // Clear input buffer
    Serial.println("\nToo long!");          // Print warning
    Serial.print(CLI_PROMPT);             // Show prompt for new command
  }
}

void CLI_command_process(char *line){
  uint8_t cmd_len;
  uint8_t line_len = strlen(line);
  const struct cli_command *next_cmd = cli_commad_list;
  // roll thrue commnd list to find one
  while(next_cmd->command!=NULL){
    cmd_len = strlen(next_cmd->command);
    if (!strncmp(next_cmd->command,line,cmd_len)){
      if ((cmd_len==line_len)||(line[cmd_len]==' ')){
        next_cmd->fn(line+strlen(next_cmd->command));
        return;
      }
    }
    next_cmd++;
  }
  // No such command, warning message
  Serial.println("\nNo such command!");   // print warning
  CLI_help(NULL);                       // print help
}

void RFM73_reg_dump(char *args){
  Serial.println("RFM73_reg_dump:OK");
}

void RFM73_send(char *args){
  Serial.println("RFM73_send:OK");
}

void RFM73_send_nack(char *args){
  Serial.println("RFM73_send_nack:OK");
}

void RFM73_set(char *args){
  Serial.println("RFM73_set:OK");
}

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
  digitalWrite(LED,HIGH);

  // RFM73 module init
  rfm73.SPI_1byte_IO = spi_io;
  rfm73.SPI_NSS_set = spi_nss_set;
  rfm73.CE_set = rfm73_ce_set;
  rfm73.init(0,32);

  // Show CLI prompt
  Serial.print(CLI_PROMPT);
}

void loop() {

    // if no sympols - it returns immediatly
    CLI_process(1000); 
}
