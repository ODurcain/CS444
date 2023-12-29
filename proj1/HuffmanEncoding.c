#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHARACTERS 256 // Assuming ASCII characters

// Struct to store character and frequency pairs
struct CharFrequency {
    char character;
    int frequency;
};

// Function to compare two CharFrequency structs for sorting
int compareCharFrequency(const void *a, const void *b) {
    return ((struct CharFrequency *)b)->frequency - ((struct CharFrequency *)a)->frequency;
}

// Struct to represent a node in the Huffman tree
struct Node {
    char character;
    int frequency;
    struct Node *left;
    struct Node *right;
};

// Struct to represent a Min Heap of Huffman tree nodes
struct MinHeap {
    int size;
    int capacity;
    struct Node **array;
};

// Struct to store Huffman codes for characters
struct HuffmanCode {
    char character;
    char *code;
};

// Function to create a new node
struct Node *createNode(char character, int frequency) {
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    if (node) {
        node->character = character;
        node->frequency = frequency;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

// Function to create a Min Heap
struct MinHeap *createMinHeap(int capacity) {
    struct MinHeap *minHeap = (struct MinHeap *)malloc(sizeof(struct MinHeap));
    if (minHeap) {
        minHeap->size = 0;
        minHeap->capacity = capacity;
        minHeap->array = (struct Node **)malloc(capacity * sizeof(struct Node *));
    }
    return minHeap;
}

// Function to swap two nodes in the Min Heap
void swapNodes(struct Node **a, struct Node **b) {
    struct Node *temp = *a;
    *a = *b;
    *b = temp;
}

// Function to heapify a subtree with the root at given index
void minHeapify(struct MinHeap *minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->frequency < minHeap->array[smallest]->frequency) {
        smallest = left;
    }

    if (right < minHeap->size && minHeap->array[right]->frequency < minHeap->array[smallest]->frequency) {
        smallest = right;
    }

    if (smallest != idx) {
        swapNodes(&minHeap->array[idx], &minHeap->array[smallest]);
        minHeapify(minHeap, smallest);
    }
}

// Function to build a Min Heap from an array of nodes
void buildMinHeap(struct MinHeap *minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; i--) {
        minHeapify(minHeap, i);
    }
}

// Function to extract the node with the lowest frequency from the Min Heap
struct Node *extractMin(struct MinHeap *minHeap) {
    struct Node *minNode = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    minHeap->size--;
    minHeapify(minHeap, 0);
    return minNode;
}

// Function to insert a node into the Min Heap
void insertNode(struct MinHeap *minHeap, struct Node *node) {
    minHeap->size++;
    int i = minHeap->size - 1;
    while (i > 0 && node->frequency < minHeap->array[(i - 1) / 2]->frequency) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = node;
}

// Function to build a Huffman tree from character frequencies
struct Node *buildHuffmanTree(struct CharFrequency *char_frequencies) {
    struct MinHeap *minHeap = createMinHeap(MAX_CHARACTERS);

    // Create leaf nodes and insert them into the Min Heap
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        if (char_frequencies[i].frequency > 0) {
            struct Node *node = createNode(char_frequencies[i].character, char_frequencies[i].frequency);
            insertNode(minHeap, node);
        }
    }

    // Build the Huffman tree by combining nodes
    while (minHeap->size > 1) {
        struct Node *left = extractMin(minHeap);
        struct Node *right = extractMin(minHeap);

        // Create a new internal node with the sum of frequencies
        struct Node *internalNode = createNode('\0', left->frequency + right->frequency);
        internalNode->left = left;
        internalNode->right = right;

        insertNode(minHeap, internalNode);
    }

    // The remaining node in the Min Heap is the root of the Huffman tree
    return extractMin(minHeap);
}

// Function to traverse the Huffman tree and assign binary codes
void assignHuffmanCodes(struct Node *root, char *code, int depth, struct HuffmanCode *huffmanCodes) {
    if (root->left == NULL && root->right == NULL) {
        // Leaf node, assign the code
        huffmanCodes[(unsigned char)root->character].character = (char)root->character;
        huffmanCodes[(unsigned char)root->character].code = strdup(code);
    } else {
        // Non-leaf node, traverse left and right
        if (root->left) {
            code[depth] = '1';
            code[depth + 1] = '\0';
            assignHuffmanCodes(root->left, code, depth + 1, huffmanCodes);
        }
        if (root->right) {
            code[depth] = '0';
            code[depth + 1] = '\0';
            assignHuffmanCodes(root->right, code, depth + 1, huffmanCodes);
        }
    }
}

