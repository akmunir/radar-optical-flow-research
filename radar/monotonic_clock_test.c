#include <stdio.h>
#include <time.h>

struct timespec getElapsedTime(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0)
    {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000L + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

int main()
{
    struct timespec start_time;
    struct timespec curr_time;
    if (clock_gettime(CLOCK_MONOTONIC, &start_time) != 0)
    {
        printf("start timestamp fail");
        return 0;
    }
    int i = 0;
    struct timespec temp;
    struct timespec elapsed;
    double elapsedTime = 0;
    //printf("elapsed seconds: %.6f \n elapsed nanoseconds: %.6f \n", elapsed.tv_sec, (double)elapsed.tv_nsec);
    printf("start seconds: %.6f \n", start_time.tv_nsec + 1000000000L);
    while (elapsedTime < 10.0)//elapsedTime < 10.0)
    {
        printf("loop\n");
        if (clock_gettime(CLOCK_MONOTONIC, &curr_time) != 0)
        {
            printf("current timestamp fail");
            return 0;
        }
        elapsed = getElapsedTime(start_time, curr_time);
        elapsedTime = (elapsed.tv_sec + (double)elapsed.tv_nsec) / 1000000000.0;
        printf("elapsed total: %.6fs \n", elapsedTime);
        i++;
    }
}
