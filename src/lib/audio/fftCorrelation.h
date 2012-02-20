/*
Copyright (C) 2012  Simon A. Eugster (Granjow)  <simon.eu@gmail.com>
This file is part of kdenlive. See www.kdenlive.org.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
*/

#ifndef FFTCORRELATION_H
#define FFTCORRELATION_H

#include <inttypes.h>

class FFTCorrelation
{
public:
    static void convolute(const float *left, const int leftSize,
                          const float *right, const int rightSize,
                          float **out_convolved, int &out_size);

    static void correlate(const int64_t *left, const int leftSize,
                          const int64_t *right, const int rightSize,
                          float **out_correlated, int &out_size);
};

#endif // FFTCORRELATION_H