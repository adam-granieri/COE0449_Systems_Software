#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	if (argc < 2 || argc > 2) {
		printf("Bad input arguements");
	}	

	//Open File
	FILE* input = fopen(argv[1], "rb");
	if (input == NULL) {
		printf("ERROR: File does not exist\n");
		return 0;
	}

	//Variables for detecting strings
	int counter = 0, seek_val = -5;
	char curr, prev;

	//loop to read file
	while (!feof(input)) {
		curr = fgetc(input); // read char

		if (counter > 4) {
			if (curr <= 126 && curr >= 32) { //valid char
				counter++;
				printf("%c", curr);
			} else { //not valid char, end word
				counter = 0;
				printf("\n");
			}
		} else if (counter == 4) {
			fseek(input,seek_val,SEEK_CUR);
			for (counter = 0; counter < 4; counter++) {
				prev = fgetc(input);
				printf("%c", prev);
			}

			if (curr <= 126 && curr >= 32) {
				counter++;
			} else {
				counter = 0;
				printf("\n");
			}
		} else {
			if (curr <= 126 && curr >= 32) {
				counter++;
			} else {
				counter = 0;
			}
		}
	}

	fclose(input);
	return 1;
}
