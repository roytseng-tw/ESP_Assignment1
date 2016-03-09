#include <inttypes.h>

typedef struct {
    int id;
    int src;
    int dst;
    uint64_t acc_time;
    uint64_t start_time; 
    char *str;
} message;

void *clean(void* m);