#include "unity.h"
#include "CException.h"
#include "ErrorCode.h"
#include "OutStream.h"

void setUp(){}

void tearDown(){}

void test_openOutStream_given_output_test_file_should_write_d_to_file(){
  OutStream *out;

  out = openOutStream("test/data/OutputTest.txt", "w");
  fputc('d', out->file);
  closeOutStream(out);
}

void test_streamWriteBit_given_bitToWrite_1_should_write_1_to_currentByte(){
  OutStream *out;

  out = openOutStream("test/data/OutputTestBitWrite.txt", "w");
  
  streamWriteBit(out, 1);
  
  TEST_ASSERT_EQUAL(0x01, out->currentByte);
  TEST_ASSERT_EQUAL(1, out->bitIndex);
  
  closeOutStream(out);
}

void test_streamWriteBit_given_bitToWrite_1_should_write_1_to_currentByte_case_2(){
  OutStream *out;

  out = openOutStream("test/data/OutputTestBitWrite.txt", "w");
  out->currentByte = 0x02;
  out->bitIndex = 1;
  streamWriteBit(out, 1);
  
  TEST_ASSERT_EQUAL(0x03, out->currentByte);
  TEST_ASSERT_EQUAL(2, out->bitIndex);
  
  closeOutStream(out);
}

void test_streamWriteBits_given_bitsToWrite_97_should_write_a_to_file(){
  OutStream *out;

  out = openOutStream("test/data/OutputTestBitsWrite_1.txt", "w");

  streamWriteBits(out, 97, 8);
  
  TEST_ASSERT_EQUAL(0, out->currentByte);
  TEST_ASSERT_EQUAL(1, out->bitIndex);
  
  closeOutStream(out);
}

void test_streamWriteBits_given_bitsToWrite_97_and_98_should_write_ab_to_file(){
  OutStream *out;

  out = openOutStream("test/data/OutputTestBitsWrite_2.txt", "w");

  streamWriteBits(out, 97, 8);
  
  TEST_ASSERT_EQUAL(0, out->currentByte);
  TEST_ASSERT_EQUAL(1, out->bitIndex);
  
  streamWriteBits(out, 98, 8);
  
  TEST_ASSERT_EQUAL(0, out->currentByte);
  TEST_ASSERT_EQUAL(2, out->bitIndex);
  
  closeOutStream(out);
}

void test_streamWriteBits_given_bitsToWrite_98_97_110_should_write_ban_to_file(){
  OutStream *out;

  out = openOutStream("test/data/OutputTestBitsWrite_3.txt", "w");

  streamWriteBits(out, 98, 8);
  
  TEST_ASSERT_EQUAL(0, out->currentByte);
  TEST_ASSERT_EQUAL(1, out->bitIndex);
  
  streamWriteBits(out, 97, 8);
  
  TEST_ASSERT_EQUAL(0, out->currentByte);
  TEST_ASSERT_EQUAL(2, out->bitIndex);
  
  streamWriteBits(out, 110, 8);
  
  TEST_ASSERT_EQUAL(0, out->currentByte);
  TEST_ASSERT_EQUAL(3, out->bitIndex);
  
  closeOutStream(out);
}

void test_openOutStream_given_currentByte_0x70_should_flush_p_to_file(){
  OutStream *out;

  out = openOutStream("test/data/OutputFlushTest.txt", "w");

  out->currentByte = 0x70;
  
  streamFlush(out);
}