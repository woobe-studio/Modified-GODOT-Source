#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

header = """\
/**************************************************************************/
/*  $filename                                                             */
/**************************************************************************/

"""

fname = sys.argv[1]

# Handle replacing $filename with actual filename and keep alignment
fsingle = fname.strip()
if fsingle.find("/") != -1:
    fsingle = fsingle[fsingle.rfind("/") + 1 :]
rep_fl = "$filename"
rep_fi = fsingle
len_fl = len(rep_fl)
len_fi = len(rep_fi)
# Pad with spaces to keep alignment
if len_fi < len_fl:
    for x in range(len_fl - len_fi):
        rep_fi += " "
elif len_fl < len_fi:
    for x in range(len_fi - len_fl):
        rep_fl += " "
if header.find(rep_fl) != -1:
    text = header.replace(rep_fl, rep_fi)
else:
    text = header.replace("$filename", fsingle)
text += "\n"

# We now have the proper header, so we want to ignore the one in the original file
# and potentially empty lines and badly formatted lines, while keeping comments that
# come after the header, and then keep everything non-header unchanged.
# To do so, we skip empty lines that may be at the top in a first pass.
# In a second pass, we skip all consecutive comment lines starting with "/*",
# then we can append the rest (step 2).

fileread = open(fname.strip(), "r")
line = fileread.readline()
header_done = False

while line.strip() == "":  # Skip empty lines at the top
    line = fileread.readline()

if line.find("/**********") == -1:  # Godot header starts this way
    # Maybe starting with a non-Godot comment, abort header magic
    header_done = True

while not header_done:  # Handle header now
    if line.find("/*") != 0:  # No more starting with a comment
        header_done = True
        if line.strip() != "":
            text += line
    line = fileread.readline()

while line != "":  # Dump everything until EOF
    text += line
    line = fileread.readline()

fileread.close()

# Write
filewrite = open(fname.strip(), "w")
filewrite.write(text)
filewrite.close()
