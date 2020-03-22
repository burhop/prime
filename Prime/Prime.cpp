#include <string>
#include <vector>
#include <iostream>
#include <fstream> 
#include "Prime.h"
#include <boost/dynamic_bitset.hpp>
#include <cstdio>
#include "bitblock.h"
#include <omp.h>
//Performance testing
#include <chrono> 


Prime::Prime(size_t bitsetSize)
{
	if (bitsetSize % 30)
	{
		throw std::exception("Prime bit block size must be divisible by 30 for block alignment");
	}
	if (bitsetSize > 0xFFFFFFF0)
	{
		throw std::exception("Maximum bitsetSize in Prime Constructor is 0xFFFFFFF0 (4294967280)");
	}
	if (bitsetSize == 0)
	{
		throw std::exception("bitBlockSize can not be zero");
	}
	maxValue = 0;
	bitBlockSize = bitsetSize;
	nextFreeBlockIndex = 0;
	max = (size_t)bitBlockSize;       // Can be up to 4294967280 for size_t;
	max2 = max / 2 + max % 2;         // don't need to save the even numbers
	max3 = 2 * max2 / 3 + max2 % 3;   // don't need to save numbers divisible by 3
	max5 = 4 * max3 / 5 + max3 % 5;   // don't need to save numbers divisible by 5
	arrayOfBlocks = new BitBlock*[0xFFFFF1]();  //Allocate an array of pointers and initialize them to 0.
	contiguousBlocks = 0;
}

int Prime::ContinousRun(std::string baseName)
{
	//Load any existing prime files to start wit
	this->LoadFromFile(baseName);
	this->baseFileName = baseName;
	this->SaveIncrementalFiles(true);
	this->FindPrimes((size_t)0xFFFFFFF0);
	return 0;
}
Prime::~Prime()
{
	//delete memory
	clearBitsetVector();
	delete[] arrayOfBlocks;
}
size_t Prime::GetBitBlockSize()
{
	return bitBlockSize;
}

void Prime::DeleteExistingPrimeFiles(std::string baseName)
{
	size_t fileCount = 1;
	bool nextFile = true;
	do {

		//Assume at least one file and start looping until we don't find any more
		std::string fileName = baseName + std::to_string(fileCount) + ".prm";
		//check if file is there
		std::ifstream infile(fileName);
		if (infile.good())
		{
			infile.close();
			if (remove(fileName.c_str()) != 0)
			{
				throw std::exception("Could not delete prime number file.");
			}
			fileCount++;
		}
		else
		{
			nextFile = false;
			break;
		}
	} while (nextFile);
	return;
}
void Prime::LoadFromFile(std::string baseName)
{
	size_t fileCount = 0;
	bool nextFile = true;
	//Assume at least one file and start looping until we don't find any more
	do
	{
		std::string fileName = baseName + std::to_string(fileCount) + ".prm";
		//check if file is there
		std::ifstream infile(fileName);
		if (!infile.fail())
		{
			BitBlock* bb = nullptr;
			if (verbose)
			{
				auto start = std::chrono::high_resolution_clock::now();
				BitBlock* bb = new BitBlock(fileName);
				auto stop = std::chrono::high_resolution_clock::now();
				auto duration = duration_cast<std::chrono::microseconds>(stop - start);
				std::cout << "TIme to load file " << fileName << ": " << duration.count() / 1000000. << " seconds." << std::endl;

			}
			else
			{
				BitBlock* bb = new BitBlock(fileName);
			}
			vectorOfBitSets.push_back(bb);
			arrayOfBlocks[fileCount - 1] = bb;
			updateContiguousBlocks();
		}
		else
		{
			//We didn't fine another file
			nextFile = false;
		}
		infile.close();
		fileCount++;
	} while (nextFile);
}

