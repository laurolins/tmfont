#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "smooth.h"
#include "image.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
	printf("Usage:  smoother <input_sgi> <radius> <output_sgi>\n");
	return 1;
    }
    float R = atof(argv[2]);
    Image *img = Image_read_sgi(argv[1]);
    int m = img->height;
    int n = img->width;
    byte *buffer = img->buffer;
    int i;
    for (i=0;i<m*n;i++)
    {
	if (buffer[i] == 255)
	    buffer[i] = 1;
    }
    float *f = smooth(buffer,m,n,R);
    for (i=0;i<m*n;i++)
    {
	/* if (f[i] > 0) */
	/*     printf("%10f   ",f[i]); */
	buffer[i] = (byte) (f[i] * 255.0);
	if (buffer[i] > 0)
	    printf("%5d   ",buffer[i]);
    }
    Image_save_as_sgi(img, argv[3], 0);
    printf("Saved file: %s\n",argv[3]);

    return 0;

}



