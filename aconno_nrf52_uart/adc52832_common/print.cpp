/* 
 * aconno.de
 * Made by Karlo Milicevic @ aconno, 2017
 * All rights reserved 
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "print.h"

struct text_buffer
{
    uint32_t size;
    char *at;
};

inline void printChar(text_buffer *buffer, char value)
{
    if(buffer->size)
    {
        --buffer->size;
        *buffer->at++ = value;
    }
}

inline void printChars(text_buffer *buffer, char *value)
{
    // NOTE: slow?
    while(*value)
    {
        printChar(buffer, *value++);
    }
}

int32_t int32FromStr(char **atInit){
    int32_t result = 0;
    char *at = *atInit;
    while((*at >= '0') && (*at <= '9')){
        result *= 10;
        result += (*at - '0');
        ++at;
    }
    *atInit = at;
    return result;
}

char decChars[] = "0123456789";
char lowerHexChars[] = "0123456789abcdef";
char upperHexChars[] = "0123456789ABCDEF";
void uint64ToString(text_buffer *buffer, uint64_t value, uint8_t base, char *digits){
    //Assert((base != 0) && (base <= 16));
    char *start = buffer->at;
    do{
        printChar(buffer, digits[(value % base)]);
        value /= base;
    } while(value != 0);
    char *end = buffer->at;
    while(start < end){
        --end;
        char temp = *end;
        *end = *start;
        *start = temp;
        ++start;
    }
}

uint32_t stringToHex(char *buffer, int32_t bufferSize, char *input, int32_t inputSize){
    uint32_t initialSize;
    while((bufferSize -= 2) >= 0 && inputSize--){
        *buffer++ = lowerHexChars[(*input) >> 4];
        *buffer++ = lowerHexChars[(*input++) & 0x0F];
    }
    return initialSize - bufferSize;
}

uint32_t stringToHexPretty(char *buffer, int32_t bufferSize, char *input, int32_t inputSize){
    uint32_t initialSize;
    while((bufferSize -= 5) >= 0 && inputSize--){
        *buffer++ = '0';
        *buffer++ = 'x';
        *buffer++ = lowerHexChars[(*input) >> 4];
        *buffer++ = lowerHexChars[(*input++) & 0x0F];
        *buffer++ = ' ';
    }
    return initialSize - bufferSize;
}


// NOTE: does not round up nor does it handle scientific notation
void doubleToString(text_buffer *buffer, double value, uint32_t precision, bool forceSign = false){
    if(value < 0){
        printChar(buffer, '-');
        value = -value;
    } else if(forceSign){
        printChar(buffer, '+');
    }
    
    uint64_t integerPart = (uint64_t)value;
    value -= (double)integerPart;
    uint64ToString(buffer, integerPart, 10, decChars);
    
    printChar(buffer, '.');
    
    // NOTE: not accurate
    for(uint32_t precisionIndex = 0; precisionIndex < precision; ++precisionIndex){
        value *= 10.0f;
        uint32_t integer = (uint32_t)value;
        value -= (float)integer;
        printChar(buffer, decChars[integer]);
    }
}

uint32_t printInternal(uint32_t bufferSize, char *bufferInit, char *format, va_list argList){
    text_buffer buffer = {bufferSize, bufferInit};
    if(buffer.size){
        char *at = format;
        while(at[0]){
            if(*at == '%'){
                ++at;
                
                bool forceSign = false;
                bool padWithZeros = false;
                bool leftJustify = false;
                bool positiveSignIsBlank = false;
                bool prefixIfNotZero = false;
                
                bool isParsing = true;
                
                // flags
                isParsing = true;
                while(isParsing){
                    switch(*at){
                        case '+': {forceSign = true;} break;
                        case '0': {padWithZeros = true;} break;
                        case '-': {leftJustify = true;} break;
                        case ' ': {positiveSignIsBlank = true;} break;
                        case '#': {prefixIfNotZero = true;} break;
                        default: {isParsing = false;} break;
                    }
                    
                    if(isParsing){
                        ++at;
                    }
                }
                
                // width
                //bool widthSpecified = false;
                int32_t width = 0;
                if(*at == '*'){
                    width = va_arg(argList, int32_t);
                    //widthSpecified = true;
                    ++at;
                }
                else if((*at >= '0') && (*at <= '9')){
                    width = int32FromStr(&at);
                    //widthSpecified = true;
                }
                
                // precision
                bool precisionSpecified = false;
                int32_t precision = 0;
                if(*at == '.'){
                    ++at;
                    
                    if(*at == '*'){
                        precision = va_arg(argList, int32_t);
                        precisionSpecified = true;
                        ++at;
                    }
                    else if((*at >= '0') && (*at <= '9')){
                        precision = int32FromStr(&at);
                        precisionSpecified = true;
                    }
                    else{
                        //Assert(!"Invalid precision");
                    }
                }
                
                if(!precisionSpecified){
                    precision = 6;
                }
                
                // length
                uint32_t integerLength = 4;
                //uint32_t floatLength = 8;
                // TODO: actually implement different integer/float sizes
                if((at[0] == 'h') && (at[1] == 'h')){
                    at += 2;
                }
                else if((at[0] == 'l') && (at[1] == 'l')){
                    integerLength = 8;
                    at += 2;
                }
                else if(*at == 'h'){
                    // TODO: set properly
                    ++at;
                }
                else if(*at == 'l'){
                    integerLength = 8;
                    ++at;
                }
                
                char bufferTemp[64];
                char *tempBufferStart = bufferTemp;
                text_buffer tempBuffer = {sizeof(bufferTemp), tempBufferStart};
                char *prefix = "";
                bool isFloat = false;
                
                switch(*at){
                    case 'd':
                    case 'i':
                    {
                        int64_t value;
                        if(integerLength == 8){
                            value = va_arg(argList, int64_t);
                        }
                        else{
                            value = va_arg(argList, int32_t);
                        }
                        bool wasNegative = (value < 0);
                        if(wasNegative){
                            value = -value;
                        }
                        uint64ToString(&tempBuffer, (uint64_t)value, 10, decChars);
                        
                        if(wasNegative){
                            prefix = "-";
                        }
                        else if(forceSign){
                            prefix = "+";
                        }
                        else if(positiveSignIsBlank){
                            prefix = " ";
                        }
                    } break;
                    
                    case 'u':
                    {
                        uint64_t value;
                        if(integerLength == 8){
                            value = va_arg(argList, uint64_t);
                        }
                        else{
                            value = (uint64_t)va_arg(argList, uint32_t);
                        }
                        uint64ToString(&tempBuffer, value, 10, decChars);
                    } break;
                    
                    case 'o':
                    {
                        uint64_t value;
                        if(integerLength == 8){
                            value = va_arg(argList, uint64_t);
                        }
                        else{
                            value = (uint64_t)va_arg(argList, uint32_t);
                        }
                        uint64ToString(&tempBuffer, value, 8, decChars);
                        if(prefixIfNotZero && (value != 0)){
                            prefix = "0";
                        }
                    } break;
                    
                    case 'x':
                    {
                        uint64_t value;
                        if(integerLength == 8){
                            value = va_arg(argList, uint64_t);
                        }
                        else{
                            value = (uint64_t)va_arg(argList, uint32_t);
                        }
                        uint64ToString(&tempBuffer, value, 16, lowerHexChars);
                        if(prefixIfNotZero && (value != 0)){
                            prefix = "0x";
                        }
                    } break;
                    
                    case 'X':
                    {
                        uint64_t value;
                        if(integerLength == 8){
                            value = va_arg(argList, uint64_t);
                        }
                        else{
                            value = (uint64_t)va_arg(argList, uint32_t);
                        }
                        uint64ToString(&tempBuffer, value, 16, upperHexChars);
                        if(prefixIfNotZero && (value != 0)){
                            prefix = "0X";
                        }
                    } break;
                    
                    case 'f':
                    case 'F':
                    case 'e':
                    case 'E':
                    case 'g':
                    case 'G':
                    case 'a':
                    case 'A':
                    {
                        // TODO: actually use floatLength here
                        double value = va_arg(argList, double);
                        doubleToString(&tempBuffer, value, precision, forceSign);
                        isFloat = true;
                    } break;
                    
                    case 'c':
                    {
                        int32_t value = va_arg(argList, int32_t);
                        printChar(&tempBuffer, (char)value);
                    } break;
                    
                    case 's':
                    {
                        char *str = va_arg(argList, char *);
                        
                        // TODO: use precision, width, etc.
                        tempBufferStart = str;
                        if(precisionSpecified){
                            tempBuffer.size = 0;
                            for(char *scan = str; *scan && (tempBuffer.size < precision); ++scan){
                                ++tempBuffer.size;
                            }
                        }
                        else{
                            tempBuffer.size = strlen(str);
                        }
                        tempBuffer.at = str + tempBuffer.size;
                    } break;
                    
                    case 'p':
                    {
                        void *value = va_arg(argList, void *);
                        uint64ToString(&tempBuffer, *(uint32_t *)&value, 16, lowerHexChars);
                    } break;
                    
                    case '%':
                    {
                        printChar(&buffer, '%');
                    } break;
                    
                    default:
                    {
                        //Assert(!"Unrecognized format specifier");
                    } break;
                }
                
                if(tempBuffer.at - tempBufferStart){
                    int32_t usePrecision = precision;
                    if(isFloat || !precisionSpecified){
                        usePrecision = (tempBuffer.at - tempBufferStart);
                    }
                    
                    int32_t prefixLength = strlen(prefix);
                    int32_t useWidth = width;
                    int32_t computedWidth = usePrecision + prefixLength;
                    if(useWidth < computedWidth){
                        useWidth = computedWidth;
                    }
                    
                    if(padWithZeros){
                        //Assert(!leftJustify); // Makes no sense to left justify and pad
                        leftJustify = false;
                    }
                    
                    if(!leftJustify){
                        while(useWidth > (usePrecision + prefixLength)){
                            printChar(&buffer, padWithZeros ? '0' : ' ');
                            --useWidth;
                        }
                    }
                    
                    for(char *pre = prefix; *pre && useWidth; ++pre){
                        printChar(&buffer, *pre);
                        --useWidth;
                    }
                    
                    if(usePrecision > useWidth){
                        usePrecision = useWidth;
                    }
                    while(usePrecision > (tempBuffer.at - tempBufferStart)){
                        printChar(&buffer, '0');
                        --usePrecision;
                        --useWidth;
                    }
                    while(usePrecision && (tempBuffer.at != tempBufferStart)){
                        printChar(&buffer, *tempBufferStart++);
                        --usePrecision;
                        --useWidth;
                    }
                    
                    if(leftJustify){
                        while(useWidth){
                            printChar(&buffer, ' ');
                            --useWidth;
                        }
                    }
                }
                
                if(*at){
                    ++at;
                }
            }
            else{
                printChar(&buffer, *at++);
            }
        }
        
        if(buffer.size){
            buffer.at[0] = 0;
        }
        else{
            buffer.at[-1] = 0;
        }
    }
    
    uint32_t result = buffer.at - bufferInit;
    return result;
}

uint32_t print(char *buffer, uint32_t bufferSize, char *format, ...) {
    va_list argList;
    
    va_start(argList, format);
    uint32_t result = printInternal(bufferSize, buffer, format, argList);
    va_end(argList);
    
    return result;
}
