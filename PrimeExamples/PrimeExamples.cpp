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
const size_t arraySize = 0xFFFFFFF0;
int main(int argc, char*argv[])
{
	//Create our Instance
	Prime prime(arraySize);
	std::cout << " Selection Option: \n";
	std::cout << "     1 - find and count primes \n";
	std::cout << "     2 - read existing files and count primes. \n";

	int opt;
	std::cin >> opt;

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
	}
return 0;
}
