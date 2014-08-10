//=== returns the number of usecs of (t2 - t1)
// FPS
volatile float FPS;
volatile long timestamp;
#define USEC_PER_SEC 1000000L
volatile long time_elapsed (struct timeval *t1, struct timeval *t2) {
long sec, usec;
sec = t2->tv_sec - t1->tv_sec;
usec = t2->tv_usec - t1->tv_usec;
if (usec < 0) {
--sec;
usec = usec + USEC_PER_SEC;
}
return sec*USEC_PER_SEC + usec;
}

struct timeval start_time;
struct timeval end_time;

volatile void start_timer() {
gettimeofday (&start_time, NULL);
}
volatile long end_timer() {
gettimeofday (&end_time, NULL);
return time_elapsed(&start_time, &end_time);
}
