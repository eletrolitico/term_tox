#include <pthread.h>

class ToxHandler
{
public:
    ToxHandler();

private:
    pthread_t thread;
};