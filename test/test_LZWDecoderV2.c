#include "unity.h"
#include "CException.h"
#include "Dictionary.h"
#include "mock_InStream.h"
#include "mock_OutStream.h"
#include "ErrorCode.h"
#include "LZWDecoder.h"
#include <string.h>

#define bitSize   8

char *(*oldGetDictTranslation)(Dictionary *dict, int inputIndex);

char *newGetDictTranslation(Dictionary *dict, int inputIndex);
void updateDictionary(Dictionary *dict, char code, int index);
void emitCodeForTesting(Dictionary *dict, int index, OutStream *out);
void lzwDecodeForTesting(InStream *in, Dictionary *dict, OutStream *out);
int getBitsToReadForTesting(Dictionary *dict);

void setUp(void){
  oldGetDictTranslation = _getDictTranslation;
  _getDictTranslation = newGetDictTranslation;
}
void tearDown(void){
  _getDictTranslation = oldGetDictTranslation;
}

char *newGetDictTranslation(Dictionary *dict, int inputIndex){
  char *translation = "";

  if((dict->length) < inputIndex)
    Throw(ERR_EXCEEDING_DICTIONARY_SIZE);

  translation = dict->entries[inputIndex].code;
  
  return translation;
}

void lzwDecodeForTesting(InStream *in, Dictionary *dict, OutStream *out){
  int inputCode, dictIndex = 4, bitLimit, bitsToRead, counter = 0;
  char *currentString, *translation, *newDictEntry;
  
  bitsToRead = getBitsToReadForTesting(dict);
  inputCode = streamReadBits(in, (bitsToRead - 1));
  emitCodeForTesting(dict, inputCode, out);
  translation = _getDictTranslation(dict, inputCode);
  currentString = translation;
  bitLimit = 1 << (bitsToRead - 1);
  
  while(inputCode != -1){
    inputCode = streamReadBits(in, bitsToRead);
    if(inputCode == -1)
      Throw(END_OF_STREAM);
    else if(inputCode < -1)
      Throw(ERR_INVALID_INDEX);
    translation = _getDictTranslation(dict, inputCode);
    
    if(translation == NULL)
      translation = codeNewAndAppend(currentString, currentString[0]);
    else{}//Do nothing
    
    newDictEntry = codeNewAndAppend(currentString, translation[0]);
    
    if(dictionaryAdd(dict, newDictEntry, dictIndex) == 1){
      dictIndex++;
    }
    else{
      Throw(ERR_EXCEEDING_DICTIONARY_SIZE);
    }
      
    emitCodeForTesting(dict, inputCode, out);
    currentString = translation;
    
    counter++;
    if(counter == bitLimit){
      bitsToRead++;
      bitLimit = 1 << (bitsToRead - 1);
    }
  }
}

void emitCodeForTesting(Dictionary *dict, int index, OutStream *out){
  char *translation;
  int i;
  
  if(index >= 0){
    translation = _getDictTranslation(dict, index);
    for(i = 0; i < strlen(translation); i++){
      streamWriteBits(out, translation[i], bitSize);
    }
  }
  else{
    Throw(ERR_INVALID_INDEX);
  }
}

int getBitsToReadForTesting(Dictionary *dict){
  int i;
  
  for(i = 0; i < dict->size; i++){
    if((1 << i) > dict->size){
      return i;
    }
  }
}

void updateDictionary(Dictionary *dict, char code, int index){
  char *codeToAdd = codeNewAndAppend("", code);
  int temp;
  
  temp = dictionaryAdd(dict, codeToAdd, index);
}

/*
 *Input : 0, 1, 2, 3, 4, 2
 *
 *Output : abcdabc
 */
