
# Byte Pair Encoding

Think of a file as an array of numbers 0..255.

Build a histogram of two-number sequences in the file.

Assign a new number (>255) to the most common sequence, and replace each
instance of the sequence with its assigned number.

We repeat this process as long as there is a two-number sequence that appears
at least two times.

Now we store the modified array and a table that explains the replacements.

This is known as byte-pair encoding, and it's pretty effective at mapping
real-world data to shorter representations. Given a good encoding of the table
and final array, it shrinks the output of `gcc-13 bpe.cpp' by about 68%, when
gzip shrinks it by 72% on its slowest setting.

Obviously, not all data is mapped to a shorter representation. (or it would not
be injective and thus not reversible)
