/* Name: Sam Kelly
 * unikey: SKEL4720
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>  //check these are allowed!!!
#include <string.h>
#include <math.h>

//Translates a string hexadecimal value to int and verifies its correctness
int hexToDec(char *term) {
  //Each of the two characters that make up the hex number
  char byte1 = toupper(term[2]);
  char byte2 = toupper(term[3]);

  //Checks the formatting of the hexidecimal value to ensure it is valid
  if (term[0] == '0' && term[1] == 'x' && byte1 >= '0' && byte1 <= 'F' && byte2 >= '0' && byte2 <= 'F' && strlen(term) == 4) {
    //Using the ASCII values of the symbols, the hex value is converted into decimal form
    int first_value = byte1 - '0';
    int second_value = byte2 - '0';
    //To account for the symbols that appear inbetween 9 and A in the ASCII encoding system
    if (first_value > 9) {
      first_value -= 7;
    }
    if (second_value > 9) {
      second_value -= 7;
    }
    //returns the decimal value if the operation is a success
    return first_value*16 + second_value;
  } else {
    //returns a -1 (error) if the value is invalid
    return -1;
  }
}

//Calculates the checksum of an array of numbers
int paritySolver(int dec_value[], int size) {
  int checksum = 0;
  //Counts the number of (1) bits in each column of the set of numbers with a 
  //bit mask, then finds if there is an odd or even number of each. In the output
  //the a one is placed in each column where there is an odd  number of 1s present.
  for (int bit = 8; bit > 0; bit--) {
    int mod = 0;
    for (int entry = 0; entry < size; entry++) {
      if (dec_value[entry] & 1 << (bit-1)) {
        mod = (mod + 1)%2;
      }
    }
  checksum += mod * 1 << (bit-1);
  }
  //returns the calculated checksum as an integer
  return checksum;
}

int main(int argc, char **argv) {
  //For the case in which an incorrect number of command line arguments have been entered.
  if (argc != 5) {
    printf("Incorrect number of command line arguments entered. Correct format: ./vr2017 <.bin file> <byte 1 of delimiter> <byte 2 of delimiter> <byte 3 of delimiter>\n");
    return -1;
  }

  //For the case in which the requested file is invalid
  FILE *file;
  if (!(file = fopen(argv[1], "rb"))) {
    printf("Invalid file.\n");
    return -1;
  }

 //Converting all delimiter arguments into integers 
  int dec_value[3];
  for (int arg = 2; arg < 5; arg++) {
     dec_value[arg-2] = hexToDec(argv[arg]);
    //Verifying all delimiters were correctly parsed
    if (dec_value[arg-2] != -1) {
      printf("Delimiter byte %d is: %d\n", arg-2, dec_value[arg-2]);
    } else {
      printf("Invalid delimiter byte.\n");
      return -1;
    }
  }

  //Calculating the checksum
  int checksum = paritySolver(dec_value, 3);
  printf("Checksum is: %d\n\n", checksum);
  //Defining the delimiter values
  int delimiter[] = {dec_value[0], dec_value[1], dec_value[2], checksum};
  //Internal counter to help find delimiter in the data
  int delimiter_count = 0;
  //Counts the number of values processed since the start of the packet
  int value_count = 0;
  //Counts the  number of  packets since the start of the chunk
  int pacet_count = 0;
  //Counts the number of chunks
  int chunk_count = 0;
  //The current offset in memory
  int offset = 0;
  //Stores all values of the current chunk
  unsigned char chunk[5];
  //Stores coordinate data of the most recent valid chunk
  unsigned char last_valid_chunk[3];
  int sums[3] = { 0 };
  //Stores the sum of each coordinate's values in each chunk 
  int valid_count = 1;
  //Stores the names of each coordinate for display  purposes
  char coords[] = {'X', 'Y', 'Z'};

  //Reads and  processes data from the given file until an error occurs or the end of the file
  while(!feof(file)) {
    //Reads a value into the current chunk's array
    chunk[value_count] = fgetc(file);
    //Checks if the value is the correct part of the delimiter
    if (chunk[value_count] == delimiter[delimiter_count]) {
      	//Begins a new chunk if the delimiter is completed
	if (delimiter_count++ == 3 || (feof(file) && pacet_count != 0)) {
        printf("    Chunk Average X: %.2f, Average Y: %.2f, Average Z: %.2f\n\n", 
	      (double)sums[0]/(double)valid_count,(double)sums[1]/(double)valid_count,
	      (double)sums[2]/(double)valid_count);
	//Resets or increments necessary values for the next chunk
        chunk_count++;
        value_count = 0;
        valid_count = 1;
        pacet_count = 0;
        delimiter_count = 0;
        continue;
      }
    } else {
      delimiter_count = 0;
    }
    //Checks if the current value is the last of the packet
    if (value_count++ == 4) {
      value_count = 0;
      //Prints necessary information and starts the next packet
      printf("Chunk: %d at offset: %d\n", chunk_count, offset);
      offset+=value_count;
      printf("    Packet: %d\n", pacet_count++);
      printf("        Data before swizzle -> B0: %d, B1: %d, B2: %d\n",chunk[0], chunk[1], chunk[2]);

      //Stores the string representation of the swizzle order
      char *swizzle;
      //Stores the new values of the chunk after it has been swizzled
      int swizzled_chunk[4] = { 0 };
      swizzled_chunk[3] = chunk[3];
      //Swizzles chunks as required
    	switch(chunk[3]) {
    	      case 1:
              swizzle = "XYZ";
    		      swizzled_chunk[0] = chunk[0];
              swizzled_chunk[1] = chunk[1];
              swizzled_chunk[2] = chunk[2];
    		      break;
    	      case 2:
              swizzle = "XZY";
              swizzled_chunk[0] = chunk[0];
              swizzled_chunk[2] = chunk[1];
              swizzled_chunk[1] = chunk[2];
    		      break;
    	      case 3:
              swizzle = "YXZ";
              swizzled_chunk[1] = chunk[0];
              swizzled_chunk[0] = chunk[1];
              swizzled_chunk[2] = chunk[2];
              break;
    	      case 4:
              swizzle = "YZX";
              swizzled_chunk[1] = chunk[0];
              swizzled_chunk[2] = chunk[1];
              swizzled_chunk[0] = chunk[2];
    		      break;
            case 5:
              swizzle = "ZXY";
              swizzled_chunk[2] = chunk[0];
              swizzled_chunk[0] = chunk[1];
              swizzled_chunk[1] = chunk[2];
    		      break;
            case 6:
              swizzle = "ZYX";
              swizzled_chunk[2] = chunk[0];
              swizzled_chunk[1] = chunk[1];
              swizzled_chunk[0] = chunk[2];
    		      break;
            default:
              printf("Invalid Swizzle Byte.\n");  //check if this works at all
              continue;
    	}

    	printf("        Swizzle: %s\n", swizzle);
      printf("        Data after swizzle -> X: %d, Y: %d, Z: %d\n", swizzled_chunk[0],swizzled_chunk[1], swizzled_chunk[2]);
      //Finds and checks the checksum of the swizzled bytes
      if (paritySolver(swizzled_chunk, 4) != chunk[4]) {
        printf("Parity byte does not match. Skipping packet.");
        continue;
      }
      //Checks the validity of the current packet against the previous valid packet in the chunk 
      for (int coord = 0; coord < 3; coord++) {
	//Sets the default most recent valid chunk in a new packet
        if (pacet_count == 1) {
          sums[coord] = 0;
          last_valid_chunk[coord] = swizzled_chunk[coord];
          sums[coord] += last_valid_chunk[coord];
          continue;
        }
	//Checks the validity of the current packet against the values in the most recent valid packet.
	//If it is invalid, the packet is skipped, otherwise it is set to be the new most recent valid 
	//packet
        if (abs(swizzled_chunk[coord] - last_valid_chunk[coord]) > 25) {
          printf("    Ignoring packet. %c: %d. Previous valid packet's %c: %d. %d > 25.\n", coords[coord], swizzled_chunk[coord], coords[coord], last_valid_chunk[coord], abs(last_valid_chunk[coord] - swizzled_chunk[coord]));
          break;
        } else {
          last_valid_chunk[coord] = swizzled_chunk[coord];
          sums[coord] += last_valid_chunk[coord];
          if (coord == 2) {
            valid_count++;
          }
        }
      }
    }
  }
  fclose(file);
}
