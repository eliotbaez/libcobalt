map/construct_map: map/construct_map.c src/wordtable.h
	gcc -Isrc -o map/construct_map map/construct_map.c

map/wordmap.bin: map/construct_map
	cd map && ./construct_map

plaintext/sort_wordlist: plaintext/sort_wordlist.c
	gcc -o plaintext/sort_wordlist plaintext/sort_wordlist.c

plaintext/50k-newline-separated.txt: plaintext/uncomment.py
	cd plaintext && python3 uncomment.py

plaintext/50k-newline-separated-sorted.txt: plaintext/sort_wordlist plaintext/50k-newline-separated.txt
	cd plaintext && ./sort_wordlist

src/wordtable.h: plaintext/50k-newline-separated-sorted.txt plaintext/convert_to_c.py
	cd plaintext && python3 convert_to_c.py

# remove intermediate products
clean:
	rm -f plaintext/50k-newline-separated.txt
	rm -f plaintext/50k-newline-separated.sorted.txt

# remove even the stuff that we actually want
clean-all: clean
	rm -f map/wordmap.bin
	rm -f map/construct_map
	rm -f plaintext/sort_wordlist
	rm -f src/wordtable.h
