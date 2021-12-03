plaintext/50k-newline-separated.txt:
	cd plaintext && python3 uncomment.py

src/wordtable.h: plaintext/50k-newline-separated.txt
	cd plaintext && python3 convert_to_c.py

clean:
	rm -f plaintext/50k-newline-separated.txt

clean-all: clean
	rm -f src/wordtable.h
