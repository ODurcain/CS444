#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to encode data using Hamming(7,4) code
void encodeHamming74(char* data, int dataLength, char* encodedData) {
    // Check if the input data is exactly 4 binary digits
    if (dataLength != 4) {
        printf("Input data must be exactly 4 binary digits.\n");
        return;
    }

    // Initialize the Hamming(7,4) encoded data array
    char hamming74[8];

    // Copy the original 4 data bits to their positions
    hamming74[2] = data[0];
    hamming74[4] = data[1];
    hamming74[5] = data[2];
    hamming74[6] = data[3];

    // Calculate the parity bits (P1, P2, P4)
    hamming74[0] = (hamming74[2] ^ hamming74[4] ^ hamming74[6]) ; // P1
    hamming74[1] = (hamming74[2] ^ hamming74[5] ^ hamming74[6]) ; // P2
    hamming74[3] = (hamming74[4] ^ hamming74[5] ^ hamming74[6]) ; // P4

    // Set the last character as null-terminator
    hamming74[7] = '\0';

    // Copy the Hamming(7,4) data to the output parameter
    strcpy(encodedData, hamming74);
}


// Function to convert a character to its hexadecimal representation
void charToHex(char c, char* hex) {
    snprintf(hex, 3, "%02X", c);
}

// Function to convert a hexadecimal character to binary
void hexToBinary(char hex, char* binary) {
    switch (hex) {
        case '0':
            strcpy(binary, "0000");
            break;
        case '1':
            strcpy(binary, "0001");
            break;
        case '2':
            strcpy(binary, "0010");
            break;
        case '3':
            strcpy(binary, "0011");
            break;
        case '4':
            strcpy(binary, "0100");
            break;
        case '5':
            strcpy(binary, "0101");
            break;
        case '6':
            strcpy(binary, "0110");
            break;
        case '7':
            strcpy(binary, "0111");
            break;
        case '8':
            strcpy(binary, "1000");
            break;
        case '9':
            strcpy(binary, "1001");
            break;
        case 'A':
        case 'a':
            strcpy(binary, "1010");
            break;
        case 'B':
        case 'b':
            strcpy(binary, "1011");
            break;
        case 'C':
        case 'c':
            strcpy(binary, "1100");
            break;
        case 'D':
        case 'd':
            strcpy(binary, "1101");
            break;
        case 'E':
        case 'e':
            strcpy(binary, "1110");
            break;
        case 'F':
        case 'f':
            strcpy(binary, "1111");
            break;
        default:
            strcpy(binary, "0000"); // Set to 0000 for non-hex characters
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-f") != 0) {
        printf("Usage: %s -f <filename>\n", argv[0]);
        return 1;
    }

    // Open the file for reading
    FILE *file = fopen(argv[2], "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Open seven output files, one for each bit of the Hamming(7,4) code
    FILE *outputFiles[7];
    char filenames[7][100];
    for (int i = 0; i < 7; i++) {
        snprintf(filenames[i], sizeof(filenames[i]), "%s.part%d", argv[2], i);
        outputFiles[i] = fopen(filenames[i], "w");
        if (outputFiles[i] == NULL) {
            perror("Error opening output file");
            return 1;
        }
    }

    char line[100]; // Adjust the size as needed
    int currentOutputFileIndex = 0;

    while (fgets(line, sizeof(line), file)) {
        int dataLength = strlen(line) - 1; // Exclude the newline character
        int numChunks = (dataLength + 3) / 4; // Calculate the number of 4-character chunks

        for (int i = 0; i < numChunks; i++) {
            char chunk[5]; // Reserve space for 4 characters and the null terminator
            strncpy(chunk, line + i * 4, 4); // Copy 4 characters from the line into the chunk
            chunk[4] = '\0'; // Null-terminate the chunk

          //  printf("Original Data Chunk: %s\n", chunk);

            // Convert and print the original data chunk to hex
            char hex[9]; // Reserve space for 8 characters and the null terminator
            hex[0] = '\0'; // Initialize hex as an empty string
            for (int j = 0; j < 4; j++) {
                char hexChar[3];
                charToHex(chunk[j], hexChar);
                strcat(hex, hexChar);
            }
        //    printf("Hexadecimal Representation: %s\n", hex);

            // Convert and print the hexadecimal representation to binary
            // printf("Binary Representation: ");
            for (int j = 0; j < 8; j++) {
                char binary[5];
                hexToBinary(hex[j], binary);
          //      printf("Binary Representation: %s \n", binary);
                
                // Encode the 4-digit binary chunk as Hamming(7,4)
                char hamming74[8]; // To store the Hamming(7,4) encoded data
                encodeHamming74(binary, 4, hamming74);

            //    printf("Hamming(7,4) Encoded Data: %s\n", hamming74);

                // Write each bit of the Hamming(7,4) encoded data to corresponding output files
                for (int k = 0; k < 7; k++) {
                    fputc(hamming74[k], outputFiles[k]);
                }
            }
        //    printf("\n");
        }
        currentOutputFileIndex = (currentOutputFileIndex + 1) % 7;
    }

    // Close all output files
    for (int i = 0; i < 7; i++) {
        fclose(outputFiles[i]);
    }
    
    fclose(file);
    return 0;
}
