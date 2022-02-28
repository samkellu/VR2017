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
    int firstValue = byte1 - '0';
    int secondValue = byte2 - '0';
    if (firstValue > 9) {
      firstValue -= 7;
    }
    if (secondValue > 9) {
      secondValue -= 7;
    }
    return firstValue*16 + secondValue;
  } else {
    return -1;
  }
}

int paritySolver(int decValue[], int size) {
  int checksum = 0;
  for (int bit = 8; bit > 0; bit--) {
    int mod = 0;
    for (int entry = 0; entry < size; entry++) { //replace with arraylength
      if (decValue[entry] & 1 << (bit-1)) {
        mod = (++mod)%2;
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

    int decValue[3];
    for (int arg = 2; arg < 5; arg++) {
       decValue[arg-2] = hexToDec(argv[arg]);
      if (decValue[arg-2] != -1) {
        printf("Delimiter byte %d is: %d\n", arg-2, decValue[arg-2]);
      } else {
        printf("Invalid delimiter byte.\n");
        return -1;
      }
    }
    int checksum = paritySolver(decValue, 3);
    printf("Checksum: %d\n\n", checksum);

    int delimiter[] = {decValue[0], decValue[1], decValue[2], checksum};
    int delimiterCount = 0;
    int valueCount = 0;
    int packetCount = 0;
    int chunkCount = 0;
    int offset = 0;
    unsigned char chunk[5];
    unsigned char last_valid_chunk[3];
    int sums[3] = { 0 };
    int valid_count = 1;
    char coords[] = {'X', 'Y', 'Z'};

    while(!feof(file)) {
      chunk[valueCount] = fgetc(file);
      if (chunk[valueCount] == delimiter[delimiterCount]) {
        if (delimiterCount++ == 3 || (feof(file) && packetCount != 0)) {
          printf("Chunk Average X. %.2f, Average Y. %.2f, Average Z. %.2f\n\n", (double)sums[0]/(double)valid_count,(double)sums[1]/(double)valid_count,(double)sums[2]/(double)valid_count);
          chunkCount++;
          valueCount = 0;
          valid_count = 1;
          packetCount = 0;
          delimiterCount = 0;
          continue;
        }
      } else {
        delimiterCount = 0;
      }
      if (valueCount++ == 4) {
        valueCount = 0;
        printf("Chunk: %d at offset: %d\n", chunkCount, offset);
        offset+=valueCount;
        printf("  Packet: %d\n", packetCount++);
        printf("    Data before swizzle -> B0: %d, B1: %d, B2: %d\n",chunk[0], chunk[1], chunk[2]);

        char *swizzle;
        int swizzledChunk[4] = { 0 };
        swizzledChunk[3] = chunk[3];

      	switch(chunk[3]) {
      	      case 1:
                swizzle = "XYZ";
      		      swizzledChunk[0] = chunk[0];
                swizzledChunk[1] = chunk[1];
                swizzledChunk[2] = chunk[2];
      		      break;
      	      case 2:
                swizzle = "XZY";
                swizzledChunk[0] = chunk[0];
                swizzledChunk[2] = chunk[1];
                swizzledChunk[1] = chunk[2];
      		      break;
      	      case 3:
                swizzle = "YXZ";
                swizzledChunk[1] = chunk[0];
                swizzledChunk[0] = chunk[1];
                swizzledChunk[2] = chunk[2];
                break;
      	      case 4:
                swizzle = "YZX";
                swizzledChunk[1] = chunk[0];
                swizzledChunk[2] = chunk[1];
                swizzledChunk[0] = chunk[2];
      		      break;
              case 5:
                swizzle = "ZXY";
                swizzledChunk[2] = chunk[0];
                swizzledChunk[0] = chunk[1];
                swizzledChunk[1] = chunk[2];
      		      break;
              case 6:
                swizzle = "ZYX";
                swizzledChunk[2] = chunk[0];
                swizzledChunk[1] = chunk[1];
                swizzledChunk[0] = chunk[2];
      		      break;
              default:
                printf("Invalid Swizzle Byte.\n");  //check if this works at all
                continue;
      	}

      	printf("    Swizzle: %s\n", swizzle);
        printf("    Data after swizzle -> X: %d, Y: %d, Z: %d\n", swizzledChunk[0],swizzledChunk[1], swizzledChunk[2]);
        if (paritySolver(swizzledChunk, 4) != chunk[4]) {
          printf("Parity byte does not match. Skipping packet.");
          continue;
        }
        for (int coord = 0; coord < 3; coord++) {
          if (packetCount == 1) {
            sums[coord] = 0;
            last_valid_chunk[coord] = swizzledChunk[coord];
            sums[coord] += last_valid_chunk[coord];
            continue;
          }
          if (abs(swizzledChunk[coord] - last_valid_chunk[coord]) > 25) {
            printf("    Ignoring packet. %c: %d. Previous valid packet's %c: %d. %d > 25.\n", coords[coord], swizzledChunk[coord], coords[coord], last_valid_chunk[coord], abs(last_valid_chunk[coord] - swizzledChunk[coord]));
            break;
          } else {
            last_valid_chunk[coord] = swizzledChunk[coord];
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
