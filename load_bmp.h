//This program loads 3-channel BMP
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// bitmap header structure, has to be packed to avoid compiler padding
#pragma pack(1)
typedef struct BITMAPFILEHEADER {
  char magic[2];       // "BM" 0x424d - char[2] to avoid endian problems
  uint32_t filesize;   // size of the bitmap file (data + headers)
  uint16_t reserved1;  // must be 0
  uint16_t reserved2;  // must be 0
  uint32_t dataoffset; // when does the data start
} BITMAPFILEHEADER;

typedef struct BITMAPINFOHEADER {
  uint32_t headersize;      // size of this header
  int32_t width;            // width of the bmp
  int32_t height;           // height of the bmp
  uint16_t colorplanes;     // must be 1
  uint16_t bitdepth;        // bits per pixel
  uint32_t compression;     // 0 = uncompressed
  uint32_t imagesize;       // can be 0 if bmp is uncompressed
  int32_t hresolution;      // print resolution
  int32_t vresolution;      // print resolution
  uint32_t palettecolors;   // can be 0 if uncompressed
  uint32_t importantcolors; // number of important colors, 0 = all are important
} BITMAPINFOHEADER;

typedef struct BITMAPFULLHEADER {
  BITMAPFILEHEADER fileinfo;
  BITMAPINFOHEADER bmpinfo;
} BITMAPFULLHEADER;
#pragma pack(0)

// BMP HEADER
BITMAPFULLHEADER header;

// image data
unsigned char *data;        // loaded image

// dynamic programming to improve performance

int loadBMP(const char *imagepath) {
  // Open the file
  FILE *file = fopen(imagepath, "rb");
  if (!file) {
    printf("Image could not be opened\n");
    return 0;
  }

  // Read header
  if (fread(&header, 1, sizeof(BITMAPFULLHEADER), file) !=
      54) { // If not 54 bytes read : problem
    printf("Not a correct BMP file - Wrong header size\n");
    return 1;
  }

  if (header.fileinfo.magic[0] != 'B' || header.fileinfo.magic[1] != 'M') {
    printf("Not a correct BMP file - Wrong magic number\n");
    return 1;
  }

  // Read ints from the header
  unsigned long dataPos = header.fileinfo.dataoffset;
  unsigned long imageSize = header.bmpinfo.imagesize;
  long width = header.bmpinfo.width;
  long height = header.bmpinfo.height;

  // Some BMP files are misformatted, guess missing information
  if (imageSize == 0) {
    imageSize = width * height * 3;
  }
  if (dataPos == 0) {
    dataPos = 54; // The BMP header is usually done that way
  }

  // Allocate buffer
  data = (unsigned char *)malloc(imageSize);

  // Read the actual data from the file into the buffer
  fseek(file, dataPos, SEEK_SET);
  if (!fread(data, 1, imageSize, file)) {
    printf("Couldn't read BMP Data. Giving up.\n");
    return 1;
  }

  // copying components BGR to RGB
  unsigned char B,R;
  for(int i = 0; i < width * height ; i++){
   int index = i*3;
   B = data[index];
   R = data[index+2];
   data[index] = R;
   data[index+2] = B;
  }

  fclose(file);
  return 0;
}

