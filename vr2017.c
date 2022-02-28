/* Name: Sam Kelly
 * unikey: SKEL4720
 */

#include <stdio.h>
#include <ctype.h>  //check these are allowed!!!
#include <string.h>
#include <math.h>
#include <stdlib.h>

int hexToDec(char *term) {

  char byte1 = toupper(term[2]);
  char byte2 = toupper(term[3]);

  if (term[0] == '0' && term[1] == 'x' && byte1 >= '0' && byte1 <= 'F' && byte2 >= '0' && byte2 <= 'F' && strlen(term) == 4) {
    int first_value = byte1 - '0';
    int second_value = byte2 - '0';
    if (first_value > 9) {
      first_value -= 7;
    }
    if (second_value > 9) {
      second_value -= 7;
    }
    return first_value*16 + second_value;
  } else {
    return -1;
  }
}

int paritySolver(int dec_value[], int size) {
  int checksum = 0;
  for (int bit = 8; bit > 0; bit--) {
    int mod = 0;
    for (int entry = 0; entry < size; entry++) { //replace with arraylength
      if (dec_value[entry] & 1 << (bit-1)) {
        mod = (mod + 1)%2;
      }
    }
  checksum += mod * 1 << (bit-1);
  }
  return checksum;
}

int main(int argc, char **argv) {

  if (argc != 5) {
    printf("Incorrect number of command line arguments entered. Correct format: ./vr2017 <.bin file> <byte 1 of delimiter> <byte 2 of delimiter> <byte 3 of delimiter>\n");
    return -1;
  }

    FILE *file;
    if (!(file = fopen(argv[1], "rb"))) {
      printf("Invalid file.\n");
      return -1;
    }

    int dec_value[3];
    for (int arg = 2; arg < 5; arg++) {
       dec_value[arg-2] = hexToDec(argv[arg]);
      if (dec_value[arg-2] != -1) {
        printf("Delimiter byte %d is: %d\n", arg-2, dec_value[arg-2]);
      } else {
        printf("Invalid delimiter byte.\n");
        return -1;
      }
    }
    int checksum = paritySolver(dec_value, 3);
    printf("Checksum is: %d\n\n", checksum);

    int delimiter[] = {dec_value[0], dec_value[1], dec_value[2], checksum};
    int delimiter_count = 0;
    int value_count = 0;
    int pacet_count = 0;
    int chunk_count = 0;
    int offset = 0;
    unsigned char chunk[5];
    unsigned char last_valid_chunk[3];
    int sums[3] = { 0 };
    int valid_count = 1;
    char coords[] = {'X', 'Y', 'Z'};

    while(!feof(file)) {
      chunk[value_count] = fgetc(file);
      if (chunk[value_count] == delimiter[delimiter_count]) {
        if (delimiter_count++ == 3 || (feof(file) && pacet_count != 0)) {
          printf("    Chunk Average X: %.2f, Average Y: %.2f, Average Z: %.2f\n\n", (double)sums[0]/(double)valid_count,(double)sums[1]/(double)valid_count,(double)sums[2]/(double)valid_count);
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
      if (value_count++ == 4) {
        value_count = 0;
        printf("Chunk: %d at offset: %d\n", chunk_count, offset);
        offset+=value_count;
        printf("    Packet: %d\n", pacet_count++);
        printf("        Data before swizzle -> B0: %d, B1: %d, B2: %d\n",chunk[0], chunk[1], chunk[2]);

        char *swizzle;
        int swizzled_chunk[4] = { 0 };
        swizzled_chunk[3] = chunk[3];

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
        if (paritySolver(swizzled_chunk, 4) != chunk[4]) {
          printf("Parity byte does not match. Skipping packet.");
          continue;
        }
        for (int coord = 0; coord < 3; coord++) {
          if (pacet_count == 1) {
            sums[coord] = 0;
            last_valid_chunk[coord] = swizzled_chunk[coord];
            sums[coord] += last_valid_chunk[coord];
            continue;
          }
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
