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

#include "./trialdiv.h"

bool trialdiv_prime_p(int64_t p) {
  // The smallest prime is 2
  if (p < 2) {
    return false;
  }

  // Check all possibe divisors D of P
  for (int64_t d = 2; d * d <= p; ++d) {
    if (0 == p % d) {
      // D divides P
      return false;
    }
  }
  return true;
}

int64_t trialdiv_count_primes_in_interval(int64_t start, int64_t length) {
  int64_t num_primes = 0;

  // Return 0 primes for nonpositive-length intervals.
  if (length <= 0) {
    return 0;
  }

  // Return 0 primes for intervals whose high endpoint is at most 2.
  // Because we treat all negative numbers as composite, there are no
  // primes less than 2.
  if (start + length <= 2) {
    return 0;
  }

  // Ensure that the smallest value of START is 2.
  if (start < 2) {
    length -= 2 - start;
    start = 2;
  }

  // Check each integer in [START, START+LENGTH) for primality
  for (int64_t p = start; p < start + length; ++p) {
    if (trialdiv_prime_p(p)) {
      // Increment NUM_PRIMES to count prime P
      ++num_primes;
    }
  }
  return num_primes;
}
