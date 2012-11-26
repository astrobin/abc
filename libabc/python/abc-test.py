#! /usr/bin/env python

import unittest
import datetime
import PyABC

class ABCTest(unittest.TestCase):
    def test_load(self):
        image = PyABC.Image.fromFile('../tests/1_32i.fit')
        self.assertTrue(isinstance(image, PyABC.Image))

        self.assertEqual(image.type(), PyABC.ImageType.Light)
        self.assertEqual(int(image.temperature()), -20)
        self.assertEqual(image.cameraModel(), 'G2-1600, Id: 2115')
        self.assertEqual(image.observationDate(),
                datetime.datetime(2012, 9, 16, 21, 2, 19))

if __name__ == '__main__':
    unittest.main()
