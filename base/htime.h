#ifndef HV_TIME_H_
#define HV_TIME_H_

#include "hexport.h"
#include "hplatform.h"

BEGIN_EXTERN_C

#define SECONDS_PER_HOUR    3600
#define SECONDS_PER_DAY     86400   // 24*3600
#define SECONDS_PER_WEEK    604800  // 7*24*3600

#define IS_LEAP_YEAR(year) (((year)%4 == 0 && (year)%100 != 0) || (year)%100 == 0)

typedef struct datetime_s {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    int ms;
} datetime_t;

#ifdef _MSC_VER
/* @see winsock2.h
// Structure used in select() call, taken from the BSD file sys/time.h
struct timeval {
    long    tv_sec;
    long    tv_usec;
};
*/

struct timezone {
    int tz_minuteswest; /* of Greenwich */
    int tz_dsttime;     /* type of dst correction to apply */
};

#include <sys/timeb.h>
static inline int gettimeofday(struct timeval *tv, struct timezone *tz) {
    struct _timeb tb;
    _ftime(&tb);
    if (tv) {
        tv->tv_sec =  (long)tb.time;
        tv->tv_usec = tb.millitm * 1000;
    }
    if (tz) {
        tz->tz_minuteswest = tb.timezone;
        tz->tz_dsttime = tb.dstflag;
    }
    return 0;
}
#endif

// sleep(s), msleep(ms), usleep(us)
#ifdef OS_WIN
static inline void sleep(unsigned int s) {
    Sleep(s * 1000);
}

static inline void msleep(unsigned int ms) {
    Sleep(ms);
}

static inline void usleep(unsigned int us) {
    Sleep(us / 1000);
}
#else
static inline void msleep(unsigned int ms) {
    usleep(ms * 1000);
}
#endif

// ms
HV_EXPORT unsigned int gettick();

static inline unsigned long long gettimeofday_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * (unsigned long long)1000 + tv.tv_usec/1000;
}

// us
HV_EXPORT unsigned long long gethrtime_us();

HV_EXPORT datetime_t datetime_now();
HV_EXPORT time_t     datetime_mktime(datetime_t* dt);

HV_EXPORT datetime_t* datetime_past(datetime_t* dt, int days DEFAULT(1));
HV_EXPORT datetime_t* datetime_future(datetime_t* dt, int days DEFAULT(1));

#define TIME_FMT            "%02d:%02d:%02d"
#define TIME_FMT_BUFLEN     12
HV_EXPORT char* duration_fmt(int sec, char* buf);

#define DATETIME_FMT        "%04d-%02d-%02d %02d:%02d:%02d.%03d"
#define DATETIME_FMT_BUFLEN 24
HV_EXPORT char* datetime_fmt(datetime_t* dt, char* buf);

#define GMTIME_FMT          "%.3s, %02d %.3s %04d %02d:%02d:%02d GMT"
#define GMTIME_FMT_BUFLEN   30
HV_EXPORT char* gmtime_fmt(time_t time, char* buf);

HV_EXPORT int days_of_month(int month, int year);

HV_EXPORT int month_atoi(const char* month);
HV_EXPORT const char* month_itoa(int month);

HV_EXPORT int weekday_atoi(const char* weekday);
HV_EXPORT const char* weekday_itoa(int weekday);

HV_EXPORT datetime_t hv_compile_datetime();

/*
 * minute   hour    day     week    month       action
 * 0~59     0~23    1~31    0~6     1~12
 *  30      -1      -1      -1      -1          cron.hourly
 *  30      1       -1      -1      -1          cron.daily
 *  30      1       15      -1      -1          cron.monthly
 *  30      1       -1       0      -1          cron.weekly
 *  30      1        1      -1      10          cron.yearly
 */
HV_EXPORT time_t cron_next_timeout(int minute, int hour, int day, int week, int month);

END_EXTERN_C

#endif // HV_TIME_H_
