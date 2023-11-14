#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>

struct dataStream
{
    uint8_t client_id;
    uint32_t msgId;
    char payload[512];
    uint16_t payload_length;
};

#endif