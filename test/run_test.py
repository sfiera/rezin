#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright 2017 The Procyon Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import absolute_import, division, print_function, unicode_literals

import collections
import os
import subprocess
import sys

TEST = os.path.dirname(os.path.realpath(__file__))
ROOT = os.path.dirname(TEST)
REZIN = os.path.join(ROOT, "out/cur/rezin")
TEST = os.path.relpath(TEST)
ROOT = os.path.relpath(ROOT)
REZIN = os.path.relpath(REZIN)


def test_ls(source):
    ls = lambda *args: subprocess.check_output(source + ["ls"] + list(map(str, args))).decode("utf-8")

    assert ls() == ("PICT\n"
                    "RECT\n"
                    "STR#\n"
                    "TEXT\n"
                    "TMPL\n"
                    "cicn\n"
                    "clut\n"
                    "snd \n"
                    "url \n"
                    "vers\n")
    assert ls("TMPL") == ("128\tTMPL\n"
                          "129\tclut\n"
                          "130\tRECT\n"
                          "131\tSTR#\n"
                          "132\tTEXT\n"
                          "133\turl \n"
                          "134\tvers\n")
    assert ls("TMPL", 128) == ("128\tTMPL\n")


def test_cat(source):
    cat = lambda *args: subprocess.check_output(source + ["cat"] + list(map(str, args)))

    assert cat("RECT", 128) == b"\000\000\000\000\000\040\000\040"


def test_convert_text(source):
    convert = lambda *args: subprocess.check_output(source + ["convert"] + list(map(str, args))).decode("utf-8")

    assert convert("TEXT", 128) == "TEXT 128"
    assert convert("TEXT", 129) == "1\n2\n3\n"
    assert convert("TEXT", 130) == "“Ain’t” ain’t åin’t."

    assert convert("TEXT", 129, "-l", "cr") == "1\r2\r3\r"
    assert convert("TEXT", 129, "-l", "nl") == "1\n2\n3\n"
    assert convert("TEXT", 129, "-l", "crnl") == "1\r\n2\r\n3\r\n"


def test_convert_str(source):
    convert = lambda *args: subprocess.check_output(source + ["convert"] + list(map(str, args))).decode("utf-8")

    assert convert("STR#", 128) == ("[\n"
                                    "  \"STR#\",\n"
                                    "  \"String\",\n"
                                    "  \"List\",\n"
                                    "  \"resource\",\n"
                                    "  \"type\"\n"
                                    "]")


def test_convert_clut(source):
    convert = lambda *args: subprocess.check_output(source + ["convert"] + list(map(str, args))).decode("utf-8")

    assert convert("clut", 128) == ("{\n"
                                    "  \"0\": {\n"
                                    "    \"blue\": 0,\n"
                                    "    \"green\": 0,\n"
                                    "    \"red\": 65535\n"
                                    "  },\n"
                                    "  \"1\": {\n"
                                    "    \"blue\": 0,\n"
                                    "    \"green\": 65535,\n"
                                    "    \"red\": 0\n"
                                    "  },\n"
                                    "  \"2\": {\n"
                                    "    \"blue\": 65535,\n"
                                    "    \"green\": 0,\n"
                                    "    \"red\": 0\n"
                                    "  }\n"
                                    "}")


def test_convert_snd(source):
    convert = lambda *args: subprocess.check_output(source + ["convert"] + list(map(str, args)))

    assert convert("snd ", 128) == open(os.path.join(TEST, "coin.aiff"), "rb").read()


def test_convert_pict(source):
    convert = lambda *args: subprocess.check_output(source + ["convert"] + list(map(str, args)))

    assert convert("PICT", 128) == open(os.path.join(TEST, "ozma.png"), "rb").read()


def test_convert_cicn(source):
    convert = lambda *args: subprocess.check_output(source + ["convert"] + list(map(str, args)))

    assert convert("cicn", 128) == open(os.path.join(TEST, "red-circle.png"), "rb").read()
    assert convert("cicn", 129) == open(os.path.join(TEST, "oz.png"), "rb").read()


def pytest_generate_tests(metafunc):
    sources = collections.OrderedDict([
        ("as", [REZIN, "-a", os.path.join(TEST, "testdata.as")]),
        ("rsrc", [REZIN, "-f", os.path.join(TEST, "testdata.rsrc")]),
        ("zip", [REZIN, "-z", os.path.join(TEST, "testdata.zip,testdata.rsrc")]),
    ])
    metafunc.parametrize("source", sources.values(), ids=list(sources.keys()))


if __name__ == "__main__":
    import pytest
    raise SystemExit(pytest.main())
