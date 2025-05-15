target remote  192.168.0.135:1234
add-symbol-file Kernel/kernel.elf 0x100000
add-symbol-file Userland/0000-sampleCodeModule.elf 0x400000

define src-prof
    dashboard -layout source expressions stack variables
    dashboard source -style height 20
end

define asm-prof
    dashboard -layout registers assembly memory stack
    dashboard registers -style list 'rax rbx rcx rdx rsi rdi rbp rsp r8 r9 r10 r11 r12 r13 r14 r15 rip eflags cs ss ds es fs gs fs_base gs_base k_gs_base cr0 cr2 cr3 cr4 cr8 efer'
end

python

# GDB dashboard - Modular visual interface for GDB in Python.
#
# https://github.com/cyrus-and/gdb-dashboard

# License ----------------------------------------------------------------------

# Copyright (c) 2015-2025 Andrea Cardaci <cyrus.and@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Imports ----------------------------------------------------------------------

import ast
import io
import itertools
import math
import os
import re
import struct
import traceback

# Common attributes ------------------------------------------------------------

class R():

    @staticmethod
    def attributes():
        return {
            # miscellaneous
            'ansi': {
                'doc': 'Control the ANSI output of the dashboard.',
                'default': True,
                'type': bool
            },
            'syntax_highlighting': {
                'doc': '''Pygments style to use for syntax highlighting.

Using an empty string (or a name not in the list) disables this feature. The
list of all the available styles can be obtained with (from GDB itself):

    python from pygments.styles import *
    python for style in get_all_styles(): print(style)''',
                'default': 'monokai'
            },
            'discard_scrollback': {
                'doc': '''Discard the scrollback buffer at each redraw.

This makes scrolling less confusing by discarding the previously printed
dashboards but only works with certain terminals.''',
                'default': True,
                'type': bool
            },
            # values formatting
            'compact_values': {
                'doc': 'Display complex objects in a single line.',
                'default': True,
                'type': bool
            },
            'max_value_length': {
                'doc': 'Maximum length of displayed values before truncation.',
                'default': 100,
                'type': int
            },
            'value_truncation_string': {
                'doc': 'String to use to mark value truncation.',
                'default': '…',
            },
            'dereference': {... (Tiempo restante: 45 KB)