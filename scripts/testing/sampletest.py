# This file is a proof of concept for running tests from Jenkins.
# It needs to be modified to run actual Akaros tests and parse stdout or
# something to extract test results and pass/fail based on them.
import random
try:
    import unittest2 as unittest
except ImportError:
    import unittest

class TestWrapper(unittest.TestCase):
    def test_kern(self):
        self.assertEqual(10, 7 + 3)
        self.assertEqual(10, 7 + 3)
        # 'tmp/akaros_out.txt'

    # def test_fail(self):
    #     self.assertEqual(11, 7 + 3)

# TODO: for each test, tag: @attr('kernel','compiler')
