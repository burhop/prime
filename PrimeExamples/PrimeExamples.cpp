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
	
	// 
	int maxValueToSearch = 10000;
	//if you have no data, lets find some primes
	prime.FindPrimes(10);
	// Lets see how mnay we found
	size_t count = prime.CountPrimes(0,maxValueToSearch);
	std::cout << "Found " << count << " prime numbers between 0 and " << maxValueToSearch << std::endl;

	// Lets see how many primes in groups of 100
	std::cout << "0-100   : " << prime.CountPrimes(0, 100)   << std::endl;
	std::cout << "101-200 : " << prime.CountPrimes(101, 200) << std::endl;
	std::cout << "201-300 : " << prime.CountPrimes(201, 300) << std::endl;
	std::cout << "301-400 : " << prime.CountPrimes(301, 400) << std::endl;

	//Lets see the primes under 100.
	std::vector<size_t> primes = prime.GetPrimesAsVector(0, 100);
	count = 1;
	for (size_t i : primes)
		std::cout << count++<<" : "<< i << std::endl;


	//Lets Save our primes
	prime.SaveToFile("myPrimes");

	//Lets create a new Prime class instance and load it up with the data we just saved
	Prime prime2 = Prime();
	prime2.LoadFromFile("myPrimes");

	count =prime2.CountPrimes(0, 1000);
	std::cout << "Found " << count << " prime numbers between 0 and 1000" << std::endl;

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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
