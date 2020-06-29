#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif
#include <string.h>
#include "Date.h"

static const uint8_t DAYS_IN_MONTH[] PROGMEM = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const char WEEKDAY_NAMES[] PROGMEM = { 'M', 'o', 'n', 'T', 'u', 'e', 'W', 'e', 'd', 'T', 'h', 'u', 'F', 'r', 'i', 'S', 'a', 't', 'S', 'u', 'n' };
static const char MONTH_NAMES[] PROGMEM = { 'J', 'a', 'n', 'F', 'e', 'b', 'M', 'a', 'r', 'A', 'p', 'r', 'M', 'a', 'y', 'J', 'u', 'n', 'J', 'u', 'l', 'A', 'u', 'g', 'S', 'e', 'p', 'O', 'c', 't', 'N', 'o', 'v', 'D', 'e', 'c' };

const uint16_t EPOCH_TIME_2000 = 10957; // Days from 01.01.1970 to 01.01.2000
const uint16_t EPOCH_TIME_2019 = 17897; // Days from 01.01.1970 to 01.01.2019

const char DATE_SEPARATOR = '.';
const char TIME_SEPARATOR = ':';

bool isLeapYear(uint16_t year) {
  return (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));
}

uint8_t lastDayOfMonth(uint8_t month, uint16_t year) {
  uint8_t result = pgm_read_byte(DAYS_IN_MONTH + month - 1);

  if ((month == 2) && isLeapYear(year))
    ++result;

  return result;
}

void parseUnixTime(uint32_t unixtime, uint8_t &hour, uint8_t &minute, uint8_t &second, uint8_t &weekday, uint8_t &day, uint8_t &month, uint16_t &year) {
  second = unixtime % 60;
  unixtime /= 60;
  minute = unixtime % 60;
  unixtime /= 60;
  hour = unixtime % 24;

  uint16_t days = unixtime / 24;
  bool leap;

  weekday = (days + 3) % 7; // 1 Jan 1970 is Thursday
  if (days >= EPOCH_TIME_2019) {
    year = 2019;
    days -= EPOCH_TIME_2019;
  } else if (days >= EPOCH_TIME_2000) {
    year = 2000;
    days -= EPOCH_TIME_2000;
  } else
    year = 1970;
  for (; ; year++) {
    leap = isLeapYear(year);
    if (days < 365 + leap)
      break;
    days -= 365 + leap;
  }
  for (month = 1; ; ++month) {
    uint8_t daysPerMonth = pgm_read_byte(DAYS_IN_MONTH + month - 1);

    if (leap && (month == 2))
      ++daysPerMonth;
    if (days < daysPerMonth)
      break;
    days -= daysPerMonth;
  }
  day = days + 1;
}

void parseUnixTime(uint32_t unixtime, uint8_t &hour, uint8_t &minute, uint8_t &second, uint8_t &day, uint8_t &month, uint16_t &year) {
  second = unixtime % 60;
  unixtime /= 60;
  minute = unixtime % 60;
  unixtime /= 60;
  hour = unixtime % 24;

  uint16_t days = unixtime / 24;
  bool leap;

  if (days >= EPOCH_TIME_2019) {
    year = 2019;
    days -= EPOCH_TIME_2019;
  } else if (days >= EPOCH_TIME_2000) {
    year = 2000;
    days -= EPOCH_TIME_2000;
  } else
    year = 1970;
  for (; ; ++year) {
    leap = isLeapYear(year);
    if (days < 365 + leap)
      break;
    days -= 365 + leap;
  }
  for (month = 1; ; ++month) {
    uint8_t daysPerMonth = pgm_read_byte(DAYS_IN_MONTH + month - 1);

    if (leap && (month == 2))
      ++daysPerMonth;
    if (days < daysPerMonth)
      break;
    days -= daysPerMonth;
  }
  day = days + 1;
}

