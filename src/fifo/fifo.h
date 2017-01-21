#include <stdbool.h>

typedef struct pipes *PipesPtr;

PipesPtr initPipes(bool host);
void     sendStringToPipe(PipesPtr channel, const char *data);
bool     getStringFromPipe(PipesPtr channel, char *buffer, size_t size);
void     closePipes(PipesPtr channel);
