#include "SparkFunMPU9250-DMP.h"
#include "screens.h"
//#include "sensor.h"


TextSelection::TextSelection (TFT_eSPI * tft, MPU9250_DMP * imu) : Screen(tft,imu),
    m_alpha("abcdefghijklmnopqrstuvwxyz"),
    m_count(0),
    m_xofs(0)
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

#if 0
    if (INV_SUCCESS != m_imu->dmpBegin(DMP_FEATURE_TAP, 10))
        Serial.println("dmpBegin fails");
    // dmpSetTap parameters, in order, are:
    // x threshold: 1-1600 (0 to disable)
    // y threshold: 1-1600 (0 to disable)
    // z threshold: 1-1600 (0 to disable)
    // (Threshold units are mg/ms)
    // taps: Minimum number of taps needed for interrupt (1-4)
    // tap time: milliseconds between valid taps
    // tap time multi: max milliseconds between multi-taps
    unsigned short xThresh = 0;   // Disable x-axis tap
    unsigned short yThresh = 0;   // Disable y-axis tap
    unsigned short zThresh = 100; // Set z-axis tap thresh to 100 mg/ms
    unsigned char taps = 1;       // Set minimum taps to 1
    unsigned short tapTime = 100; // Set tap time to 100ms
    unsigned short tapMulti = 1000;// Set multi-tap time to 1s
    if(INV_SUCCESS != m_imu->dmpSetTap(xThresh, yThresh, zThresh, taps, tapTime, tapMulti))
    {
        Serial.println("dmpSetTap fails");
    }
#endif
}

void TextSelection::out (void) {}

#if 0
void TextSelection::loop (void) 
{
    char buff[32];
    readMPU9250(*m_imu);
    int y = (int)norm(m_imu->ay, m_imuYRange[0], m_imuYRange[1], -10.0, 10.0);
    sprintf(buff, "y %d", y);
    m_tft->drawString(buff, 0, 0);
    sprintf(buff, "tw %d", m_tft->textWidth("x"));
    m_tft->drawString(buff, 0, 8);

    if (y < -1) m_current--;
    else if (y > 1) m_current++;

    if (m_current < 0) m_current = m_alpha.length() - 1;
    if (m_current >= m_alpha.length()) m_current = 0;

    sprintf(buff, "c %02d", m_current);
    m_tft->drawString(buff, m_tft->width()/2, 0);

    for (int8_t ii = -7; ii < 8; ii++)
    {
        buff[ii+7] = m_alpha[(m_current + m_alpha.length() + ii) % m_alpha.length()];
    }
    buff[15] = 0;

    m_tft->setTextSize(2);
    m_tft->drawCentreString(buff,
        m_tft->width()/2,
        m_tft->height()/2 - m_tft->fontHeight(1)/2, 1);

    m_tft->setTextSize(1);

    uint16_t d = 500-(100*std::min(4, abs(y)));
    sprintf(buff, "d %03d", d);
    delay(d);
}
#endif // 0

