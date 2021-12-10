# Makefile for libcobalt
# by Eliot Baez

# edit these to fit your configuration
PREFIX=/usr/local
CC = gcc
RM = rm

lib/libcobalt.so: src/findword.c src/sentence.c src/blocksize.c src/wordtable.h src/wordmap.h src/guidetable.h
	$(CC) -I include/ -o lib/libcobalt.so -shared -fPIC src/findword.c src/sentence.c src/blocksize.c

map/construct_guidetable: map/construct_guidetable.c src/wordmap.h src/wordtable.h
	$(CC) -I src/ -o map/construct_guidetable map/construct_guidetable.c
map/construct_map: map/construct_map.c src/wordtable.h
	$(CC) -I src/ -o map/construct_map map/construct_map.c
map/guidetable.bin: map/construct_guidetable
	cd map && ./construct_guidetable
map/uint32_array_to_c: map/uint32_array_to_c.c
	$(CC) -o map/uint32_array_to_c map/uint32_array_to_c.c
map/wordmap.bin: map/construct_map
	cd map && ./construct_map

plaintext/sort_wordlist: plaintext/sort_wordlist.c
	$(CC) -o plaintext/sort_wordlist plaintext/sort_wordlist.c
plaintext/50k-newline-separated.txt: plaintext/uncomment.py
	cd plaintext && python3 uncomment.py
plaintext/50k-newline-separated-sorted.txt: plaintext/sort_wordlist plaintext/50k-newline-separated.txt
	cd plaintext && ./sort_wordlist

src/guidetable.h: map/guidetable.bin util/c_hexdump
	cd map && ../util/c_hexdump 2 guidetable.bin ../src/guidetable.h
src/wordmap.h: map/wordmap.bin util/c_hexdump
	cd map && ../util/c_hexdump 4 wordmap.bin ../src/wordmap.h
src/wordtable.h: plaintext/50k-newline-separated-sorted.txt plaintext/convert_to_c.py
	cd plaintext && python3 convert_to_c.py

util/c_hexdump: util/c_hexdump.c
	$(CC) -o util/c_hexdump util/c_hexdump.c

install: lib/libcobalt.so include/cobalt.h
	cp lib/libcobalt.so $(PREFIX)/lib/libcobalt.so
	cp include/cobalt.h $(PREFIX)/include/cobalt.h
uninstall:
	$(RM) -f $(PREFIX)/lib/libcobalt.so $(PREFIX)/include/cobalt.h

default: lib/libcobalt.so

examples/encode: examples/encode.c
	$(CC) -I include/ -o examples/encode examples/encode.c -lcobalt
examples/decode: examples/decode.c
	$(CC) -I include/ -o examples/decode examples/decode.c -lcobalt
examples: examples/encode examples/decode

# remove intermediate products
clean:
	$(RM) -f map/guidetable.bin map/wordmap.bin
	$(RM) -f plaintext/50k-newline-separated.txt plaintext/50k-newline-separated.txt

# Remove even the stuff that we actually want. This is usually locally-
# generated header files and executables.
clean-all: clean
	$(RM) -f examples/encode examples/decode
	$(RM) -f lib/libcobalt.so
	$(RM) -f map/construct_map map/construct_guidetable
	$(RM) -f plaintext/sort_wordlist
	$(RM) -f src/guidetable.h src/wordmap.h src/wordtable.h
	$(RM) -f util/c_hexdump
