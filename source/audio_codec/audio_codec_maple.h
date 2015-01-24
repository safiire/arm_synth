/* AudioCodec_maple.h
   guest openmusiclabs 7.13.11
   this is the library file for MAPLE -> there is a different
   file for Arduino, make sure you are using the right one.
   place this file in th libraries file of your Arduino build.
   e.g. C:\maple-ide-0.0.10-windowsxp32\libraries\ */


#ifndef AudioCodec_h // include guard
#define AudioCodec_h


#include <wirish.h>


#define ADDR 0x34 // i2c address and write bit
#define SDA_PIN 19 // i2c data line
#define SCL_PIN 20 // i2c clock line
#define LED_PIN 13 // board led pin

#ifndef SAMPLE_RATE
  #define SAMPLE_RATE 44
#elif (SAMPLE_RATE == 88)||(SAMPLE_RATE == 44)||(SAMPLE_RATE == 22)||(SAMPLE_RATE == 8)||(SAMPLE_RATE == 2)
#else
  #error SAMPLE_RATE value not defined
#endif

#ifndef ADCHPD
  #define ADCHPD 0
#elif (ADCHPD == 0)||(ADCHPD == 1)
#else
  #error ADCHPD value not defined
#endif

#ifndef ADCS
  #define ADCS 2
#elif (ADCS >=0)&&(ADCS <= 2)
#else
  #error ADCS value not defined
#endif

#ifndef HYST
  #define HYST 64 // default is higher for Maple due to noisier ADCs
#elif (HYST >= 0)&&(HYST <= 255)
#else
  #error HYST value not defined
#endif

#ifndef LINVOL
  #define LINVOL 23
#elif (LINVOL >= 0) && (LINVOL <= 0x1f)
#else
  #error LINVOL value not defined
#endif

#ifndef RINVOL
  #define RINVOL 23
#elif (RINVOL >= 0) && (RINVOL <= 0x1f)
#else
  #error RINVOL value not defined
#endif

#ifndef LHPVOL
  #define LHPVOL 121
#elif (LHPVOL == 0) || ((LHPVOL >= 0x30) && (LHPVOL <= 0x7f))
#else
  #error LHPVOL value not defined
#endif

#ifndef RHPVOL
  #define RHPVOL 121
#elif (RHPVOL == 0) || ((RHPVOL >= 0x30) && (RHPVOL <= 0x7f))
#else
  #error RHPVOL value not defined
#endif

#ifndef MICBOOST
  #define MICBOOST 0
#elif (MICBOOST == 0)||(MICBOOST == 1)
#else
  #error MICBOOST value not defined
#endif

#ifndef MUTEMIC
  #define MUTEMIC 1
#elif (MUTEMIC == 0)||(MUTEMIC == 1)
#else
  #error MUTEMIC value not defined
#endif

#ifndef INSEL
  #define INSEL 0
#elif (INSEL == 0)||(INSEL == 1)
#else
  #error INSEL value not defined
#endif

#ifndef BYPASS
  #define BYPASS 0
#elif (BYPASS == 0)||(BYPASS == 1)
#else
  #error BYPASS value not defined
#endif

#ifndef DACSEL
  #define DACSEL 1
#elif (DACSEL == 0)||(DACSEL == 1)
#else
  #error DACSEL value not defined
#endif

#ifndef SIDETONE
  #define SIDETONE 0
#elif (SIDETONE == 0)||(SIDETONE == 1)
#else
  #error SIDETONE value not defined
#endif

#ifndef SIDEATT
  #define SIDEATT 0
#elif (SIDEATT >= 0)&&(SIDEATT <= 3)
#else
  #error SIDEATT value not defined
#endif

#ifndef KILLSWITCH
  #define KILLSWITCH 0
#elif (KILLSWITCH == 1)||(KILLSWITCH == 0)
#else
  #error KILLSWITCH engaged
#endif

// setup variables for ADC
#if ADCS == 0
  // do nothing
#elif ADCS == 1
  unsigned char _i = 64;
  unsigned int _mod0temp = 0x0000;
#elif ADCS == 2
  unsigned char _i = 128;
  unsigned int _mod0temp = 0x0000;
  unsigned int _mod1temp = 0x0000;
#endif


// codec data transfer interrupt prototype
void AudioCodec_interrupt(void);


// the first section is i2c bit banging and setup
// as the maple uses different pins for i2c than arduino


// us delay timer (~1us per unit)
// sets the i2c clock rate
void mydelay(unsigned char t) {
  while((t << 4) > 0) {
    togglePin(LED_PIN);
    t--;
  }
}