uint32_t combineUnixTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t month, uint16_t year) {
  uint16_t days = day - 1;
  uint16_t y;

  if (year >= 2019) {
    days += EPOCH_TIME_2019;
    y = 2019;
  } else if (year >= 2000) {
    days += EPOCH_TIME_2000;
    y = 2000;
  } else
    y = 1970;
  for (; y < year; ++y)
    days += 365 + isLeapYear(y);
  for (y = 1; y < month; ++y)
    days += pgm_read_byte(DAYS_IN_MONTH + y - 1);
  if ((month > 2) && isLeapYear(year))
    ++days;

  return (((uint32_t)days * 24 + hour) * 60 + minute) * 60 + second;
}

static void twoDigits(char *str, uint8_t value) {
  *str++ = value / 10 + '0';
  *str = value % 10 + '0';
}

char *timeToStr(char *str, uint8_t hour, uint8_t minute, uint8_t second) {
  char *pstr = str;

  twoDigits(pstr, hour);
  pstr += 2;
  *pstr++ = TIME_SEPARATOR;
  twoDigits(pstr, minute);
  pstr += 2;
  *pstr++ = TIME_SEPARATOR;
  twoDigits(pstr, second);
  pstr += 2;
  *pstr = '\0';

  return str;
}

char *timeToStr(char *str, uint32_t unixtime) {
  uint8_t hh, mm, ss;
  uint8_t d, m;
  uint16_t y;

  parseUnixTime(unixtime, hh, mm, ss, d, m, y);

  return timeToStr(str, hh, mm, ss);
}

char *dateToStr(char *str, uint8_t day, uint8_t month, uint16_t year) {
  char *pstr = str;

  twoDigits(pstr, day);
  pstr += 2;
  *pstr++ = DATE_SEPARATOR;
  twoDigits(pstr, month);
  pstr += 2;
  *pstr++ = DATE_SEPARATOR;
  twoDigits(pstr, year / 100);
  pstr += 2;
  twoDigits(pstr, year % 100);
  pstr += 2;
  *pstr = '\0';

  return str;
}

char *dateToStr(char *str, uint32_t unixtime) {
  uint8_t hh, mm, ss;
  uint8_t d, m;
  uint16_t y;

  parseUnixTime(unixtime, hh, mm, ss, d, m, y);

  return dateToStr(str, d, m, y);
}

char *timeDateToStr(char *str, uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t month, uint16_t year) {
  char *pstr = str;

  timeToStr(pstr, hour, minute, second);
  pstr += 8;
  *pstr++ = ' ';
  dateToStr(pstr, day, month, year);

  return str;
}

char *timeDateToStr(char *str, uint32_t unixtime) {
  uint8_t hh, mm, ss;
  uint8_t d, m;
  uint16_t y;

  parseUnixTime(unixtime, hh, mm, ss, d, m, y);

  return timeDateToStr(str, hh, mm, ss, d, m, y);
}

char *dateTimeToStr(char *str, uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second) {
  char *pstr = str;

  dateToStr(pstr, day, month, year);
  pstr += 10;
  *pstr++ = ' ';
  timeToStr(pstr, hour, minute, second);

  return str;
}

char *dateTimeToStr(char *str, uint32_t unixtime) {
  uint8_t hh, mm, ss;
  uint8_t d, m;
  uint16_t y;

  parseUnixTime(unixtime, hh, mm, ss, d, m, y);

  return dateTimeToStr(str, d, m, y, hh, mm, ss);
}

char *weekdayName(char *str, uint8_t weekday) {
  if (weekday < 7) {
    strncpy_P(str, &WEEKDAY_NAMES[weekday * 3], 3);
    str[3] = '\0';
  } else
    *str = '\0';

  return str;
}

char *monthName(char *str, uint8_t month) {
  if ((month >= 1) && (month <= 12)) {
    strncpy_P(str, &MONTH_NAMES[(month - 1) * 3], 3);
    str[3] = '\0';
  } else
    *str = '\0';

  return str;
}
