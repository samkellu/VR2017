#include <stdio.h>

int main(int argc, char **argv) {
		int checksum = 0;
		int values[4];
		for (int x = 0; x < 4; x++) {
			scanf("%d", &values[x]);
		}
		//Counts the number of (1) bits in each column of the set of numbers with a
		//bit mask, then finds if there is an odd or even number of each. In the output
		//the a one is placed in each column where there is an odd  number of 1s present.
		for (int bit = 8; bit > 0; bit--) {
			int mod = 0;
			for (int entry = 0; entry < 4; entry++) {
				if (values[entry] & 1 << (bit-1)) {
					mod = (mod + 1)%2;
				}
			}
			checksum += mod * 1 << (bit-1);
		}
		//returns the calculated checksum as an integer
		printf("%d\n", checksum);
		return 1;
}
