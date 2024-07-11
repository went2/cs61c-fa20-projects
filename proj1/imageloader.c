/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

// Opens a .ppm P3 image file, and constructs an Image object.
// You may find the function fscanf useful.
// Make sure that you close the file with fclose before returning.
Image *readData(char *filename) {
  FILE *fp = fopen(filename, "r");
  
  char format[3];
  uint32_t width, height, max_color;
  
  // read ppm header
  fscanf(fp, "%2s", format);
  fscanf(fp, "%d %d", &width, &height);
  fscanf(fp, "%d", &max_color);

  // initiate Image object
  Image* image = malloc(sizeof(Image));
  image->rows = height;
  image->cols = width;
  image->image = malloc(height * sizeof(Color*));
  for(int i = 0; i < height; i++) {
    image->image[i] = malloc(width * sizeof(Color));
  }

  // read pixel data
  for(int i = 0; i < height; i++ ) {
    for(int j = 0; j < width; j++) {
      uint8_t r, g, b;
      fscanf(fp, "%hhd %hhd %hhd", &r, &g, &b);
      image->image[i][j].R = r;
      image->image[i][j].G = g;
      image->image[i][j].B = b;
    }
  }

  fclose(fp);
  return image;
}

// Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image) {
  uint32_t height = image->rows;
  uint32_t width = image->cols;

  printf("P3\n");
  printf("%d %d\n", width, height);
  printf("255\n");

  // print pixel data
  for (int i=0; i<height; i++) {
    // print rows
    for (int j=0; j<width; j++) {
      // print a columns in a row
      uint8_t r = image->image[i][j].R;
      uint8_t g = image->image[i][j].G;
      uint8_t b = image->image[i][j].B;
      printf("%3d %3d %3d", r,g,b);

      if (j < width - 1) {
        printf("   ");
      }
    }
    printf("\n");
  }
}

// Frees an image
void freeImage(Image *image) {
  if(image == NULL) return;

  uint32_t height = image->rows;

  for(int i=0; i<height; i++) {
    // free each row
    free(image->image[i]);
  }
  // free the array of row pointers
  free(image->image);

  // free Image structure itself
  free(image);
}