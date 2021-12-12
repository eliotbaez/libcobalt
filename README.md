# CObaLT - Compression, Obviously by a Lookup Table

CObaLT is a text-compression library that uses a lookup table to compress text
on a word-by-word basis.

## Building

### Linux

On Linux-based systems, building is as easy as running `make` or `make default`
on your command line at the root directory. Optionally, run `make clean`
afterward to get rid of some unnecessary intermediate files.

```sh
make -f Makefile.old default
make -f Makefile.old clean
```

### Windows or Linux (experimental)

We're currently experimenting with using CMake to streamline the build process.
On Windows or Linux, run the following commands in your terminal where you have
access to the necessary build tools, like `CMake` and your C compiler.

```sh
mkdir build
cd build
cmake ..
```

On Linux, run `make` immediately once that's done. On Windows, good luck.

## Installing

The default installation path is in `/usr/local`. This can be configured
directly inside `Makefile`. Root permissions are required to install to
`/usr/local.`

```sh
sudo make install
```

## How it Works

### The Word List

The entire functionality of CObaLT revolves around a lookup table. In the
simplest of terms, this lookup table is just a list of words. By default, the
word list used to compile CObaLT is `plaintext/wiki-100k.txt`. From here, only
the first 50,000 words are used. Any lines beginning with `#` are ignored as
comments, and the filtered words are placed in
`plaintext/50k-newline-separated.txt`.

### The Sorting Algorithm

This is a bit of a misnomer, since the function doing the actual sorting is just
the `qsort()` function from the standard C library. The real magic happens in
how word strings are compared. From the newline-separated word list, all newline
characters are converted to null characters. The first two characters of each
word are concatenated into a single 16-bit integer that can be directly compared
with another 2-byte integer. The first 2 bytes may include the null terminator.

```
       +----+----+----+----+-   -+----+----+--
Chars: | t  | h  | e  | \0 | ... | a  | \0 |
       +----+----+----+----+-   -+----+----+--
Chars: |0x74|0x68|0x65|0x00| ... |0x61|0x00|
       +----+----+----+----+-   -+----+----+--
Ints:  | 0x7468  |         | ... | 0x6100  |
       +---------+---------+-   -+---------+--
```

NOTE: The actual order that the bytes appear in the 16-bit integer depend on the
endianness of the machine where CObaLT is compiled. This diagram is accurate for
big-endian machines, since that makes the demonstration more intuitive. However,
the process works equally well on little-endian machines, since the same machine
that does the encoding is doing the decoding as well.

This sorting is done to make it significantly easier to look up words when 
encoding text, as we can just take the combined value of the first 2 characters
of each word and look it up in a hash table.

Once the words are sorted, a null-separated version of the separated word list
is written to a header file as a `const char[]`, to be hard-coded into the
library under the name `WORDTABLE`.

### The WORDMAP Array

`WORDMAP` is an array of 32-bit integers that provides us a layer of abstraction
from `WORDTABLE`. Every element in `WORDMAP` stores an index within `WORDTABLE`,
such that the nth element of `WORDMAP` is the index of the first character of
the nth word in `WORDTABLE`. In other words,

```c
const char *word807 = WORDTABLE + WORDMAP[807];
/* word807 is now a valid null-terminated string */
```

We can do this by the virtue of the fact that the words in `WORDTABLE` are
null-separated.

### The GUIDETABLE Array

I'm tired right now. I'll finish writing this documentation when I wake up
tomorrow.
