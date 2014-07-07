#include "ivf.h"

static void mem_put_le16(char *mem, unsigned int val)
{
    mem[0] = val;
    mem[1] = val>>8;
}

static void mem_put_le32(char *mem, unsigned int val)
{
    mem[0] = val;
    mem[1] = val>>8;
    mem[2] = val>>16;
    mem[3] = val>>24;
}

void write_ivf_header(FILE *file, int w, int h)
{
    char header[32];

    header[0] = 'D';
    header[1] = 'K';
    header[2] = 'I';
    header[3] = 'F';
    mem_put_le16(header + 4, 0);                     // version
    mem_put_le16(header + 6, 32);                    // header size
    mem_put_le32(header + 8, 0x30385056);                // fourcc
    mem_put_le16(header + 12, w);             // width
    mem_put_le16(header + 14, h);             // height
    mem_put_le32(header + 16, 90000);  // rate
    mem_put_le32(header + 20, 16777216);  // scale
    mem_put_le32(header + 24, 0);            // length
    mem_put_le32(header + 28, 0);                    // unused

    fwrite(header, 1, 32, file);
}

void write_ivf_frame_header(FILE *outfile, int64_t pts, size_t frame_size) {
  char header[12];

  mem_put_le32(header, (int)frame_size);
  mem_put_le32(header + 4, (int)(pts & 0xFFFFFFFF));
  mem_put_le32(header + 8, (int)(pts >> 32));
  fwrite(header, 1, 12, outfile);
}