#if 0
void TextSelection::loop (void) 
{
    char buff[32];
//    readMPU9250(*m_imu);
    if (!m_imu->dataReady())
    {
//        delay(200);
        return;
    }
    m_imu->update(UPDATE_ACCEL | UPDATE_GYRO | UPDATE_COMPASS);
    int y = (int)norm(m_imu->calcAccel(m_imu->ay), m_imuYRange[0], m_imuYRange[1], -10.0, 10.0);
    m_imu->dmpUpdateFifo();

    // Check for new tap data by polling tapAvailable
    if (m_imu->tapAvailable())
    {
        // If a new tap happened, get the direction and count
        // by reading getTapDir and getTapCount
        unsigned char tapDir = m_imu->getTapDir();
        unsigned char tapCnt = m_imu->getTapCount();
        switch (tapDir)
        {
            case TAP_X_UP:
                m_tft->drawString("Tap X+", 0, 8); 
                break;
            case TAP_X_DOWN:
                m_tft->drawString("Tap X-", 0, 8); 
                break;
            case TAP_Y_UP:
                m_tft->drawString("Tap Y+", 0, 8); 
                break;
            case TAP_Y_DOWN:
                m_tft->drawString("Tap Y-", 0, 8); 
                break;
            case TAP_Z_UP:
                m_tft->drawString("Tap Z+", 0, 8); 
                break;
            case TAP_Z_DOWN:
                m_tft->drawString("Tap Z-", 0, 8); 
                break;
        }
    }
    sprintf(buff, "y %d", y);
    m_tft->drawString(buff, 0, 0);

    for (; y < -1; y++)
    {
        m_xofs--;
        if (m_xofs < -12)
        {
            m_xofs = 0;
            m_current++;
            m_current %= m_alpha.length();
        }
    }
    for (; y > 1; y--)
    {
        m_xofs++;
        if (m_xofs > 0)
        {
            m_xofs = -12;
            m_current += m_alpha.length() - 1;
            m_current %= m_alpha.length();
        }
    }

    sprintf(buff, "xofs %3d", m_xofs);
    m_tft->drawString(buff, m_tft->width()/2, 0);

    sprintf(buff, "curr %3d", m_current);
    m_tft->drawString(buff, m_tft->width()/2, 8);

//     if (m_xofs == -6 || m_xofs == 6)
//     {
//         delay(50);
//     }
    for (int8_t ii = 0; ii < 20; ii++)
    {
        buff[ii] = m_alpha[(m_current + m_alpha.length() + ii) % m_alpha.length()];
    }
    buff[20] = 0;
    m_tft->setTextSize(2);
    m_tft->drawString(buff, m_xofs, m_tft->height()/2 - 8);
    sprintf(buff, "%c", buff[7]);
    m_tft->drawString(buff, m_tft->width()/2, m_tft->height()/2 + 12);
    m_tft->setTextSize(1);
//    uint16_t d = 500-(100*std::min(4, abs(y)));
 //   sprintf(buff, "d %03d", d);
//    delay(d);
}
#endif // 0

void TextSelection::loop (void) 
{
    char buff[32];
//    readMPU9250(*m_imu);
    if (!m_imu->dataReady())
    {
//        delay(200);
        return;
    }
    m_imu->update(UPDATE_ACCEL | UPDATE_GYRO | UPDATE_COMPASS);
    int y = (int)norm(m_imu->calcAccel(m_imu->ay), m_imuYRange[0], m_imuYRange[1], -10.0, 10.0);
    sprintf(buff, "y %d", y);
    m_tft->drawString(buff, 0, 0);

    for (; y < -1; y++)
    {
        m_xofs--;
        if (m_xofs < -12)
        {
            m_xofs = 0;
            m_current++;
            m_current %= m_alpha.length();
        }
    }
    for (; y > 1; y--)
    {
        m_xofs++;
        if (m_xofs > 0)
        {
            m_xofs = -12;
            m_current += m_alpha.length() - 1;
            m_current %= m_alpha.length();
        }
    }

    sprintf(buff, "xofs %3d", m_xofs);
    m_tft->drawString(buff, m_tft->width()/2, 0);

    sprintf(buff, "curr %3d", m_current);
    m_tft->drawString(buff, m_tft->width()/2, 8);

//     if (m_xofs == -6 || m_xofs == 6)
//     {
//         delay(50);
//     }
    for (int8_t ii = 0; ii < 20; ii++)
    {
        buff[ii] = m_alpha[(m_current + m_alpha.length() + ii) % m_alpha.length()];
    }
    buff[20] = 0;
    m_tft->setTextSize(2);
    m_tft->drawString(buff, m_xofs, m_tft->height()/2 - 8);
    sprintf(buff, "%c", buff[7]);
    m_tft->drawString(buff, m_tft->width()/2, m_tft->height()/2 + 12);
    m_tft->setTextSize(1);
//    uint16_t d = 500-(100*std::min(4, abs(y)));
 //   sprintf(buff, "d %03d", d);
//    delay(d);
}
