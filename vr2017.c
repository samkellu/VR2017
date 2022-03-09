/* Name: Sam Kelly
 * unikey: SKEL4720
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
  if (argc > 5) {
    printf("Error: Too many command line arguments.\n");
    return 1;
  } else if (argc < 5) {
    printf("Error: Not enough command line arguments.\n");
    return 1;
  }

  //For the case in which the requested file is invalid
  FILE *file;
  if (!(file = fopen(argv[1], "rb"))) {
    printf("Error: File %s does not exist!\n", argv[1]);
    return 1;
  }

 //Converting all delimiter arguments into integers
  int dec_value[3];
  for (int arg = 2; arg < 5; arg++) {
    //posts an error if the argument is of the wrong length
     if (strlen(argv[arg]) != 4) {
       printf("Error: Argument for delimiter byte %d is not of the correct length\n", arg-2);
          return 1;
     }
     //posts an error if the argument does not begin with 0x
     if (argv[arg][0] != '0' || argv[arg][1] != 'x') {
       printf("Error: Argument for delimiter byte %d does not begin with 0x\n",arg-2);
       return 1;
     }
    //Converts command line arguments to integers if they are of valid hexidecimal form
    if (isxdigit(argv[arg][2]) && isxdigit(argv[arg][3])) {
      dec_value[arg-2] = (int)strtol(argv[arg],NULL, 0);
    } else {
      //posts an error is the format is invalid
      printf("Error: Argument for delimiter byte %d is not a valid hex value\n", arg-2);
      return 1;
    }
  }
  for (int byte = 0; byte < 3; byte++) {
    printf("Delimiter byte %d is: %d\n", byte, dec_value[byte]);
  }

  //Calculating the checksum
  int checksum = paritySolver(dec_value, 3);
  printf("Checksum is: %d\n\n", checksum);
  //Defining the delimiter values
  int delimiter[] = {dec_value[0], dec_value[1], dec_value[2], checksum};
  //Internal counter to help find delimiter in the data
  int delimiter_count = 0;
  //Counts the number of chunks
  int chunk_count = 0;
  //The current offset in memory
  int offset = 0;
  //The offset at which the chunk started
  int offset_current_chunk = 0;
  //Stores the current chunk
  int chunk[128][5] = { '\0' };
  //stores all values in the chunk before they are processed
  int unprocessed_chunk[680] = { '\0' };
  //Stores coordinate data of the most recent valid chunk
  int last_valid_chunk[3] = { '\0'};
  //Stores the sum of each value for calculating the chunk averages
  int sums[3] = { 0 };
  //Stores the sum of each coordinate's values in each chunk
  int valid_count = 1;
  //Stores the names of each coordinate for display  purposes
  char coords[] = {'X', 'Y', 'Z'};

  //Reads and  processes data from the given file until an error occurs or the end of the file
  while(!feof(file)) {
    //Resets arrays to store the current chunk
    if (offset_current_chunk == 0) {
      memset(chunk, 0, 640*sizeof(int));
      memset(unprocessed_chunk, 0, 680*sizeof(int));
      memset(last_valid_chunk, '\0',3*sizeof(int));
    }
    //Check for the case in which the chunk is larger than the specified limit
    if (offset_current_chunk == 639) {
      printf("Error: Chunk size exceeds the maximum allowable chunk size of 640 bytes.\n");
      continue;
    }
    //Reads a value into the current chunk's array
    unprocessed_chunk[offset_current_chunk] = fgetc(file);
    //Checks if the value read in is the EOF character and records it as such
    if (unprocessed_chunk[offset_current_chunk] == EOF) {
      unprocessed_chunk[offset_current_chunk] = '\0';
    } else {
      //Increases the offset counters if not the EOF character
      offset++;
      offset_current_chunk++;
    }
    //For the case where a delimiter appears as the last packet
    if (feof(file) && offset_current_chunk == 0) {
      continue;
    }

    //Checks if the value is the correct part of the delimiter
    if (unprocessed_chunk[offset-1] == delimiter[delimiter_count] || feof(file)) {
      //Begins a new chunk if the delimiter is completed
	    if (delimiter_count++ == 3 || feof(file)) {
        delimiter_count = 0;
        //Checks to see if the chunk is divisible into 5 byte packets, ignores the chunk if not and
        //processes it if it is.
        if ((offset_current_chunk - 4) % 5 != 0 && !(feof(file) && offset_current_chunk % 5 != 5)) {
          printf("Error: Chunk must be divisible by 5 bytes.\n");
        } else {
          int offset_counter = 0;
          printf("Chunk: %d at offset: %d\n", chunk_count, offset - offset_current_chunk);
          chunk_count++;
          //Creates the processed array out of the initial collection of chunk data
          for (int packet = 0; packet < offset_current_chunk/5; packet++) {
            for (int value = 0; value < 5; value ++) {
              chunk[packet][value] = unprocessed_chunk[offset_counter++];
            }
            printf("    Packet: %d\n", packet);
            printf("        Data before swizzle -> B0: %d, B1: %d, B2: %d\n", chunk[packet][0], chunk[packet][1], chunk[packet][2]);
            //Stores the string representation of the swizzle order
            char *swizzle;
            //Stores the new values of the chunk after it has been swizzled
            int swizzled_chunk[4] = { '\0' };
            swizzled_chunk[3] = chunk[packet][3];
            //Swizzles chunks as required
            switch(chunk[packet][3]) {
                  case 1:
                    swizzle = "XYZ";
                    swizzled_chunk[0] = chunk[packet][0];
                    swizzled_chunk[1] = chunk[packet][1];
                    swizzled_chunk[2] = chunk[packet][2];
                    break;
                  case 2:
                    swizzle = "XZY";
                    swizzled_chunk[0] = chunk[packet][0];
                    swizzled_chunk[2] = chunk[packet][1];
                    swizzled_chunk[1] = chunk[packet][2];
                    break;
                  case 3:
                    swizzle = "YXZ";
                    swizzled_chunk[1] = chunk[packet][0];
                    swizzled_chunk[0] = chunk[packet][1];
                    swizzled_chunk[2] = chunk[packet][2];
                    break;
                  case 4:
                    swizzle = "YZX";
                    swizzled_chunk[1] = chunk[packet][0];
                    swizzled_chunk[2] = chunk[packet][1];
                    swizzled_chunk[0] = chunk[packet][2];
                    break;
                  case 5:
                    swizzle = "ZXY";
                    swizzled_chunk[2] = chunk[packet][0];
                    swizzled_chunk[0] = chunk[packet][1];
                    swizzled_chunk[1] = chunk[packet][2];
                    break;
                  case 6:
                    swizzle = "ZYX";
                    swizzled_chunk[2] = chunk[packet][0];
                    swizzled_chunk[1] = chunk[packet][1];
                    swizzled_chunk[0] = chunk[packet][2];
                    break;
                  default:
                    printf("Invalid Swizzle Byte. Ignoring packet.\n");
                    continue;
            }
            printf("        Swizzle: %s\n", swizzle);
            printf("        Data after swizzle -> X: %d, Y: %d, Z: %d\n", swizzled_chunk[0],swizzled_chunk[1], swizzled_chunk[2]);

            //Finds and checks the checksum of the swizzled bytes
            if (paritySolver(swizzled_chunk, 4) != chunk[packet][4]) {
              printf("        Parity byte does not match. Skipping packet.\n");
              continue;
            }
            //Checks the validity of the current packet against the previous valid packet in the chunk
            for (int coord = 0; coord < 3; coord++) {
              //Sets the default most recent valid chunk in a new packet
              if (packet == 0) {
                sums[coord] = 0;
                last_valid_chunk[coord] = swizzled_chunk[coord];
                sums[coord] += last_valid_chunk[coord];
                continue;
              }
              //Checks the validity of the current packet against the values in the most recent valid packet.
              //If it is invalid, the packet is skipped, otherwise it is set to be the new most recent valid
              //packet
              if (abs(swizzled_chunk[coord] - last_valid_chunk[coord]) > 25) {
                printf("        Ignoring packet. %c: %d. Previous valid packet's %c: %d. %d > 25.\n", coords[coord], swizzled_chunk[coord], coords[coord], last_valid_chunk[coord], abs(last_valid_chunk[coord] - swizzled_chunk[coord]));
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
          printf("%d,%d,%d", last_valid_chunk[0],last_valid_chunk[1],last_valid_chunk[2]);
          if (last_valid_chunk == NULL) {
            printf("    No valid packets were found for this chunk.");
          } else {
            printf("    Chunk Average X: %.2f, Average Y: %.2f, Average Z: %.2f\n\n", (double)sums[0]/(double)valid_count,(double)sums[1]/(double)valid_count, (double)sums[2]/(double)valid_count);
          }
          //resets chunk parameters at the end of the chunk
          offset_current_chunk = 0;
          valid_count = 1;
        }
      }
    } else {
      delimiter_count = 0;
    }
  }
  fclose(file);
  return 0;
}
