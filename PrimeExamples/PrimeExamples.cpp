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
	//Create our Instance
	Prime prime(0xFFFFFFF0);
	//Lets time it.
	//auto start = std::chrono::system_clock::now();
	// see how many primes under 10 billion
	size_t maxValueToSearch = 1000000; // 10000000000;
	//if you have no data, lets find some primes.  Do 4 blocks
	std::cout << " Selection Option: \n";
	std::cout << "     1 - count primes \n";
	std::cout << "     2 - read existing files \n";

	int opt;
	std::cin >> opt;

	if (opt == 1)
	{

		prime.FindPrimes(3);
		// Lets see how mnay we found
		size_t count = prime.CountPrimes(0, maxValueToSearch);
		std::cout << "Found " << count << " prime numbers between 0 and " << maxValueToSearch << std::endl;

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
		Prime prime2 = Prime(3000);
		prime2.LoadFromFile("myPrimes");

		std::cout << "total integers: " << prime2.GetMaxCount();
		std::cout << "Largest Prime : " << prime2.GetMaxPrime();
		std::cout << "Primes Found  : " << prime2.GetMaxCount();

		size_t count2 = prime2.CountPrimes(0, 3000);
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
	}
	else if (opt == 2)
	{
		prime.LoadFromFile("RunRunRun");
		std::cout << "total integers: " << prime.GetMaxCount();
		std::cout << "Largest Prime : " << prime.GetMaxPrime();
		std::cout << "Primes Found  : " << prime.GetMaxCount();

		std::cout << "0-10000000000000 : " << prime.CountPrimes(0, 10000000000000) << std::endl;
	}
return 0;
}
