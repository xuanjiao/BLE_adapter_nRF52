/* 
 * aconno.de
 * Made by Karlo Milicevic and Jurica Resetar @ aconno, 2017
 * All rights reserved 
 *
 */

#include "utilities.h"

int32_t randomNumberGenerator(void *userData, unsigned char *output, size_t size){
    for(uint32_t i = 0; i < size; i++){
        *output++ = random8();
    }
    return 0;
}

void randomNumberGenerator(unsigned char *output, size_t size){
    for(uint32_t i = 0; i < size; i++){
        *output++ = random8();
    }
}

void vector3ToBuffer(vector3_s vector, uint8_t buffer[6]){
    uint16_t *buffer_ = (uint16_t *)buffer;
    buffer_[0] = vector.e[0];
    buffer_[1] = vector.e[1];
    buffer_[2] = vector.e[2];
}

void vector3ToBuffer(vector3_f vector, uint8_t buffer[12]){
    float *buffer_ = (float *)buffer;
    buffer_[0] = vector.e[0];
    buffer_[1] = vector.e[1];
    buffer_[2] = vector.e[2];
}

uint32_t pcg32_random(pcg32_random_state* randomState)
{
    uint64_t oldState = randomState->state;
    randomState->state = oldState * 6364136223846793005ULL + (randomState->inc|1);
    uint32_t xorShifted = (uint32_t)(((oldState >> 18u) ^ oldState) >> 27u);
    uint32_t rot = oldState >> 59u;
    return (xorShifted >> rot) | (xorShifted << ((~rot + 1) & 31));
}

int32_t pcg32_randomNumberGenerator(void *userData, unsigned char *output, size_t size){
    pcg32_random_state *randomState = (pcg32_random_state *)userData;
    size >>= 2;
    for(uint32_t i = 0; i < size; i++){
        *output++ = pcg32_random(randomState);
    }
    return 0;
}