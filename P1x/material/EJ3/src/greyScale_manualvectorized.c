#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <sys/time.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <x86intrin.h>
#include <xmmintrin.h>
#include <immintrin.h>


static inline void getRGB(uint8_t *im, int width, int height, int nchannels, int x, int y, int *r, int *g, int *b)
{

    unsigned char *offset = im + (x + width * y) * nchannels;
    *r = offset[0];
    *g = offset[1];
    *b = offset[2];
}

int main(int nargs, char **argv)
{
    int width, height, nchannels;
    struct timeval fin,ini;

    if (nargs < 2)
    {
        printf("Usage: %s <image1> [<image2> ...]\n", argv[0]);
    }
    // For each image
    // Bucle 0
    for (int file_i = 1; file_i < nargs; file_i++)
    {
        printf("[info] Processing %s\n", argv[file_i]);
        /****** Reading file ******/
        uint8_t *rgb_image = stbi_load(argv[file_i], &width, &height, &nchannels, 4);
        if (!rgb_image)
        {
            perror("Image could not be opened");
        }

        /****** Allocating memory ******/
        // - RGB2Grey
        uint8_t *grey_image = malloc(width * height);
        if (!grey_image)
        {
            perror("Could not allocate memory");
        }

        // - Filenames 
        for (int i = strlen(argv[file_i]) - 1; i >= 0; i--)
        {
            if (argv[file_i][i] == '.')
            {
                argv[file_i][i] = 0;
                break;
            }
        }

        char *grey_image_filename = 0;
        asprintf(&grey_image_filename, "%s_grey.jpg", argv[file_i]);
        if (!grey_image_filename)
        {
            perror("Could not allocate memory");
            exit(-1);
        }

        /****** Computations ******/
        printf("[info] %s: width=%d, height=%d, nchannels=%d\n", argv[file_i], width, height, nchannels);

        if (nchannels != 3 && nchannels != 4)
        {
            printf("[error] Num of channels=%d not supported. Only three (RGB), four (RGBA) are supported.\n", nchannels);
            continue;
        }

        //RGB Scale

        __m256 value = _mm256_setr_ps(0.2989f, 0.5870f, 0.1140f, 0.0f, 0.2989f, 0.5870f, 0.1140f, 0.0f);
        __m256 tot;
        __m256 tot1;
        __m128i grey;
        __m256i of = _mm256_setr_epi32(0, 4, 1, 5,0,0,0,0);
        

        gettimeofday(&ini,NULL);
        // RGB to grey scale
        int r, g, b;
        for (int j = 0; j <  height; j++)
        {   
            for (int i = 0; i<width; i+=4)
            {
                __m128i datal = _mm_loadl_epi64((__m128i *)(rgb_image + (i + width * j) * 4));
                __m128i datah = _mm_loadl_epi64((__m128i *)(rgb_image + (i + width * j) * 4 + 8));
                // Convertimos a entero de 32 bits
                __m256i data32bI = _mm256_cvtepu8_epi32(datal);
                __m256i data32bH = _mm256_cvtepu8_epi32(datah);
                // Convertimos a float (single precision)
                __m256 datafloatI = _mm256_cvtepi32_ps(data32bI);
                __m256 datafloatH = _mm256_cvtepi32_ps(data32bH);
                // Multiplicamos por el vector de pesos
                __m256 mulI = _mm256_mul_ps(datafloatI, value);
                __m256 mulH = _mm256_mul_ps(datafloatH, value);
                // Sumamos los elementos
                tot1 = _mm256_hadd_ps(mulI, mulH);
                // Redondeamos los valores del vector de su valor a float a integer
                tot = _mm256_hadd_ps(tot1, tot1); // tras segundo hadd hemos añadido 4byte por cada pixel
                
                // permutamos los valores para que queden en el orden correcto
                tot = _mm256_permutevar8x32_ps(tot, of);
                // Convertimos a entero de 32 bits
                grey = _mm_cvtps_epi32(_mm256_extractf128_ps(tot, 0));
                // Guardamos los valores en el vector de salida
                //__m128i arr = _mm_setr_epi8(0, 1 * 4, 2 * 4, 3 * 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

                /*grey_image[(j * width) + i] = grey[0];
                grey_image[(j * width) + i + 1] = grey[1];
                grey_image[(j * width) + i + 2] = grey[2];
                grey_image[(j * width) + i + 3] = grey[3];*/

                uint32_t *ptr = (__m128i *)&grey;

                grey_image[(j * width) + i] = ptr[0];
                grey_image[(j * width) + i + 1] = ptr[1];
                grey_image[(j * width) + i + 2] = ptr[2];
                grey_image[(j * width) + i + 3] = ptr[3];
            }
        }

        stbi_write_jpg(grey_image_filename, width, height, 1, grey_image, 10);
        free(rgb_image);

        gettimeofday(&fin,NULL);

	printf("Tiempo: %f\n", ((fin.tv_sec*1000000+fin.tv_usec)-(ini.tv_sec*1000000+ini.tv_usec))*1.0/1000000.0);
        free(grey_image_filename);
    }
}
