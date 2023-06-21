#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "wave.h"

void reverseAudio(int numSamples, int blockSize, uint8_t samples[numSamples][blockSize]);

int main(int argc, char *argv[]) {
    // Valid flags are v for volume and r for reverse
    char *validFlags = "v:r"; 

    // Get flag from command line arguments
    char flag = getopt(argc, argv, validFlags);
    if (flag == '?') {
        printf("Invalid flag\n");
        return 1;
    }
    
    // Ensure user passes in command line arguments
    if (argc != optind + 2) {
        printf("Usage: ./editor [flag] input.wav output.wav\n");
        return 1;
    }

    // Get file names from command line arguments
    char *infile = argv[optind];
    char *outfile = argv[optind + 1];

    // Open infile for reading
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL) {
        printf("Could not open %s\n", infile);
        return 1;
    }

    // Read infile header and check that it is a WAV file
    WAVHEADER header;
    fread(&header, sizeof(header), 1, inptr);
    if (!iswave(header)) {
        printf("Inavlid file format\n");
        fclose(inptr);
        return 1;
    }

    // Read all audio data from infile
    unsigned int blockSize = header.bitsPerSample / 8, numSamples = header.subchunk2Size / blockSize;
    uint8_t samples[numSamples][blockSize];
    fread(samples, header.subchunk2Size, 1, inptr);

    // Close infile since we have read all of its data
    fclose(inptr);

    // TODO: Manipulate audio samples according to flag passed in by user
    switch (flag) {
        case 'v':
            break;
        
        case 'r':
            reverseAudio(numSamples, blockSize, samples);
            break;
    }

    // Open outfile for writing
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL) {
        printf("Could not open %s\n", outfile);
        return 1;
    }
    
    // Write audio data to outfile
    fwrite(&header, sizeof(header), 1, outptr);
    fwrite(samples, header.subchunk2Size, 1, outptr);

    // Close outfile since we have written all of the data
    fclose(outptr);

    return 0;
}

void reverseAudio(int numSamples, int blockSize, uint8_t samples[numSamples][blockSize]) {
    for (int i = 0; i < numSamples / 2; i++) {
        uint8_t temp[blockSize];
        memcpy(temp, samples[i], blockSize);
        memcpy(samples[i], samples[numSamples - i - 1], blockSize);
        memcpy(samples[numSamples - i - 1], temp, blockSize);
    }
}