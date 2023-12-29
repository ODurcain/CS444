#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Function to convert a 4-bit binary to hexadecimal character
char binaryToHex(char* binary) {
    int decimal = 0;
    for (int i = 0; i < 4; i++) {
        decimal = decimal * 2 + (binary[i] - '0');
    }
    return "0123456789ABCDEF"[decimal];
}

// Function to decode Hamming(7,4) code from binary representation
void decodeHamming74(const char* binaryData, char* decodedData) {
    char dataBits[4];
    int parityBits[3];
    
    // Extract data and parity bits from the binary representation
    dataBits[0] = binaryData[2]; // D1
    dataBits[1] = binaryData[3]; // D2
    dataBits[2] = binaryData[4]; // D3
    dataBits[3] = binaryData[5]; // D4
    
    parityBits[0] = binaryData[0]; // P1
    parityBits[1] = binaryData[1]; // P2
    parityBits[2] = binaryData[6]; // P3
    
    // Calculate the syndrome to check for errors
    int syndrome = (parityBits[0] << 2) | (parityBits[1] << 1) | parityBits[2];
    
    // Check if there is an error
    if (syndrome != 0) {
        // There is an error, try to correct it
        int errorPosition = syndrome - 1;
        
        // Correct the bit at the error position
        dataBits[errorPosition] = (dataBits[errorPosition] == '0') ? '1' : '0';
    }
    
    // Reconstruct the original 4 data bits
    decodedData[0] = dataBits[0];
    decodedData[1] = dataBits[1];
    decodedData[2] = dataBits[2];
    decodedData[3] = dataBits[3];
    decodedData[4] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 5 || strcmp(argv[1], "-f") != 0 || strcmp(argv[3], "-s") != 0) {
        printf("Usage: %s -f <filename> -s <number of bytes>\n", argv[0]);
        return 1;
    }

    FILE *inputFiles[4];
    char *partSuffixes[4] = { ".part2", ".part4", ".part5", ".part6" };
    for (int i = 0; i < 4; i++) {
        char filename[100];
        snprintf(filename, sizeof(filename), "%s%s", argv[2], partSuffixes[i]);
        inputFiles[i] = fopen(filename, "r");
        if (inputFiles[i] == NULL) {
            perror("Error opening input file");
            return 1;
        }
    }

    // Create a new file for the decoded data
    FILE *decodedFile = fopen(strcat(argv[2], ".2"), "w");

    if (decodedFile == NULL) {
        perror("Error creating decoded file");
        return 1;
    }

    char binaryData[8];
    char decodedData[5];
    int bytesRead = 0;

    while (1) {
        int binaryDataIndex = 0;
        for (int i = 0; i < 4; i++) {
            char hexChar;
            if (fscanf(inputFiles[i], "%c", &hexChar) == 1) {
                char binaryChunk[5];
                hexToBinary(hexChar, binaryChunk);
                for (int j = 0; j < 4; j++) {
                    binaryData[binaryDataIndex++] = binaryChunk[j];
                }
            } else {
                // If we reach the end of any input file, break the loop
                bytesRead = -1;
                break;
            }
        }

        if (bytesRead == -1) {
            break;
        }

        // Decode and write to the decoded file
        decodeHamming74(binaryData, decodedData);

        // Write the 4-bit binary data to the decoded file
        for (int j = 0; j < 4; j++) {
            fputc(binaryToHex(decodedData + j), decodedFile);
        }
    }

    // Close all input and output files
    for (int i = 0; i < 4; i++) {
        fclose(inputFiles[i]);
    }
    fclose(decodedFile);

    return 0;
}