void test_lzwDecode_given_bits_to_read_2_should_increase_to_4(){
	CEXCEPTION_T e;
  Dictionary *dictionary = dictionaryNew(20);
  InStream in;
  OutStream out;
  char *result;
  
  dictionary->size = 0;
	updateDictionary(dictionary, 'a', 0);
	updateDictionary(dictionary, 'b', 1);
	updateDictionary(dictionary, 'c', 2);
	updateDictionary(dictionary, 'd', 3);
  
  streamReadBits_ExpectAndReturn(&in, 2, 0);
  streamWriteBits_Expect(&out, 97, 8);
  streamReadBits_ExpectAndReturn(&in, 3, 1);
  streamWriteBits_Expect(&out, 98, 8);
  streamReadBits_ExpectAndReturn(&in, 3, 2);
  streamWriteBits_Expect(&out, 99, 8);
  streamReadBits_ExpectAndReturn(&in, 3, 3);
  streamWriteBits_Expect(&out, 100, 8);
  streamReadBits_ExpectAndReturn(&in, 3, 4);
  streamWriteBits_Expect(&out, 97, 8);
  streamWriteBits_Expect(&out, 98, 8);
  streamReadBits_ExpectAndReturn(&in, 4, 2);
  streamWriteBits_Expect(&out, 99, 8);
  streamReadBits_ExpectAndThrow(&in, 4, END_OF_STREAM);
  
  Try{
    lzwDecodeForTesting(&in, dictionary, &out);
	}Catch(e){
    TEST_ASSERT_EQUAL(END_OF_STREAM, e);
    TEST_ASSERT_EQUAL_STRING("ab", dictionary->entries[4].code);
    TEST_ASSERT_EQUAL_STRING("bc", dictionary->entries[5].code);
    TEST_ASSERT_EQUAL_STRING("cd", dictionary->entries[6].code);
    TEST_ASSERT_EQUAL_STRING("da", dictionary->entries[7].code);
    TEST_ASSERT_EQUAL_STRING("abc", dictionary->entries[8].code);
  }
}

/*
 *Input : 0, 4, 5, 6, 7, 0
 *
 *Output : aaaaaaaaaaaaaaaaa (17 'a's)
 */
void test_lzwDecode_given_bits_to_read_2_should_increase_to_4_case_2(){
	CEXCEPTION_T e;
  Dictionary *dictionary = dictionaryNew(20);
  InStream in;
  OutStream out;
  char *result;
  
  dictionary->size = 0;
	updateDictionary(dictionary, 'a', 0);
	updateDictionary(dictionary, 'b', 1);
	updateDictionary(dictionary, 'c', 2);
	updateDictionary(dictionary, 'd', 3);
  
  streamReadBits_ExpectAndReturn(&in, 2, 0);
  streamWriteBits_Expect(&out, 97, 8);
  streamReadBits_ExpectAndReturn(&in, 3, 4);
  streamWriteBits_Expect(&out, 97, 8);
  streamWriteBits_Expect(&out, 97, 8);
  streamReadBits_ExpectAndReturn(&in, 3, 5);
  streamWriteBits_Expect(&out, 97, 8);
  streamWriteBits_Expect(&out, 97, 8);
  streamWriteBits_Expect(&out, 97, 8);
  streamReadBits_ExpectAndReturn(&in, 3, 6);
  streamWriteBits_Expect(&out, 97, 8);
  streamWriteBits_Expect(&out, 97, 8);
  streamWriteBits_Expect(&out, 97, 8);
  streamWriteBits_Expect(&out, 97, 8);
  streamReadBits_ExpectAndReturn(&in, 3, 7);
  streamWriteBits_Expect(&out, 97, 8);
  streamWriteBits_Expect(&out, 97, 8);
  streamWriteBits_Expect(&out, 97, 8);
  streamWriteBits_Expect(&out, 97, 8);
  streamWriteBits_Expect(&out, 97, 8);
  streamReadBits_ExpectAndReturn(&in, 4, 0);
  streamWriteBits_Expect(&out, 97, 8);
  streamReadBits_ExpectAndThrow(&in, 4, END_OF_STREAM);
  
  Try{
    lzwDecodeForTesting(&in, dictionary, &out);
	}Catch(e){
    TEST_ASSERT_EQUAL(END_OF_STREAM, e);
    TEST_ASSERT_EQUAL_STRING("aa", dictionary->entries[4].code);
    TEST_ASSERT_EQUAL_STRING("aaa", dictionary->entries[5].code);
    TEST_ASSERT_EQUAL_STRING("aaaa", dictionary->entries[6].code);
    TEST_ASSERT_EQUAL_STRING("aaaaa", dictionary->entries[7].code);
    TEST_ASSERT_EQUAL_STRING("aaaaaa", dictionary->entries[8].code);
  }
}