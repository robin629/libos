#ifndef OS_PRIVATE_H
#define OS_PRIVATE_H
#include "config.h"

#include <stdio.h>

/* [ERR][src/serial.c:21]: Error message */
#if __OS_ENABLE_LOGGING
#define OS_PRV_LOG(sev, fmt, ...) fprintf(stderr, "[" sev "][" __FILE__ ":%d]: " fmt "\n", __LINE__, ##__VA_ARGS__)
#else
#define OS_PRV_LOG(sev, fmt, ...)
#endif

#define OS_PRV_CRT(fmt, ...) OS_PRV_LOG("CRT", fmt, ##__VA_ARGS__)
#define OS_PRV_ERR(fmt, ...) OS_PRV_LOG("ERR", fmt, ##__VA_ARGS__)
#define OS_PRV_WRN(fmt, ...) OS_PRV_LOG("WRN", fmt, ##__VA_ARGS__)
#define OS_PRV_NTC(fmt, ...) OS_PRV_LOG("NTC", fmt, ##__VA_ARGS__)
#define OS_PRV_DBG(fmt, ...) OS_PRV_LOG("DBG", fmt, ##__VA_ARGS__)
#define OS_PRV_TRC(fmt, ...) OS_PRV_LOG("TRC", fmt, ##__VA_ARGS__)

#define OS_PRV_ABORT(fmt, ...) do { OS_PRV_LOG("ABORT", fmt, ##__VA_ARGS__); exit(1); } while (0)

#endif