// i2c start condition
char i2cbb_start(void) {
  if (digitalRead(SDA_PIN) == 0) { // check if data line released
    return -1; // end with failure if not released
  }
  else if (digitalRead(SCL_PIN) == 0) { // check if clock line released
    return -2; // end with failure if not released
  }
  else { // send start condition
    digitalWrite(SDA_PIN, LOW);  // data low
    mydelay(10); // delay
    digitalWrite(SCL_PIN, LOW); // clock low
    mydelay(10); // delay
    return 1; // set state to success
  }
}


// i2c stop condition
void i2cbb_stop(void) {
  digitalWrite(SDA_PIN, LOW); // pull data low
  mydelay(10); // delay
  digitalWrite(SCL_PIN, HIGH); // release clock line
  mydelay(10); // delay
  digitalWrite(SDA_PIN, HIGH); // release data line
  mydelay(40); // delay to make sure a new data transfer doesnt occur too quickly
}


// i2c data send
char i2cbb_send(unsigned char data) {  // clock out data
  unsigned char state = 0; // initialize return state
  unsigned char i;
  for(i = 8 ; i > 0  ; i--) {
    digitalWrite(SDA_PIN, (data & (1 << (i - 1))));
    mydelay(10);
    digitalWrite(SCL_PIN, HIGH);
    mydelay(10);
    digitalWrite(SCL_PIN, LOW);
  }
  // check for ack
  digitalWrite(SDA_PIN, HIGH); // release line
  mydelay(10); // wait a bit
  unsigned char d = 30; // initialize timeout
  while(digitalRead(SDA_PIN) == 1){ // wait for ack
    // timeout in case of nack
    togglePin(LED_PIN);
    d--;
    if (d == 0) {
      state = 2; // set i2c state to nack
      break;
    }
  }
  // clock the ack or nack
  digitalWrite(SCL_PIN, HIGH);
  mydelay(10);
  digitalWrite(SCL_PIN, LOW);
  // make sure line is released
  d = 30;
  while(digitalRead(SDA_PIN) == 0){
    // timeout in case of failure
    togglePin(LED_PIN);
    d--;
    if (d == 0) {
      state = 3; // set i2c state to no line release
      break;
    }
  }
  if (state > 1) { // send stop if failure
    i2cbb_stop();
  }
  else { // set state to success
    state = 1;
  }
  return state;
}

// full i2c protocol for 3 byte transfer
unsigned char i2cbb(unsigned char reg, unsigned char data) {
  if (i2cbb_start() != 1) { // send start condition
    return 2;
  }
  else if (i2cbb_send(ADDR) != 1) { // send address and write bit
    return 3;
  }
  else if (i2cbb_send(reg) != 1) { // send register to write to
    return 4;
  }
  else if (i2cbb_send(data) != 1) { // write data to register
    return 5;
  }
  else {
  i2cbb_stop(); // send stop condition
  return 1;
  }
}

// codec register setup using i2c
// see wm8731 datasheet for other options (starting pg 46)
// each register retries until success
// if communication fails the device will hang
void codec_maple_reg_setup(void) {
  while (i2cbb(0x0c, 0x00) != 1) { // power save registers -> all on
    delay(10);
  }
  while (i2cbb(0x0e, 0x03) != 1) { // digital data format -> 16b spi mode
    delay(10);
  }
  while (i2cbb(0x00, LINVOL) != 1) { // left input configure
    delay(10);
  }
  while (i2cbb(0x02, RINVOL) != 1) { // right input configure
    delay(10);
  }
  while (i2cbb(0x04, LHPVOL) != 1) { // left headphone configure
    delay(10);
  }
  while (i2cbb(0x06, RHPVOL) != 1) { // right headphone configure
    delay(10);
  }
  while (i2cbb(0x0a, ADCHPD) != 1) { // digital pathway configure
    delay(10);
  }
  while (i2cbb(0x08, (SIDEATT << 6)|(SIDETONE << 5)|(DACSEL << 4)|(BYPASS << 3)|(INSEL << 2)|(MUTEMIC << 1)|(MICBOOST << 0)) != 1) { // analog pathway configure
    delay(10);
  }

  #if SAMPLE_RATE == 88
    while (i2cbb(0x10, 0xbc) != 1) { // clock select 88.2kHz
      delay(10);
    }
  #elif SAMPLE_RATE == 44
    while (i2cbb(0x10, 0xa0) != 1) { // clock select 44.1kHz
      delay(10);
    }
  #elif SAMPLE_RATE == 22
    while (i2cbb(0x10, 0xe0) != 1) { // clock select 22.05kHz
      delay(10);
    }
  #elif SAMPLE_RATE == 8
    while (i2cbb(0x10, 0xac) != 1) { // clock select 8.018kHz
      delay(10);
    }
  #elif SAMPLE_RATE == 2
    while (i2cbb(0x10, 0xce) != 1) { // clock select 2.45kHz
      delay(10);
    }
  #endif

  while (i2cbb(0x12, 0x01) != 1) { // device enable
    delay(10);
  }
}


