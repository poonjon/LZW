#include "CException.h"
#include "Dictionary.h"
#include "InStream.h"
#include "OutStream.h"
#include "LZWDecoder.h"
#include "ErrorCode.h"
#include "Utils.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define getIndex(x) (x - 256)
#define bitWriteSize     8
#define bitReadSize     12

char *(*_getDictTranslation)(Dictionary *dict, int inputIndex) = getDictTranslation;

/*To decode a given stream of inputs
 *
 *Input: *in -> the input stream from file.
 *       *dict -> the dictionary.
 *       *out -> the output stream to file.
 *
 *Output: -
 *
 *Throw:  ERR_INVALID_INDEX -> Thrown when given input code is <0.
 *        ERR_EXCEEDING_DICTIONARY_SIZE -> Thrown when attempting to add another
 *                                         entry to a full custom dictionary
 *
 */
void lzwDecode(InStream *in, Dictionary *dict, OutStream *out){
  int inputCode, dictIndex = 0;
  char *currentString, *translation, *newDictEntry;
  
  inputCode = streamReadBits(in, bitReadSize);
  
  if(inputCode <= -1 || inputCode > (dict->length))
    Throw(ERR_INVALID_INDEX);
    
  emitCode(dict, inputCode, out);
  translation = _getDictTranslation(dict, inputCode);
  currentString = translation;
  
  while(1){
    if(dictIndex == 3840){//4096 - 256. For renewing dictionary together with encoder.
      dictionaryDel(dict);
      dictIndex = 0;
      inputCode = streamReadBits(in, bitReadSize);
      emitCode(dict, inputCode, out);
      translation = _getDictTranslation(dict, inputCode);
      currentString = translation;
    }
    
    inputCode = streamReadBits(in, bitReadSize);
    
    if(inputCode < -1 || inputCode > (dict->length))
      Throw(ERR_INVALID_INDEX);
      
    translation = _getDictTranslation(dict, inputCode);
    
    if(translation == NULL)//To deal with missing dictionary entry
      translation = codeNewAndAppend(currentString, currentString[0]);
    else{}//Do nothing
    
    newDictEntry = codeNewAndAppend(currentString, translation[0]);
    
    if(dictionaryAdd(dict, newDictEntry, dictIndex) == 1){
      dictIndex++;
    }
    else{
      Throw(ERR_EXCEEDING_DICTIONARY_SIZE);
    }
    
    emitCode(dict, inputCode, out);
    currentString = translation;
  }
}

/*To find the translation of a given code if it is within the custom dictionary
 *
 *Input: *dict -> the dictionary.
 *       inputIndex -> input code.
 *
 *Output:translation -> the translation of the input code.
 *
 *Throw:  -
 *
 */
char *getDictTranslation(Dictionary *dict, int inputIndex){
  char *translation = "";
  int index;
  
  if(inputIndex >= 0 && inputIndex < 256)
    translation = codeNewAndAppend("", (uint8)inputIndex);
  else if(inputIndex > 255){
    index = getIndex(inputIndex);//To find the corresponding index for the custom dictionary
    translation = dict->entries[index].code;
  }
  
  return translation;
}

/*To find the translation of a given code and output through streamWriteBits function.
 *
 *Input: *dict -> the dictionary.
 *       inputIndex -> input code (which is also the dictionary index to be searched).
 *       *out  -> output stream to file.
 *
 *Output: -
 *
 *Throw:  ERR_INVALID_INDEX -> Thrown if index is <0.
 *
 */
void emitCode(Dictionary *dict, int inputIndex, OutStream *out){
  char *translation;
  int i;
  
  if(inputIndex < 256 && inputIndex > 0){
    streamWriteBits(out, inputIndex, bitWriteSize);
  }
  else if(inputIndex == 0){}//Do Nothing
  else if(inputIndex >= 256){
    translation = _getDictTranslation(dict, inputIndex);
    for(i = 0; i < strlen(translation); i++){
      streamWriteBits(out, translation[i], bitWriteSize);
    }
  }
  else{
    Throw(ERR_INVALID_INDEX);
  }
}