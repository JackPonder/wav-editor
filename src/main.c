#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "wave.h"

void changeVolume(WAVHEADER *header, int16_t *samples, float factor);
void changeSpeed(WAVHEADER *header, float factor);
void reverse(WAVHEADER *header, int16_t *samples);

int main(int argc, char *argv[]) {
    // Valid flags are v for volume and r for reverse
    char *validFlags = "v:s:r"; 

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
    if (samples == NULL) {
        printf("Memory allocation error\n");
        return 1;
    }
    fread(samples, header.subchunk2Size, 1, inptr);

    // Close infile since we have read all of its data
    fclose(inptr);

    // Manipulate audio samples according to flag passed in by user
    switch (flag) {
        case 'v':
            changeVolume(&header, samples, atof(optarg));
            break;

        case 's':
            changeSpeed(&header, atof(optarg));
            break;

        case 'r':
            reverse(&header, samples);
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

void changeVolume(WAVHEADER *header, int16_t *samples, float factor) {
    uint32_t numSamples = header->subchunk2Size / sizeof(int16_t);
    for (size_t i = 0; i < numSamples; i++) {
        samples[i] *= factor;
    }
}

void changeSpeed(WAVHEADER *header, float factor) {
    header->sampleRate *= factor;
    header->byteRate *= factor;
}

void reverse(WAVHEADER *header, int16_t *samples) {
    uint32_t numSamples = header->subchunk2Size / sizeof(int16_t);
    for (size_t i = 0; i < numSamples / 2; i++) {
        int16_t temp = samples[i];
        samples[i] = samples[numSamples - i - 1];
        samples[numSamples - i - 1] = temp;
    }
}