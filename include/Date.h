#ifndef __DATE_H
#define __DATE_H

#include <inttypes.h>

bool isLeapYear(uint16_t year);
uint8_t lastDayOfMonth(uint8_t month, uint16_t year);

void parseUnixTime(uint32_t unixtime, uint8_t &hour, uint8_t &minute, uint8_t &second, uint8_t &weekday, uint8_t &day, uint8_t &month, uint16_t &year);
void parseUnixTime(uint32_t unixtime, uint8_t &hour, uint8_t &minute, uint8_t &second, uint8_t &day, uint8_t &month, uint16_t &year);
uint32_t combineUnixTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t month, uint16_t year);

char *timeToStr(char *str, uint8_t hour, uint8_t minute, uint8_t second);
char *timeToStr(char *str, uint32_t unixtime);
char *dateToStr(char *str, uint8_t day, uint8_t month, uint16_t year);
char *dateToStr(char *str, uint32_t unixtime);
char *timeDateToStr(char *str, uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t month, uint16_t year);
char *timeDateToStr(char *str, uint32_t unixtime);
char *dateTimeToStr(char *str, uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second);
char *dateTimeToStr(char *str, uint32_t unixtime);

char *weekdayName(char *str, uint8_t weekday);
char *monthName(char *str, uint8_t month);

#endif