int main(int argc, char *argv[]) {
    char *input_filename = NULL;
    char *output_filename = NULL;

    // Check if the correct number of command line arguments is provided
    if (argc != 5) {
        printf("Usage: %s -i input_filename -o output_filename\n", argv[0]);
        return 1; // Exit with an error code
    }

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            if (i + 1 < argc) {
                input_filename = argv[i + 1];
                i++; // Skip the next argument since it's the input filename
            } else {
                printf("Error: Missing input filename\n");
                return 1; // Exit with an error code
            }
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                output_filename = argv[i + 1];
                i++; // Skip the next argument since it's the output filename
            } else {
                printf("Error: Missing output filename\n");
                return 1; // Exit with an error code
            }
        } else {
            printf("Error: Invalid argument '%s'\n", argv[i]);
            return 1; // Exit with an error code
        }
    }

    // Open the input file for reading
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        perror("Error opening input file");
        return 1; // Exit with an error code
    }

    // Initialize an array of CharFrequency structs to store character frequencies
    struct CharFrequency char_frequencies[MAX_CHARACTERS];
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        char_frequencies[i].character = i;
        char_frequencies[i].frequency = 0;
    }

    // Read characters from the input file and update their frequencies
    int c;
    while ((c = fgetc(input_file)) != EOF) {
        if (c >= 0 && c < MAX_CHARACTERS) {
            char_frequencies[c].frequency++;
        }
    }

    // Build the Huffman tree
    struct Node *huffmanRoot = buildHuffmanTree(char_frequencies);

    // Initialize an array of HuffmanCode structs to store Huffman codes
    struct HuffmanCode huffmanCodes[MAX_CHARACTERS];
    for (int i = 0; i < 256; i++) {
        huffmanCodes[i].character = i;
        huffmanCodes[i].code = NULL;
    }

    // Assign Huffman codes to characters
    char code[MAX_CHARACTERS];
    code[0] = '\0';
    assignHuffmanCodes(huffmanRoot, code, 0, huffmanCodes);

    // Open the output file for writing
    FILE *output_file = fopen(output_filename, "wb"); // Use binary mode for writing
    if (output_file == NULL) {
        perror("Error opening output file");
        return 1; // Exit with an error code
    }

    // Encode the input text using Huffman codes and write to the output file
    int totalFrequency = 0;
    int buffer = 0; // Buffer to store bits
    int buffer_length = 0; // Number of bits in the buffer

    // Encode and write each character from the input file
    fseek(input_file, 0, SEEK_SET); // Reset the file pointer to the beginning
    while ((c = fgetc(input_file)) != EOF) {
        if (c >= 0 && c < MAX_CHARACTERS && huffmanCodes[c].code) {
            char *char_code = huffmanCodes[c].code;
            for (int i = 0; char_code[i] != '\0'; i++) {
                buffer <<= 1; // Shift left
                if (char_code[i] == '1') {
                    buffer |= 1; // Set the least significant bit to 1
                }
                buffer_length++;
                if (buffer_length == 8) {
                    fputc(buffer, output_file);
                    buffer = 0;
                    buffer_length = 0;
                }
            }
            totalFrequency += char_frequencies[c].frequency;
        }
    }

    // Write any remaining bits in the buffer to the output file
    if (buffer_length > 0) {
        buffer <<= (8 - buffer_length);
        fputc(buffer, output_file);
    }

    // Print Huffman codes for characters
    printf("Huffman Codes:\n");
    printf("%-10s %-20s %-10s\n", "Character", "Code", "Frequencies");
    printf("------------------------------------------------\n");
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        if (huffmanCodes[i].code != NULL) {
            if (huffmanCodes[i].character == '\n') {
                printf("'\\n'       %-20s %-10d\n", huffmanCodes[i].code, char_frequencies[i].frequency); // Print '\\n' for newline character
            }
            else {
            printf("'%c'        %-20s %-10d\n", huffmanCodes[i].character, huffmanCodes[i].code, char_frequencies[i].frequency);
            } 
        }
        // Strictly for debugging the extended ascii table's outputs
        // else {
        //     // Add a debug print statement here to see if any codes are NULL
        //     printf("Character: %c, Code: NULL\n", huffmanCodes[i].character);
        // }
    }

    // // Calculate the sum of frequencies at the root node (should be the total frequency)
    // int totalFrequency = huffmanRoot->frequency;
    // printf("Total Frequency: %d\n", totalFrequency);

    fclose(input_file); // Close the input file
    fclose(output_file); //Closes the output file

    return 0; // Exit successfully
}
