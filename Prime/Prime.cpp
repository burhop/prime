#include <string>
#include <vector>
#include <iostream>
#include <fstream> 
#include "Prime.h"
#include <boost/dynamic_bitset.hpp>
#include <cstdio>
#include "bitblock.h"
#include <omp.h>

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
	size_t fileCount = 1;
	bool nextFile = true;
	//Assume at least one file and start looping until we don't find any more
	do
	{
		std::string fileName = baseName + std::to_string(fileCount) + ".prm";
		//check if file is there
		std::ifstream infile(fileName);
		if (!infile.fail())
		{
				BitBlock* bb = new BitBlock(fileName);
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

//int Prime::saveToFile(std::string baseName, size_t count)
//{
//	auto b = vectorOfBitSets[count - 1];
//
//	std::string fileName = baseName + std::to_string(count) + ".prm";
//	b->SaveFile(fileName);
//
//	return true;
//}

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

//void Prime::writeBitSetToStream(const std::string fileName, BitBlock *bitss)
//{
//	std::ofstream OutFile;
//	//auto my_file = "d:\\binary.bin";
//	OutFile.open(fileName, std::ios::out | std::ios::binary);
//	OutFile.write((char*)&max5, sizeof(size_t));
//	char buffer = 0;
//	for (size_t i = 0;i < max5;i += 8)
//	{
//		for (char c = 0;c < 8;c++)
//		{
//			char v = 0;
//			if (max5 <= ( i + c))//if we overflow at the end, pack the rest with 0
//			{
//				v = 0;
//			}
//			else
//			{
//				v = (*bitss)[i + c] << c;
//				//printf("%d", (bitss->test(i + c)) ? 1 : 0);
//			}
//			buffer = buffer + v; //set one bit at a time
//		}
//		OutFile.write(&buffer, 1);
//		buffer = 0;
//	}
//	OutFile.close();
//	return;
//}

//void Prime::readSparseBitSetFromStream(const char *my_file, boost::dynamic_bitset<> *b)
//{
//	std::ifstream InFile;
//	//auto my_file = "d:\\binary.bin";
//	InFile.open(my_file, std::ios::out | std::ios::binary);
//	size_t num = 0;
//	do
//	{
//		InFile.read((char*)&num, sizeof(size_t));
//		if (!InFile.eof()) b->set(num);
//		else break;
//	} while (true);
//	InFile.close();
//}

//void Prime::readBitSetFromStream(const std::string my_file, boost::dynamic_bitset<> *b)
//{
//	std::ifstream InFile;
//	InFile.open(my_file, std::ios::out | std::ios::binary);
//	size_t count = 0;
//	InFile.read((char*)&count, sizeof(size_t));
//	if (count != this->max5)
//	{
//		//Our files and class hold a different number of bits per block
//		throw std::exception("Error: the file and prime object hold a different number of bits.");
//	}
//	//printf("there are %d bitss in this file", count);
//	char buffer = 0;
//	for (size_t i = 0;i <= count / 8;i++)
//	{	
//		InFile.read(&buffer, 1);
//		for (char c = 0;c < 8;c++)
//		{
//			if ((i * 8 + c) < count)
//			{
//				char bit = 1;
//				if (buffer & (bit << c))
//				{
//					b->set(i * 8 + c);
//					//printf("%d", 1);
//				}
//				else
//				{
//					b->set(i * 8 + c, false);
//					//printf("%d", 0);
//				}
//			}
//			else
//			{
//				//we are in the last byte in the file with buffer bits to be ignored.
//			}
//		}
//	}
//	InFile.close();
//}

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
	//if (vectorOfBitSets.size() == 0)
	if(contiguousBlocks==0)
	{
		findFirstBlockOFPrimes();
		if (this->saveIcrementalFiles)
		{
			std::string name = this->baseFileName + "0" + ".prm";
			//vectorOfBitSets[0]->SaveFile(name);
			//saveToFile(this->baseFileName, 1);
		}
	}
	//TODO:  Parallelize this part
//#pragma omp parallel num_threads(3)
#pragma omp parallel
	{
		while (nextFreeBlockIndex < numberOfBlocks)
		{
			//int procCount= omp_get_num_procs();
			//int ID = omp_get_thread_num();
			size_t myBlock = 0;
#pragma omp critical
			{
				myBlock = nextFreeBlockIndex++;
			}
			//because are are parallel, nextFreeBlock could still be > numberOfBlocks
			if (myBlock >= numberOfBlocks) continue;

			//std::cout << "Doing Block " << myBlock << " On thread " << omp_get_thread_num() << std::endl;
			auto b = primeSieve(myBlock);	
			//Don't add to the vector inside the parallel part as it is not thread safe

			//vectorOfBitSets.push_back(b);
			arrayOfBlocks[myBlock] = b;
			updateContiguousBlocks();
			//std::vector<size_t>primes = b->GetPrimes();
			//while (vectorOfBitSets.size() < myBlock)
			if (this->saveIcrementalFiles)
			{
				std::string name = this->baseFileName + std::to_string(myBlock) + ".prm";
				b->SaveFile(name);
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
	//BitBlock x(max, 0);
	//x[0] = false;
	//bool d = x[0];
	//bool a = (*b)[0];
	//(*b)[0]= false;
	//bool c = (*b)[0];
	//b->set(0, false);  //[0] = 1;
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
	while (prime <= srt)
	{
		//Start index is where to start making numbers as not prime.
		size_t startIndex = prime - (startBitNumber % prime);  //Figure out the starting index.
		if (startIndex == prime) startIndex = 0;

		//Start marking non-primes every second number after 2 every third number after 3, and so on...
		prime = index + 1;
		for (size_t i = startIndex; i < max; i += prime )
		{
			(*b)[i] = false; 
		}
		size_t currentBitset = 0;     //Index to the bitset vector we need to look at
		size_t currentLocation = 0;;  //location in that index

		//Find the next prime number 
		bool isPrime = false;
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
			// We might need a prime that hasn't been found yet (if we are doing Sieves in Parallel)
			//while (currentBitset >= this->vectorOfBitSets.size())
			//{
			//	std::chrono::milliseconds timespan(Prime::threadPauseOnDataWait);
			//	std::this_thread::sleep_for(timespan);

			//}
			//use at() and not [] becuase we want to thrown an exception if there is a bug
			//BitBlock* b = this->vectorOfBitSets.at(currentBitset);
			BitBlock* b_current = this->arrayOfBlocks[currentBitset];
			isPrime= b_current->test(currentLocation);
		} while (!isPrime);

	}
	return b;

}

//void Prime::compressBitSet(boost::dynamic_bitset<> *iBitSet, boost::dynamic_bitset<> *b)
//{
//	size_t count = 0;
//	for (size_t i = 1; i < max; i++)
//	{
//		if (i % 2 == 0 || i % 3 == 0 || i % 5 == 0 )
//		{
//			// don't do anything
//		}
//		else
//		{
//			//note that bits are off by 1 (e.g. 1 bit is a location o, 7 bit is at location 6)
//			bool isPrime = iBitSet->test(i-1);
//			b->set(count, isPrime);
//			count++;
//		}
//	}
//	return;
//}
//void Prime::uncompressBitSet(boost::dynamic_bitset<> *iBitSet, boost::dynamic_bitset<> *b)
//{
//	b->reset(); //clear out the bitset
//	size_t count = 0;  //first unmasked prime
//
//	for (size_t i = 1; i < max; i++)
//	{
//		if (i % 2 == 0 || i % 3 == 0 || i % 5 == 0 )
//		{
//			// don't do anything
//		}
//		else
//		{
//			b->set(i-1, iBitSet->test(count));
//			count++;
//		}
//	}
//	return;
//}

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