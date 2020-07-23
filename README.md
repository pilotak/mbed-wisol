# Mbed Wisol library

[![Framework Badge mbed](https://img.shields.io/badge/framework-mbed-008fbe.svg)](https://os.mbed.com/)

Mbed library for Wisol WSSFM10 SigFox modem

# Example
```cpp
#include "mbed.h"
#include "Wisol.h"

Wisol wisol(PC_4, PC_5);

int main() {
    uint8_t id[SIGFOX_ID_LENGTH];
    uint8_t pac[SIGFOX_PAC_LENGTH];
    int temp;
    int current_mv;
    int last_mv;

    if (!wisol.init()) {
        return 0;
    }

    if (wisol.getId(id)) {
        printf("ID: ");

        for (auto i = 0; i < SIGFOX_ID_LENGTH; i++) {
            printf("%02X", id[i]);
        }

        printf("\n");
    }

    if (wisol.getPac(pac)) {
        printf("PAC: ");

        for (auto i = 0; i < SIGFOX_PAC_LENGTH; i++) {
            printf("%02X", pac[i]);
        }

        printf("\n");
    }

    if (wisol.getTemperature(&temp)) {
        printf("Temperature: %d *C(1/10th)\n", temp);
    }

    if (wisol.getVoltage(&current_mv, &last_mv)) {
        printf("Current voltage: %dmV\n", current_mv);
        printf("Voltage during last transmission: %dmV\n", last_mv);
    }

    return 0;
}
```

## Debug example
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

    uint8_t id[SIGFOX_ID_LENGTH];
    uint8_t pac[SIGFOX_PAC_LENGTH];
    int temp;
    int current_mv;
    int last_mv;

    if (!wisol.init(true)) {
        return 0;
    }

    if (wisol.getId(id)) {
        printf("ID: ");

        for (auto i = 0; i < SIGFOX_ID_LENGTH; i++) {
            printf("%02X", id[i]);
        }

        printf("\n");
    }

    if (wisol.getPac(pac)) {
        printf("PAC: ");

        for (auto i = 0; i < SIGFOX_PAC_LENGTH; i++) {
            printf("%02X", pac[i]);
        }

        printf("\n");
    }

    if (wisol.getTemperature(&temp)) {
        printf("Temperature: %d *C(1/10th)\n", temp);
    }

    if (wisol.getVoltage(&current_mv, &last_mv)) {
        printf("Current voltage: %dmV\n", current_mv);
        printf("Voltage during last transmission: %dmV\n", last_mv);
    }

    return 0;
}
```
