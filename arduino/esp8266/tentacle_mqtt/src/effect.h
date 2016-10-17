#include <map>
#include <tuple>
#include <Adafruit_NeoPixel.h>

class Effect
{
protected:
    Adafruit_NeoPixel & m_pixels;
    void effectChase();
    void effectNone() {};
    void effectOff();
    void effectTrail();
    typedef void (Effect::*memfun)();
    std::map<std::string, memfun> m_effects =
    {
        { "off", &Effect::effectOff },
        { "none", &Effect::effectNone },
        { "chase", &Effect::effectChase },
        { "trail", &Effect::effectTrail }
    };
    memfun m_currentEffect = &Effect::effectNone;
    std::tuple<uint8_t, uint8_t, uint8_t> m_color { 0, 0, 0 };
    uint16_t m_period;
    uint32_t m_lastUpdate;
    uint16_t m_effectCount;
    uint8_t m_numPixels;

public:
    Effect(Adafruit_NeoPixel & pixels, int period)
    : m_pixels(pixels)
    , m_period(period)
    , m_numPixels(pixels.numPixels())
    {
        m_pixels.begin();
        m_pixels.show();
        m_lastUpdate = millis();
    }
    //Effect(const Effect &) = delete;
    void select(const char * type);
    void loop();
    void color(uint8_t red, uint8_t green, uint8_t blue);
};
