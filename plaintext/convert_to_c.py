# convert_to_c.py
# by Eliot Baez
# 
# Takes the sorted newline-separated wordlist as input, and converts it
# into a block of C code that can be included as a header file.
#
# The format of this script's output is inspired by xxd.

NUMBER_OF_WORDS = 50000

if __name__ == "__main__":
    with open("50k-newline-separated-sorted.txt", "rt") as i, open("../src/wordtable.h", "wt") as o:
        o.write( ("#ifndef WORDTABLE_H\n"
            "#define WORDTABLE_H\n\n"
            "#define NUMBER_OF_WORDS 50000\n\n"
            "const char *WORDTABLE =\n") )
        
        length = 0;
        for count in range(49999):
            line = i.readline().rstrip('\n')
            length += len(line.encode("utf-8")) + 1
            # trust me, it works
            o.write("\t\"%s\\0\"\n" % line)
        
        # special case for last word
        line = i.readline().rstrip('\n')
        length += len(line) + 1
        o.write("\t\"%s\"\n" % line)
        
        o.write(";\n\n")
        o.write("const size_t WORDTABLE_LEN = %d;\n\n" % length)
        o.write("#endif /* WORDTABLE_H */")
