#ifndef MY_FAST_PWM
#define MY_FAST_PWM

#include <ESP32_FastPWM.h>

class MyFastPWM(public ESP32_FAST_PWM)
{
    public:
    MyFastPWM(const uint8_t& pin, const float& frequency, const float& dutycycle,
                   const uint8_t& channel = 0, const float& resolution = 8, const uint8_t & invert = 0) :
        ESP32_FAST_PWM(pin, frequency,dutycycle, channel, resolution)
    {
      if (resolution <= SOC_LEDC_TIMER_BIT_WIDE_NUM)
        _resolution = resolution;
      else
      {
        _resolution = SOC_LEDC_TIMER_BIT_WIDE_NUM;
        PWM_LOGERROR1(F("ESP32_FastPWM: _resolution too big, reset to"), SOC_LEDC_TIMER_BIT_WIDE_NUM);
      }

      if (channel <= _LEDC_CHANNELS)
        _channel    = (ledc_channel_t) channel;
      else
      {
        _channel    = (ledc_channel_t) _LEDC_CHANNELS;
        PWM_LOGERROR1(F("ESP32_FastPWM: _channel too big, reset to"), _LEDC_CHANNELS);
      }

      if (channels_resolution[_channel] == 0)
      {
        PWM_LOGDEBUG1(F("ESP32_FastPWM: new _channel ="), _channel);
        channels_resolution[_channel] = _resolution;
      }
      else if (channels_resolution[_channel] != _resolution)
      {
        PWM_LOGDEBUG3(F("ESP32_FastPWM: Change resolution of used _channel ="), _channel, F("to"), _resolution);
        channels_resolution[_channel] = _resolution;
      }

      _group      = (ledc_mode_t) (_channel / 8);
      _timer      = (_channel / 2) % 4;

      _pin        = pin;
      _frequency  = frequency;

      _dutycycle  = round(map(dutycycle, 0, 100.0f, 0, MAX_COUNT_16BIT));

      PWM_LOGDEBUG5(F("ESP32_FastPWM: SOC_LEDC_CHANNEL_NUM ="), SOC_LEDC_CHANNEL_NUM, F(", LEDC_CHANNELS ="), _LEDC_CHANNELS,
                    F(", LEDC_MAX_BIT_WIDTH ="), SOC_LEDC_TIMER_BIT_WIDE_NUM);

      PWM_LOGDEBUG1(F("ESP32_FastPWM: _dutycycle ="), _dutycycle);

      pinMode(pin, OUTPUT);

      ledcSetup(_channel, frequency, _resolution);
      ledcAttachPin(pin, _channel, invert);
    }
}


#endif // MY_FAST_PWM
