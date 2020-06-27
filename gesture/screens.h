#ifndef _SCREENS_H
#define _SCREENS_H

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include "sensor.h"

class Screen
{
    protected:
        TFT_eSPI * m_tft;
        MPU9250 * m_imu;
    public:
        Screen (TFT_eSPI * tft, MPU9250 * imu) : m_tft(tft), m_imu(imu) {}
        virtual void in (void) = 0;
        virtual void out (void)  = 0;
        virtual void loop (void) = 0;
};


class IMU
{
    protected:
        float m_imuXRange[2];
        float m_imuYRange[2];
        float m_imuZRange[2];
        float norm(float i,float in,float ix,float on,float ox)
        {
            return ((i-in)/(ix-in))*(ox-on)+on;
        }
    public:
        IMU() : m_imuXRange({-0.4314, 1.6202}),
        m_imuYRange({-1.3550, 0.7074}),
        m_imuZRange({-1.9759, 0.5992}) {}
};

class FreeFontsDemo : public Screen
{
    protected:
        uint8_t m_count;
        const GFXfont ** m_freeFonts;
    public:
        FreeFontsDemo (TFT_eSPI * tft, MPU9250 * imu, const GFXfont ** freeFonts) : Screen(tft,imu), m_freeFonts(freeFonts) {}
        void in (void)
        {
            m_count = 0;
            m_tft->fillScreen(TFT_BLACK);
            m_tft->setTextColor(TFT_WHITE);
            char buff[128];
            sprintf(buff, "sizeof m_FreeFonts %d sizeof font0 %d", sizeof(*m_freeFonts), sizeof(**m_freeFonts));
            Serial.println(buff);
        }
        void out (void)
        {
            m_tft->setFreeFont(NULL);
        }
        void loop (void)
        {
            m_tft->setFreeFont(m_freeFonts[m_count]);
            m_tft->fillScreen(TFT_BLACK);
            m_tft->drawString("Sans", 10, 10, 1);
            m_count = ++m_count % 4;
            delay(500);
        }
};

class IMUStats : public Screen
{
    protected:
        float m_imuAXMin;
        float m_imuAXMax;
        float m_imuAYMin;
        float m_imuAYMax;
        float m_imuAZMin;
        float m_imuAZMax;

    public:
        IMUStats (TFT_eSPI * tft, MPU9250 * imu) : Screen(tft,imu),
        m_imuAXMin(0.0),
        m_imuAXMax(0.0),
        m_imuAYMin(0.0),
        m_imuAYMax(0.0),
        m_imuAZMin(0.0),
        m_imuAZMax(0.0)
        {}
        void in (void)
        {
            m_tft->setTextColor(TFT_GREEN, TFT_BLACK);
        }
        void out (void) {};
        void loop (void)
        {
            char buff[128];
            readMPU9250(*m_imu);
            m_tft->fillScreen(TFT_BLACK);
            m_tft->drawString(buff, 0, 16);

            m_imuAXMin = std::min(m_imuAXMin, m_imu->ax); 
            m_imuAXMax = std::max(m_imuAXMax, m_imu->ax); 
 
            m_imuAYMin = std::min(m_imuAYMin, m_imu->ay); 
            m_imuAYMax = std::max(m_imuAYMax, m_imu->ay); 
 
            m_imuAZMin = std::min(m_imuAZMin, m_imu->az); 
            m_imuAZMax = std::max(m_imuAZMax, m_imu->az); 
 
            m_tft->fillScreen(TFT_BLACK);
            snprintf(buff, sizeof(buff), "X %.4f  %.4f  %.4f", m_imuAXMin, m_imu->ax, m_imuAXMax);
            m_tft->drawString(buff, 0, 16);
            snprintf(buff, sizeof(buff), "Y %.4f  %.4f  %.4f", m_imuAYMin, m_imu->ay, m_imuAYMax);
            m_tft->drawString(buff, 0, 32);
            snprintf(buff, sizeof(buff), "Z %.4f  %.4f  %.4f", m_imuAZMin, m_imu->az, m_imuAZMax);
            m_tft->drawString(buff, 0, 48);
            delay(200);
        }
};

class NormIMU : public Screen, public IMU
{
    public:
        NormIMU (TFT_eSPI * tft, MPU9250 * imu) : Screen(tft,imu)
        {}

        void in (void) 
        {
            m_tft->setFreeFont(NULL);
            m_tft->fillScreen(TFT_BLUE);
            m_tft->setTextColor(TFT_YELLOW, TFT_BLUE); // Note: the new fonts do not draw the background colour
            m_tft->drawString("Scaled",0, 0);
        }
        void out (void) {}
        void loop (void)
        {
            char buff[128];
            readMPU9250(*m_imu);
            float x = norm(m_imu->ax, m_imuXRange[0], m_imuXRange[1], -10.0, 10.0);
            snprintf(buff, sizeof(buff), "X %.4f", x);
            m_tft->fillScreen(TFT_BLUE);
            m_tft->drawString(buff, 0, 16);
            float y = norm(m_imu->ay, m_imuYRange[0], m_imuYRange[1], -10.0, 10.0);
            snprintf(buff, sizeof(buff), "Y %.4f", y);
            m_tft->drawString(buff, 0, 24);
            delay(200);
        }
};

class Sketch : public Screen, public IMU
{
    protected:
        int16_t m_dotX;
        int16_t m_dotY;
    public:
        Sketch (TFT_eSPI * tft, MPU9250 * imu) : Screen(tft,imu)
        {}
        void in (void)
        {
            m_tft->setFreeFont(NULL);
            m_tft->fillScreen(TFT_RED);
            m_dotY = m_tft->width() / 2;
            m_dotX = m_tft->height() / 2;
        }
        void out (void) {}
        void loop (void)
        {
            readMPU9250(*m_imu);
            float x = norm(m_imu->ax, m_imuXRange[0], m_imuXRange[1], -10.0, 10.0);
            float y = norm(m_imu->ay, m_imuYRange[0], m_imuYRange[1], -10.0, 10.0);
            m_dotX -= (int)x;
            m_dotY += (int)y;
            m_dotY = std::min(m_dotY, m_tft->width());
            m_dotY = std::max(m_dotY, (int16_t)0);

            m_dotX = std::min(m_dotX, m_tft->height());
            m_dotX = std::max(m_dotX, (int16_t)0);
            m_tft->drawPixel(m_dotY, m_dotX, TFT_WHITE);
            delay(20);
        }
};

class TextSelection : public Screen, public IMU
{
    protected:
        uint8_t m_current;
        const String m_alpha;
    public:
        TextSelection (TFT_eSPI * tft, MPU9250 * imu); 
        void in (void);
        void out (void); 
        void loop (void); 
};
#endif // _SCREENS_H
