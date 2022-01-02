#!/usr/bin/env python

import sys
import cxxfilt
import struct

class Symbol:
    def __init__(self, addr, name) -> None:
        self.address = addr
        self.name = name

def symbol_sorter(e):
    return e.address

def main() -> int:
    syms = []

    dir = sys.argv[1]

    with open(dir + '/kernel.sym') as f:
        for linet in f:
            line = linet.rstrip('\n')
            tok = line.split(" ")

            name = tok[2]
            name = cxxfilt.demangle(name)

            syms.append(Symbol(int(tok[0], 16), name))


    syms.sort(key=symbol_sorter)
    numSyms = len(syms)
    offset_strings = (numSyms * 16) + 8 # Sizeof 2 pointers, plus offset of the # syms

    fout = open(dir + '/syms.bin', 'wb')
    fout.write(struct.pack("Q", numSyms))

    current_string_offset = offset_strings
    for s in syms:
        fout.write(struct.pack("QQ", s.address, current_string_offset))
        current_string_offset += len(s.name) + 1

    for s in syms:
        name = bytes(s.name.encode('ascii')) + b'\x00'
        fout.write(name)

    fout.close()

    return 0

# Make this file executable...
if __name__ == '__main__':
    sys.exit(main())