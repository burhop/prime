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
	//Create our Class
	Prime prime = Prime();
	//Lets time it.
	//auto start = std::chrono::system_clock::now();
	
	// see how many primes under 10 billion
	size_t maxValueToSearch = 3000; // 10000000000;
	//if you have no data, lets find some primes 
	prime.FindPrimes(40);
	// Lets see how mnay we found
	size_t count = prime.CountPrimes(0,maxValueToSearch);
	std::cout << "Found " << count << " prime numbers between 0 and " << maxValueToSearch << std::endl;

	// Lets see how many primes in groups of 100
	std::cout << "0-100   : " << prime.CountPrimes(0, 100)   << std::endl;
	std::cout << "101-200 : " << prime.CountPrimes(101, 200) << std::endl;
	std::cout << "201-300 : " << prime.CountPrimes(201, 300) << std::endl;
	std::cout << "301-400 : " << prime.CountPrimes(301, 400) << std::endl;
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

	//Lets see the primes under 100.
	std::vector<size_t> primes = prime.GetPrimesAsVector(0, 100);
	count = 1;
	//for (size_t i : primes)
	//	std::cout << count++<<" : "<< i << std::endl;


	//Lets Save our primes
	prime.SaveToFile("myPrimes");

	//Lets create a new Prime class instance and load it up with the data we just saved
	Prime prime2 = Prime();
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
//	std::bitset<max> *bStart = primeSieve();
//vectorOfBitSets.push_back(bStart);
//std::bitset<max> *bSecond = primeSieve2(vectorOfBitSets);
//vectorOfBitSets.push_back(bSecond);
//std::bitset<max> *bThird = primeSieve2(vectorOfBitSets);
//vectorOfBitSets.push_back(bThird);
//std::bitset<max> *bForth = primeSieve2(vectorOfBitSets);
//size_t count1 = countPrimes(bStart);
//size_t count2 = countPrimes(bSecond);
//size_t count3 = countPrimes(bThird);
//size_t count4 = countPrimes(bForth);
//size_t sum = count1 + count2 + count3 + count4;
//std::cout << "Prime count : " << sum << "\n";
//std::bitset<max5> *bComp = new std::bitset<max5>();
//compressBitSet(bStart, bComp);
//writeBitSetToStream("d:\\binary.bin", bComp);
////std::bitset<max>* bNext=primeSieve2(bStart);
//std::bitset<max5> *bCompIn = new std::bitset<max5>();
//readBitSetFromStream("d:\\binary.bin", bCompIn);
//std::bitset<max> *bFromFile = new std::bitset<max>();
//uncompressBitSet(bCompIn, bFromFile);
////std::bitset<max> *bNext2 = new std::bitset<max>();
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
