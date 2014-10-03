#include "Dictionary.h"
#include "CException.h"
#include "ErrorCode.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "InStream.h"

unsigned char currentByte;

/*
 *  To create a new custom dictionary.
 *
 *  Input: length -> the length dictionary.
 *
 * Output: dictionary -> the dictionary.
 *
 *  Throw:  -
 *
 */
Dictionary *dictionaryNew(int length){
	Dictionary *dictionary = malloc(sizeof(Dictionary));
	dictionary->entries = calloc(length, sizeof(DictionaryEntry));
	dictionary->length = length;
  dictionary->size = 256;
	
	return dictionary;
}

/*
 * To append a character to a string
 *
 * Input: *oldCode -> the old string
 *        codeToAppend -> the character to be appended to the old string.
 *
 * Output:newCode -> the new string with the appended character.
 *
 * Throw:  -
 *
 */
char *codeNewAndAppend(char *oldCode, char codeToAppend){
	char *newCode = malloc((strlen(oldCode)) + 1 + 1); 
	int codeLen = strlen(oldCode);
	
	newCode = memcpy(newCode, oldCode, codeLen);
	newCode[codeLen] = codeToAppend;
	newCode[codeLen+1] = '\0';
	
	return newCode;
}

/*To add an entry into the custom dictionary.
 *
 *Input: *dict -> the dictionary.
 *       *code -> the string(entry) to be added into the custom dictionary.
 *       index -> the index where the new entry should be inserted at.
 *
 *Output:1 -> if entry was successfully added.
 *       0 -> if entry failed to be added into custom dictionary.
 *
 *Throw:  -
 *
 */
int dictionaryAdd(Dictionary *dict, char *code, int index){
  
  if(index >= dict->length || index < 0){
    return 0;
  }
  else{
    dict->entries[index].code = code;
    dict->entries[index].length = strlen(code);
    dict->size++;
    return 1;
  }
}

/*To delete the dictionary.
 *
 *Input: *dict -> the dictionary.
 *
 *Output: -
 *
 *Throw:  -
 *
 */
void dictionaryDel(Dictionary *dict){
	int i;
	
	for(i = 0; i < dict->length; i++){
		if(dict->entries[i].code != NULL){
      dict->entries[i].code = NULL;
    }
	}
}

/*
* to find the longest matching dictionary entry
*
* input : -        in: the input from file
*         -dictionary: the dictionary
*
* output: -&dictionary->entries[markIndex]: the longest matching entry
*/
DictionaryEntry *dictionaryFindLongestMatchingEntry(InStream *in, Dictionary *dictionary){
  int byte = 0, index = 0, markIndex = 0, longestCode = 0, k = 0;
  int e;
  
 /*
  * check if in->currentbyte is 0
  *  0: byte will get value from file
  * !0: byte will get value from in->currentbyte
  * 
  * firstMarkIndex will locate the first dictionary index that matches byte
  */
  if(currentByte == 0)
    byte = streamReadBits(in, 8);
  else
    byte = currentByte;
  
  markIndex = firstMarkIndex(dictionary, byte);
  
  //to loop through dictionary
  for(index = 0 ; dictionary->length > index ; index++){

    //ensure code is not null
    if(dictionary->entries[markIndex].code != NULL && dictionary->entries[index].code != NULL){

      /* 
      * compare if marked code and current code is the same, if not go to the next entry
      * longestCode will determine how many characters markIndex and index should compare
      */
      if(isBlockSame(dictionary->entries[markIndex].code, dictionary->entries[index].code, longestCode) == 0){

        for(k ; dictionary->entries[index].length > k ; k++){

          /*
          * compare if current byte is the same with byte in entry
          *     same: mark current index and mark code as longest
          * not same: return to entry
          *
          * destroy: longest code & mark index
          */
          if(byte == dictionary->entries[index].code[k]){
            markIndex = index;
            longestCode = k;
          }

          else if(byte != dictionary->entries[index].code[k])
           break;
          
          //after marking longest index, read a byte
          Try{
            byte = streamReadBits(in, 8);
          
            //solution to problem: streamReadBits will read null before reading -1, therefore encoder will write an extra byte
            if(byte == 0){
              byte = streamReadBits(in, 8);
            }
          }Catch(e){
            if(e == END_OF_STREAM){
              in->byteIndex = -1;
              return &dictionary->entries[markIndex];
            }
          }
          
          //save byte in currentByte global variable
          currentByte = byte;
        }
      }
    }
  }
  
  return &dictionary->entries[markIndex];
}

/*
* to compare source and source2
*
* input : -  source: the first source for comparison 
*         - source2: the second source for comparison
*         -byteSize: the amount of bits that source and source2 should compare
*
* output: -result: 0 if same, 1 if different
*/
int isBlockSame(char *source, char *source2, int byteSize){
  int i, result = 0;
  
  for(i = 0 ; byteSize >= i ; i++){

    if(source[i] == source2[i]){
    }
    else{
      result = 1;
      break;
    }
  }
  
  return result;
}

/*
* to find the first entry for the code to start
*
* note: this function will cause bad memory if dictionary->entries[i].code[0] were to access an empty entry but this function
*       will not access an empty entry because dictionaryEntryInitializer will initialize all 255 entries with ASCII characters
*       and this function will only be called by dictionaryFindLongestMatchingEntry which will provide the value for the
*       "byte" parameter, which must be an ASCII character  
*
* input : -dictionary: the dictionary
*         -      byte: the input byte to be located in the dictionary
*
* output: -i: the dictionary index with the code that matches with the input byte
*/
int firstMarkIndex(Dictionary *dictionary, int byte){
  int i, code;

  for(i = 0 ; i < 256 ; i++){

    if(byte == dictionary->entries[i].code[0])
      return i;
  }
}

//to initialize ASCII into dictionary
void dictionaryEntryInitializer(Dictionary *dictionary){
  int i;
  
  for(i = 0 ; i < 256 ; i++){
    dictionary->entries[i].code = codeNewAndAppend("", i);
    dictionary->entries[i].length = 1;
  }

}
