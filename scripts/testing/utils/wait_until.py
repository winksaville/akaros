#!/usr/bin/python
"""Reads a file specified by arg1 until it detects that a line with arg2 is
printed into it.
"""
import sys
import time
import re


OUTPUT_FILE = sys.argv[1]
REGEX_END_PB_KERNEL_TESTS = r'^.*' + sys.argv[2] + '.*$'


def is_end_line(line) :
	if re.match(REGEX_END_PB_KERNEL_TESTS, line) :
		return True
	else :
		return False

def main() :
	file = open(OUTPUT_FILE, 'r')

	# Min and max waiting times between reading two lines of the file.
	MIN_WAITING_TIME = 0.1
	MAX_WAITING_TIME = 5
	WAITING_TIME_EXPONENT_BASE = 1.5 # Times what the waiting time is increased.

	wait_time = 2
	end_not_reached = True

	while end_not_reached :
		line = file.readline()
		
		if (len(line) == 0) :
			time.sleep(wait_time)
			# Sleep with exponential decay
			wait_time = MAX_WAITING_TIME if (wait_time > MAX_WAITING_TIME) \
				else wait_time * WAITING_TIME_EXPONENT_BASE
		else :
			wait_time = MIN_WAITING_TIME
			end_not_reached = not is_end_line(line)

	exit(0)

main()
