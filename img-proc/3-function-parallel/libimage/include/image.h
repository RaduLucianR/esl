#ifndef __IMAGE_PROCESSING_FUNCTIONS_H__
#define __IMAGE_PROCESSING_FUNCTIONS_H__

#include <stdlib.h>

extern void median_filter(uint8_t const volatile * const frame_in, uint32_t const xsize_in, uint32_t const ysize_in, uint32_t const bitsperpixel_in, uint8_t volatile * const frame_out);
extern double const conv_sharpen3[];
extern double const conv_avgxy1[];
extern double const conv_avgxy3[];
extern double const conv_avgxy5[];
extern double const conv_avgx3[];
extern double const conv_avgxy7[];
extern double const conv_gaussianblur5[];
extern void convolution(uint8_t const volatile * const frame_in, uint32_t const xsize_in, uint32_t const ysize_in, uint32_t const bitsperpixel_in,
                        double const * const f, uint32_t const fxsize, uint32_t const fysize, uint8_t volatile * const frame_out);
extern void scale(uint8_t const volatile * const frame_in, uint32_t const xsize_in, uint32_t const vysize_in, uint32_t const bitsperpixel_in,
                  uint32_t const xsize_out, uint32_t const ysize_out, uint8_t volatile * const frame_out);
// frame_out may be the same as frame_in
extern void greyscale(uint8_t const volatile * const frame_in, uint32_t const xsize_in, uint32_t const ysize_in, uint32_t const bitsperpixel_in,
                      uint8_t volatile * const frame_out);
// frame_out may be the same as frame_in1 or frame_in2
extern void overlay(uint8_t const volatile * const frame_in1, uint32_t const xsize_in1, uint32_t const ysize_in1, uint32_t const bitsperpixel_in1,
                    uint8_t const volatile * const frame_in2, uint32_t const xsize_in2, uint32_t const ysize_in2, uint32_t const bitsperpixel_in2,
                    uint32_t const xoffset, uint32_t const yoffset, double const ratio, uint8_t volatile * const frame_out);
extern void sobel(uint8_t const volatile * const frame_in, uint32_t const xsize_in, uint32_t const ysize_in, uint32_t const bitsperpixel_in,
                  uint8_t const threshold, uint8_t volatile * const frame_out);
extern void overlay_sobel(uint8_t const volatile * const frame_in, uint32_t const xsize_in, uint32_t const ysize_in, uint32_t const bitsperpixel_in,
                          uint8_t const threshold, uint8_t volatile * const frame_out);

extern void convolution_line(uint8_t const volatile * const line0, uint8_t const volatile * const line1, uint8_t const volatile * const line2,
                 uint8_t const volatile * const line3, uint8_t const volatile * const line4, uint32_t const volatile xsize_in,
                 uint32_t const volatile ysize_in, uint32_t const volatile y,
                 double const * const f, uint32_t const fxsize, uint32_t const fysize, uint8_t volatile * const line_out);

extern void sobel_line(uint8_t const volatile * const line0, uint8_t const volatile * const line1, uint8_t const volatile * const line2,
           uint32_t const xsize_in, uint32_t const ysize_in, uint32_t const y,
           uint8_t const threshold, uint8_t volatile * const line_out);
#endif
