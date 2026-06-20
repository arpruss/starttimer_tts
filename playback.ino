static volatile const uint8_t* sample = NULL;
static unsigned sampleLength;
static volatile unsigned samplePos;

static unsigned sampleRate = 8000;
static unsigned pin1 = PA3;
static unsigned pin2 = PA2;
static unsigned initialized = 0;
static const uint16_t* boostFunction = NULL;

static unsigned channel;
static unsigned channel2;
static timer_dev* dev;
static HardwareTimer interruptTimer(3);
static uint32_t boost = 1;

static void handler() {
  if (sample == NULL)
    return;
  if (samplePos >= sampleLength) {
    sample = NULL;
    interruptTimer.pause();
    return;
  }
  
  int32_t v = sample[samplePos];
  if (boostFunction != NULL)
    v = boostFunction[v];
  
  timer_set_compare(dev, channel, v);
  timer_set_compare(dev, channel2, v);
  samplePos++;
}

static void setBoostFunction(const uint16_t* bf) {
  boostFunction = bf;
}

static void pwm_mode(timer_dev *dev, uint8 channel, bool reversed) {
    timer_disable_irq(dev, channel);
    timer_set_mode(dev,channel, TIMER_PWM );    
    timer_oc_set_mode(dev, channel, reversed?TIMER_OC_MODE_PWM_2:TIMER_OC_MODE_PWM_1, TIMER_OC_PE);
    timer_cc_enable(dev, channel);
}

void initializePlayback() {
  interruptTimer.pause();

  dev = PIN_MAP[pin1].timer_device;
  channel = PIN_MAP[pin1].timer_channel;
  channel2 = PIN_MAP[pin2].timer_channel;

  gpio_set_mode(PIN_MAP[pin1].gpio_device, PIN_MAP[pin1].gpio_bit, GPIO_AF_OUTPUT_PP);
  gpio_set_mode(PIN_MAP[pin2].gpio_device, PIN_MAP[pin2].gpio_bit, GPIO_AF_OUTPUT_PP);
  pwm_mode(dev, channel, true);
  pwm_mode(dev, channel2, false);
  timer_set_prescaler(dev, 0);
  timer_set_reload(dev, boostFunction ? 1026 : 256);

  interruptTimer.setPeriod(1000000/sampleRate); 
  interruptTimer.attachInterrupt(TIMER_CH1, handler);
  interruptTimer.refresh();
}

void terminatePlayback() {
  interruptTimer.pause();
  pinMode(pin1, OUTPUT);
  digitalWrite(pin1, 0);
  pinMode(pin2, OUTPUT);
  digitalWrite(pin2, 0);
}

void playSample(const uint8_t* s, unsigned n, bool wait) {
  interruptTimer.pause();
  sample = s;
  samplePos = 0;
  sampleLength = n;
  interruptTimer.resume();
  while (wait && sample != NULL) ;
}

void stopSample() {
  timer_set_compare(dev, channel, 0x80);
  timer_set_compare(dev, channel2, 0x80);
  interruptTimer.pause();
}

void resumeSample() {
  interruptTimer.resume();
}