// timer setup for interrupt clock
// uses TIM4 CH1 on pin5 (gpio PB6)
void codec_maple_timer_setup(void) {
  TIMER4_BASE->SMCR = 0x00000057; // set to external clock on CH1
  TIMER4_BASE->CCMR1 = 0x00000001; // more setting to external clock on CH1
  TIMER4_BASE->CCER = 0x00000000; // input mode, and positive polarity

  #if SAMPLE_RATE == 88
    TIMER4_BASE->ARR = 0x0000003f; // set overflow value for CH1
  #elif (SAMPLE_RATE == 44) || (SAMPLE_RATE == 22)
    TIMER4_BASE->ARR = 0x0000007f; // set overflow value for CH1
  #elif SAMPLE_RATE == 8
    TIMER4_BASE->ARR = 0x000002bf; // set overflow value for CH1
  #elif SAMPLE_RATE == 2
    TIMER4_BASE->ARR = 0x0000047f; // set overflow value for CH1
  #else // add your own here if you want
    #error SAMPLE_RATE value not defined
  #endif

  TIMER4_BASE->DIER = 0x00000001; // enable interrupt on update (overflow)
  TIMER4_BASE->PSC = 0x00000000; // make sure prescaler is 0

  // this is a bunch of stuff that is not used
  // but im setting anyways just to be on the safe side
  TIMER4_BASE->CR2 = 0x00000000; // not used
  TIMER4_BASE->CCMR2 = 0x00000000; // not used
  TIMER4_BASE->DMAR = 0x00000000; // dont use DMA
  TIMER4_BASE->DCR = 0x00000000; // dont use DMA
  
  //off to the races
  TIMER4_BASE->CNT = 0x00000000; //clear the counter                         
  TIMER4_BASE->CR1 = 0x00000001; //turn on counter, immediate preload, upcount, etc
}


// spi setup procedure
void codec_maple_spi_setup(void) {
  HardwareSPI spi(1); // use spi1
  spi.begin(SPI_9MHZ, MSBFIRST, 0);
  SPI1_BASE->CR1 = 0x00000b54; // master mode 0, 9MHz, 16b
  SPI1_BASE->CR2 = 0x00000000; // make sure interrupts and DMA are off
  pinMode(10, OUTPUT); // reset ss as output
}

// full initialization procedure
void AudioCodec_init(void) {

  // initialize i2c lines
  digitalWrite(SCL_PIN, HIGH); // release clock line
  digitalWrite(SDA_PIN, HIGH); // release data line
  pinMode(20, OUTPUT_OPEN_DRAIN); // setup clock line
  pinMode(19, OUTPUT_OPEN_DRAIN); // setup data line
  
  // intialize counter input pin
  pinMode(5, INPUT); // set pin5 to input
  
  // initialize spi lines
  digitalWrite(11, LOW); // set mosi low
  digitalWrite(13, LOW); // set sck low
  digitalWrite(10, LOW); // set ss low
  pinMode(13, OUTPUT); // set sck as output
  pinMode(10, OUTPUT); // set ss as output
  pinMode(11, OUTPUT); // set mosi as output
  pinMode(12, INPUT); // set miso as input
  
  // initialize pot lines
  pinMode(15, INPUT_ANALOG); // set mod0 as analog input
  pinMode(16, INPUT_ANALOG); // set mod1 as analog input
  
  delay(10); // wait a bit before setup
  
  // initialize codec
  codec_maple_reg_setup();
  
  // setup spi communication
  codec_maple_spi_setup();
  
  // setup the adcs for the mod pots
  #if ADCS == 0
    // do nothing
  #elif (ADCS == 1) || (ADCS == 2)
    adc_init(ADC1); // enable RCC for ADC1
    ADC1_BASE->CR2 |= 0x00000001; // turn on ADC1
    adc_set_reg_seqlen(ADC1, 1); // single conversion mode
    ADC1_BASE->SQR3 = 10; // set to CH10 (mod0) to begin with 
    ADC1_BASE->SMPR1 = 0x00000009; // set to 7.5 cycle sample time
    ADC1_BASE->CR2 |= 0x00000001; // start conversion
  #endif
  
  #if KILLSWITCH == 1
    // turn off all interrupts
    nvic_irq_disable_all();
  #endif
  
  // setup TIM4 CH1 to trigger an interrupt for data transfers
  TIMER4_BASE->SR = 0x00000000; // clear any pending interrupts
  timer_attach_interrupt(TIMER4, // attach overflow interrupt
                         TIMER_UPDATE_INTERRUPT,
                         AudioCodec_interrupt);
  codec_maple_timer_setup();
}

