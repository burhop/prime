#include <iostream>
#include <exception>
#include "Prime.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

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


TEST_CASE("Test counting of primes is correct (pass)", "[single-file]") {

	REQUIRE(TestMaxCount(1,    30) == 10);
	REQUIRE(TestMaxCount(10,   60) == 109);
	REQUIRE(TestMaxCount(1000, 90) == 8713);
	REQUIRE(TestMaxCount(1,    3000000) == 216816);
	REQUIRE(TestMaxCount(321,   3000) == 75845);
	REQUIRE(TestMaxCount(999, 900) == 71217);
}
TEST_CASE("Test Max Prime is correct (pass)", "[single-file]") {

	REQUIRE(TestMaxPrime(1, 30) == 29);
	REQUIRE(TestMaxPrime(10, 60) == 599);
	REQUIRE(TestMaxPrime(1000, 90) == 89989);
	REQUIRE(TestMaxPrime(1, 3000000) == 2999999);
	REQUIRE(TestMaxPrime(321, 3000) == 962993);
	REQUIRE(TestMaxPrime(999, 900) == 899069);
}

TEST_CASE("Test Max Value is correct (pass)", "[single-file]") {

	REQUIRE(TestMaxValue(1, 30) == 30);
	REQUIRE(TestMaxValue(10, 60) == 600);
	REQUIRE(TestMaxValue(1000, 90) == 90000);
	REQUIRE(TestMaxValue(1, 3000000) == 3000000);
	REQUIRE(TestMaxValue(321, 3000) == 963000);
	REQUIRE(TestMaxValue(999, 900) == 899100);
}
TEST_CASE("Test Max Count is correct with no calculation (pass)", "[single-file]") {

	Prime prime(30);
	REQUIRE(prime.GetMaxCount() == 0);
}
TEST_CASE("Test Max Prime is correct with no calculation (pass)", "[single-file]") {

	Prime prime(30);
	REQUIRE(prime.GetMaxPrime() == 0);
}

TEST_CASE("Test Max Value is correct with no calculation (pass)", "[single-file]") {

	Prime prime(30);
	REQUIRE(prime.GetMaxValue() == 0);
}
TEST_CASE("Test for class instance trying to load wrong size file (fail)", "[single-file]") {

	REQUIRE_THROWS(TestFileErrors1() == 0);
}


TEST_CASE("Find Primes using different block sizes (pass)", "[single-file]") {
	REQUIRE(TestFindPrimes1(1, 300, 100) == 25);
	REQUIRE(TestFindPrimes1(1, 300, 10) == 4);
	REQUIRE(TestFindPrimes1(1, 3000, 100) == 25);
	REQUIRE(TestFindPrimes1(1, 3000, 3000) == 430);
	//check with zero data sets. No exception since we can give a valid answer
	REQUIRE(TestFindPrimes1(0, 300, 100) == 0);
}
TEST_CASE("Find Primes using different number of blocks(pass)", "[single-file]") {
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
TEST_CASE("Find Primes using different block sizes (fail)", "[single-file]") {
	REQUIRE_THROWS(TestFindPrimes1(1, 301, 100));
	REQUIRE_THROWS(TestFindPrimes1(-1, 300, 100));
	REQUIRE_THROWS(TestFindPrimes1(0xFFFFFFF1, 300, 100));
}

TEST_CASE("Verify saving and loading of prime files works)", "[single-file]") {
	REQUIRE(TestFindPrimes2(2, 300, 100) == 25);
	REQUIRE(TestFindPrimes2(3, 300, 10) == 4);
	REQUIRE(TestFindPrimes2(3, 300, 900) == 154);
	REQUIRE(TestFindPrimes2(2, 3000, 3000) == 430);
	REQUIRE(TestFindPrimes2(2, 3000, 6000) == 783);
	REQUIRE(TestFindPrimes2(10, 3000, 9000) == 1117);
	REQUIRE(TestFindPrimes2(10, 3000, 30000) == 3245);
	REQUIRE(TestFindPrimes2(10, 3000, 30000) == 3245);
	REQUIRE(TestFindPrimes2(10, 3000, 30000) == 3245);
	REQUIRE(TestFindPrimes2(100, 300, 30000) == 3245);
	REQUIRE(TestFindPrimes2(1000, 300, 30000) == 3245);
	REQUIRE(TestFindPrimes2(1000, 300, 300000) == 25997);
}
