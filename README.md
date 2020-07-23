# Mbed Wisol library

[![Framework Badge mbed](https://img.shields.io/badge/framework-mbed-008fbe.svg)](https://os.mbed.com/)

Mbed library for Wisol WSSFM10 SigFox modem

# Example
```cpp
#include "mbed.h"
#include "Wisol.h"

Wisol wisol(PC_4, PC_5);

int main() {
    if (!wisol.init()) {
        printf("Could not init");
        return 0;
    }

    uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C};

    if (wisol.sendFrame(data, sizeof(data))) {
        printf("Message sent!\n");

    } else {
        printf("Sending message failed\n");
    }

    return 0;
}
```

## Detailed example
`mbed_app.json`
```json
{
  "config": {
    "trace-level": {
      "help": "Options are TRACE_LEVEL_ERROR,TRACE_LEVEL_WARN,TRACE_LEVEL_INFO,TRACE_LEVEL_DEBUG",
      "macro_name": "MBED_TRACE_MAX_LEVEL",
      "value": "TRACE_LEVEL_DEBUG"
    }
  },
  "target_overrides": {
    "*": {
      "mbed-trace.enable": true,
      "target.printf_lib": "std"
    }
  }
}
```
`main.cpp`
```cpp
#include "mbed.h"
#include "Wisol.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#include "mbed-trace/mbed_trace.h"
static Mutex trace_mutex;

static void trace_wait() {
    trace_mutex.lock();
}

static void trace_release() {
    trace_mutex.unlock();
}

void trace_init() {
    mbed_trace_init();
    mbed_trace_mutex_wait_function_set(trace_wait);
    mbed_trace_mutex_release_function_set(trace_release);
}
#endif

Wisol wisol(PC_4, PC_5);

int main() {
#if MBED_CONF_MBED_TRACE_ENABLE
    trace_init();
#endif

    uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C};
    uint8_t id[SIGFOX_ID_LENGTH];
    uint8_t pac[SIGFOX_PAC_LENGTH];

    if (!wisol.init(true)) {
        printf("Could not init");
        return 0;
    }

    wisol.getId(id);
    wisol.getPac(pac);
    wisol.setTransmitRepeat(2);
    wisol.sendFrame(data, sizeof(data));
    wisol.getTemperature(nullptr);
    wisol.getVoltage(nullptr, nullptr);

    if (wisol.setPowerMode(Wisol::Sleep)) {
        ThisThread::sleep_for(5s);
        wisol.sendBreak();

        while (!wisol.init(true)) {
            ThisThread::sleep_for(250ms);
        }
    }

    wisol.reset();

    return 0;
}
```
