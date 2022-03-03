Name: Sam Kelly
unikey: skel4720

# VR2017 COMP2017 Assignment 1

In this program I have made use of the stdio.h file operations (fopen(), fclose(), fgetc()) in combinationb with the "rb" (read binary) mode to read binary data from the given files.
In combination with a relatively simple data parsing system that involves checking chunks for delimiter values, different bytes within the chunks and checksums, we are able to decode the information contained in the files to retrieve meaningful data as it was intended to be read.
In the first example the file reader progresses as such:

Bytes at offset 0 through 4 are read and then parsed and processed as normal (swizzle value is verified, the swizzle operation is performed and the checksum is validated), then at offset 5 the first byte of the delimiter is read and noted. At offsets 6, 7 and 8 more bytes of the delimiter appear in the correct order and are recorded, signifying the end of the chunk. As there are no more bytes following the delimiter, the program ends.

In the second example, a similar process is followed. Bytes at offset 0 through 4 are processed normally, at the end of byte 4 the program begins a new packet to be recorded and continues, reading and verifying bytes from offset 5 to 9, which turn out to be invalid. Then another packet begins at 10, and bytes are verified from offset 10 to 14. Then the delimiter is read and verified from offset 15 to 18 and a new packet begins at 19 and is read and checked from offset 19 to 23.

In each of these examples, after the data is read and the End Of File is reached, the program closes the file with fclose() and ends. The data from the files are read in character by character with the fgetc() function, and are read in as unsigned characters to be processed as integers if they are valid.
The delimiter arguments that are passed in as command line arguments are converted to integers with some manual format checking and use of the strtol() function of ctype.h. The program will return an error and end if any delimiter values are invalid, if the wrong number of arguments are passed to the program or if the file does not exist.
