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

#include "Wisol.h"

Wisol::Wisol(PinName tx, PinName rx):
    _serial(tx, rx, 9600) {
}

Wisol::~Wisol() {
    if (_parser == reinterpret_cast<ATCmdParser *>(parser_buffer)) {
        _parser->~ATCmdParser();
    }
}

bool Wisol::init(bool debug) {
    tr_debug("Init");

    if (_parser == reinterpret_cast<ATCmdParser *>(parser_buffer)) {
        _parser->~ATCmdParser();
    }

    _parser = new (parser_buffer) ATCmdParser(&_serial, "\r\n", 256, MBED_CONF_WISOL_TIMEOUT, debug);
    _parser->send("AT");

    bool res = _parser->recv("OK");

    if (res) {
        tr_info("Alive");

    } else {
        tr_error("No response");
    }

    return res;
}

bool Wisol::getPac(uint8_t *pac) {
    uint16_t data[SIGFOX_PAC_LENGTH] = {0};
    tr_debug("Getting PAC");
    _parser->send("AT$I=11");

    bool ok = _parser->recv("%02hX%02hX%02hX%02hX%02hX%02hX%02hX%02hX\n",
                            &data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6], &data[7]);

    if (!ok) {
        tr_error("Couldn't parse string");
        return false;
    }

    for (auto i = 0; i < SIGFOX_PAC_LENGTH; i++) {
        pac[i] = (uint8_t)data[i];
    }

    tr_info("PAC: %s", tr_array(pac, SIGFOX_PAC_LENGTH));

    return true;
}

bool Wisol::getId(uint8_t *id) {
    uint16_t data[SIGFOX_ID_LENGTH] = {0};
    tr_debug("Getting ID");
    _parser->send("AT$I=10");

    bool ok = _parser->recv("%02hX%02hX%02hX%02hX\n", &data[0], &data[1], &data[2], &data[3]);

    if (!ok) {
        tr_error("Couldn't parse string");
        return false;
    }

    for (auto i = 0; i < SIGFOX_ID_LENGTH; i++) {
        id[i] = (uint8_t)data[i];
    }

    tr_info("ID: %s", tr_array(id, SIGFOX_ID_LENGTH));

    return true;
}

bool Wisol::setPowerMode(power_mode_t mode) {
    tr_debug("Setting power mode: %u", mode);

    return _parser->recv("OK");
}

bool Wisol::reset() {
    return setPowerMode(Reset);
}

void Wisol::sendBreak() {
    // pseudo break, because _serial.send_break() is private
    _parser->putc(0xFF);
}

bool Wisol::sendBit(bool bit, bool downlink) {
    tr_debug("Sending bit: %u%s", bit, downlink ? ", expecting a downlink" : "");
    _parser->send("AT$SB=%u,%u", bit, downlink);

    return _parser->recv("OK");
}

bool Wisol::sendFrame(const void *data, size_t length, bool downlink) {
    if (length > SIGFOX_MAX_DATA_LENGTH) {
        return false;
    }

    auto *buffer = reinterpret_cast<const char *>(data);
    tr_debug("Sending frame[%u]: %s %s", length, tr_array(reinterpret_cast<const uint8_t *>(data), length),
             downlink ? ", expecting a downlink" : "");

    _parser->printf("AT$SF=");
    _parser->write(buffer, length);
    _parser->printf(",%u\r\n", downlink);

    // set longer timeout
    _parser->set_timeout(MBED_CONF_WISOL_TIMEOUT << 2);

    bool ok = _parser->recv("OK");

    // restore timeout
    _parser->set_timeout(MBED_CONF_WISOL_TIMEOUT);

    return ok;
}

bool Wisol::setTransmitRepeat(uint8_t repeats) {
    tr_debug("Setting transmit repeat: %u", repeats);
    _parser->send("AT$TR=%u", repeats);

    if (!_parser->recv("OK")) {
        return false;
    }

    // get transmit repeat to compare
    _parser->send("AT$TR?");
    uint16_t data;
    bool ok = _parser->recv("%hu", &data);

    if (!ok || data != repeats) {
        return false;
    }

    return true;
}

bool Wisol::getTemperature(int *temperature) {
    int temp;
    _parser->send("AT$T?");

    bool ok = _parser->recv("%d\n", &temp);

    if (!ok) {
        return false;
    }

    tr_info("Temperature: %d", temp);

    if (temperature) {
        *temperature = temp;
    }

    return true;
}

bool Wisol::getVoltage(int *current, int *last) {
    int voltage[2];
    _parser->send("AT$V?");

    // first response is current voltage
    bool ok = _parser->recv("%d\n", &voltage[0]);

    if (!ok) {
        return false;
    }

    // second response is during last transmission
    ok = _parser->recv("%d\n", &voltage[1]);

    if (!ok) {
        return false;
    }

    tr_info("Voltage: %d, %d", voltage[0], voltage[1]);

    if (current) {
        *current = voltage[0];
    }

    if (last) {
        *last = voltage[1];
    }

    return true;
}
