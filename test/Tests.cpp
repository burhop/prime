#include <iostream>
#include <exception>
#include "Prime.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

size_t TestFindPrimes1(int blocks, size_t blocksize, size_t mmaxValueToSearch)
{
	//Create our Class with a given block size
	//try {
		Prime prime(blocksize);
		//if you have no data, lets find some primes
		prime.FindPrimes(blocks);
		// Lets see how mnay we found
		size_t count = prime.CountPrimes(0, mmaxValueToSearch);
		return count;

	//}
	//catch (std::exception &e)
	//{
	//	const char* error= e.what();
	//	return -1;
	//}
}
	
int Factorial(int number) {
	return number <= 1 ? number : Factorial(number - 1) * number;  // fail
    //return number <= 1 ? 1      : Factorial( number - 1 ) * number;  // pass
}

TEST_CASE("Find Primes using different block sizes (pass)", "[single-file]") {
	REQUIRE(TestFindPrimes1(1, 300, 100) == 25);
	REQUIRE(TestFindPrimes1(1, 300, 10) == 4);
	REQUIRE(TestFindPrimes1(1, 3000, 100) == 25);
	//check with zero data sets. No exception since we can give a valid answer
	REQUIRE(TestFindPrimes1(0, 300, 100)==0);
}
TEST_CASE("Find Primes using different block sizes (fail)", "[single-file]") {
	REQUIRE_THROWS(TestFindPrimes1(1, 301, 100));
	REQUIRE_THROWS(TestFindPrimes1(-1, 300, 100));
	REQUIRE_THROWS(TestFindPrimes1(0xFFFFFFF1, 300, 100));
}

//TEST_CASE("Factorials of 1 and higher are computed (pass)", "[single-file]") {
//	REQUIRE(Factorial(1) == 1);
//	REQUIRE(Factorial(2) == 2);
//	REQUIRE(Factorial(3) == 6);
//	REQUIRE(Factorial(10) == 3628800);
//}