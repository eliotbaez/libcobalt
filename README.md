# CObaLT - Compression, Obviously by a Lookup Table

CObaLT is a text-compression library that uses a lookup table to compress text
on a word-by-word basis.

## Installing

### Linux

Installing on Linux and other unix-like systems is fairly straightforward.
Before proceeding, make sure you have both `make` and `CMake` installed. You may
need root privileges to fully install the library.

```sh
mkdir build && cd build
cmake ..
make
make install
```

### Windows

On Windows, run the following commands in your terminal where you have access
to the necessary build tools, like `CMake` and your C compiler. 

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

If you find the best way to install the library to your system, please add that
information to this document.

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

`GUIDETABLE` is an arary of 16-bit integers that functions as the hash table for
words with respect to the first 2 characters of the word (See [The Sorting
Algorithm](#the-sorting-algorithm) above). `GUIDETABLE` exists to expedite the
process of finding words in the word table. For a more detailed explanation of
how CObaLT finds a word in the word table, see the definition of
`cblt_findWord()` in `src/findword.c`.

The elements in `GUIDETABLE` are organized in such away that the nth element of
`GUIDETABLE` is the index of the **first word** in WORDMAP whose first 2
characters, when concatenated, are equal to n. Using the same "the" example from
above, an example of using `GUIDETABLE` to help locate a word would be:

```c
uint16_t first2chars = 0x7468;
uint16_t searchStartPoint = GUIDETABLE[first2chars];
const char *firstMatch = WORDMAP[searchStartPoint];
/* firstMatch is now the index of the first word in WORDTABLE whose first 2
   characters are equal to "th". */
```

In this case, if the words in WORDTABLE are arranged in descending order of
frequency, `firstMatch` would point to the word "the". However, this will not
always be the case for all words, which is why it is important to keep searching
through the words in WORDTABLE until a true match is found. The use of
GUIDETABLE just allows us to skip past thousands of words with an O(1) time
complexity.
