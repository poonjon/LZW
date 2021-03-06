#include "unity.h"
#include "CException.h"
#include "ErrorCode.h"
#include "InStream.h"

void setUp(){}
void tearDown(){}

unsigned int tempCurrentByte;

void test_openInStream_given_nonexistant_file_should_throw_error(){
	CEXCEPTION_T e;
  InStream *in;

  Try{
    in = openInStream("NonExistant.txt", "r");
  }Catch(e){
    TEST_ASSERT_EQUAL(ERR_CANNOT_OPEN_FILE, e);
  }
}

void test_openInStream_given_existant_file_should_read_a(){
	CEXCEPTION_T e;
  InStream *in;
  int result;

  Try{
    in = openInStream("test/data/InputTest.txt", "r");
    result = fgetc(in->file);
    closeInStream(in);
  }Catch(e){
    TEST_ASSERT_EQUAL(ERR_CANNOT_OPEN_FILE, e);
  }

  TEST_ASSERT_EQUAL('a', result);
}

void test_openInStream_given_existant_file_should_read_246(){
	CEXCEPTION_T e;
  InStream *in;
  int result;

  Try{
    in = openInStream("test/data/read_test_246.txt", "r");
    result = fgetc(in->file);
    closeInStream(in);
  }Catch(e){
    TEST_ASSERT_EQUAL(ERR_CANNOT_OPEN_FILE, e);
  }

  TEST_ASSERT_EQUAL(246, result);
}

void test_openInStream_given_existant_file_should_read_6(){
	CEXCEPTION_T e;
  InStream *in;
  int result;

  Try{
    in = openInStream("test/data/InputTest_2.txt", "r");
    result = fgetc(in->file);
    closeInStream(in);
  }Catch(e){
    TEST_ASSERT_EQUAL(ERR_CANNOT_OPEN_FILE, e);
  }

  TEST_ASSERT_EQUAL('6', result);
}

void test_streamReadBits_given_246_should_return_a(){
	CEXCEPTION_T e;
  InStream *in;
  int result;
  tempCurrentByte = 0;
  Try{
    in = openInStream("test/data/read_test_246.txt", "r");
  }Catch(e){
    TEST_ASSERT_EQUAL(ERR_CANNOT_OPEN_FILE, e);
  }
  
  Try{
    result = streamReadBits(in, 12);
  }Catch(e){
    TEST_ASSERT_EQUAL(END_OF_STREAM, e);
  }
  
  TEST_ASSERT_EQUAL(0, in->bitIndex);
  TEST_ASSERT_EQUAL(0, in->currentByte);
  
  closeInStream(in);

  TEST_ASSERT_EQUAL(246, result);
}

void test_streamReadBits_given_a_0x06_0x10_should_return_a(){
	CEXCEPTION_T e;
  InStream *in;
  int result;
  tempCurrentByte = 0;
  Try{
    in = openInStream("test/data/InputTest_3.txt", "r");
  }Catch(e){
    TEST_ASSERT_EQUAL(ERR_CANNOT_OPEN_FILE, e);
  }
  
  Try{
    result = streamReadBits(in, 12);
  }Catch(e){
    TEST_ASSERT_EQUAL(END_OF_STREAM, e);
  }
  
  TEST_ASSERT_EQUAL(4, in->bitIndex);
  TEST_ASSERT_EQUAL(0, in->currentByte);
  
  closeInStream(in);

  TEST_ASSERT_EQUAL('a', result);
}

void test_streamReadBits_given_a_0x10_0x00_should_return_256(){
	CEXCEPTION_T e;
  InStream *in;
  int result;
  tempCurrentByte = 0;
  Try{
    in = openInStream("test/data/InputTest_4.txt", "r");
  }Catch(e){
    TEST_ASSERT_EQUAL(ERR_CANNOT_OPEN_FILE, e);
  }

  Try{
    result = streamReadBits(in, 12);
  }Catch(e){
    TEST_ASSERT_EQUAL(END_OF_STREAM, e);
  }  
    
  TEST_ASSERT_EQUAL(4, in->bitIndex);
  TEST_ASSERT_EQUAL(0, in->currentByte);
  
  closeInStream(in);

  TEST_ASSERT_EQUAL(256, result);
}

void test_streamReadBits_given_a_0x10_0x01_0x01_should_return_256_and_257(){
	CEXCEPTION_T e;
  InStream *in;
  int result, result2;
  tempCurrentByte = 0;
  Try{
    in = openInStream("test/data/InputTest_5.txt", "r");
  }Catch(e){
    TEST_ASSERT_EQUAL(ERR_CANNOT_OPEN_FILE, e);
  }

  result = streamReadBits(in, 12);
  result2 = streamReadBits(in, 12);
    
  TEST_ASSERT_EQUAL(0, in->bitIndex);
  TEST_ASSERT_EQUAL(0, in->currentByte);
  
  closeInStream(in);

  TEST_ASSERT_EQUAL(256, result);
  TEST_ASSERT_EQUAL(257, result2);
}

void test_streamReadBits_given_a_0x06_0x11_0x01_should_return_97_and_257(){
	CEXCEPTION_T e;
  InStream *in;
  int result, result2;
  tempCurrentByte = 0;
  Try{
    in = openInStream("test/data/InputTest_6.txt", "r");
  }Catch(e){
    TEST_ASSERT_EQUAL(ERR_CANNOT_OPEN_FILE, e);
  }

  Try{
    result = streamReadBits(in, 12);
    result2 = streamReadBits(in, 12);
  }Catch(e){
    TEST_ASSERT_EQUAL(END_OF_STREAM, e);
  }  
    
  TEST_ASSERT_EQUAL(0, in->bitIndex);
  TEST_ASSERT_EQUAL(0, in->currentByte);
  
  closeInStream(in);

  TEST_ASSERT_EQUAL(97, result);
  TEST_ASSERT_EQUAL(257, result2);
}

void test_streamReadBits_given_a_0x06_0x20_0x61_0x06_0xe0_should_return_98_97_110(){
	CEXCEPTION_T e;
  InStream *in;
  int result, result2, result3;
  tempCurrentByte = 0;
  Try{
    in = openInStream("test/data/InputTest_7.txt", "r");
  }Catch(e){
    TEST_ASSERT_EQUAL(ERR_CANNOT_OPEN_FILE, e);
  }

  Try{
    result = streamReadBits(in, 12);
    result2 = streamReadBits(in, 12);
    result3 = streamReadBits(in, 12);
  }Catch(e){
    TEST_ASSERT_EQUAL(END_OF_STREAM, e);
  }  
    
  TEST_ASSERT_EQUAL(4, in->bitIndex);
  TEST_ASSERT_EQUAL(0, in->currentByte);
  
  closeInStream(in);

  TEST_ASSERT_EQUAL(98, result);
  TEST_ASSERT_EQUAL(97, result2);
  TEST_ASSERT_EQUAL(110, result3);
}

void test_streamReadBit_given_0x97_should_return_1(){
	int result;

  result = streamReadBit(0x97);

  TEST_ASSERT_EQUAL(1, result);
}

void test_streamReadBit_given_0x37_should_return_0(){
	int result;

  result = streamReadBit(0x37);

  TEST_ASSERT_EQUAL(0, result);
}