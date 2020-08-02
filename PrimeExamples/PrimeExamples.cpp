// PrimeExamples.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// Output should look like this depending on how many primes you have searched for.

/* 

Selection Option:
1 - find and count primes
2 - read existing files and count primes.
2
total integers : 8408427012
Largest Prime : 210453396701
Primes Found : 8408427012
0 - 100 : 25
101 - 200 : 20
201 - 300 : 16
301 - 400 : 16
0 - 10000 : 1229
0 - 100000 : 9592
0 - 1000000 : 78498
0 - 10000000 : 664579
0 - 100000000 : 5761455
0 - 1000000000 : 50847534
0 - 10000000000 : 455052511
0 - 100000000000 : 4118054813
0 - 1000000000000 : 0
0 - 10000000000000 : 0

*/

//#include "pch.h"
#include <string>
#include "Prime.h"
#include <ctime>
#include <chrono>
#include <iostream>
#include <vector>
#include <bitset>
#include <iostream>
#include <fstream> 
#include <boost/format.hpp>
const size_t arraySize = 0xFFFFFFF0;
void CreatePrimeMetaData()
{
	std::string filename = "Findme.txt";
	size_t size = 1000;
	size_t index = 1000;
	std::ofstream outfile;
	outfile.open(filename, std::ios::out | std::ios::binary);
	outfile.write((char*)&size, sizeof(size_t));
	outfile.write((char*)&index, sizeof(size_t));
	outfile.close();
	//std::cout << boost::format("#1.0 %16i %16i %16i") % size % index % 0xFFFFFFF0;
	Prime prime("RunRunRun", 0, 2);
	size_t numbersPerBlock = prime.GetBitBlockSize();
	size_t maxValue = prime.GetMaxValue();
	size_t numberOfBlocks = maxValue / numbersPerBlock;
	size_t totalCount = 0;
	for (size_t i = 0; i < numberOfBlocks; ++i)
	{
		size_t start = i * numbersPerBlock;
		size_t end = start + numbersPerBlock;
		size_t count = prime.CountPrimes(start,end);
		std::cout << boost::format("%16i %16i %16i\n") % start % end % count;
		//std::cout << start << "\t - " << end << "\t" << count << std::endl;
		totalCount = totalCount + count;
		std::cout << "total primes less than " << end << "\t " << totalCount << std::endl;
	}

	//prime.LoadFromFile("RunRunRun");
	//size_t bitsPerBlock = prime.GetBitBlockSize();

}



int main(int argc, char*argv[])
{
	//Create our Instance
	Prime prime(arraySize);
	std::cout << " Selection Option: \n";
	std::cout << "     1 - find and count primes \n";
	std::cout << "     2 - read existing files and count primes. \n";
	std::cout << "     3 - Create metadata on files. \n";

	int opt;
	//std::cin >> opt;
	opt = 3;
	if (opt == 1)
	{

		prime.FindPrimes(3);
		// Lets see how many we found
		size_t count = prime.GetMaxCount(); // prime.CountPrimes(0, maxValueToSearch);
		size_t maxValue= prime.GetMaxValue();
		std::cout << "Found " << count << " prime numbers between 0 and " << maxValue << std::endl;

		// Lets see how many primes in groups of 100
		std::cout << "0-100   : " << prime.CountPrimes(0, 100) << std::endl;
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
			std::cout << count++ << " : " << i << std::endl;


		//Lets Save our primes
		prime.SaveToFile("myPrimes");

		//Lets create a new Prime class instance and load it up with the data we just saved. Data block sizes must be the same.
		Prime prime2 = Prime(arraySize);

		try {
			prime2.LoadFromFile("myPrimes");
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}

		std::cout << "total integers: " << prime2.GetMaxValue() << std::endl;
		std::cout << "Largest Prime : " << prime2.GetMaxPrime() << std::endl;
		std::cout << "Primes Found  : " << prime2.GetMaxCount() << std::endl;

		count = prime2.GetMaxCount(); // prime.CountPrimes(0, maxValueToSearch);
		maxValue = prime2.GetMaxValue();
		std::cout << "Found " << count << " prime numbers between 0 and " << maxValue << std::endl;
		// Lets see how many primes in groups of 100
		std::cout << "0-1000   : " << prime2.CountPrimes(0, 1000) << std::endl;
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
	}
	else if (opt == 2)
	{
		prime.LoadFromFile("RunRunRun");
		std::cout << "Total integers searched: " << prime.GetMaxValue() << std::endl;
		std::cout << "Largest prime : " << prime.GetMaxPrime() << std::endl;
		std::cout << "Total primes Found  : " << prime.GetMaxCount() << std::endl;
		std::cout << "0-1000 : " << prime.CountPrimes(0, 1000) << std::endl;
		std::cout << "0-10,000 : " << prime.CountPrimes(0, 10000) << std::endl;
		std::cout << "0-100,000 : " << prime.CountPrimes(0, 100000) << std::endl;
		std::cout << "0-1,000,000 : " << prime.CountPrimes(0, 1000000) << std::endl;
		std::cout << "0-10,000,000 : " << prime.CountPrimes(0, 10000000) << std::endl;
		std::cout << "0-100,000,000 : " << prime.CountPrimes(0, 100000000) << std::endl;
		std::cout << "0-1,000,000,000 : " << prime.CountPrimes(0, 1000000000) << std::endl;
		std::cout << "0-10,000,000,000 : " << prime.CountPrimes(0, 10000000000) << std::endl;
		std::cout << "0-100,000,000,000 : " << prime.CountPrimes(0, 100000000000) << std::endl;
		std::cout << "0-1,000,000,000,000 : " << prime.CountPrimes(0, 1000000000000) << std::endl;
		std::cout << "0-10,000,000,000,000 : " << prime.CountPrimes(0, 10000000000000) << std::endl;
	}
	else if (opt == 3)
	{
		CreatePrimeMetaData();
	}
return 0;
}
