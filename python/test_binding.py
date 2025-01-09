# Tank Game (@kennedyengineering)

import sys
import os

# Add the build directory to the sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../build")))

import python_bindings

import pytest


class TestBinding:
    def test_one(self):
        x = "this"
        assert "h" in x
