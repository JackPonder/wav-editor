#include <stdint.h>
#include <stdbool.h>

// Struct for containing .wav file header metadata
typedef struct wavheader {
    uint8_t chunkID[4];
    uint32_t chunkSize;
    uint8_t format[4];
    uint8_t subchunk1ID[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint8_t subchunk2ID[4];
    uint32_t subchunk2Size;
} __attribute__((__packed__))
WAVHEADER;

// Returns true if header format contains the letters "WAVE", otherwise false
bool iswave(WAVHEADER header);