#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "wave.h"

int main(int argc, char *argv[]) {
    // Valid flags are v for volume and r for reverse
    char *validFlags = "v:r"; 

    // Get flag from command line arguments
    char flag = getopt(argc, argv, validFlags);
    if (flag == '?') {
        printf("Invalid flag\n");
        return 1;
    }
    
    // Ensure user passes in correct command line arguments
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

    // Read infile header and check that it is a 16-bit WAV file
    WAVHEADER header;
    fread(&header, sizeof(WAVHEADER), 1, inptr);
    if (!iswave(&header) || header.bitsPerSample != 16) {
        printf("Inavlid file format\n");
        fclose(inptr);
        return 1;
    }

    // Read all audio data from infile
    int16_t *samples = malloc(header.subchunk2Size);
    uint32_t numSamples = header.subchunk2Size / sizeof(int16_t);
    fread(samples, header.subchunk2Size, 1, inptr);

    // Close infile since we have read all of its data
    fclose(inptr);

    // Manipulate audio samples according to flag passed in by user
    switch (flag) {
        case 'v':
            for (size_t i = 0; i < numSamples; i++) {
                samples[i] *= atof(optarg);
            }
            break;

        case 'r':
            for (size_t i = 0; i < numSamples / 2; i++) {
                int16_t temp = samples[i];
                samples[i] = samples[numSamples - i - 1];
                samples[numSamples - i - 1] = temp;
            }
            break;
    }

    // Open outfile for writing
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL) {
        printf("Could not open %s\n", outfile);
        return 1;
    }

    // Write audio data to outfile
    fwrite(&header, sizeof(WAVHEADER), 1, outptr);
    fwrite(samples, header.subchunk2Size, 1, outptr);

    // Free allocated memory for audio samples
    free(samples);

    // Close outfile since we have written all of the data
    fclose(outptr);

    return 0;
}