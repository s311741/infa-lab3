#! /usr/bin/python
import sys

SEEK_TAG = 0
IN_TAG_START = 1
IN_OPENING_TAG = 2
IN_CLOSING_TAG = 3

state = SEEK_TAG

indent_str = "  "
indent = 0
tag = ""
text = ""

leading_newline = ''

def is_any_text (t):
    return t != "" and not t.isspace()

for line in sys.stdin:
    for c in line:

        if state == SEEK_TAG:
            if c == '<':
                state = IN_TAG_START
            else:
                text += c

        elif state == IN_TAG_START:
            if c.isspace(): continue

            if c == '/':
                state = IN_CLOSING_TAG
            else:
                tag = "" + c
                state = IN_OPENING_TAG

        elif state == IN_OPENING_TAG:
            if is_any_text(text):
                raise Exception("Cannot represent both text and subtags in a tag")
            if c.isspace(): continue

            if c == '>':
                sys.stdout.write(leading_newline + indent_str * indent + tag + ':')
                leading_newline = '\n'
                indent += 1
                state = SEEK_TAG
                text = ""
            else:
                tag += c

        elif state == IN_CLOSING_TAG:
            if c == '>':
                indent -= 1
                state = SEEK_TAG
                if is_any_text(text):
                    text.strip()
                    sys.stdout.write(" \"" + text + '\"')
                text = ""
sys.stdout.write(leading_newline)
