/*
The MIT License (MIT)

Copyright (c) blurryroots innovation qanat OÜ

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
/*! \file misc.h
    \brief Miscellaneous utilities.
    
    ^^
*/

/** Finds max value. */
#define GENERIC_MAX(x, y) ((x) > (y) ? (x) : (y))
/** Finds min value. */
#define GENERIC_MIN(x, y) ((x) < (y) ? (x) : (y))

/** Type check for int. */
#define ENSURE_int(i)   _Generic((i), int:   (i))
/** Type check for float. */
#define ENSURE_float(f) _Generic((f), float: (f))

/** Finds typed max value. */
#define MAX(type, x, y) \
  (type)GENERIC_MAX(ENSURE_##type(x), ENSURE_##type(y))
/** Finds typed min value. */
#define MIN(type, x, y) \
  (type)GENERIC_MIN(ENSURE_##type(x), ENSURE_##type(y))

#define CLAMP(type, v, a, b) \
   MAX(type, MIN(type, v, b), a)