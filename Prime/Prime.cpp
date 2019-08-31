// Prime.cpp : Defines the entry point for the console application.
//
//#include "stdafx.h"
#include <string>
#include <bitset>
#include <vector>
#include <iostream>
#include <fstream> 
#include "Prime.h"



void Prime::LoadFromFile(std::string baseName)
{
	size_t fileCount = 1;

	std::string fileName = baseName + std::to_string(fileCount) + ".prm";
	//alocate on heap as stack is limited
	std::bitset<max5>* bsc = new std::bitset<max5>();
	//check if file is there
	std::ifstream infile(fileName);
	if (infile.good())
	{
		//Read the bits and uncompress
		this->readBitSetFromStream(fileName, bsc);
		std::bitset<max>* primes = new std::bitset<max>();
		this->uncompressBitSet(bsc, primes);

		//Save the bitset to this class instance
		vectorOfBitSets.push_back(primes);
	}

}

int Prime::SaveToFile(std::string baseName)
{
	size_t fileCount = 1;

	//Save a file for each bitset numbering them from 1 on....
	for (auto b : vectorOfBitSets)
	{
		//Stack size may cause problems so we allocate on the heap
		std::bitset<max5>* bc = new std::bitset<max5>();
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
	//Return -1 if caller asks to count more primes than have been calculated
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
		for (size_t vec = startVector + 1; vec < endVector - 1; vec++)
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

///  This method created files that were too large.
//void Prime::writeSparseBitSetToStream(const string my_file, std::bitset<Prime::max> *bitss)
//{
//	std::ofstream OutFile;
//	//auto my_file = "d:\\binary.bin";
//	OutFile.open(my_file, std::ios::out | std::ios::binary);
//	//OutFile.write((char*)&max, sizeof(size_t));
//	char buffer = 0;
//
//	for (unsigned int i = 0; i < max; i ++)
//	{
//		if (bitss->test(i))
//		{
//			OutFile.write((char*)&i, sizeof(unsigned int));
//		}
//
//	}
//	return;
//}

void Prime::writeBitSetToStream(const std::string fileName, std::bitset<Prime::max5> *bitss)
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

void Prime::readSparseBitSetFromStream(const char *my_file, std::bitset<Prime::max> *b)
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

void Prime::readBitSetFromStream(const std::string my_file, std::bitset<Prime::max5> *b)
{
	std::ifstream InFile;
	//auto my_file = "d:\\binary.bin";
	InFile.open(my_file, std::ios::out | std::ios::binary);
	size_t count = 0;
	InFile.read((char*)&count, sizeof(size_t));
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
size_t Prime::NextPrime(std::bitset<Prime::max>*bSet, size_t index)
{
	size_t nextP = index;
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

void Prime::FindPrimes(size_t numberOfBlocks)
{
	findFirstBlockOFPrimes();
	for (size_t i = 1; i < numberOfBlocks; i++)
	{
		auto b = primeSieve2(vectorOfBitSets);
		vectorOfBitSets.push_back(b);
	}

}



void Prime::findFirstBlockOFPrimes()
{
	// Reset the search since we are looking for the first block
	this->clearBitsetVector();
	size_t srt = this->Sqrt(max);
	std::bitset<max> *b = new std::bitset<max>(); // b(max); // = new std::bitset(max);
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



std::bitset<Prime::max>* Prime::primeSieve2(std::vector<std::bitset<Prime::max>*> vec)
{
	size_t maxMax = max + max * vec.size();
	size_t srt = this->Sqrt(maxMax);

	std::bitset<max> *b = new std::bitset<max>(); // b(max); // = new std::bitset(max);
	b->set();  //mark them all as prime suspects

	size_t startBitNumber = max * vec.size() + 1;  //This is the number represented by the first bit in the new bitset

	size_t index = 2;
	while (index <= srt)
	{

		size_t startIndex = index - (startBitNumber % index);  //Figure out the starting index.
		if (startIndex == index) startIndex = 0;
		for (size_t i = startIndex; i < max; i += index)
		{
			b->set(i, 0);
		}
		size_t currentBitset = 0;     //Index to the bitset vector we need to lookat
		size_t currentLocation = 0;;  //location in that index

		do
		{
			index++;
			currentBitset = index / max;
			// for first bitset, all aligned.  Everything starts at bit 0.   For second bitset, there is an offset of index % max.  For third bitset, offset of index % (2 max)
			// As an example, assume max=10  -  Then for 2nd bitset of 11-20, remove 2 starting at bit 1,3,5,7,9 remove 3 starting at bit 1,4,7
			//                                       for 3rd bitset of 21-30, remove 2 starting at bit 1,3,5,7,9 remove 3 starting at bit 0,3,6,9

			currentLocation = index - (max*currentBitset) - 1; //  2->1  3 ->2   21 -
			//currentLocation = index % (max *(currentBitset + 1));  // 2-> 0,1  3-> 0,2  10-> 0,9, 11 -> 1,0  21 -> 2,0  31 -> 3,0
		} while (vec[currentBitset]->test(currentLocation) == 0);
		//2  -> [0][1]
	}
	return b;

}


/**
	Creates a bitset with prime numbers set to true and none prime set to false 
*/

std::bitset<Prime::max>* Prime::primeSieve()
{
	std::cout << "bit is " << max;
	size_t srt = (size_t)sqrt(max);
	std::bitset<max> *b = new std::bitset<max>(); // b(max); // = new std::bitset(max);
	b->set();
	std::cout << " worked" <<std::endl;
	b->set(0,false);  //[0] = 1;
	size_t prime = 2;
	while (prime <= srt)
	{
		for (size_t i = prime*2 ;i <= max;i += prime) 
		{
			b->set(i-1, 0);  //Because of C index at 0, we have to subtract 1
		}
		do
		{
			prime++;
		} while (b->test(prime-1) == 0);
	}

	//all done.  Now lets count how many we have
	//size_t count = 0;
	//index = 0;
	//do {
	//	if (b->test(index) != 0)
	//	{
	//		//printf("%d\n", index);
	//		count = count + 1;
	//	}
	//	index++;
	//} while (index < max);
	//printf("Total primes less than %zu is %zu\n", max, count);	
	return b;
}
size_t Prime::countPrimes(std::bitset<Prime::max>* b)
{
	size_t count = 0;
	size_t index = 0;
	do {
		if (b->test(index) != 0)
		{
			count = count + 1;
		}
		index++;
	} while (index < max);
	printf("Total primes less than %zu is %zu\n", max, count);
	return count;
}

//std::bitset<Prime::max>* Prime::primeSieve2(std::vector<std::bitset<Prime::max>*> vec)
//{
//	size_t maxMax = max + max * vec.size();
//	size_t srt = this->Sqrt(maxMax);
//
//	std::bitset<max> *b = new std::bitset<max>(); // b(max); // = new std::bitset(max);
//	b->set();  //mark them all as prime suspects
//
//	size_t startBitNumber = max * vec.size() + 1;  //This is the number represented by the first bit in the new bitset
//
//	size_t index = 2; 
//	while (index <= srt)
//	{
//		
//		size_t startIndex = index - (startBitNumber % index ) ;  //Figure out the starting index.
//		if (startIndex == index) startIndex = 0;
//		for (size_t i = startIndex;i < max;i += index)
//		{
//			b->set(i, 0);
//		}
//		size_t currentBitset = 0;     //Index to the bitset vector we need to lookat
//		size_t currentLocation = 0;;  //location in that index
//
//		do
//		{
//			index++;
//			currentBitset= index / max;
//			// for first bitset, all aligned.  Everything starts at bit 0.   For second bitset, there is an offset of index % max.  For third bitset, offset of index % (2 max)
//			// As an example, assume max=10  -  Then for 2nd bitset of 11-20, remove 2 starting at bit 1,3,5,7,9 remove 3 starting at bit 1,4,7
//			//                                       for 3rd bitset of 21-30, remove 2 starting at bit 1,3,5,7,9 remove 3 starting at bit 0,3,6,9
//
//			currentLocation = index - (max*currentBitset) - 1; //  2->1  3 ->2   21 -
//			//currentLocation = index % (max *(currentBitset + 1));  // 2-> 0,1  3-> 0,2  10-> 0,9, 11 -> 1,0  21 -> 2,0  31 -> 3,0
//		} 
//		while (vec[currentBitset]->test(currentLocation) == 0);
//		//2  -> [0][1]
//	}
//	return b;

	//size_t count = 0;
	//index = 0;
	//do {
	//	if (b->test(index) != 0)
	//	{
	//		//printf("%d\n", index);
	//		count = count + 1;
	//		index++;
	//	}
	//} while (index < max);
	//printf("Total primes less than %zu is %zu\n", max, count);
	//writeBitSetToStream("d:\\binary2.bin", b);

	//writeBitSetToStream(b);
	//std::bitset<max> *bfile = new std::bitset<max>();
	//readBitSetFromStream(bfile);
	//return 0;
	//}

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

void Prime::compressBitSet(std::bitset<Prime::max> *iBitSet, std::bitset<Prime::max5> *b)
{
	//b->set(); //mark all bits as prime, then remove the ones that are not
	size_t count = 0;
	for (size_t i = 7; i < max; i++)
	{
		if (i % 2 == 0 || i % 3 == 0 || i % 5 == 0  )
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
void Prime::uncompressBitSet(std::bitset<Prime::max5> *iBitSet, std::bitset<Prime::max> *b)
{
	b->reset(); //clear out the bitset
	size_t count = 0;  //first unmasked prime
	b->set(2, 1);
	b->set(3, 1);
	b->set(5, 1);


	for (size_t i = 7; i < max; i++)
	{
		if (i % 2 == 0 || i % 3 == 0 || i % 5 == 0 )
		{
			// don't do anything
		}
		else
		{
			b->set(i, iBitSet->test(count));
			count++;
		}
	}
	return;
}


//#include "catalyst.hpp"
//S#include "Prime.h"
//int bigPrime()
//{
//	std::ofstream OutFile;
//	auto my_file = "d:\\primes.txt";
//	OutFile.open(my_file, std::ios::out);
//
//	//OutFile << 2 << std::endl;
//	//OutFile << 3 << std::endl;
//	bigint num(5);
//	bool prime = true;
//	bigint count(3);
//	bigint max(4294977295);
//	while (num < max)
//	{
//		do
//		{
//			if (num%count == 0)
//			{
//				prime = false;
//			}
//			else
//			{
//				count += 2;
//			}
//		} while (prime && (count < num));
//		if (prime)
//		{
//			std::cout << num << std::endl;
//			OutFile << num << std::endl;
//		}
//		prime = true;
//		num = num + 2;
//		count = 3;
//	}
//	OutFile.close();
//
//	return 0;
//}


//int main(int argc, char*argv[])
//{
//	Prime prime = Prime();
//	prime.main(argc, argv);
//}


Prime::Prime()
{
}


Prime::~Prime()
{
	clearBitsetVector();
}

//void Prime::SetBaseFileName(std::string BaseName)
//{
//	this->baseFileName = BaseName;
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

