#ifndef IMAGE_H
#define IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BYTE
#define BYTE
typedef unsigned char byte;
#endif

typedef struct _Image
{
    byte *buffer;
    int width;
    int height;
} Image;

Image *Image_new(int width, int height);
byte Image_get_value(Image *img, int row, int col);
byte Image_get_value(Image *img, int row, int col);
void Image_set_value(Image *img, int row, int col, byte value);
void Image_copy(Image *source, Image *target, int xoff, int yoff);


void Image_save_as_sgi(Image *g, const char *filename, byte invert_y);
Image *Image_read_sgi(const char *filename);

#ifdef __cplusplus
}
#endif

#endif
