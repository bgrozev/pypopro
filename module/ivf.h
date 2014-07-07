#include "stdio.h"
#include "stdint.h"
static void mem_put_le16(char *mem, unsigned int val);
static void mem_put_le32(char *mem, unsigned int val);
void write_ivf_header(FILE *file, int w, int h);
void write_ivf_frame_header(FILE *outfile, int64_t pts, size_t frame_size);
