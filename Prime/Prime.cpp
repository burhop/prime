#include <string>
#include <vector>
#include <iostream>
#include <fstream> 
#include "Prime.h"
#include <boost/dynamic_bitset.hpp>
#include <cstdio>
#include <thread>
#include <omp.h>
//Performance testing
#include <chrono> 
#include <filesystem>


Prime::Prime(size_t bitsetSize, unsigned int number_of_threads,unsigned int cashedBlocks)
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
	if (number_of_threads == 0)
	{
		//let the system decide
		threadCount = std::thread::hardware_concurrency();
	}
	else 
	{
		threadCount = number_of_threads;
		omp_set_num_threads(threadCount);
	}
	maxValue = 0;
    bitBlockSize = bitsetSize;
	nextFreeBlockIndex = 0;

	blockManager = new DataCacheManager(0xFFFFF1, cashedBlocks);  //Allocate an array of pointers and initialize them to 0.
	contiguousBlocks = 0;
}

Prime::Prime(std::string baseName, unsigned int number_of_threads, unsigned int cashedBlocks)
{
	if (number_of_threads == 0)
	{
		//let the system decide
		threadCount = std::thread::hardware_concurrency();
	}
	else
	{
		threadCount = number_of_threads;
		omp_set_num_threads(threadCount);
	}
	blockManager = new DataCacheManager(0xFFFFF1, cashedBlocks);  //Allocate an array of pointers and initialize them to 0.
	contiguousBlocks = 0;
	try 
	{
		this->LoadFromFile(baseName);
	}
	catch (std::exception& ex)
	{
		//destructor won't be called so need to clean up
		delete blockManager;
		blockManager = nullptr;
		throw ex;
	}

}

Prime::~Prime()
{
	//delete memory
	delete blockManager;
}
void Prime::SetBaseName(std::string basename)
{
	this->baseFileName = basename;
}
size_t Prime::GetBitBlockSize()
{
	return bitBlockSize;
}

void Prime::DeleteExistingPrimeFiles(std::string baseName)
{
	size_t fileCount = 0;
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
	uintmax_t fileSize=0;
	//Assume at least one file and start looping until we don't find any more
	do
	{
		std::string fileName = baseName + std::to_string(fileCount) + ".prm";

		//check if file is there
		//std::ifstream infile(fileName);
		if (std::filesystem::exists(fileName))
		{
			if (fileCount == 0)
			{
				fileSize = std::filesystem::file_size(fileName);
			}
			else
			{
				uintmax_t nextFileSize = std::filesystem::file_size(fileName);
				if (nextFileSize != fileSize)
				{
					//We have a bad file. The previous run probably didn't finish writting it. This is where we want to start next
					nextFile = false;
				}
			}
			if (nextFile) //if its  
			{
				std::shared_ptr<BitBlock>  bb = nullptr;
				if (verbose)
				{
					auto start = std::chrono::high_resolution_clock::now();
					bb = blockManager->MakeBitBlock(fileName, false, this->bitBlockSize);

					auto stop = std::chrono::high_resolution_clock::now();
					auto duration = duration_cast<std::chrono::microseconds>(stop - start);
					std::cout << "Time to load file " << fileName << ": " << duration.count() / 1000000. << " seconds." << std::endl;
					blockManager->set(bb, fileCount);
				}
				else
				{
					bb = blockManager->MakeBitBlock(fileName, false, this->bitBlockSize);
					blockManager->set(bb, fileCount);
				}
				//If we are going to create more BitBLocks, keep using the same size
				this->bitBlockSize = bb->GetSize();
				updateContiguousBlocks();
			}
		}
		else
		{
			//We didn't fine another file
			nextFile = false;
		}
		//infile.close();
		fileCount++;
	} while (nextFile);

	// We can't just leave it empty as we don't know the size to make future BitBlocks
	if (! this->blockManager[0].BlockExists(0))
	{
		throw std::exception("Attempted to load data from disk but no files found.");
	}
		

	//Flag the next free block if user wants to find more primes.  if we loaded 10 files, the next free index is 10 (or the 11th location)
	nextFreeBlockIndex = contiguousBlocks;
}

