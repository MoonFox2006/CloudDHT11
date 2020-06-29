#ifndef __DEBUG_H
#define __DEBUG_H

#include <pgmspace.h>
#include "Customization.h"

//#define DEBUG_LOG_LEVEL 0
/***
 * DEBUG_LOG_LEVEL
 * 0 - none
 * 1 - errors
 * 2 - errors and warnings
 * 3 - errors, warnings and informations
 * 4 - errors, warnings, informations and verbose
 * 5 - errors, warnings, informations, verbose and debug
 */

#ifndef DEBUG_LOG_LEVEL
#ifdef DEBUG
#define DEBUG_LOG_LEVEL 5
#else
#define DEBUG_LOG_LEVEL 1
#endif
#endif

#ifdef USE_SERIAL
void debug_log(char type, const char *fmt, ...);

#define DEBUG_LOG(fmt, ...) debug_log('\0', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#if DEBUG_LOG_LEVEL >= 5
#define DEBUG_LOGE(fmt, ...) debug_log('E', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGW(fmt, ...) debug_log('W', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGI(fmt, ...) debug_log('I', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGV(fmt, ...) debug_log('V', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGD(fmt, ...) debug_log('D', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#elif DEBUG_LOG_LEVEL == 4
#define DEBUG_LOGE(fmt, ...) debug_log('E', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGW(fmt, ...) debug_log('W', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGI(fmt, ...) debug_log('I', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGV(fmt, ...) debug_log('V', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGD(...)
#elif DEBUG_LOG_LEVEL == 3
#define DEBUG_LOGE(fmt, ...) debug_log('E', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGW(fmt, ...) debug_log('W', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGI(fmt, ...) debug_log('I', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGV(...)
#define DEBUG_LOGD(...)
#elif DEBUG_LOG_LEVEL == 2
#define DEBUG_LOGE(fmt, ...) debug_log('E', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGW(fmt, ...) debug_log('W', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGI(...)
#define DEBUG_LOGV(...)
#define DEBUG_LOGD(...)
#elif DEBUG_LOG_LEVEL == 1
#define DEBUG_LOGE(fmt, ...) debug_log('E', (PGM_P)PSTR(fmt), ## __VA_ARGS__)
#define DEBUG_LOGW(...)
#define DEBUG_LOGI(...)
#define DEBUG_LOGV(...)
#define DEBUG_LOGD(...)
#else
#define DEBUG_LOGE(...)
#define DEBUG_LOGW(...)
#define DEBUG_LOGI(...)
#define DEBUG_LOGV(...)
#define DEBUG_LOGD(...)
#endif
#else
#define DEBUG_LOG(...)
#define DEBUG_LOGE(...)
#define DEBUG_LOGW(...)
#define DEBUG_LOGI(...)
#define DEBUG_LOGV(...)
#define DEBUG_LOGD(...)
#endif

#endif
