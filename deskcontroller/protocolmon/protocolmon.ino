struct Bits {
    uint8_t pData : 1;
    uint8_t pClock : 1;
};

// Interframe 0 for ms
#define INTERFRAME 580
#define SAMPLES 100
Bits bits = {0, 0};
uint32_t data[SAMPLES];
uint8_t tindex;
uint32_t transAt;

#define PIN_CLOCK_DESK 16
#define PIN_DATA_DESK  14
#define PIN_CLOCK_CTLR 12
#define PIN_DATA_CTLR  13

void setup() {
    Serial.begin(115200);
    Serial.println("Setup");
    data[0] = 0;
    transAt = micros();
    tindex = 0;
//    pinMode(PIN_CLOCK_CTLR, OUTPUT);
//    pinMode(PIN_DATA_CTLR, OUTPUT);
}

void loopclock()
{
    if (digitalRead(PIN_CLOCK_DESK) == bits.pClock) { return; }
    bits.pClock ^= 1;
//    digitalWrite(PIN_CLOCK_CTLR, bits.pClock);
    data[tindex++] = micros() - transAt;
    transAt = micros();
    if (tindex == SAMPLES)
    {
        for (tindex = 0; tindex < SAMPLES; tindex++)
        {
            Serial.println(data[tindex]);
        }
        tindex = 0;
    }
}


void loopdata()
{
    if (digitalRead(PIN_DATA_DESK) == bits.pData) { return; }
    bits.pData ^= 1;
//    digitalWrite(PIN_DATA_CTLR, bits.pData);

    // If this is a 0->1 transition and pulseLen > 590ms, start a new block
    if (!bits.pData && ((micros() - transAt) > INTERFRAME))
    {
        data[++tindex] = 0;
        if (tindex == SAMPLES)
        {
            for (tindex = 1; tindex < SAMPLES; tindex++)
            {
                Serial.println(data[tindex], 2);
            }
            tindex = 0;
        }
    }

//    transAt = micros();
//    data[tindex] <<= 1;
//    data[tindex] |=(bits.pData);
//    bits.pData ^= 1;
}

void loop()
{
//    loopclock();
    loopdata();
}


// void loopmeasure() {
// 
//     if ((digitalRead(PIN_DATA_DESK) != bits.pData) && (tindex < SAMPLES))
//     {
//         state16[tindex] = bits.pData;
//         bits.pData ^= 1;
//         timer16[tindex++] = micros();
// //        Serial.println(bits.pData);
//     }
//     if (tindex == SAMPLES)
//     {
//         for (tindex = 1; tindex < SAMPLES; tindex++)
//         {
//             Serial.print(state16[tindex]);
//             Serial.print(" ");
//             Serial.println(timer16[tindex] - timer16[tindex-1]);
//         }
//         tindex = 0;
//     }
// 
// //    Serial.print("GPIO16 ");
// //    Serial.println(digitalRead(16));
// //    Serial.print("GPIO14 ");
// //    Serial.println(digitalRead(14));
// }
