Name: Sam Kelly
unikey: skel4720

# VR2017 COMP2017 Assignment 1

In this program I have made use of the stdio.h file operations (fopen(), fclose(), fgetc()) in
combinationb with the "rb" (read binary) mode to read binary data from the given files.
In combination with a relatively simple data parsing system that involves checking chunks for
delimiter values, different bytes within the chunks and checksums, we are able to decode the
information contained in the files to retrieve meaningful data as it was intended to be read.
In the first example the file reader progresses as such:

Bytes at offset 0 through 4 are read into an unprocessed array of ints. Then as the delimiter begins at
offset 5, the beginning of a possible delimiter is flagged and again written into the current chunk array.
At offsets 6, 7 and 8 more bytes of the delimiter appear and are written to memory in the correct order
and are recorded, signifying the end of the chunk. After this, the array of ints for the current chunk
parsed, verified for length and divisibility by 5 and processed as normal (swizzle value is
verified, the swizzle operation is performed, the checksum is validated, the coordinate values are
validated and the chunk averages are calculated.). As there are no more bytes following the
delimiter, the program ends.

In the second example, a similar process is followed. Bytes at offset 0 through 14 are read from file and
written into memory as an array of integers. Then the delimiter is read and verified from offset 15 to 18,
signifying the end of the current chunk. The program then validates the length of the current chunk, its
divisibility by 5 and the chunk is stored as discrete packets. Then each packet is parsed, swizzled, has
its checksum verified, its coordinates validated and if successful, its values added into the current total
for the chunk averages. Packet 0 is located from offset 0 to 4, packet 1 is located from 5 to 9, (which
turns out to be invalid and is skipped) and packet 2 is at offset 10 to 14. Then the delimiter is
recorded from offset 15 to 19, the current chunk's average is calculated and a new chunk begins. The
memory for the previous chunk's bytes is cleared and bytes are read from file once more. Bytes 20 to
24 are read in, then the EOF is reached and the chunk is processed. Once again, divisibility, length
etc. are checked and the chunk is split into packets, verified and calculated.

In each of these examples, after the data is read and the End Of File is reached, the program
closes the file with fclose() and ends. The data from the files are read in character by
character with the fgetc() function, and are read in as unsigned characters to be processed
as integers if they are valid.
The delimiter arguments that are passed in as command line arguments are converted to integers
with some manual format checking and use of the strtol() function of ctype.h. The program
will return an error and end if any delimiter values are invalid, if the wrong number of
arguments are passed to the program or if the file does not exist.
