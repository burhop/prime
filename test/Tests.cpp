#include <iostream>
#include <exception>
#include "Prime.h"
#include "bitblock.h"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
size_t TestBitBlockOMP(int number_of_threads, int blocksize)
{
	omp_set_num_threads(number_of_threads);
	try
	{
		BitBlock* block1 = new BitBlock("t1.prm", true, blocksize);
		BitBlock* block2 = new BitBlock("t2.prm", true, blocksize);
		BitBlock* block3 = new BitBlock("t3.prm", true, blocksize);
		BitBlock* block4 = new BitBlock("t4.prm", true, blocksize);
		BitBlock* block5 = new BitBlock("t4.prm", true, blocksize);
#pragma omp parallel
		{
			int th=omp_get_thread_num();
			std::cout <<th << "u1";
			block1->UnCache();
			std::cout << "uC2";
			block2->Uncompress();
			for (size_t i = 0; i < blocksize ; ++i)
			{
				block2->set(i, true);	
			}
			std::cout << "c1";
			block1->Cache();
			std::cout << "s1";
			block1->SaveFile("Junkt1.prm");
			std::cout << "U2";
			block2->UnCache();
			std::cout << "C3";
			block3->Compress();
			std::cout << "i4";
			block4->GetIndex();
		}
	}
	catch (std::exception e)
	{
		return 1;
	}
	Prime p;
	p.DeleteExistingPrimeFiles("t");
	return 0;
}
size_t TestFindPrimes1(int blocks, size_t blocksize, size_t mmaxValueToSearch)
{
	Prime prime(blocksize);

	//if you have no data, lets find some primes
	prime.FindPrimes(blocks);
	// Lets see how mnay we found
	size_t count = prime.CountPrimes(0, mmaxValueToSearch);
	return count;
}

size_t TestFindPrimes2(int blocks, size_t blocksize, size_t mmaxValueToSearch)
{
	Prime prime(blocksize);
	prime.DeleteExistingPrimeFiles("test");
	//if you have no data, lets find some primes
	prime.FindPrimes(blocks);
	// Save them and reload them
	prime.SaveToFile("test");
	Prime newPrime(blocksize);
	newPrime.LoadFromFile("test");
	size_t count = newPrime.CountPrimes(0, mmaxValueToSearch);
	prime.DeleteExistingPrimeFiles("test");
	return count;
}
size_t TestMultiCores(int blocks, size_t blocksize, size_t mmaxValueToSearch,unsigned int cores)
{

	Prime prime(blocksize,cores);
	//if you have no data, lets find some primes
	//prime.SetVerbose(true);
	prime.FindPrimes(blocks);
	std::cout << "If compiled with OpenMP, planning to use " << prime.GetThreadCount() << " threads\n";
	size_t count = prime.CountPrimes(0, mmaxValueToSearch);;
	return count;
}
size_t TestFileErrors1()
{

	Prime prime(300);
	prime.DeleteExistingPrimeFiles("test");
	//if you have no data, lets find some primes
	prime.FindPrimes(2);
	// Save them and reload them
	prime.SaveToFile("test");
	Prime newPrime(600);
	//should throw an exception
	try
	{
		newPrime.LoadFromFile("test");
	}
	catch (...)
	{
		//first clean up
		prime.DeleteExistingPrimeFiles("test");
		throw;
	}
	return 0;
}
size_t TestMaxCount(int blocks, size_t blocksize)
{
	Prime prime(blocksize);
	prime.FindPrimes(blocks);
	return prime.GetMaxCount();

}
size_t TestMaxCountCompressed(int blocks, size_t blocksize)
{
	Prime prime(blocksize);
	prime.FindPrimes(blocks);
	prime.Compress();
	return prime.GetMaxCount();
}
size_t TestMaxCountUncompressed(int blocks, size_t blocksize)
{
	Prime prime(blocksize);
	prime.FindPrimes(blocks);
	prime.Compress();
	prime.Uncompress();
	return prime.GetMaxCount();
}
size_t TestMaxPrime(int blocks, size_t blocksize)
{
	Prime prime(blocksize);
	prime.FindPrimes(blocks);
	return prime.GetMaxPrime();
}
size_t TestMaxValue(int blocks, size_t blocksize)
{
	Prime prime(blocksize);
	prime.FindPrimes(blocks);
	return prime.GetMaxValue();
}

