#include <effect.h>

void
Effect::effectChase()
{
    m_pixels.setPixelColor(m_effectCount, 0, 0, 0);
    m_effectCount++;
    m_effectCount %= m_numPixels;
    m_pixels.setPixelColor(m_effectCount,
        std::get<0>(m_color),
        std::get<1>(m_color),
        std::get<2>(m_color));
    m_pixels.show();
}

void
Effect::effectTrail()
{
    m_pixels.setPixelColor(m_effectCount, 0, 0, 0);
    m_effectCount++;
    m_effectCount %= m_numPixels;
    for (uint8_t ii = 0; ii < 4; ii++)
    {
        m_pixels.setPixelColor((m_effectCount + (ii * (m_numPixels - 1))) % m_numPixels,
            std::get<0>(m_color) >> ii,
            std::get<1>(m_color) >> ii,
            std::get<2>(m_color) >> ii);
    }
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
Effect::loop() {
    if ((millis() - m_lastUpdate) > m_period)
    {
        (this->*m_currentEffect)();
        m_lastUpdate = millis();
    }
}
