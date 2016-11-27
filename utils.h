#ifndef __UTILS_H__
#define __UTILS_H__
#include <stdint.h>
#include <inttypes.h>

//export functions
FILE *open_file(const char *name, int *size);
uint8_t *load_file(const char *name, int *size);
int save_file(const char *name, void *buf, int size);
void hex_dump(const char *str, void *buf, int size);

#endif /* __UTILS_H__ */