TEST_CASE("Test Thread safety of Bitblock", "[single-file]")
{
	REQUIRE(TestBitBlockOMP(10, 3000) == 0);
	REQUIRE(TestBitBlockOMP(5, 3000) == 0);

	REQUIRE(TestBitBlockOMP(10, 30) == 0);
	REQUIRE(TestBitBlockOMP(10, 30000) == 0);
	REQUIRE(TestBitBlockOMP(1, 3000) == 0);
	REQUIRE(TestBitBlockOMP(10, 30) == 0);
	REQUIRE(TestBitBlockOMP(100, 300) == 0);

}

TEST_CASE("Test counting of primes is correct", "[single-file]") {

	REQUIRE(TestMaxCount(1,    30) == 10);
	REQUIRE(TestMaxCount(10,   60) == 109);
	REQUIRE(TestMaxCount(1000, 90) == 8713);
	REQUIRE(TestMaxCount(1,    3000000) == 216816);
	REQUIRE(TestMaxCount(321,   3000) == 75845);
	REQUIRE(TestMaxCount(999, 900) == 71217);
}
TEST_CASE("Test counting of compressed primes is correct", "[single-file]") {

	REQUIRE(TestMaxCountCompressed(1, 30) == 10);
	REQUIRE(TestMaxCountCompressed(10, 60) == 109);
	REQUIRE(TestMaxCountCompressed(1000, 90) == 8713);
	REQUIRE(TestMaxCountCompressed(1, 3000000) == 216816);
	REQUIRE(TestMaxCountCompressed(321, 3000) == 75845);
	REQUIRE(TestMaxCountCompressed(999, 900) == 71217);
}TEST_CASE("Test counting of primes that have been compressed and uncompressed is correct", "[single-file]") {

	REQUIRE(TestMaxCountUncompressed(1, 30) == 10);
	REQUIRE(TestMaxCountUncompressed(10, 60) == 109);
	REQUIRE(TestMaxCountUncompressed(1000, 90) == 8713);
	REQUIRE(TestMaxCountUncompressed(1, 3000000) == 216816);
	REQUIRE(TestMaxCountUncompressed(321, 3000) == 75845);
	REQUIRE(TestMaxCountUncompressed(999, 900) == 71217);
}
TEST_CASE("Test Max Prime is correct", "[single-file]") {

	REQUIRE(TestMaxPrime(1, 30) == 29);
	REQUIRE(TestMaxPrime(10, 60) == 599);
	REQUIRE(TestMaxPrime(1000, 90) == 89989);
	REQUIRE(TestMaxPrime(1, 3000000) == 2999999);
	REQUIRE(TestMaxPrime(321, 3000) == 962993);
	REQUIRE(TestMaxPrime(999, 900) == 899069);
}

TEST_CASE("Test Max Value is correct", "[single-file]") {

	REQUIRE(TestMaxValue(1, 30) == 30);
	REQUIRE(TestMaxValue(10, 60) == 600);
	REQUIRE(TestMaxValue(1000, 90) == 90000);
	REQUIRE(TestMaxValue(1, 3000000) == 3000000);
	REQUIRE(TestMaxValue(321, 3000) == 963000);
	REQUIRE(TestMaxValue(999, 900) == 899100);
}
TEST_CASE("Test Max Count is correct with no calculation", "[single-file]") {

	Prime prime(30);
	REQUIRE(prime.GetMaxCount() == 0);
}
TEST_CASE("Test Max Prime is correct with no calculation", "[single-file]") {

	Prime prime(30);
	REQUIRE(prime.GetMaxPrime() == 0);
}

TEST_CASE("Test Max Value is correct with no calculation", "[single-file]") {

	Prime prime(30);
	REQUIRE(prime.GetMaxValue() == 0);
}
// In version .4, it will now figure the right size of the file and load it. This is no longer an error.

//TEST_CASE("Test for class instance trying to load wrong size file (fail)", "[single-file]") {
//
//	REQUIRE_THROWS(TestFileErrors1() == 0);
//}


