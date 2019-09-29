// PrimeExamples.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"
#include <string>
#include "Prime.h"
#include <ctime>
#include <chrono>
#include <iostream>
#include <vector>
#include <bitset>
int main(int argc, char*argv[])
{
	//Create our Class with a block size of 3000 bits
	Prime prime(300);
	//Lets time it.
	//auto start = std::chrono::system_clock::now();
	prime.ContinousRun(std::string("tada"));
	// see how many primes under 10 billion
	size_t maxValueToSearch = 1000000; // 10000000000;
	//if you have no data, lets find some primes.  Do 4 blocks
	prime.FindPrimes(3);
	// Lets see how mnay we found
	size_t count = prime.CountPrimes(0,maxValueToSearch);
	std::cout << "Found " << count << " prime numbers between 0 and " << maxValueToSearch << std::endl;

	// Lets see how many primes in groups of 100
	std::cout << "0-100   : " << prime.CountPrimes(0, 100)   << std::endl;
	std::cout << "101-200 : " << prime.CountPrimes(101, 200) << std::endl;
	std::cout << "201-300 : " << prime.CountPrimes(201, 300) << std::endl;
	std::cout << "301-400 : " << prime.CountPrimes(301, 400) << std::endl;
	std::cout << "0-900 : " << prime.CountPrimes(0, 900) << std::endl;
	std::cout << "0-10000 : " << prime.CountPrimes(0, 10000) << std::endl;
	std::cout << "0-100000 : " << prime.CountPrimes(0, 100000) << std::endl;
	std::cout << "0-1000000 : " << prime.CountPrimes(0, 1000000) << std::endl;
	std::cout << "0-10000000 : " << prime.CountPrimes(0, 10000000) << std::endl;
	std::cout << "0-100000000 : " << prime.CountPrimes(0, 100000000) << std::endl;
	std::cout << "0-1000000000 : " << prime.CountPrimes(0, 1000000000) << std::endl;
	std::cout << "0-10000000000 : " << prime.CountPrimes(0, 10000000000) << std::endl;
	std::cout << "0-100000000000 : " << prime.CountPrimes(0, 100000000000) << std::endl;
	std::cout << "0-1000000000000 : " << prime.CountPrimes(0, 1000000000000) << std::endl;
	std::cout << "0-10000000000000 : " << prime.CountPrimes(0, 10000000000000) << std::endl;

	//Lets see the primes under 900.
	std::vector<size_t> primes = prime.GetPrimesAsVector(0, 900);
	count = 1;
	for (size_t i : primes)
		std::cout << count++<<" : "<< i << std::endl;


	//Lets Save our primes
	prime.SaveToFile("myPrimes");

	//Lets create a new Prime class instance and load it up with the data we just saved. Data block sizes must be the same.
	Prime prime2 = Prime(3000);
	prime2.LoadFromFile("myPrimes");

	size_t count2 =prime2.CountPrimes(0, 3000);
	std::cout << "Found " << count2 << " prime numbers between 0 and " << 3000 << std::endl;
	// Lets see how many primes in groups of 100
	std::cout << "0-100   : " << prime2.CountPrimes(0, 100) << std::endl;
	std::cout << "101-200 : " << prime2.CountPrimes(101, 200) << std::endl;
	std::cout << "201-300 : " << prime2.CountPrimes(201, 300) << std::endl;
	std::cout << "301-400 : " << prime2.CountPrimes(301, 400) << std::endl;
	std::cout << "0-10000 : " << prime2.CountPrimes(0, 10000) << std::endl;
	std::cout << "0-100000 : " << prime2.CountPrimes(0, 100000) << std::endl;
	std::cout << "0-1000000 : " << prime2.CountPrimes(0, 1000000) << std::endl;
	std::cout << "0-10000000 : " << prime2.CountPrimes(0, 10000000) << std::endl;
	std::cout << "0-100000000 : " << prime2.CountPrimes(0, 100000000) << std::endl;
	std::cout << "0-1000000000 : " << prime2.CountPrimes(0, 1000000000) << std::endl;
	std::cout << "0-10000000000 : " << prime2.CountPrimes(0, 10000000000) << std::endl;
	std::cout << "0-100000000000 : " << prime2.CountPrimes(0, 100000000000) << std::endl;
	std::cout << "0-1000000000000 : " << prime2.CountPrimes(0, 1000000000000) << std::endl;
	std::cout << "0-10000000000000 : " << prime2.CountPrimes(0, 10000000000000) << std::endl;

//	std::bitset<Prime::BitBlockSize> primeAsBitset = 
//	boost::dynamic_bitset<> *bStart = primeSieve();
//vectorOfBitSets.push_back(bStart);
//boost::dynamic_bitset<> *bSecond = primeSieve2(vectorOfBitSets);
//vectorOfBitSets.push_back(bSecond);
//boost::dynamic_bitset<> *bThird = primeSieve2(vectorOfBitSets);
//vectorOfBitSets.push_back(bThird);
//boost::dynamic_bitset<> *bForth = primeSieve2(vectorOfBitSets);
//size_t count1 = countPrimes(bStart);
//size_t count2 = countPrimes(bSecond);
//size_t count3 = countPrimes(bThird);
//size_t count4 = countPrimes(bForth);
//size_t sum = count1 + count2 + count3 + count4;
//std::cout << "Prime count : " << sum << "\n";
//boost::dynamic_bitset<> *bComp = new boost::dynamic_bitset<>();
//compressBitSet(bStart, bComp);
//writeBitSetToStream("d:\\binary.bin", bComp);
////boost::dynamic_bitset<>* bNext=primeSieve2(bStart);
//boost::dynamic_bitset<> *bCompIn = new boost::dynamic_bitset<>();
//readBitSetFromStream("d:\\binary.bin", bCompIn);
//boost::dynamic_bitset<> *bFromFile = new boost::dynamic_bitset<>();
//uncompressBitSet(bCompIn, bFromFile);
////boost::dynamic_bitset<> *bNext2 = new boost::dynamic_bitset<>();
////size_t count2 = countPrimes(bFromFile);
////std::cout << "Prime count : " << count2 << "\n";
//
//writeSparseBitSetToStream("d:\\binarySparse.bin", bStart);
//readSparseBitSetFromStream("d:\\binarySparse.bin", bNext2);

//bigPrime();
//WritePrime();
//auto end = std::chrono::system_clock::now();
//
//std::chrono::duration<double> elapsed_seconds = end - start;
//std::time_t end_time = std::chrono::system_clock::to_time_t(end);
//#pragma warning(disable : 4996)
//std::cout << "finished computation at " << std::ctime(&end_time)
//<< "elapsed time: " << elapsed_seconds.count() << "s\n";
//std::cout << "end\n";
return 0;
}
