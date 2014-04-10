# This file is a proof of concept for running tests from Jenkins.
# It needs to be modified to run actual Akaros tests and parse stdout or
# something to extract test results and pass/fail based on them.
try:
	import unittest2 as unittest
except ImportError:
	import unittest

from nose.plugins.attrib import attr

import re

class PBKernelTestsParser() :
	"""This class is a helper for parsing the output from post-boot kernel tests
	ran inside AKAROS.
	"""

	REGEX_BEGIN_PB_KERNEL_TESTS = r'^.*BEGIN_PB_KERNEL_TESTS.*$'
	REGEX_END_PB_KERNEL_TESTS = r'^.*END_PB_KERNEL_TESTS.*$'

	def __init__(self, test_output_path) :
		self.test_output = open(test_output_path, 'r')
		self.__advance_to_beginning_of_tests()

	def __advance_to_beginning_of_tests(self) :
		beginning_reached = False
		while not beginning_reached :
			line = self.test_output.readline()
			if (re.match(self.REGEX_BEGIN_PB_KERNEL_TESTS, line)) :
				beginning_reached = True

	def next_test(self) :
		"""Parses the next test from the test output file.
		Returns:
			First, True if there was a next test and we had not reached the end.
			Second, True if the test passed.
			Third, a String with the name of the test.
		"""
		# Look for test.
		line = ''
		while len(line) < 8 :
			line = self.test_output.readline()
			# TODO: Exit if EOF

		if (re.match(self.REGEX_END_PB_KERNEL_TESTS, line)) :
			return False, False, ''
		else : 
			# TODO: Parse actual test output with:
			# self.__extract_test_result
			# self.__extract_test_name
			# self.__extract_test_fail_msg
			return True, True, line

class TestWrapper(unittest.TestCase):
	# TODO: programatically add test cases and see if it works.

	@attr('kernel','cross-compiler','userspace','busybox')
	def test_kern(self):
		self.assertEqual(10, 7 + 3)
		self.assertEqual(10, 7 + 3)
		# 'tmp/akaros_out.txt'

	@attr('kernel','cross-compiler','userspace','busybox')
	def test_buti(self):
		parser = PBKernelTestsParser('deleteme')
		while True :
			a, b, c = parser.next_test()
			if not a :
				break
			print a
			print b
			print c
			print "*****"
			self.assertTrue(b)

# TODO: for each test, tag: @attr('kernel','cross-compiler','userspace','busybox')
