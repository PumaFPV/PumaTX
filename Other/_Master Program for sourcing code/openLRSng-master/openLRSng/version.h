#ifndef _VERSION_H_
#define _VERSION_H_

// Version number in single uint16 [8bit major][4bit][4bit]
// a.b.c == 0xaabc
#define OPENLRSNG_VERSION 0x0390
static uint16_t version = OPENLRSNG_VERSION;

#endif
