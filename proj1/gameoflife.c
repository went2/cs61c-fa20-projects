/************************************************************************
**
** NAME:        gameoflife.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Justin Yokota - Starter Code
**				WenTommy
**
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

typedef struct ColorCount {
	uint8_t R;
	uint8_t G;
	uint8_t B;
  uint8_t count;
} ColorCount;

uint32_t hex2int(const char *hex);
ColorCount *alive_neighbor(Image*, int, int);

ColorCount *alive_neighbor(Image *image, int row, int col) {
  ColorCount *colorCount = (ColorCount *)malloc(sizeof(ColorCount));
  if(colorCount == NULL) return NULL;
  colorCount->count = 0;

  for(int i=row-1; i<=row+1; i++) {
    for(int j=col-1; j<=col+1; j++) {
      if(i == row && j == col) continue;

      int real_row = i; 
      int real_col = j;
      // boundry check
      if(i < 0) {
        real_row = image->rows - 1;
      } else if(i >= image->rows) {
        real_row = 0;
      }

      if(j < 0) {
        real_col = image->cols - 1;
      } else if(j >= image->cols) {
        real_col = 0;
      }

      // check if the neighbor is alive
      if(image->image[real_row][real_col].R != 0 || 
         image->image[real_row][real_col].G != 0 ||
         image->image[real_row][real_col].B != 0
      ) {
        colorCount->count++;
        colorCount->R = image->image[real_row][real_col].R;
        colorCount->G = image->image[real_row][real_col].G;
        colorCount->B = image->image[real_row][real_col].B;
      }
        
    }
  }
  return colorCount;
}

//Determines what color the cell at the given row/col should be. This function allocates space for a new Color.
//Note that you will need to read the eight neighbors of the cell in question. The grid "wraps", so we treat the top row as adjacent to the bottom row
//and the left column as adjacent to the right column.
Color *evaluateOneCell(Image *image, int row, int col, uint32_t rule) {
  Color *new_color = (Color *)malloc(sizeof(Color));
  if(new_color == NULL) return NULL;

  uint8_t red = image->image[row][col].R;
  uint8_t green = image->image[row][col].G;
  uint8_t blue = image->image[row][col].B;

  int curr_state = 1;
  int LIVE_OFFSET = 9;
  if(red == 0 && green == 0 && blue == 0) {
    curr_state = 0;
  }

  ColorCount *cCount = alive_neighbor(image, row, col);
  int next_state = (rule >> (cCount->count + curr_state*LIVE_OFFSET)) & 1;

  // next_state is either 1 or 0
  if(next_state == 0) {
    new_color->R = new_color->G = new_color->B = 0;
  } if (next_state == 1) { // should use the neighbor's color
    new_color->R = cCount->R;
    new_color->G = cCount->G;
    new_color->B = cCount->B;
  }
  
  free(cCount);
  return new_color;
}

//The main body of Life; given an image and a rule, computes one iteration of the Game of Life.
//You should be able to copy most of this from steganography.c
Image *life(Image *image, uint32_t rule) {
	Image *new_image = (Image *)malloc(sizeof(Image));
  if(new_image == NULL) return NULL;

  new_image->rows = image->rows;
  new_image->cols = image->cols;
  new_image->image = malloc(image->rows * sizeof(Color*));
  if(new_image->image == NULL) {
    free(new_image);
    return NULL;
  }

  for(int i=0; i<image->rows; i++) {
    new_image->image[i] = malloc(image->cols * sizeof(Color));
    if (new_image->image[i] == NULL) {
      for (int j=0; j<i; j++) {
        free(new_image->image[j]);
      }
      free(new_image->image);
      free(new_image);
      return NULL;
    }

    for(int j=0; j<image->cols; j++) {
      Color *pixel = evaluateOneCell(image, i, j, rule);
      if(pixel == NULL) return NULL;
      new_image->image[i][j] = *pixel;
      free(pixel);
    }
  }

  return new_image;
}

uint32_t hex2int(const char *hex_str) {
  return (uint32_t)strtol(hex_str, NULL, 0);
}

/*
Loads a .ppm from a file, computes the next iteration of the game of life, then prints to stdout the new image.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
argv[2] should contain a hexadecimal number (such as 0x1808). Note that this will be a string.
You may find the function strtol useful for this conversion.
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!

You may find it useful to copy the code from steganography.c, to start.
*/
int main(int argc, char **argv) {
  if(argc < 3) {
    printf("usage: ./gameOfLife filename rule\n"
           "filename is an ASCII PPM file (type P3) with maximum value 255.\n"
           "rule is a hex number beginning with 0x; Life is 0x1808."
          );
    return -1;
  }

  char *filename = argv[1];
  uint32_t rule = hex2int(argv[2]);

  Image *image = readData(filename);
  if(image == NULL) return -1;

  Image *new_image = life(image, rule);
  if(new_image == NULL) {
    freeImage(image);
    return -1;
  }
  writeData(new_image);
  freeImage(image);
  freeImage(new_image);
	
  return 0;
}
