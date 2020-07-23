/*
MIT License

Copyright (c) 2020 Pavel Slama

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef WISOL_H
#define WISOL_H

#include "mbed.h"
#include "mbed-trace/mbed_trace.h"
#ifndef TRACE_GROUP
    #define TRACE_GROUP "WISL"
#endif

#define SIGFOX_ID_LENGTH       4
#define SIGFOX_PAC_LENGTH      8
#define SIGFOX_MAX_DATA_LENGTH 12

class Wisol {
  public:
    Wisol(PinName tx, PinName rx);
    ~Wisol(void);

    typedef enum {
        Reset = 0,
        Sleep,
        DeepSleep
    } power_mode_t;

    /**
     * @brief Initializes library and checks communication with the modem
     *
     * @param debug enable/disable AT commands printout
     * @return success/failure
     */
    bool init(bool debug = false);

    /**
     * @brief Get an ID - 4 bytes
     *
     * @param id
     * @return success/failure
     */
    bool getId(uint8_t *id);

    /**
     * @brief Get a PAC number - 8 bytes
     *
     * @param pac pointer where to copy
     * @return success/failure
     */
    bool getPac(uint8_t *pac);

    /**
     * @brief Set a power mode
     *
     * @see power_mode_t
     *
     * @param mode any value from power_mode_t
     * @return success/failure
     */
    bool setPowerMode(power_mode_t mode);

    /**
     * @brief Software reset
     *
     * @return success/failure
     */
    bool reset();

    /**
     * @brief Send a break condition for wake up after sleep (not deep sleep!)
     *
     */
    void sendBreak();

    /**
     * @brief Send a bit
     *
     * @param bit to send
     * @param downlink point to downlink response should you need
     * @return success/failure
     */
    bool sendBit(bool bit, char *downlink = nullptr);

    /**
     * @brief Send a data buffer
     *
     * @param data pointer to a data buffer
     * @param length length of data
     * @param downlink point to downlink response should you need
     * @return success/failure
     */
    bool sendFrame(const void *data, size_t length, char *downlink = nullptr);

    /**
     * @brief Get a temperature (result in 1/10th °C)
     *
     * @param temperature pointer to where to copy the temperature
     * @return success/failure
     */
    bool getTemperature(int *temperature);

    /**
     * @brief Get a voltage (result in mV)
     *
     * @param current pointer to where to copy current voltage
     * @param last pointer to where to copy voltage during last transmission
     * @return true
     * @return false
     */
    bool getVoltage(int *current, int *last);

    /**
     * @brief Sets transmit repeat
     *
     * @param repeats number or repeats (default: 2)
     * @return success/failure
     */
    bool setTransmitRepeat(uint8_t repeats);

  private:
    BufferedSerial _serial;
    ATCmdParser *_parser;

    // this will force parser to be in RAM instead of HEAP
    uint32_t parser_buffer[sizeof(ATCmdParser) / sizeof(uint32_t)];
    const char hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
};

#endif  // WISOL_H
