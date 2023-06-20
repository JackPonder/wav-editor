#include "wave.h"

bool iswave(WAVHEADER header) {
    return (
        header.format[0] == 'W' &&
        header.format[1] == 'A' &&
        header.format[2] == 'V' &&
        header.format[3] == 'E'
    );
}