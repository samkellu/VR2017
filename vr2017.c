/* Name: Sam Kelly
 * unikey: SKEL4720
 */

#include <stdio.h>
#include <ctype.h>  //check these are allowed!!!
#include <string.h>
#include <math.h>

int hexToDec(char *term) {

  char byte1 = toupper(term[2]);
  char byte2 = toupper(term[3]);

  if (term[0] == '0' && term[1] == 'x' && byte1 >= '0' && byte1 <= 'F' && byte2 >= '0' && byte2 <= 'F' && strlen(term) == 4) {
    printf("This is a valid hex string %s\n", term);
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

int paritySolver(int *decValue) {
  int checksum = 0;
  for (int bit = 8; bit > 0; bit--) {
    int mod = 0;
    for (int entry = 0; entry < 3; entry++) { //replace with arraylength
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
    if ((file = fopen(argv[1], "rb"))) {
      printf("Valid file!\n");
    } else {
      printf("Invalid file you monkey\n");
      return -1;
    }

    int decValue[3];
    for (int arg = 2; arg < 5; arg++) {
       decValue[arg-2] = hexToDec(argv[arg]);
      if (decValue[arg-2] != -1) {
        printf("Delimiter byte %d is: %d\n", arg-2, decValue[arg-2]);
      } else {
        printf("u fucked up\n");
        return -1;
      }
    }
    int checksum = paritySolver(decValue);
    printf("Checksum: %d\n", checksum);

    int delimiter[] = {decValue[0], decValue[1], decValue[2], checksum};
    printf("%d %d %d %d\n", delimiter[0], delimiter[1], delimiter[2], delimiter[3]);

    int delimiterCount = 0;
    int valueCount = 0;
    int packetCount = 0;
    int chunkCount = 0;
    int offset = 0;
    unsigned char chunk[5];

    printf("\nChunk: %d at offset: %d\n", chunkCount, offset);
    printf("  Packet: %d\n", packetCount);
    while(!feof(file)) {
      chunk[valueCount] = fgetc(file);
      printf("    Data: 0x%02x\n", chunk[valueCount]);
      if (chunk[valueCount] == delimiter[delimiterCount]) {
        if (delimiterCount++ == 3) {
          chunkCount++;
          printf("Chunk: %d at offset: %d\n", chunkCount, offset);
          valueCount = 4;
          packetCount = -1;
          delimiterCount = 0;
        }
      } else {
        delimiterCount = 0;
      }
      if (valueCount++ == 4) {
        valueCount = 0;
        printf("    Data before swizzle -> B0: %d, B1: %d, B2: %d\n",chunk[0], chunk[1], chunk[2]);
        printf("    Swizzle: %d\n", chunk[3]);

        printf("  Packet: %d\n", ++packetCount);
      }
      offset++;
    }
    fclose(file);
}