size_t Prime::GetMaxValue()
{
	//if there are any bitsets, it is because we have calulated the primes for it.
	return this->contiguousBlocks * this->max;

	//return this->vectorOfBitSets.size()*this->max;
}
size_t Prime::GetMaxPrime()
{
	//if (vectorOfBitSets.size() == 0) return 0;
	if (contiguousBlocks == 0) return 0;
	size_t bitsetIndex = 1;  //Start with the last bitset and work toward the front
	//BitBlock *lastBitset = vectorOfBitSets[vectorOfBitSets.size() - bitsetIndex];
	BitBlock* lastBitset = arrayOfBlocks[contiguousBlocks - 1];
	bool primeFlag = false;

	//GO to the end of the list and back up until we fined a prime.
	size_t index = max;

	do
	{
		index--;
		//If we get a bitset with no primes, reset to the end of the previous bitset
		if (index == 0)
		{
			index = max - 1;
			bitsetIndex = bitsetIndex + 1;
			//lastBitset = vectorOfBitSets[vectorOfBitSets.size() - bitsetIndex];
			lastBitset = arrayOfBlocks[contiguousBlocks - bitsetIndex];
		}
		primeFlag = (*lastBitset)[index];
	} while (primeFlag == false);

	//  remember, index starts at 0, so index+1 is the actual count in the bitset  (e.g 1st number is at index 0)
	//return (this->vectorOfBitSets.size() - 1) *this->max + index + 1;
	return (contiguousBlocks - 1) *this->max + index + 1;
}
size_t Prime::GetMaxCount()
{
	size_t count = this->CountPrimes(0, this->GetMaxPrime()+1);
	return count;
}

//TODO: REFACTOR to remove fileCount
int Prime::SaveToFile(std::string baseName)
 {
	size_t fileCount = 1;

	//Save a file for each bitset numbering them from 1 on....
	//for (auto b : vectorOfBitSets)
	for(size_t i=0;i<contiguousBlocks;++i)
	{	
			std::string fileName = baseName + std::to_string(fileCount) + ".prm";
			//b->SaveFile(fileName);
			arrayOfBlocks[i]->SaveFile(fileName);
			fileCount++;
	}
	return true;
}

//TODO needs to be written correctly.
std::vector<size_t> Prime::GetPrimesAsVector(size_t lowVal, size_t highVal)
{
	std::vector<size_t> primes;
	//size_t index = 1;
	//for (auto bs : this->vectorOfBitSets)
	//{
	//	for (size_t count = lowVal; count <highVal; count++)
	//	{
	//		bool isPrime = bs->test(count);
	//		if (isPrime)
	//		{
	//			primes.push_back(index);
	//		}
	//		index++;
	//	}
	//}
	return primes ;
}

size_t Prime::CountPrimes(size_t lowVal, size_t highVal)
{
	std::vector<size_t> primes;
	size_t count = 0;
	size_t startVector = lowVal / max;
	size_t endVector = (highVal-1) / max;
	size_t startLoc = lowVal % max;
	size_t endLoc = (highVal-1) % max;
	//Return 0 if caller asks to count more primes than have been calculated
	//if (endVector >= vectorOfBitSets.size() || lowVal>=highVal)
	if(endVector>=contiguousBlocks)
	{
		return 0;
	}
	//See if everything is in one bitset
	if (startVector == endVector)
	{
		//auto bs = vectorOfBitSets[startVector];
		auto bs = arrayOfBlocks[startVector];
		for (size_t index = startLoc; index < endLoc; index++)
		{
			if ((*bs)[index])
			{
				count++;
			}
		}
	}
	else
	{

		//auto bs = vectorOfBitSets[startVector];
		//auto be = vectorOfBitSets[endVector];
		auto bs = arrayOfBlocks[startVector];
		auto be = arrayOfBlocks[endVector];
		for (size_t index = startLoc; index < max; index++)
		{
			if ((*bs)[index])
			{
				count++;
			}
		}
		for (size_t vec = startVector + 1; vec < endVector; vec++)
		{
			for (size_t index = 0; index < max; index++)
			{
				//auto bb = vectorOfBitSets[vec];
				auto bb = arrayOfBlocks[vec];
				if ((*bb)[index])
				{
					count++;
				}
			}
		}
		for (size_t index = 0; index < endLoc; index++)
		{
			if ((*be)[index])
			{
				count++;
			}

		}
	}
	return count;
}

void Prime::Compress()
{
	//for (auto&& x : this->vectorOfBitSets)
	for (size_t i = 0; i<contiguousBlocks;++i)
	{
		arrayOfBlocks[i]->Compress();
	}
}
void Prime::Uncompress()
{
	for (size_t i = 0; i < contiguousBlocks; ++i)
	{
		arrayOfBlocks[i]->Uncompress();
	}
}

void Prime::SetVerbose(bool val)
{
	verbose = val;
}

//Gets the next prime number in a bitset given the previous primenumber
size_t Prime::NextPrime(boost::dynamic_bitset<>*bSet, size_t index)
{
	size_t nextP = index;
	//TODO: Need to handle situation where next prime is in next memory block
	bool b = false;
	do {
		nextP = nextP + 1;
		b = bSet->test(nextP);
	} while (b);
	return nextP;
}


