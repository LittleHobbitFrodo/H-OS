//
//	time.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once
#include "../time.h"

void time_init() {
	//	read unixtime given by bootloader

	u64 unixtime;

	if (unlikely(req_boot_time.response == null)) {
		report("unable to get boot time, initializing system time with UNIX epoch", report_error);
		unixtime = 0;
	} else {
		unixtime = req_boot_time.response->boot_time;
	}

	timespec.date.days = unixtime / SECONDS_PER_DAY + 1;

	unixtime %= SECONDS_PER_DAY;

	timespec.time.hours = (unixtime / SECONDS_PER_HOUR) + 2;	//hypothetical number lol
	unixtime %= SECONDS_PER_HOUR;

	timespec.time.minutes = unixtime / SECONDS_PER_MINUTE;
	unixtime %= SECONDS_PER_MINUTE;
	timespec.time.seconds = unixtime;

	//	year
	timespec.date.years = 1970;	//	unix epoch
	for (; timespec.date.days >= (is_leap_year(timespec.date.years)? 366 : 365); timespec.date.years++) {
		timespec.date.days -= (is_leap_year(timespec.date.years)? 366 : 365);
	}

	//	month
	timespec.date.months = 0;
	for (;timespec.date.days >= (u32)(days_in_month[timespec.date.months] + ((timespec.date.months == 1) && (is_leap_year(timespec.date.years)))); timespec.date.months++) {
		timespec.date.days -= days_in_month[timespec.date.months] + ((timespec.date.months == 1) && (is_leap_year(timespec.date.years)));
	}
	timespec.date.months++;

}

void time_update() {
	timespec.time.seconds += tick / 1000;
	timespec.time.milliseconds = tick %= 1000;	//	reset tick
	timespec.time.minutes += timespec.time.seconds / 60;
	timespec.time.seconds %= 60;
	timespec.time.hours += timespec.time.minutes / 60;
	timespec.date.days += timespec.time.hours / 24;

	//	months and years
	for (; timespec.date.days >= (u32)(days_in_month[timespec.date.months] + ((timespec.date.months == 1) && (is_leap_year(timespec.date.years)))); timespec.date.months++) {
		timespec.date.days -= days_in_month[timespec.date.months] + (timespec.date.months == 1) && (is_leap_year(timespec.date.years));
		if (timespec.date.months >= 12) {
			timespec.date.months -= 12;
			timespec.date.years++;
		}
	}
}

char* format_time(timespec_t* time, const char* format) {
	string ret;
	str(&ret, &heap.global);
	size_t len = strlen(format);
	for (size_t i = 0; i < len; i++) {
		if (format[i] == '{') {
			if (++i < len) {
				size_t start = i;
				for (; (i < len) && (format[i] != '}'); i++);
				char str[INTLEN_I64];
				switch (__format_time_token(&format[start], i - start)) {
					case __ftts_year: {
						to_stringi((char*)&str, time->date.years);
						str_push(&ret, (const char*)&str);
						break;
					}
					case __ftts_month: {
						to_string((char*)&str, time->date.months);
						str_push(&ret, (const char*)&str);
						break;
					}
					case __ftts_day: {
						to_string((char*)&str, time->date.days);
						str_push(&ret, (const char*)&str);
						break;
					}
					case __ftts_hour: {
						to_string((char*)&str, time->time.hours);
						str_push(&ret, (const char*)&str);
						break;
					}
					case __ftts_minute: {
						to_string((char*)&str, time->time.minutes);
						str_push(&ret, (const char*)&str);
						break;
					}
					case __ftts_sec: {
						to_string((char*)&str, time->time.seconds);
						str_push(&ret, (const char*)&str);
						break;
					}
					case __ftts_ms: {
						to_string((char*)&str, time->time.milliseconds);
						str_push(&ret, (const char*)&str);
						break;
					}
					default: {
						i = start-1;
					}
				}
				++i;
			} else {
				break;
			}
		}
		str_pushc(&ret, format[i]);
	}
	return ret.data;
}


enum __format_time_token_switch __format_time_token(const char* format, size_t len) {
	switch (len) {
		case 3: {
			//	mon, day, min, sec
			if ((format[0] == 'd') && (format[1] == 'a') && (format[2] == 'y')) {
				return __ftts_day;
			}
			if ((format[0] == 'm') && (format[1] == 'o') && (format[2] == 'n')) {
				return __ftts_month;
			}
			if ((format[0] == 'm') && (format[1] == 'i') && (format[2] == 'n')) {
				return __ftts_minute;
			}
			if ((format[0] == 's') && (format[1] == 'e') && (format[2] == 'c')) {
				return __ftts_sec;
			}
			break;
		}
		case 4: {
			//	year, hour
			if ((format[0] == 'y') && (format[1] == 'e') && (format[2] == 'a') && (format[3] == 'r')) {
				return __ftts_year;
			}
			if ((format[0] == 'h') && (format[1] == 'o') && (format[2] == 'u') && (format[3] == 'r')) {
				return __ftts_hour;
			}
			break;
		}
		case 2: {
			//	ms
			if ((format[0] == 'm') && (format[1] == 's')) {
				return __ftts_ms;
			}
		}
		default: break;
	}
	return __ftts_unknown;
}