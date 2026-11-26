#include <Arduino.h>
//#include <TFT_eSPI.h>
#include <SPI.h>
//#include "WiFi.h"
#include <Wire.h>
#include <driver/gpio.h>
#include <math.h>
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

#define _TIMERINTERRUPT_LOGLEVEL_ 0

#define PIN_PWM0 12 // -> input 3/4
#define PIN_PWM1 13 // -> input 1/2
#define FSH(s) reinterpret_cast<const __FlashStringHelper *>(s)

typedef struct
{
    char key;
    const __FlashStringHelper * help;
    void (*fun)();
}Command;

typedef enum
{
    IDLE = 0,
    SWEEP,
    AUDIO_SOURCE,
    FREQUENCY
}Mode;

Command * commandset;
int16_t param;
volatile bool isrKick = false;
uint8_t sine_table[256];
float envelope_table[256];
uint8_t duty_table[256];

constexpr float MODULATION_DEPTH = 0.85f;
constexpr uint32_t AUDIO_RATE = 16000;
constexpr uint8_t PWM_BITS = 8;
constexpr uint32_t CARRIER_FREQ = 40000;

constexpr uint32_t INPUT_RATE = 44100;

Mode mode = IDLE;

float sweep_low = 300.0;
float sweep_high = 3500.0;
uint8_t sweep_index = 0;
//bool sweep_event = false;
hw_timer_t *audioTimer = nullptr;

volatile uint32_t phase = 0;
volatile uint8_t current_duty, prevDuty = 0;
volatile bool audio_tick = false;

volatile uint32_t phase_inc = 0;

constexpr uint16_t A2DP_BUFLEN = 2560;
uint16_t a2dp_incoming_length = 0;
uint16_t a2dp_incoming[A2DP_BUFLEN];
volatile uint32_t sample_pos = 0;
constexpr uint32_t step_size = ((uint64_t)INPUT_RATE << 16) / AUDIO_RATE;

BluetoothA2DPSink a2dp_sink;

void initSineTable()
{
    for (int i = 0; i < 256; i++)
    {
        float angle = (2.0f * PI * i) / 256.0f;
        sine_table[i] = (uint8_t)(127.5f + 127.5f * sinf(angle));
        float audio = ((float)sine_table[i] - 128.0f) / 128.0f;
        envelope_table[i] = sqrt(1.0f + MODULATION_DEPTH * audio);
        duty_table[i] = (uint8_t)(envelope_table[i] * 127.0f);
    }
}

void audio_data_callback(const uint8_t * data, uint32_t length)
{
    printf("callback\r\n");
//    memcpy(a2dp_incoming, data, length);
//    a2dp_incoming_length = length;
//    const int16_t * samples = (const int16_t *)data;
//    size_t count = length / sizeof(int16_t);
//    printf("Count %d\r\n", count);
//    static char vu[] = "*******************************";

//    Serial.printf("%d\r\n", (samples[0] + samples[1])/2);
//    return;
//    for (size_t ii = 0; ii < count; ii =+ 2)
//    {
//        int16_t left = samples[ii];
//        int16_t right = samples[ii + 1];
//        int16_t mono = (left + right) / 2;
//        if (0 == ii)
//        {
//            char myvu[32];
//            strncpy(myvu, vu, mono/32);
//            printf("%s                                   \n", myvu);
//        }
//    }
}

void IRAM_ATTR on_audio_tick()
{
  audio_tick = true;
}

void help();

void cmd_frequency(uint16_t param)
{
    mode = FREQUENCY;
    float audio_freq = (float)param;
    phase_inc = (uint32_t)(audio_freq * 4294967296.0 / AUDIO_RATE);
}

extern Command commands_main[];

const char help_audio_input[] PROGMEM="Audio Input";
const char help_param0[] PROGMEM="param = 0";
const char help_frequency[] PROGMEM="Set Frequency Hz (param)";
const char help_help[] PROGMEM="help";
const char help_sweep[] PROGMEM="Frequency Sweep";
const char help_idle[] PROGMEM="Idle";
//const char help_stop_sweep[] PROGMEM="Stop Frequency Sweep";
const char empty[] PROGMEM="";