// codec data transfer function
static inline void AudioCodec_data(int16* _lin, int16* _rin, int16 _lout, int16 _rout) {

  GPIOA_BASE->BSRR = 0x00000010; // set ss high
  SPI1_BASE->DR = _lout; // send out left data
  asm volatile ("nop"); // delay for appropriate timing
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  GPIOA_BASE->BSRR = 0x00100000; // set ss low
  while (!(SPI1_BASE->SR & (1 << SPI_SR_TXE_BIT))) { // wait for data to be sent
  }
  SPI1_BASE->DR = _rout; // send out right data
  /*
   * Let's hope we don't need this shit since we don't do input..  -Saf
   * Try to remember we've fucked with it tho..
   *
  while (!(SPI1_BASE->SR & (1 << SPI_SR_RXNE_BIT))) { // wait for data to arrive
  }
  *_lin = SPI1_BASE->DR; // recieve left data
  while (!(SPI1_BASE->SR & (1 << SPI_SR_RXNE_BIT))) { // wait for data to arrive
  }
  *_rin = SPI1_BASE->DR; // recieve right data
  */
  //*_rin = *_lin = 0;
}


// adc sample routine
// this creates relatively low noise 16b values from adc samples
#if ADCS == 0
  static inline void AudioCodec_ADC() {
    // do nothing
  }
#elif ADCS == 1
  static inline void AudioCodec_ADC(uint16* _mod0value) {
    if (ADC1_BASE->SR & ADC_SR_EOC) { // check if conversion done
      _mod0temp += ADC1_BASE->DR;  // accumulate ADC samples
      if (--_i == 0) { // check if enough samples have been averaged
        // add in hysteresis to remove jitter
        uint16 _mod0temp1 = _mod0temp >> 2;
        if (((_mod0temp1 - *_mod0value) > HYST) || ((*_mod0value - _mod0temp1) > HYST)) {
          *_mod0value = _mod0temp1; // move temp value
        }
        _mod0temp = 0; // reset temp value
        _i = 64; // reset counter
      }
      ADC1_BASE->CR2 |= 0x00000001; // restart conversion
    }
  }
#elif ADCS == 2
  static inline void AudioCodec_ADC(uint16* _mod0value, uint16* _mod1value) {
    if (ADC1_BASE->SR & ADC_SR_EOC) { // check if conversion done
      --_i;
      if (_i >= 64) { // sample CH10 for first 64 samples
        _mod0temp += ADC1_BASE->DR; // accumulate ADC samples
        if (_i == 64) { // check if enough samples have been averaged
          uint16 _mod0temp1 = _mod0temp >> 2;
          if (((_mod0temp1 - *_mod0value) > HYST) || ((*_mod0value - _mod0temp1) > HYST)) {
            *_mod0value = _mod0temp1; // move temp value
          }
          _mod0temp = 0x0000; // reset temp value
          ADC1_BASE->SQR3 = 11; // change to CH11 (mod1)
        }
      }
      else if (_i < 64) { // sample CH11 for next 16 samples
        _mod1temp += ADC1_BASE->DR; // accumulate ADC samples
        if (_i == 0) { // check if enough samples have been averaged
          uint16 _mod1temp1 = _mod1temp >> 2;
          if (((_mod1temp1 - *_mod1value) > HYST) || ((*_mod1value - _mod1temp1) > HYST)) {
            *_mod1value = _mod1temp1; // move temp value
          }
          _mod1temp = 0x0000; // reset temp value
          ADC1_BASE->SQR3 = 10; // change to CH10 (mod0)
          _i = 128; // reset counter
        }
      }
      ADC1_BASE->CR2 |= 0x00000001; // restart conversion
    }
  }
#endif


#endif // end include guard