size_t Prime::GetMaxValue()
{
	//if there are any bitsets, it is because we have calulated the primes for it.
	return this->contiguousBlocks * this->bitBlockSize;

	//return this->vectorOfBitSets.size()*this->max;
}
size_t Prime::GetMaxPrime()
{

	if (contiguousBlocks == 0) return 0;
	size_t bitsetIndex = 1;  //Start with the last bitset and work toward the front
	std::shared_ptr<BitBlock> lastBitset = blockManager->get(contiguousBlocks - 1);
	bool primeFlag = false;

	//Go to the end of the list and back up until we fined a prime.
	size_t index = bitBlockSize;

	do
	{
		index--;
		//If we get a bitset with no primes, reset to the end of the previous bitset
		if (index == 0)
		{
			index = bitBlockSize - 1;
			bitsetIndex = bitsetIndex + 1;
			lastBitset = blockManager->get(contiguousBlocks - bitsetIndex);
		}
		primeFlag = (*lastBitset)[index];
	} while (primeFlag == false);

	//  remember, index starts at 0, so index+1 is the actual count in the bitset  (e.g 1st number is at index 0)
	return (contiguousBlocks - 1) *this->bitBlockSize + index + 1;
}
size_t Prime::GetMaxCount()
{
	size_t count = this->CountPrimes(0, this->GetMaxPrime()+1);
	return count;
}

int Prime::SaveToFile(std::string baseName)
 {
	for(size_t i=0;i<contiguousBlocks;++i)
	{	
			std::string fileName = baseName + std::to_string(i) + ".prm";
			blockManager->get(i)->SaveFile(fileName);
			//(*blockManager)[i]->SaveFile(fileName);
	}
	return true;
}

std::vector<size_t> Prime::GetPrimesAsVector(size_t lowVal, size_t highVal)
{
	std::vector<size_t> primes;
	return primes ;
}