/** since we are storing pointers, need to clean up */
//TODO:  refactor name as we are not working with Vectors
void Prime::clearBitsetVector()
{
	//for (auto p : vectorOfBitSets)
	//{
	//	delete p;
	//}
	//vectorOfBitSets.clear();
	//if we have no more bits, set the search distance back to 0
	searchDisttance = 0;
	for (size_t i = 0; i < contiguousBlocks; ++i)
	{
		delete arrayOfBlocks[i];
		arrayOfBlocks[i] = 0;
	}
	
}

void Prime::FindPrimes(int numberOfBlocks)
{
	if (numberOfBlocks < 0)
	{
		throw std::exception("The number of Blocks to search can not be negative");
	}
	this->FindPrimes((size_t)numberOfBlocks);
}
void Prime::FindPrimes(size_t numberOfBlocks)
{
	// if the user wants to find all primes in zero data sets, I guess we let them. Maybe we should throw an exception?
	if (numberOfBlocks == 0) return;

	if (contiguousBlocks == 0)
	{
		if (verbose)
		{
			std::cout << "Doing first (seed) block. The first is not done in parallel" << std::endl;
		}
		if (verbose)
		{

			auto start = std::chrono::high_resolution_clock::now();
			findFirstBlockOFPrimes();
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = duration_cast<std::chrono::microseconds>(stop - start);
			std::cout << "Find First Block of size " << this->GetBitBlockSize() << ": " << duration.count() / 1000000. << " seconds." << std::endl;

		}
		else
		{
			findFirstBlockOFPrimes();
		}
		if (this->saveIcrementalFiles)
		{
			std::string name = this->baseFileName + "0" + ".prm";
			if (verbose)
			{
				auto start = std::chrono::high_resolution_clock::now();
				std::cout << "Saving first block to " << name << std::endl;
				this->arrayOfBlocks[0]->SaveFile(name);
				auto stop = std::chrono::high_resolution_clock::now();
				auto duration = duration_cast<std::chrono::microseconds>(stop - start);
				std::cout << "Time to save first block : " << duration.count() / 1000000. << " seconds." << std::endl;

			}
			else
			{
				std::cout << "Saving first block to " << name << std::endl;
				this->arrayOfBlocks[0]->SaveFile(name);
			}
		}
		//TODO:  Parallelize this part
#pragma omp parallel num_threads(1)
//#pragma omp parallel
		{
			while (nextFreeBlockIndex < numberOfBlocks)
			{
				size_t myBlock = 0;
#pragma omp critical
				{
					myBlock = nextFreeBlockIndex++;
					if (verbose)
					{
						std::cout << "starting block " << myBlock << std::endl;
					}
				}
				//because are are parallel, nextFreeBlock could still be > numberOfBlocks
				if (myBlock >= numberOfBlocks) continue;

				BitBlock* b = nullptr;
				if (verbose)
				{
					auto start = std::chrono::high_resolution_clock::now();
					b = primeSieve(myBlock);
					auto stop = std::chrono::high_resolution_clock::now();
#pragma omp critical
					{
						auto duration = duration_cast<std::chrono::microseconds>(stop - start);
						std::cout << "Time for block " << myBlock << " : " << duration.count() / 1000000. << "seconds." << std::endl;
					}
					//Don't add to the vector inside the parallel part as it is not thread safe
				}
				else
				{
					b = primeSieve(myBlock);
				}
				//vectorOfBitSets.push_back(b);
				arrayOfBlocks[myBlock] = b;
				updateContiguousBlocks();
				//std::vector<size_t>primes = b->GetPrimes();
				//while (vectorOfBitSets.size() < myBlock)
				if (this->saveIcrementalFiles)
				{

					std::string name = this->baseFileName + std::to_string(myBlock) + ".prm";
					std::cout << "Writing block " << myBlock << std::endl;

					if (verbose)
					{
						auto start = std::chrono::high_resolution_clock::now();
						b->SaveFile(name);
						auto stop = std::chrono::high_resolution_clock::now();
#pragma omp critical
						{
							auto duration = duration_cast<std::chrono::microseconds>(stop - start);
							std::cout << "Time to save  " << name << " : " << duration.count() / 1000000. << "seconds." << std::endl;
						}
					}
					else
					{
						b->SaveFile(name);
					}

				}
			}
		}
	}
}

