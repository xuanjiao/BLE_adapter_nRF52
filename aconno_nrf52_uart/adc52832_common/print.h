/* 
 * aconno.de
 * Made by Karlo Milicevic @ aconno, 2017
 * All rights reserved 
 *
 */

#ifndef PRINT_H
#define PRINT_H

/** Print function that mimics the functionality of snprintf_s/snprintf
 * @returns
 *     number of bytes written to the buffer
 */
uint32_t print(char *buffer, uint32_t bufferSize, char *format, ...);

/** Converts ASCII to hex.
 * @notes
 *     buffer should be at least 2 times the size of the original to completely print
 * @returns
 *     number of bytes written to the buffer
 * example:
 *     "abc" -> "616263"
 */
uint32_t stringToHex(char *buffer, int32_t bufferSize, char *input, int32_t inputSize);
/** Converts ASCII to hex.
 * @notes
 *     buffer should be at least 5 times the size of the original to completely print
 * @returns
 *     number of bytes written to the buffer
 * example:
 *     "abc" -> "0x61 0x62 0x63 "
 */
uint32_t stringToHexPretty(char *buffer, int32_t bufferSize, char *input, int32_t inputSize);

#endif