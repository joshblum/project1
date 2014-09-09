/**
 * Copyright (c) 2014 MIT License by 6.172 Staff
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 **/

#ifndef INCLUDED_TRIALDIV_DOT_H
#define INCLUDED_TRIALDIV_DOT_H

#include <stdbool.h>
#include <inttypes.h>

// Use trial division to test if P is prime.  Returns TRUE if P is
// prime, FALSE otherwise.
//
//   P -- The integer to test for primality.
//
bool trialdiv_prime_p(int64_t p);

// Return the number of primes in [START, START+LENGTH), using trial
// division to count this number.
//
//   START -- The low endpoint of the interval.
//
//   LENGTH -- The length of the interval.
int64_t trialdiv_count_primes_in_interval(int64_t start, int64_t length);

#endif  // INCLUDED_TRIALDIV_DOT_H
