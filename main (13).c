#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


char* load_png_file(const char *filename, int *width, int *height) {
    unsigned char *image = NULL;
    int error = lodepng_decode32_file(&image, width, height, filename);
    if (error) {
        printf("error %u: %s\n", error, lodepng_error_text(error));
        return NULL;
    }

    return (image);
}

void applyPrevit(unsigned char *image, int width, int height) {

    int gx[3][3] = {{1, 0, -1},
                    {1, 0, -1},
                    {1, 0, -1}};

    int gy[3][3] = {{1, 1, 1},
                    {0, 0, 0},
                    {-1, -1, -1}};


    unsigned char* result = malloc(width * height * 4 * sizeof(unsigned char));
    int y, x, dy, dx, summ_X, summ_Y, index , gray, res_Index, i;

    for (y = 1; y < height - 1; ++y) {
        for (x = 1; x < width - 1; ++x) {

            summ_X = 0, summ_Y = 0;

            for (dy = -1; dy <= 1; ++dy) {
                for (dx = -1; dx <= 1; ++dx) {

                    index = ((y+dy) * width + (x+dx)) * 4;
                    gray = (image[index] + image[index + 1] + image[index + 2]) / 3;

                    summ_X += gx[dy + 1][dx + 1] * gray;
                    summ_Y += gy[dy + 1][dx + 1] * gray;
                }
            }

            int magnitude = sqrt(summ_X * summ_X + summ_Y * summ_Y);
            if (magnitude > 255) magnitude = 255;

            res_Index = (y * width + x) * 4;

            result[res_Index] = (unsigned char)magnitude;
            result[res_Index + 1] = (unsigned char)magnitude;
            result[res_Index + 2] = (unsigned char)magnitude;
            result[res_Index + 3] = image[res_Index + 3];
        }
    }

    for (i = 0; i < width * height * 4; i++)  image[i] = result[i];


    free(result);
}


void floodFill(unsigned char* image, int x, int y, int newColor1, int newColor2, int newColor3, int a, int oldColor, int width, int height) {
    if(x < 0 || x >= width || y < 0 || y >= height)
        return;

    int resultIndex = (y * width + x) * 4;
    if(image[resultIndex] > oldColor)
        return;

    image[resultIndex] = newColor1;
    image[resultIndex + 1] = newColor2;
    image[resultIndex + 2] = newColor3;
    image[resultIndex + 3] = a;

    floodFill(image, x+1, y, newColor1, newColor2, newColor3, a, oldColor, width, height);
    floodFill(image, x-1, y, newColor1, newColor2, newColor3, a, oldColor, width, height);
    floodFill(image, x, y+1, newColor1, newColor2, newColor3, a, oldColor, width, height);
    floodFill(image, x, y-1, newColor1, newColor2, newColor3, a, oldColor, width, height);
}


void color(unsigned char* image, int width, int height, int epsilon) {
    int color1, color2, color3, a;
    for(int y = 1; y < height - 1; y++) {
        for(int x = 1; x < width - 1; x++) {
            if(image[4 * (y * width + x)] < epsilon) {
                color1 = rand() % (255 - epsilon * 2) + epsilon * 2;
                color2 = rand();
                color3 = rand();
                a = rand();
                floodFill(image, x, y, color1, color2, color3, a, epsilon, width, height);

            }

        }
    }
}


int main() {
    int w = 0;
    int h = 0;
    char *filename = "input.png";

    char *picture = load_png_file(filename, &w, &h);

    applyPrevit(picture, w, h);
    lodepng_encode32_file("output_.png", picture, w, h);

    color(picture, w, h, 26);

    lodepng_encode32_file("output.png", picture, w, h);
    free(picture);

    return 0;
}
