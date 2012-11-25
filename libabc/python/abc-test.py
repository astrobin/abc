#! /usr/bin/env python

import unittest
import PyABC

class ABCTest(unittest.TestCase):
    def test_load(self):
        image = PyABC.Image.fromFile('../tests/1_32i.fit')
        self.assertTrue(isinstance(image, PyABC.Image))

        self.assertEqual(image.type(), PyABC.ImageType.Light)
        self.assertEqual(int(image.temperature()), -20)
        self.assertEqual(image.cameraModel(), 'G2-1600, Id: 2115')

if __name__ == '__main__':
    unittest.main()
