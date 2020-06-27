#include "screens.h"
#include "sensor.h"

//extern MPU9250 IMU;

TextSelection::TextSelection (TFT_eSPI * tft, MPU9250 * imu) : Screen(tft,imu),
    m_alpha("abcdefghijklmnopqrstuvwxyz")
{}

void TextSelection::in (void)
{
    m_tft->fillScreen(TFT_WHITE);
    m_tft->setTextColor(TFT_BLACK, TFT_WHITE);
    uint8_t startX = m_tft->width()/2;
    uint8_t startY = m_tft->height()/2 - (2 * m_tft->fontHeight(1)/2) -2;
    uint8_t tH = 10;
    uint8_t tW = 10;
    m_tft->fillTriangle(startX, startY, startX - tW/2, startY - tH,
        startX + tW/2, startY - tH, TFT_RED);
}

void TextSelection::out (void) {}

void TextSelection::loop (void) 
{
    char buff[128];
    readMPU9250(*m_imu);
    int y = (int)norm(m_imu->ay, m_imuYRange[0], m_imuYRange[1], -10.0, 10.0);
    sprintf(buff, "y %d", y);
    m_tft->drawString(buff, 0, 0);
    if (y < -1) m_current--;
    else if (y > 1) m_current++;

    if (m_current < 0) m_current = m_alpha.length() - 1;
    if (m_current >= m_alpha.length()) m_current = 0;

    sprintf(buff, "c %02d", m_current);
    m_tft->drawString(buff, m_tft->width()/2, 0);

    for (int8_t ii = -4; ii < 5; ii++)
    {
        buff[ii+4] = m_alpha[(m_current + m_alpha.length() + ii) % m_alpha.length()];
    }
    buff[9] = 0;

    m_tft->setTextSize(2);
    m_tft->drawCentreString(buff,
        m_tft->width()/2,
        m_tft->height()/2 - m_tft->fontHeight(1)/2, 1);
    m_tft->setTextSize(1);

    uint16_t d = 500-(100*std::min(4, abs(y)));
    sprintf(buff, "d %03d", d);
    delay(d);
}
