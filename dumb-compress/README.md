
# dumb-compressor

A bad file compressor

It creates a table of what the most frequent byte value is after each byte then
it processes a file byte-by-byte and:

- if the following byte has the most frequent value, it outputs a 1-bit
- if it isn't, it outputs a 0-bit followed by the actual value

This is a net win if the most frequent byte value is really frequent.
Mathematically:

- let a be the number of appearances of the most frequent byte value
- let b be the number of appearances of non-most-frequent bytes
- then we get a win if a>(a+b)/8

So, it is a win if the most frequent byte value represents over an eight of all
bytes. This turns out to be the case for my source code at least.

## TO-DO

Turn on compression only for byte values where it is a win (the cost of encoding
this in the file would be 256 bits = 32 bytes)
