lib/libcobalt.so: src/findword.c src/sentence.c src/blocksize.c src/wordtable.h src/wordmap.h src/guidetable.h
	gcc -I include/ -o lib/libcobalt.so -shared -fPIC src/findword.c src/sentence.c src/blocksize.c

map/construct_guidetable: map/construct_guidetable.c src/wordmap.h src/wordtable.h
	gcc -I src/ -o map/construct_guidetable map/construct_guidetable.c

map/construct_map: map/construct_map.c src/wordtable.h
	gcc -I src/ -o map/construct_map map/construct_map.c

map/guidetable.bin: map/construct_guidetable
	cd map && ./construct_guidetable

map/uint32_array_to_c: map/uint32_array_to_c.c
	gcc -o map/uint32_array_to_c map/uint32_array_to_c.c

map/wordmap.bin: map/construct_map
	cd map && ./construct_map

plaintext/sort_wordlist: plaintext/sort_wordlist.c
	gcc -o plaintext/sort_wordlist plaintext/sort_wordlist.c

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
	gcc -o util/c_hexdump util/c_hexdump.c

default: lib/libcobalt.so

# remove intermediate products
clean:
	rm -f map/guidetable.bin
	rm -f map/wordmap.bin
	rm -f plaintext/50k-newline-separated.txt
	rm -f plaintext/50k-newline-separated.sorted.txt

# Remove even the stuff that we actually want. This is usually locally-
# generated header files and executables.
clean-all: clean
	rm -f lib/libcobalt.so
	rm -f map/construct_map
	rm -f map/construct_guidetable
	rm -f map/uint32_array_to_c
	rm -f plaintext/sort_wordlist
	rm -f src/guidetable.h
	rm -f src/wordmap.h
	rm -f src/wordtable.h
	rm -f util/c_hexdump
