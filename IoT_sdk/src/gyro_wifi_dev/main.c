#include "gyro_mcu.h"
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{    
    OpenDemoData();

    StartWork();

    while (1)
    {
        sleep(1);
    }

    return 1;
}