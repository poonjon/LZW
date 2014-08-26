#include "CException.h"
#include "InStream.h"
#include "ErrorCode.h"
#include <stdio.h>
#include <malloc.h>

unsigned int tempCurrentByte;

/*To open a file with read method for reading
 *
 *Input: *fileName -> the file name.
 *       *openMethod -> open method ("w" for write, "r" for read)
 *
 *Output:inStream -> pointer to InStream struct
 *
 *Throw:  ERR_CANNOT_OPEN_FILE -> Thrown when the file cannot be found.
 *
 */
InStream *openInStream(char *fileName, char *openMethod){
	InStream *inStream = calloc(1, sizeof(InStream));

	inStream->file = fopen(fileName, openMethod);

  if(inStream->file == NULL)
    Throw(ERR_CANNOT_OPEN_FILE);

	inStream->filename = fileName;
	inStream->currentByte = 0;
	inStream->bitIndex = 0;
	inStream->byteIndex = 0;

  return inStream;
}

/*To read a number of bits from the file
 *
 *Input: *in -> pointer to InStream struct.
 *       bitSize -> number of bits to read.
 *
 *Output:outputWhole -> the combined bits read as an integer.
 *
 *Throw:  -
 *
 */
int streamReadBits(InStream *in, int bitSize){
  int i, inputByte, bitCount = 0, byteCount = 0;
  uint32 outputWhole = 0;
  uint8 byteToRead, tempBitRead = 0;
  
  if(feof(in->file) != 0){
    in->byteIndex = -1;
    return -1;
  }

  while(byteCount <= (bitSize/8)){
  
    if(tempCurrentByte != 0){
      in->currentByte = (uint8)(tempCurrentByte & 0x0FF);
      byteToRead = in->currentByte;
    }
    else{
      inputByte = fgetc(in->file);
      // printf("inputByte = %d\n", inputByte);
      if(inputByte == EOF){
        // printf("EOF reached\n");
        break;}
      byteToRead = inputByte;
      // printf("read Input : %x\n", inputByte);
      // printf("bit Index : %d\n", in->bitIndex);
      // printf("bit count : %d\n", bitCount);
    }

    for(i = in->bitIndex; i < 8; i++){
      outputWhole = outputWhole << 1;
      tempBitRead = streamReadBit(byteToRead);
      outputWhole = outputWhole | tempBitRead;
      byteToRead = byteToRead << 1;
      in->bitIndex++;
      // printf("bit count : %d\n", bitCount);
      if(in->bitIndex == 8)
        in->bitIndex = 0;
      if(bitCount == (bitSize - 1))
        break;
      else
        bitCount++;
    }
    if(i == 8){
      // printf("not full output\n");
      byteCount++;
      tempCurrentByte = 0;
      // printf("outputWhole : %x\n", outputWhole);
      // printf("bit size : %d\n", bitSize);
      // printf("bit count : %d\n", bitCount);
    }
    else if(bitCount == (bitSize - 1)){
      in->currentByte = byteToRead;
      // printf("full output\n");
      if(in->bitIndex != 0)
        tempCurrentByte = in->currentByte | 0x100;
      break;
    }
  }
  // printf("outputWhole : %d\n", outputWhole);
  return outputWhole;
}

/*To obtain a single bit from the byte read
 *
 *Input: byteToRead -> the byte read.
 *
 *Output:bitRead -> the bit(MSB) obtained
 *
 *Throw:  -
 *
 */
int streamReadBit(uint8 byteToRead){
  int mask = 0x80, bitRead = 0;

  bitRead = (byteToRead & mask) >> 7;

  return bitRead;
}

/*To close the file and free the InStream struct
 *
 *Input: *in -> pointer to InStream struct
 *
 *Output: -
 *
 *Throw:  -
 *
 */
void closeInStream(InStream *in){
  fclose(in->file);
  free(in);
}