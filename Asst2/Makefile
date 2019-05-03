all: fileCompressor.c fileCompressor.o
	gcc -o fileCompressor fileCompressor.o

filecompressor.o: fileCompressor.h 
	gcc -c fileCompressor.h

clean:
	rm ./fileCompressor ./*.o
		echo Clean done
