#include <string>
#include <vector>
#include <iostream>
#include <fstream> 
#include "Prime.h"
#include <boost/dynamic_bitset.hpp>
#include <cstdio>

Prime::Prime(size_t bitsetSize)
{
	if (bitsetSize % 30)
	{
		throw "Prime bit block size must be divisible by 30 for block alignment";
	}
	if (bitsetSize > 0xFFFFFFF0)
	{
		throw "Maximum bitsetSize in Prime Constructor is 0xFFFFFFF0 (4294967280)";
	}
	if (bitsetSize == 0)
	{
		throw "bitBlockSize can not be zero";
	}

	bitBlockSize = bitsetSize;
	max = (size_t)bitBlockSize;       // Can be up to 4294967280;
	max2 = max / 2 + max % 2;         // don't need to save the even numbers
	max3 = 2 * max2 / 3 + max2 % 3;   // don't need to save numbers divisible by 3
	max5 = 4 * max3 / 5 + max3 % 5;   // don't need to save numbers divisible by 5
}

int Prime::ContinousRun(std::string baseName)
{
	//Load any existing prime files to start wit
	this->LoadFromFile(baseName);
	this->baseFileName = baseName;
	this->SaveIncrementalFiles(true);
	this->FindPrimes((size_t)0xFFFFFFF0);
}
Prime::~Prime()
{
	//delete memory
	clearBitsetVector();
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
				throw "Could not delete prime number file.";
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
		//alocate on heap as stack is limited
		boost::dynamic_bitset<>*primes; // = new boost::dynamic_bitset<>(max);
		boost::dynamic_bitset<>* bsc; //= new boost::dynamic_bitset<>(max5);
		//check if file is there
		std::ifstream infile(fileName);
		if (infile.good())
		{
			//Read the bits and uncompress
			bsc= new boost::dynamic_bitset<>(max5);

			//clean up memory if we get an exception while reading file
			try {
				this->readBitSetFromStream(fileName, bsc);
			}
			catch (...) 
			{
				delete bsc;
				throw;
			}

			primes = new boost::dynamic_bitset<>(max);
			this->uncompressBitSet(bsc, primes);
			delete bsc;
			
			//if it is the first file, 2,3,5 were not stored in the compressed file.  Add them back now.
			if (fileCount == 1)
			{
				primes->set(1, 1);  //1 is the 2 bit
				primes->set(2, 1);  //2 is the 3 bit
				primes->set(4, 1);  //4 is the 5 bit
			}

			//Save the bitset to this class instance
			vectorOfBitSets.push_back(primes);
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

int Prime::saveToFile(std::string baseName, size_t count)
{
	auto b = vectorOfBitSets[count-1];
	boost::dynamic_bitset<>* bc = new boost::dynamic_bitset<>(max5);
	try
	{
		compressBitSet(b, bc);
		auto s = std::to_string(Prime::max5);
		std::string fileName = baseName + std::to_string(count) + ".prm";
		writeBitSetToStream(fileName, bc);
	}
	catch (...)
	{
		//make sure we cleanup
		delete bc;
		//likely couldn't save the file.  Send up the exception.
		throw;
	}
	delete bc;
	return true;
}

int Prime::SaveToFile(std::string baseName)
 {
	size_t fileCount = 1;

	//Save a file for each bitset numbering them from 1 on....
	for (auto b : vectorOfBitSets)
	{
		//Stack size may cause problems so we allocate on the heap
		boost::dynamic_bitset<>* bc = new boost::dynamic_bitset<>(max5);
		try
		{
			compressBitSet(b, bc);
			auto s = std::to_string(Prime::max5);
			std::string fileName = baseName + std::to_string(fileCount) + ".prm";
			writeBitSetToStream(fileName, bc);
			fileCount++;
		}
		catch (...)
		{   
			//make sure we cleanup
			delete bc;
			//likely couldn't save the file.  Send up the exception.
			throw;
		}
		delete bc;
	}
	return true;
}

std::vector<size_t> Prime::GetPrimesAsVector(size_t lowVal, size_t highVal)
{
	std::vector<size_t> primes;
	size_t index = 1;
	for (auto bs : this->vectorOfBitSets)
	{
		for (size_t count = 0; count < Prime::max; count++)
		{
			bool isPrime = bs->test(count);
			if (isPrime)
			{
				primes.push_back(index);
			}
			index++;
		}
	}
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
	if (endVector >= vectorOfBitSets.size() || lowVal>=highVal)
	{
		return 0;
	}

	//See if everything is in one bitset
	if (startVector == endVector)
	{
		auto bs = vectorOfBitSets[startVector];
		for (size_t index = startLoc; index < endLoc; index++)
		{
			if (bs->test(index))
			{
				count++;
			}
		}
	}
	else
	{

		auto bs = vectorOfBitSets[startVector];
		auto be = vectorOfBitSets[endVector];
		for (size_t index = startLoc; index < max; index++)
		{
			if (bs->test(index))
			{
				count++;
			}
		}
		for (size_t vec = startVector + 1; vec < endVector; vec++)
		{
			for (size_t index = 0; index < max; index++)
			{
				if (vectorOfBitSets[vec]->test(index))
				{
					count++;
				}
			}
		}
		for (size_t index = 0; index < endLoc; index++)
		{
			if (be->test(index))
			{
				count++;
			}

		}
	}
	return count;
}

void Prime::writeBitSetToStream(const std::string fileName, boost::dynamic_bitset<> *bitss)
{
	std::ofstream OutFile;
	//auto my_file = "d:\\binary.bin";
	OutFile.open(fileName, std::ios::out | std::ios::binary);
	OutFile.write((char*)&max5, sizeof(size_t));
	char buffer = 0;
	for (size_t i = 0;i < max5;i += 8)
	{
		for (char c = 0;c < 8;c++)
		{
			char v = 0;
			if (max5 <= ( i + c))//if we overflow at the end, pack the rest with 0
			{
				v = 0;
			}
			else
			{
				v = bitss->test(i + c) << c;
				//printf("%d", (bitss->test(i + c)) ? 1 : 0);
			}
			buffer = buffer + v; //set one bit at a time
		}
		OutFile.write(&buffer, 1);
		buffer = 0;
	}
	OutFile.close();
	return;
}

void Prime::readSparseBitSetFromStream(const char *my_file, boost::dynamic_bitset<> *b)
{
	std::ifstream InFile;
	//auto my_file = "d:\\binary.bin";
	InFile.open(my_file, std::ios::out | std::ios::binary);
	size_t num = 0;
	do
	{
		InFile.read((char*)&num, sizeof(size_t));
		if (!InFile.eof()) b->set(num);
		else break;
	} while (true);
	InFile.close();
}

void Prime::readBitSetFromStream(const std::string my_file, boost::dynamic_bitset<> *b)
{
	std::ifstream InFile;
	//auto my_file = "d:\\binary.bin";
	InFile.open(my_file, std::ios::out | std::ios::binary);
	size_t count = 0;
	InFile.read((char*)&count, sizeof(size_t));
	if (count != this->max)
	{
		//Our files and class hold a different number of bits per block
		throw std::exception("Error: the file and prime object hold a different number of bits.");
	}
	//printf("there are %d bitss in this file", count);
	char buffer = 0;
	for (size_t i = 0;i <= count / 8;i++)
	{	
		InFile.read(&buffer, 1);
		for (char c = 0;c < 8;c++)
		{
			if ((i * 8 + c) < count)
			{
				char bit = 1;
				if (buffer & (bit << c))
				{
					b->set(i * 8 + c);
					//printf("%d", 1);
				}
				else
				{
					b->set(i * 8 + c, false);
					//printf("%d", 0);
				}
			}
			else
			{
				//we are in the last byte in the file with buffer bitss to be ignored.
			}
		}
	}
	InFile.close();
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
void Prime::clearBitsetVector()
{
	for (auto p : vectorOfBitSets)
	{
		delete p;
	}
	vectorOfBitSets.clear();
	//if we have no more bits, set the search distance back to 0
	searchDisttance = 0;
}

void Prime::FindPrimes(int numberOfBlocks)
{
	if (numberOfBlocks < 0)
	{
		throw "The number of Blocks to search can not be negative";
	}
	this->FindPrimes((size_t)numberOfBlocks);
}
void Prime::FindPrimes(size_t numberOfBlocks)
{
	// if the user wants to find all primes in zero data sets, I guess we let them. Maybe we should throw an exception?
	if (numberOfBlocks == 0) return;
	if(vectorOfBitSets.size()==0)
	{
		findFirstBlockOFPrimes();
	}

	for (size_t i = vectorOfBitSets.size()-1; i < numberOfBlocks; i++)
	{
		auto b = primeSieve(vectorOfBitSets);
		vectorOfBitSets.push_back(b);
		if (this->saveIcrementalFiles)
		{
			saveToFile(this->baseFileName, vectorOfBitSets.size());
		}
	}
}

void Prime::findFirstBlockOFPrimes()
{
	// Reset the search since we are looking for the first block
	this->clearBitsetVector();
	size_t srt = this->Sqrt(max);
	boost::dynamic_bitset<> *b = new boost::dynamic_bitset<>(max); // b(max); // = new std::bitset(max);
	b->set();
	b->set(0, false);  //[0] = 1;
	size_t prime = 2;
	while (prime <= srt)
	{
		for (size_t i = prime * 2; i <= max; i += prime)
		{
			b->set(i - 1, 0);  //Because of C index at 0, we have to subtract 1
		}
		do
		{
			prime++;
		} while (b->test(prime - 1) == 0);
	}
	this->vectorOfBitSets.push_back(b);

	return;
}

boost::dynamic_bitset<>* Prime::primeSieve(std::vector<boost::dynamic_bitset<>*> vec)
{
	//maxMax will be the total number of bits.  This is how many numbers we can test for primeness
	size_t maxMax = max + max * vec.size();
	// We don't have to "sieve" all primes, just the primes >= the squre root of the number
	size_t srt = this->Sqrt(maxMax);

	//Allocate a new set of bits to run thoug hte sieve
	boost::dynamic_bitset<> *b = new boost::dynamic_bitset<>(max); // b(max); // = new std::bitset(max);
	b->set();  //mark them all as prime suspects

	size_t startBitNumber = max * vec.size() + 1;  //This is the number represented by the first bit in the new bitset

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
			b->set(i, 0);
		}
		size_t currentBitset = 0;     //Index to the bitset vector we need to lookat
		size_t currentLocation = 0;;  //location in that index

		//Find the next prime number 
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
			currentLocation = index - (max*currentBitset);     //  2->1  3 ->2   21 -
		} while (vec[currentBitset]->test(currentLocation) == 0);

	}
	return b;

}

