#!/usr/bin/env python
#
# Script to run count_primes tests.
#
import os, os.path, glob, sys, subprocess, re

# Prints usage information for this script.
def __usage():
    sys.stderr.write("Usage:\n%s [-q] [--cloud] <testfile>.csv ...\n" % __file__)
    sys.stderr.write("\tRun all tests defined in each testfile on ./count_primes.\n")
    sys.stderr.write("\tProcessing <testfile>.csv produces the results file <testfile>.results.\n")
    sys.stderr.write("\t-q: Enables quiet mode, which represses unnecessary status output.\n")
    sys.stderr.write("\t--cloud: Runs tests on the current machine, rather than on Lanka.\n")
    sys.stderr.write("%s -h\n" % __file__)
    sys.stderr.write("\tDisplay this message.\n")
    exit(0)

# Run tests defined in TESTFILE using EXECUTABLE.  Suppress non-error
# output when QUIET is TRUE.
def process_testfile(testfile, executable, quiet, cloud):
    lrun = "lrun"
    failures = 0

    # Derive RESULTSFILE from TESTFILE
    testfile_parts = testfile.rsplit('/', 1)
    testdir = "."
    testfilename = testfile
    if len(testfile_parts) == 2:
        testdir = testfile_parts[0]
        testfilename = testfile_parts[1]

    resultsfile = testdir + '/' # + executable.rsplit('/', 1)[-1] + "-"
    resultsfile += testfilename.rsplit('.', 1)[0]
    resultsfile += ".results"

    # Open the test file for reading
    try:
        testfo = open(testfile, "r")
    except IOError:
        sys.stderr.write("ERROR: Failed to open file \"%s\" for reading.\n" % testfile)
        return 1

    # Open the results file for writing
    try:
        resultsfo = open(resultsfile, "w")
    except IOError:
        sys.stderr.write("ERROR: Failed to open file \"%s\" for writing.\n" % resultsfile)
        return 1
    if not quiet:
        sys.stdout.write("Writing results to \"%s\".\n" % resultsfile)
    resultsfo.write("# Result format:\n# start\tlength\toutcome\tdata\n")
    resultsfo.write("# If outcome is PASSED, data is running time.\n")
    resultsfo.write("# If outcome is BADCOUNT, data is <received_count>!=<expected_cound>.\n")
    resultsfo.write("# If outcome is BADTEST, data is empty.\n")
    resultsfo.write("# If outcome is ERROR, data is the error code.\n")

    # Execute the tests defined in testfile
    try:
        # Read testfile line by line
        for line in testfo:
            # Skip comment lines, which start with '#'
            if "#" == line[0] or len(line.split()) == 0:
                continue

            # Attempt to parse the next line of the test file as a test.
            # Format of test file for project 1:
            # start    length    result
            # (start, length, expected_count) = line.split()
            test_line = line.strip().split()

            # Check that line has expected format
            if len(test_line) != 3:
                sys.stderr.write("ALERT: Error parsing line \"" + line.strip() + "\".  Skipping.\n")
                continue

            # Check that entries in line can be interpreted as longs
            start = test_line[0]
            length = test_line[1]
            expected_count = test_line[2]
            try:
                long(start)
                long(length)
                long(expected_count)
            except ValueError:
                sys.stderr.write("ALERT: Error parsing line \"" + line.strip() + "\".  Skipping.\n")
                continue

            # Verify that the test satisfies the specification
            if long(start) < 0 or long(start) > 2**63-1 or \
               long(length) < 0 or long(length) > 2**63-1 or \
               (long(start) + long(length)) < 0 or \
               (long(start) + long(length)) > 2**63-1:
                # Do not run tests that fail to meet specification
                sys.stderr.write("ALERT: Test \"" + start + " " + length +"\" does not meet specification.  Skipping.\n")
                result = start + "\t" + length + "\t" + "BADTEST\t"
                if long(start) < 0 or long(start) > 2**63-1:
                    result += start + ","
                if long(length) < 0 or long(length) > 2**63-1:
                    result += length + ","
                if (long(start) + long(length)) < 0 or \
                     (long(start) + long(length)) > 2**63-1:
                    result += start + "+" + length
                result += "\n"
                resultsfo.write(result)
                resultsfo.flush()
                continue
        
            # Prepare call to run test
            if cloud:
                call_count_primes = []
            else:
                call_count_primes = [lrun]
            call_count_primes += [executable, start, length]

            # Prepare result line
            result = start + "\t" + length + "\t"

            # Print the test being run
	    if not quiet:
                if cloud:
                    sys.stdout.write("Running \"" + ' '.join([executable, start, length]) + "\" on Cloud\n")
                else:
                    sys.stdout.write("Running \"" + ' '.join([executable, start, length]) + "\" on Lanka (use Ctrl-C Ctrl-C to terminate)\n")

            # Run the test
            cmd = subprocess.Popen(call_count_primes,
                                   stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            (test_result, test_stderr) = cmd.communicate()

            # If the test ran successfully, check the output
            if 0 == cmd.returncode:
                # Parse the output of the test
                test_result_array = test_result.split('\n')
                
                count_regex = re.compile(r'(\d+)\s+primes\s+found\s+in\s+\[(-?\d+),\s+(-?\d+)\)')
                time_regex = re.compile(r'(\d+\.\d+)\s+seconds')

                # Examine the lines of the output to find the reported count and running time
                for line in test_result_array:
                    count_match = count_regex.match(line)
                    time_match = time_regex.match(line)

                    # Record the success and running time of the test
                    if None != count_match:
                        count = count_match.group(1)
                        low = count_match.group(2)
                        high = count_match.group(3)
                        if long(start) != long(low) or \
                           long(start)+long(length) != long(high):
                            sys.stderr.write("WARNING: Test specified the interval [%l, %l), "\
                                             "but %s returned a result for the interval [%l, %l)\n" % \
                                             (long(start), long(start)+long(length), executable,
                                              long(low), long(high)))

                    if None != time_match:
                        time = time_match.group(1)
                        
                if count == expected_count:
                    result += "PASSED\t" + time + "\n"
                else:
                    result += "BADCOUNT\t" + count + "!=" + expected_count + "\n"
                    failures += 1

            else:
                print test_result
                # The test returned a failing (nonzero) error code
                result += "ERROR\t" + str(cmd.returncode) + "\n"
                failures += 1

            # Write result to resultsfile
            resultsfo.write(result)
            resultsfo.flush()

    except KeyboardInterrupt:
        sys.stderr.write("Caught signal; terminating tests early.\n")

    testfo.close()
    resultsfo.close()
    return failures
    

def main(argv = None):
    quiet = False
    cloud = False

    # Parse initial command-line arguments
    if argv is None:
        argv = sys.argv
    if len(argv) < 2 or "-h" == argv[1]:
        __usage()

    filename_index = 1
    while "-q" == argv[filename_index] or "--cloud" == argv[filename_index]:
        if "-q" == argv[filename_index]:
            quiet = True
            filename_index += 1
        if "--cloud" == argv[filename_index]:
            cloud = True
            filename_index += 1

    if len(argv) == filename_index:
        __usage()

    # Find setup_lanka script
    setup_lanka = "./.setup_lanka"
    if not os.path.isfile(setup_lanka):
        setup_lanka = "./setup_lanka"
        if not os.path.isfile(setup_lanka):
            sys.stderr.write("ERROR: Could not find %s.  Aborting.\n" % setup_lanka)
            exit(1)

    # This script is specific for executing "./count_primes".
    progname = "count_primes"
    executable = "./" + progname
    if not os.path.isfile(executable):
        if not quiet:
            sys.stdout.write("Making %s.\n" % executable)
        # Attempt to compile executable using make
        if cloud:
            ret = subprocess.call(["make", "CLOUD=1", progname])
        else:
            ret = subprocess.call(["make", progname])

        if 0 != ret:
            sys.stderr.write("ERROR: make returned exit status %d.  Aborting.\n" % ret)
            exit(1)
        if not os.path.isfile(executable):
            sys.stderr.write("ERROR: %s not found after running make.  Aborting.\n" % executable)
            exit(1)

    if not cloud:
        # Set up the lanka cluster for running the executable
        setup_command = [setup_lanka]
        ret = subprocess.call(setup_command)
        if 0 != ret:
            sys.stderr.write("ERROR: %s returned exit status %d.  Aborting.\n", setup_lanka)
            exit(1)

    # Process testfile arguments
    failures = 0
    try:
        for testfile_arg in argv[filename_index:]:
            for testfile in glob.glob(testfile_arg):
                failures += process_testfile(testfile, executable, quiet, cloud)

    except KeyboardInterrupt:
        sys.stderr.write("Caught signal; terminating tests early.\n")
        return 1

    return failures

if __name__ == "__main__":
    sys.exit(main())
