# uncomment.py
# by Eliot Baez
# 
# Removes #comments from the word list, to prepare it to be used as a
# binary file

if __name__ == "__main__":
    count = 0;

    with open("wiki-100k.txt", "rt") as i, \
            open("50k-newline-separated.txt", "wt", newline='\n') as o:
        while count < 50000:
            line = i.readline();
            if line[0] != '#':
                o.write(line)
                count += 1
