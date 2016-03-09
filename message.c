#include <stdlib.h>
#include "message.h"

void *clean(void* m) {
	message* m2 = (message*) m;
	free(m2->str);
}