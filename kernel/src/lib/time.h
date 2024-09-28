//
//	time.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#include "./integers.h"

#define SECONDS_PER_MINUTE 60
#define SECONDS_PER_HOUR (60 * SECONDS_PER_MINUTE)
#define SECONDS_PER_DAY (24 * SECONDS_PER_HOUR)
#define SECONDS_PER_YEAR (365 * SECONDS_PER_DAY)

#ifndef TIME_FORMAT_DEFAULT
	#define TIME_FORMAT_DEFAULT "{day}:{mon}:{year} | {hour}:{min}:{sec}.{ms}"
#endif


extern volatile size_t tick;

volatile size_t tick = 0;
//	incrementing the variable every millisecond can keep the system running for over 500 years

static const u16 days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

typedef struct time_t {
	//	daytime specification
	u32 hours;
	u32 minutes;
	u32 seconds;
	u32 milliseconds;
} __attribute__((packed)) time_t;

typedef struct date_t {
	//	date specification
	ssize_t years;
	u16 months;
	u32 days;
} __attribute__((packed)) date_t;

typedef struct timespec_t {
	//	time and date specification
	time_t time;
	date_t date;
} __attribute__((packed)) timespec_t;

static void time_init();
	//	reads the unixtime given by bootloader

static inline void time_update();
	//	writes data into

[[maybe_unused]] __attribute__((returns_nonnull)) static char* format_time(timespec_t* time, const char* format);

__attribute__((always_inline)) static inline bool is_leap_year(ssize_t year) {
	return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}


static volatile timespec_t timespec;

[[maybe_unused]] static char* time_format_str = TIME_FORMAT_DEFAULT;









//	helpers

enum __format_time_token_switch {
	__ftts_year = 255,
	__ftts_month = 254,
	__ftts_day = 253,
	__ftts_hour = 252,
	__ftts_minute = 251,
	__ftts_sec = 250,
	__ftts_ms = 249,
	__ftts_unknown = 0
} __format_time_token_switch;

enum __format_time_token_switch __format_time_token(const char* format, size_t len);
	//	helper function for format_time function
