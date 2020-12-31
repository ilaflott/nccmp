#include <config.h>
#include <pthread.h>

#if HAVE_PTHREAD
    pthread_mutex_t mutex_print;
#endif

void io_destroy()
{
    #if HAVE_PTHREAD
        pthread_mutex_destroy(& mutex_print);
    #endif
}
void io_init()
{
    #if HAVE_PTHREAD
        pthread_mutex_init(& mutex_print, NULL);
    #endif
}