size_t Prime::CountPrimes(size_t lowVal, size_t highVal)
{
	std::vector<size_t> primes;
	size_t count = 0;
	size_t startVector = lowVal / bitBlockSize;
	size_t endVector = (highVal-1) / bitBlockSize;
	size_t startLoc = lowVal % bitBlockSize;
	size_t endLoc = (highVal-1) % bitBlockSize;

	//Return 0 if caller asks to count more primes than have been calculated
	if(endVector>=contiguousBlocks)
	{
		return 0;
	}

	//See if everything is in one bitset
	if (startVector == endVector)
	{
		auto bs = blockManager->get(startVector);
		for (size_t index = startLoc; index < endLoc; index++)
		{
			if ((*bs)[index])
			{
				count++;
			}
		}
	}
	else  //Data is in multiple blocks
	{
		auto bs = blockManager->get(startVector);
		auto be = blockManager->get(endVector);
		for (size_t index = startLoc; index < bitBlockSize; index++)
		{
			if ((*bs)[index])
			{
				count++;
			}
		}
		for (size_t vec = startVector + 1; vec < endVector; vec++)
		{
			auto bb = blockManager->get(vec);
			for (size_t index = 0; index < bitBlockSize; index++)
			{
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
	for (size_t i = 0; i<contiguousBlocks;++i)
	{
		blockManager->get(i)->Compress();
	}
}
void Prime::Uncompress()
{
	for (size_t i = 0; i < contiguousBlocks; ++i)
	{
		blockManager->get(i)->Uncompress();
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

//This is mostly to catch negative numbers as they appear correct as size_t type
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
				//(*blockManager)[0]->SaveFile(name);
				blockManager->get(0)->SaveFile(name);
				auto stop = std::chrono::high_resolution_clock::now();
				auto duration = duration_cast<std::chrono::microseconds>(stop - start);
				std::cout << "Time to save first block : " << duration.count() / 1000000. << " seconds." << std::endl;

			}
			else
			{
				std::cout << "Saving first block to " << name << std::endl;
				blockManager->get(0)->SaveFile(name);
			}
		}
	}
	if (verbose)
	{
		std::cout << "Planning  to use " << this->threadCount << " threads." << std::endl;
	}
//#pragma omp parallel num_threads(2)
#pragma omp parallel
	{
		while (nextFreeBlockIndex < numberOfBlocks)
		{
			size_t myBlock = 0;

#pragma omp critical(print)
			{
				myBlock = nextFreeBlockIndex++;
				if (verbose)
				{
					std::cout << "starting block " << myBlock << " on thread "<< omp_get_thread_num() << std::endl;
				}
			}
			//because are are parallel, nextFreeBlock could still be > numberOfBlocks
			if (myBlock >= numberOfBlocks) continue;

			std::shared_ptr<BitBlock>  b = nullptr;
			if (verbose)
			{
				auto start = std::chrono::high_resolution_clock::now();
				b = primeSieve(myBlock);
				auto stop = std::chrono::high_resolution_clock::now();
#pragma omp critical(print1)
				{
					auto duration = duration_cast<std::chrono::microseconds>(stop - start);
					std::cout << "Time for block " << myBlock << " : " << duration.count() / 1000000. << "seconds." << std::endl;
				}
			}
			else
			{
				b = primeSieve(myBlock);
			}
			blockManager->set(b, myBlock);
			updateContiguousBlocks();
			//Since we have all the data, set the block as complete so that it can be moved in and out of memoery.
			b->SetComplete(true);
			if (this->saveIcrementalFiles)
			{

				std::string name = this->baseFileName + std::to_string(myBlock) + ".prm";
				std::cout << "Writing block " << myBlock << std::endl;

				if (verbose)
				{
					auto start = std::chrono::high_resolution_clock::now();
					b->SaveFile(name);
					auto stop = std::chrono::high_resolution_clock::now();
#pragma omp critical(print2)
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

void Prime::findFirstBlockOFPrimes()
{
	this->nextFreeBlockIndex = 1;
	size_t srt = this->Sqrt(bitBlockSize);
	std::shared_ptr<BitBlock> b = blockManager->MakeBitBlock(bitBlockSize,0); 
	(*b)[0] = 0;  //1 bit

	size_t prime = 2;
	while (prime <= srt)
	{
		for (size_t i = prime * 2; i <= bitBlockSize; i += prime)
		{
			(*b)[i - 1] = 0;  //Because of C index at 0, we have to subtract 1
		}
		do
		{
			prime++;
		} while (b->test(prime - 1) == 0);
	}
	//(*blockManager)[0] = b;
	blockManager->set(b, 0);
	this->contiguousBlocks = 1;

	return;
}

void Prime::updateContiguousBlocks()
{
#pragma omp critical(blocks)
	{
		while (blockManager->BlockExists(contiguousBlocks))
		{
			contiguousBlocks++;
		}
	}
}


std::shared_ptr<BitBlock> Prime::primeSieve(size_t block)
{
	//maxMax will be the total number of bits.  This is how many numbers we can test for primeness
	size_t maxMax = bitBlockSize + bitBlockSize * block;
	// We don't have to "sieve" all primes, just the primes <= the squre root of the number
	size_t srt = this->Sqrt(maxMax);

	//Allocate a new set of bits to run though the sieve
	std::shared_ptr<BitBlock>  b = blockManager->MakeBitBlock(bitBlockSize, block);

	size_t startBitNumber = bitBlockSize * block + 1;  //This is the number represented by the first bit in the new bitset

	//Start our index at the location of our first prime number, 2
	size_t index = 1;
	size_t prime = index + 1;
	size_t firstSetIndex = 0;
	std::vector<size_t> primeList;
	primeList = blockManager->get(0)->GetPrimes();
	size_t primeListIndex = 0;

	while (prime <= srt)
	{
		//Start index is where to start making numbers as not prime.
		size_t startIndex = prime - (startBitNumber % prime);  //Figure out the starting index.
		if (startIndex == prime) startIndex = 0;

		//Start marking non-primes every second number after 2 every third number after 3, and so on...
		prime = index + 1;
		for (size_t i = startIndex; i < bitBlockSize; i += prime )
		{
			b->set(i, false);
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
				currentBitset = index / bitBlockSize;
				// for first bitset, all aligned.  Everything starts at bit 0.   For second bitset, there is an offset of index % max.  
				//For third bitset, offset of index % (2*max)
				// As an example, assume max=10  -  Then for 2nd bitset of 11-20, remove 2 starting at bit 1,3,5,7,9 remove 3 starting at bit 1,4,7
				//                                       for 3rd bitset of 21-30, remove 2 starting at bit 1,3,5,7,9 remove 3 starting at bit 0,3,6,9
				currentLocation = index - (bitBlockSize * currentBitset);
				std::shared_ptr<BitBlock>  b_current = nullptr;
				do
				{ 
					b_current = blockManager->get(currentBitset);
					if (b_current == nullptr)
					{
						//We need primes that have not been found yet. They are are being calculated in another thread.
						std::this_thread::sleep_for(std::chrono::milliseconds(this->threadSleep));
					}
				} 
				while (b_current == nullptr);
				isPrime = (*b_current)[currentLocation];
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