TEST_CASE("Find Primes using different block sizes", "[single-file]") {
	REQUIRE(TestFindPrimes1(1, 300, 100) == 25);
	REQUIRE(TestFindPrimes1(1, 300, 10) == 4);
	REQUIRE(TestFindPrimes1(1, 3000, 100) == 25);
	REQUIRE(TestFindPrimes1(1, 3000, 3000) == 430);
	//check with zero data sets. No exception since we can give a valid answer
	REQUIRE(TestFindPrimes1(0, 300, 100) == 0);
}
TEST_CASE("Find Primes using different number of blocks", "[single-file]") {
	REQUIRE(TestFindPrimes1(2, 300, 100) == 25);
	REQUIRE(TestFindPrimes1(3, 300, 10) == 4);
	REQUIRE(TestFindPrimes1(3, 300, 900) == 154);
	REQUIRE(TestFindPrimes1(2, 3000, 3000) == 430);
	REQUIRE(TestFindPrimes1(2, 3000, 6000) == 783);
	REQUIRE(TestFindPrimes1(10, 3000, 9000) == 1117);
	REQUIRE(TestFindPrimes1(10, 3000, 30000) == 3245);
	REQUIRE(TestFindPrimes1(10, 3000, 30000) == 3245);
	REQUIRE(TestFindPrimes1(10, 3000, 30000) == 3245);
	REQUIRE(TestFindPrimes1(100, 300, 30000) == 3245);
	REQUIRE(TestFindPrimes1(1000, 300, 30000) == 3245);
	REQUIRE(TestFindPrimes1(1000, 300, 300000) == 25997);

	//check with zero data sets. No exception since we can give a valid answer
	REQUIRE(TestFindPrimes1(0, 300, 100) == 0);
}
TEST_CASE("Find Primes using different block sizes (test throwing of exception)", "[single-file]") {
	REQUIRE_THROWS(TestFindPrimes1(1, 301, 100));
	REQUIRE_THROWS(TestFindPrimes1(-1, 300, 100));
	REQUIRE_THROWS(TestFindPrimes1(0xFFFFFFF1, 300, 100));
}

TEST_CASE("Verify saving and loading of prime files works", "[single-file]") {
	REQUIRE(TestFindPrimes2(2, 300, 100) == 25);
	REQUIRE(TestFindPrimes2(3, 300, 10) == 4);
	REQUIRE(TestFindPrimes2(3, 300, 900) == 154);
	REQUIRE(TestFindPrimes2(2, 3000, 3000) == 430);
	REQUIRE(TestFindPrimes2(2, 3000, 6000) == 783);
	REQUIRE(TestFindPrimes2(10, 3000, 9000) == 1117);
	REQUIRE(TestFindPrimes2(100, 300, 30000) == 3245);
	REQUIRE(TestFindPrimes2(1000, 300, 30000) == 3245);
	REQUIRE(TestFindPrimes2(1000, 300, 300000) == 25997);
}
TEST_CASE("Verify code works with varying numbers of threads/cores", "[single-file]") {

	REQUIRE(TestMultiCores(1000, 300, 300000,1) == 25997);
	REQUIRE(TestMultiCores(1000, 300, 300000,2) == 25997);
	REQUIRE(TestMultiCores(1000, 300, 300000,3) == 25997);
	REQUIRE(TestMultiCores(1000, 300, 300000,8) == 25997);
	REQUIRE(TestMultiCores(1000, 300, 300000,10) == 25997);
	REQUIRE(TestMultiCores(1000, 300, 300000,30) == 25997);
	REQUIRE(TestMultiCores(1000, 300, 300000, 0) == 25997);
	REQUIRE(TestMultiCores(1000, 3000, 3000000, 0) == 216816);
	REQUIRE(TestMultiCores(1000, 30000, 30000000, 0) == 1857859);
	REQUIRE(TestMultiCores(1000, 300000, 300000000, 0) == 16252325);
	REQUIRE(TestMultiCores(100, 3000, 300000, 3) == 25997);
	REQUIRE(TestMultiCores(100, 30000, 3000000, 3) == 216816);
	REQUIRE(TestMultiCores(100, 300000, 30000000, 3) == 1857859);
	REQUIRE(TestMultiCores(100, 3000000, 300000000, 3) == 16252325);

}