Command commands_main[] = {
{'a',FSH(help_audio_input), [](){ mode = AUDIO_SOURCE; }},
{'c',FSH(help_param0), [](){param = 0;}},
{'f',FSH(help_frequency), [](){ cmd_frequency(param);}},
{'h',FSH(help_help), [](){ help();}},
{'i',FSH(help_idle), [](){ mode = IDLE; }},
{'s',FSH(help_sweep), [](){ mode = SWEEP; }},
//{'S',FSH(help_stop_sweep), [](){ mode = IDLE; }},
{'&', FSH(empty), [](){} }
};

// Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
// void espDelay(int ms)
// {
//     esp_sleep_enable_timer_wakeup(ms * 1000);
//     esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
//     esp_light_sleep_start();
// }

const char dashes[] PROGMEM="-------------------------------------------------------\r\n";
void help()
{
    Serial.printf("%s\r\n", FSH(dashes));
    for (Command * iter = commandset; iter->key != '&'; iter++)
    {
        Serial.printf("%c: %s\r\n", iter->key, iter->help);
    }
}

void handleCommand()
{
	uint8_t lastCommand;
    lastCommand = Serial.read();
    for (Command * iter = commandset; iter->key != '&'; iter++)
    {
        if (iter->key == lastCommand)
        {
            iter->fun();
            return;
        }
    }

    if (lastCommand >= '0' & lastCommand <= '9')
    {
        param *= 10;
        param += lastCommand - '0';
    }
}

void setup()
{
    delay(1000);
		initSineTable();
    Serial.begin(115200);

    commandset = commands_main;

    ledcAttach(PIN_PWM0, 40000, PWM_BITS);
    ledcAttach(PIN_PWM1, 40000, PWM_BITS);
    ledcOutputInvert(PIN_PWM1, true);

    audioTimer = timerBegin(1e6);
    timerAttachInterrupt(audioTimer, &on_audio_tick);
    timerAlarm(audioTimer, 1e6/AUDIO_RATE, true, 0);

    a2dp_sink.set_stream_reader(audio_data_callback, false);
    printf("Starting BT\r\n");
    a2dp_sink.start("Ultrasonic Speaker");

//    while(true)
//    {
//      delay(1000);
//      esp_bt_controller_status_t s = esp_bt_controller_get_status();
//      Serial.printf("BT status: %d\n", s);
//    }

}

static uint32_t next_sweep = 0;

void loop()
{
    if (audio_tick)
    {
        if ((FREQUENCY == mode) || (SWEEP == mode))
        {
            phase += phase_inc;
            current_duty = duty_table[phase >> 24];
        }
        if (SWEEP == mode & (millis() > next_sweep))
        {
            sweep_index += 3;
            next_sweep = millis() + 5;
            float new_freq = sweep_low + ((sweep_high - sweep_low) * ((float)sine_table[sweep_index])/256.0f);
            phase_inc = (uint32_t)(new_freq * 4294967296.0 / AUDIO_RATE);
//            phase += phase_inc;
//            current_duty = duty_table[phase >> 24];
//            Serial.printf("current_durty %d\r\n", current_duty);
        }
        else if (AUDIO_SOURCE == mode)
        {
            if (sample_pos < a2dp_incoming_length)
            {
                uint16_t left = a2dp_incoming[sample_pos];
                uint16_t right = a2dp_incoming[sample_pos + 1];
                uint16_t mono = ((left + right) / 2) >> 8;
                current_duty = mono;
                sample_pos +=2; // skip left and right
            }
            else
            {
                a2dp_incoming_length = 0;
            }
        }

        if (current_duty != prevDuty)
        {
            ledcWrite(PIN_PWM0, current_duty);
            ledcWrite(PIN_PWM1, current_duty);
            prevDuty = current_duty;
        }
        audio_tick = false;
    }


    if (Serial.available())
    {
        handleCommand();
    }
}
