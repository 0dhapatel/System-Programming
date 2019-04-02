all: fileCompressor.c huffmancode.o

        gcc -o fileCompressor fileCompressor.c huffmancode.o




huffmancode.o: huffmancode.c huffmancode.h

        gcc -c huffmancode.c




clean:

        rm ./fileCompressor ./*.o

            echo Clean done