void Prime::findFirstBlockOFPrimes()
{
	// Reset the search since we are looking for the first block
	this->clearBitsetVector();
	this->nextFreeBlockIndex = 1;
	size_t srt = this->Sqrt(max);
	BitBlock *b = new BitBlock(max,0); // b(max); // = new std::bitset(max);
	(*b)[0] = 0;  //1 bit

	size_t prime = 2;
	while (prime <= srt)
	{
		for (size_t i = prime * 2; i <= max; i += prime)
		{
			(*b)[i - 1] = 0; // (i - 1, 0);  //Because of C index at 0, we have to subtract 1
		}
		do
		{
			prime++;
		} while (b->test(prime - 1) == 0);
	}
	this->arrayOfBlocks[0] = b;
	this->contiguousBlocks = 1;
	//this->vectorOfBitSets.push_back(b);

	return;
}
//Last array value is null to prevent walking over the edge.

void Prime::updateContiguousBlocks()
{
#pragma omp critical
	{
		while (this->arrayOfBlocks[contiguousBlocks] != nullptr)
		{
			contiguousBlocks++;
		}
	}
}


BitBlock* Prime::primeSieve(size_t block)
{
	//maxMax will be the total number of bits.  This is how many numbers we can test for primeness
	size_t maxMax = max + max * block;
	// We don't have to "sieve" all primes, just the primes <= the squre root of the number
	size_t srt = this->Sqrt(maxMax);

	//Allocate a new set of bits to run though the sieve
	BitBlock* b = new BitBlock(max,block);

	size_t startBitNumber = max * block + 1;  //This is the number represented by the first bit in the new bitset

	//Start our index at the location of our first prime number, 2
	size_t index = 1;
	size_t prime = index + 1;
	size_t firstSetIndex = 0;
	auto primeList = this->arrayOfBlocks[0]->GetPrimes();
	size_t primeListIndex = 0;

	while (prime <= srt)
	{
		//Start index is where to start making numbers as not prime.
		size_t startIndex = prime - (startBitNumber % prime);  //Figure out the starting index.
		if (startIndex == prime) startIndex = 0;

		//Start marking non-primes every second number after 2 every third number after 3, and so on...
		prime = index + 1;
		for (size_t i = startIndex; i < max; i += prime )
		{
			b->set(i, false);
			//(*b)[i] = false; 
		}
		size_t currentBitset = 0;     //Index to the bitset vector we need to look at
		size_t currentLocation = 0;;  //location in that index

		//Find the next prime number 
		bool isPrime = false;
		if (prime < primeList.back())
		{
			primeListIndex++;
			prime = primeList[primeListIndex];
			index = prime - 1;  //Need to remember after we have used up all our list and have to use the method below
		}
		else //need to find it in our data blocks as it is not cached
		{
			do
			{
				index++;
				prime = index + 1;
				if (prime > srt)
				{
					break;
				}

				//find the bitset containing the next prime number
				currentBitset = index / max;
				// for first bitset, all aligned.  Everything starts at bit 0.   For second bitset, there is an offset of index % max.  
				//For third bitset, offset of index % (2*max)
				// As an example, assume max=10  -  Then for 2nd bitset of 11-20, remove 2 starting at bit 1,3,5,7,9 remove 3 starting at bit 1,4,7
				//                                       for 3rd bitset of 21-30, remove 2 starting at bit 1,3,5,7,9 remove 3 starting at bit 0,3,6,9
				currentLocation = index - (max * currentBitset);
				BitBlock* b_current = this->arrayOfBlocks[currentBitset];
				isPrime = b_current->test(currentLocation);
			} while (!isPrime);
		}
	}
	return b;

}

void Prime::SaveIncrementalFiles(bool b)
{
	this->saveIcrementalFiles = b;
}
//Integer based square root
size_t  Prime::Sqrt(size_t x)
{
	// Base cases 
	if (x == 0 || x == 1)
		return x;
 
	size_t start = 1, end = x , ans=0;
	while (start <= end)
	{
		size_t mid = (start + end) / 2;

		// If x is a perfect square 
		if (mid*mid == x)
			return mid;

		// Since we need floor, we update answer when mid*mid is  
		// smaller than x, and move closer to sqrt(x) 
		if (mid*mid < x)
		{
			start = mid + 1;
			ans = mid;
		}
		else // If mid*mid is greater than x 
			end = mid - 1;
	}
	return ans;
}