int WritePrime()
{
	std::ifstream InFile;
	auto in_file = "d:\\binary.bin";
	InFile.open(in_file, std::ios::out | std::ios::binary);
	std::ifstream InFile2;
	auto in_file2 = "d:\\binary2.bin";
	InFile2.open(in_file2, std::ios::out | std::ios::binary);

	std::ofstream OutFile;
	auto out_file = "d:\\primes2.txt";
	OutFile.open(out_file, std::ios::out);
	std::ofstream OutFile2;
	auto out_file2 = "d:\\primesData.txt";
	OutFile2.open(out_file2, std::ios::out);

	size_t count = 0;
	InFile.read((char*)&count, sizeof(size_t));
	//printf("there are %d bitss in this file", count);
	char buffer = 0;
	size_t primeCount = 0;
	for (size_t i = 0;i <= count / 8;i++)
	{
		InFile.read(&buffer, 1);
		for (char c = 0;c < 8;c++)
		{
			size_t val = i * 8 + c;
			if (val % 10000000 == 0)
			{
				OutFile2 << primeCount << std::endl;
				std::cout << primeCount << std::endl;
				primeCount = 0;
			}
			if (val < count)
			{
				char bit = 1;

				if (buffer & (bit << c))
				{
					//std::cout << val << std::endl;
					OutFile << val << std::endl;				
					primeCount++;

				}
				else
				{
					//b->set(i * 8 + c, false);
					//printf("%d", 0);
				}
			}
			else
			{
				//we are in the last byte in the file with buffer bitss to be ignored.
			}
		}

	}
	InFile.close();
	OutFile.close();
	return 0;
}

void Prime::compressBitSet(boost::dynamic_bitset<> *iBitSet, boost::dynamic_bitset<> *b)
{
	size_t count = 0;
	for (size_t i = 1; i < max; i++)
	{
		if (i % 2 == 0 || i % 3 == 0 || i % 5 == 0 )
		{
			// don't do anything
		}
		else
		{
			//note that bits are off by 1 (e.g. 1 bit is a location o, 7 bit is at location 6)
			bool isPrime = iBitSet->test(i-1);
			b->set(count, isPrime);
			count++;
		}
	}
	return;
}
void Prime::uncompressBitSet(boost::dynamic_bitset<> *iBitSet, boost::dynamic_bitset<> *b)
{
	b->reset(); //clear out the bitset
	size_t count = 0;  //first unmasked prime

	for (size_t i = 1; i < max; i++)
	{
		if (i % 2 == 0 || i % 3 == 0 || i % 5 == 0 )
		{
			// don't do anything
		}
		else
		{
			b->set(i-1, iBitSet->test(count));
			count++;
		}
	}
	return;
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
 
	size_t start = 1, end = x , ans;
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