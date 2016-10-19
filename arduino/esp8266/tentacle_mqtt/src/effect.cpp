#include <effect.h>

#define LED_HEARTBEAT 2


uint8_t gammaTable[16] = {
    0, 1, 2, 6, 12, 20, 31, 44, 60, 79, 100, 125, 153, 183, 218, 255
};

void
Effect::effectChase()
{
    m_pixels.setPixelColor(m_effectCount,
        std::get<0>(m_background),
        std::get<1>(m_background),
        std::get<2>(m_background));

    //m_pixels.show();
    m_effectCount++;
    m_effectCount %= m_numPixels;

    m_pixels.setPixelColor(m_effectCount,
        std::get<0>(m_color),
        std::get<1>(m_color),
        std::get<2>(m_color));

    m_pixels.show();
//    delay(50);
}

void
Effect::effectFlash()
{
    if (m_effectCount % 2)
    {
        setAll(m_color);
    }
    else
    {
        setAll(m_background);
    }
    m_effectCount++;
}

void
Effect::effectAll()
{
    setAll(m_color);
}

void
Effect::effectHeartbeat()
{
    int duty = int(sin(radians(m_effectCount)) * 15);
    if (duty > 0)
    {
        analogWrite(LED_HEARTBEAT, 1023 - gammaTable[duty] * 4);
        uint16_t dimmer(255 / gammaTable[duty]);
        RGBColor color(
            std::get<0>(m_color) / dimmer,
            std::get<1>(m_color) / dimmer,
            std::get<2>(m_color) / dimmer);
        setAll(color);
    }
    m_effectCount += 10;
    m_effectCount %= 360;
}

void
Effect::setAll(RGBColor & color)
{
    for (uint8_t ii = 0; ii < m_pixels.numPixels(); ii++)
    {
        m_pixels.setPixelColor(ii,
            std::get<0>(color),
            std::get<1>(color),
            std::get<2>(color));
    }
    m_pixels.show();
}

void
Effect::effectTrail()
{
    uint8_t ii;
    for (ii = 0; ii < 4; ii++)
    {
        m_pixels.setPixelColor((m_effectCount + (ii * (m_numPixels - 1))) % m_numPixels,
            std::get<0>(m_color) >> ii,
            std::get<1>(m_color) >> ii,
            std::get<2>(m_color) >> ii);
    }
    m_pixels.setPixelColor((m_effectCount + (ii * (m_numPixels - 1))) % m_numPixels,
        std::get<0>(m_background),
        std::get<1>(m_background),
        std::get<2>(m_background));

    m_effectCount++;
    m_effectCount %= m_numPixels;
    m_pixels.show();
}

void
Effect::effectOff()
{
    for (int ii = 0; ii < m_pixels.numPixels(); ii ++)
    {
        m_pixels.setPixelColor(ii, 0, 0, 0);
    }
    m_pixels.show();
}

void
Effect::select(const char * type)
{
    if (m_effects.find(type) != m_effects.end())
    {
        m_currentEffect = m_effects[type];
        m_effectCount = 0;
    }
}

void
Effect::color(uint8_t red, uint8_t green, uint8_t blue)
{
    m_color = std::make_tuple(red, green, blue);
}

void
Effect::background(uint8_t red, uint8_t green, uint8_t blue)
{
    m_background = std::make_tuple(red, green, blue);
}

void
Effect::loop() {
    if ((millis() - m_lastUpdate) > m_period)
    {
        (this->*m_currentEffect)();
        m_lastUpdate = millis();
    }
}
