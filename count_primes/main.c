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

/**************************************************************************
 * Main routine for COUNT_PRIMES application.  Given the signed 64-bit
 * integers START and LENGTH, this application computes the number of
 * primes in the interval [START, START+LENGTH).  Negative numbers are
 * all treated as composite.  A non-positive value for LENGTH
 * corresponds to an empty interval.
 *
 * This program expects START, LENGTH, and START+LENGTH to be integers
 * in [-2^63, 2^63-1].  This program's behavior is undefined when any
 * of START, LENGTH, or START+LENGTH fall outside of this range.
 *
 * The MAIN() routine first invokes the PARSE_ARGUMENTS() helper
 * method to retrieve the START and LENGTH integers from the command
 * line arguments, and then it invokes COUNT_PRIMES_IN_INTERVAL() to
 * count the number of primes in [START, START+LENGTH).  The MAIN()
 * function times the execution time of COUNT_PRIMES_IN_INTERVAL() and
 * prints the result of COUNT_PRIMES_IN_INTERVAL() and its running
 * time to STDOUT upon completion.
 *
 * When the --verify flag is passed, the program checks the result of
 * COUNT_PRIMES_IN_INTERVAL() by counting the number of primes in
 * [START, START+LENGTH) using trial division.  Trial division tests
 * the primality of an integer P by checking each integer D in [2,
 * \sqrt{P}] to see if D divides P.
 *************************************************************************/

// FASTTIME.H has to be included very early, so just include it first.
#include <fasttime.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

// COUNT_PRIMES.{H,C} declares and defines COUNT_PRIMES_IN_INTERVAL().
#include "./count_primes.h"
// TRIALDIV.{H,C} declares and defines
// TRIALDIV_COUNT_PRIMES_IN_INTERVAL(), which is used to verify the
// result of COUNT_PRIMES_IN_INTERVAL() when the "--verify" flag is
// passed.
#include "./trialdiv.h"

/**************************************************************************
 * Helper methods for MAIN
 *************************************************************************/

// Print the usage for this program.
//
//   PROGRAM_NAME -- the name of this executable.
//
static void print_usage(const char *program_name) {
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "%s [--verify] <start> <length>\n", program_name);
  fprintf(stderr,
          "\tPrint the number of primes in [<start>,<start>+<length>), where <start>,\n"
          "\t<length>, and <start>+<length> are all nonnegative integers less than\n"
          "\t2^{63}.\n");
  fprintf(stderr, "\t--verify: Verify the result using trial division.\n");
  fprintf(stderr, "%s -h\n", program_name);
  fprintf(stderr, "\tPrint this help message.\n");
}

// Helper function of MAIN() to parse the command-line arguments.
//
//   START -- Pointer to variable storing low endpoint of input
//     interval.
//
//   LENGTH -- Pointer to variable storing the input interval's
//     length.
//
//   VERIFY -- Pointer to storage for boolean flag dictating whether
//     to verify result.
//
//   ARGC, ARGV -- Command-line arguments originally passed to MAIN.
//
static void parse_arguments(int64_t *start, int64_t *length, bool *verify,
                            int argc, char *argv[]) {
  if (argc < 2) {
    // Print usage and quit
    print_usage(argv[0]);
    exit(1);
  }

  *verify = false;
  *start = 0;
  *length = 0;

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-h") == 0) {
      print_usage(argv[0]);
      exit(1);
    } else if (strcmp(argv[i], "--verify") == 0) {
      *verify = true;
    } else {
      *start = atol(argv[i]);
      ++i;
      if (argc == i) {
        print_usage(argv[0]);
        exit(1);
      }
      *length = atol(argv[i]);
    }
  }
}


/**************************************************************************
 * MAIN()
 *************************************************************************/

int main(int argc, char *argv[]) {
  int64_t num_primes;
  int64_t start, length;
  bool verify;

  // Parse the command-line arguments
  parse_arguments(&start, &length, &verify, argc, argv);

  // Get the start time
  fasttime_t begin = gettime();
  // Count the primes in the specified interval
  num_primes = count_primes_in_interval(start, length);
  // Get the end time
  fasttime_t end = gettime();

  // Print the number of primes found and the running time.
  printf("%"PRId64" primes found in [%"PRId64", %"PRId64")\n",
         num_primes, start, start + length);

  printf("%f seconds\n", tdiff(begin, end));

  // If "--verify" is specified, check the result of
  // COUNT_PIMRES_IN_INTERVAL() using trial division to count the
  // number of primes in [START, START+LENGTH).
  if (verify) {
    int64_t trialdiv_num_primes
        = trialdiv_count_primes_in_interval((uint64_t)start, (uint64_t)length);
    if (trialdiv_num_primes != num_primes) {
      fprintf(stderr,
              "trialdiv_num_primes (%"PRId64") does not match num_primes (%"PRId64")\n",
              trialdiv_num_primes, num_primes);
      exit(1);
    }
  }

  return 0;
}
