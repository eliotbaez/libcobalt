map/construct_map: map/construct_map.c src/wordtable.h
	gcc -Isrc -o map/construct_map map/construct_map.c

map/wordmap.bin: map/construct_map
	cd map && ./construct_map

plaintext/50k-newline-separated.txt:
	cd plaintext && python3 uncomment.py

src/wordtable.h: plaintext/50k-newline-separated.txt
	cd plaintext && python3 convert_to_c.py

# remove intermediate products
clean:
	rm -f plaintext/50k-newline-separated.txt
	rm -f map/construct_map

# remove even the stuff that we actually want
clean-all: clean
	rm -f src/wordtable.h
	rm -f map/wordmap.bin
