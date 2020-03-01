#include "bitblock.h"
#include <iostream>
#include <fstream> 
#include <boost/dynamic_bitset.hpp>
#include <cstdio>

BitBlock::BitBlock(std::string file, bool cache)
{
	filename = file;
	cached = cache;
	cachedPrimes = nullptr;
	maxValue = 0;
	//Load part or all of the file depending on the cache setting
	LoadFile();
}

BitBlock::BitBlock(size_t s, size_t i)
{
	size=s;
	index=i;  
	cached = false;
	compressed = false;
	cachedPrimes = nullptr;
	maxValue = 0;
	//auto bitsetsize = getBitsetSize(size);
	//block off the full size
	bits = new boost::dynamic_bitset(size);
	bits->set();
}

BitBlock::~BitBlock()
{
	//delete memory
	delete bits;
}

size_t BitBlock::GetSize()
{
	return this->size;
}

size_t BitBlock::GetIndex()
{
	return index;
}

//TODO  Test is if is faster to do this or just have the whole array in memory

// you can't use bool here because the setter won't work. This is because bitsets don't return a bool but a proxy
boost::dynamic_bitset<>::reference BitBlock::operator[](size_t loc)
{
	if (compressed)
	{ 
		// if it is the first block, 2,3,5 are missing.
		if (this->GetIndex() == 0)
		{
			if (loc == 1 || loc == 2 || loc == 4) return bDummy[1];
		}
		size_t index = loc + 1;
		if (index % 2 && index % 3 && index % 5)
		{
			size_t idx = index + index / 6 + index / 10 + index / 15 - index / 2 - index / 3 - index / 5 - index / 30  - 1;
			return (*bits)[idx];
		}
		return bDummy[0];
	}	
	bool x = (*bits)[loc];
	size_t number = loc + 1;
	return (*bits)[loc];
}
//const bool& BitBlock::operator[](size_t index) const
//{
//	if (compressed)
//	{
//		if (index % 2 && index % 3 && index % 5)
//		{
//			size_t idx = index - index / 2 - index / 3 - index / 5 - index / 30 + index / 5 + index / 6 + index / 10 + index / 15;
//			return (*bits)[idx];
//			//return x;
//		}
//		//auto x = false;
//		return false;
//	}
//
//	return (*bits)[index];
//}
bool BitBlock::test(size_t index)
{
	return this->bits->test(index);
}

void BitBlock::SaveFile(std::string filename)
{
	std::ofstream outfile;
	outfile.open(filename, std::ios::out | std::ios::binary);
	outfile.write((char*)&size, sizeof(size_t));
	outfile.write((char*)&index, sizeof(size_t));
	this->compressBitSet();
	char buffer = 0;
	for (size_t i = 0; i < bits->size(); i += 8)
	{
		for (char c = 0; c < 8; c++)
		{
			char v = 0;
			if (size <= (i + c))//if we overflow at the end, pack the rest with 0
			{
				v = 0;
			}
			else
			{
				v = bits->test(i + c) << c;
				//printf("%d", (bitss->test(i + c)) ? 1 : 0);
			}
			buffer = buffer + v; //set one bit at a time
		}
		outfile.write(&buffer, 1);
		buffer = 0;
	}
	outfile.close();
	return;
}
void BitBlock::LoadFile()
{
	if (filename.empty())
		throw std::exception("Filename is empty");

	//load into a compressed dataset
	compressed = true;
	std::ifstream InFile;

	//First read in our object data
	InFile.open(filename, std::ios::out | std::ios::binary);
	InFile.read((char*)&size, sizeof(size_t));
	InFile.read((char*)&index, sizeof(size_t));

	// If we don't need to read the rest of the file now, skip it.
	if (cached == false) return;

	// We want to cache the file so bring it into memory as a bitset

	//Allocate memory 
	size_t bitsetsize = getBitsetSize(size);   
	//do it on the heap to avoid stack issues
	this->bits = new boost::dynamic_bitset(bitsetsize);


	char buffer = 0;
	for (size_t i = 0; i <= bitsetsize / 8; i++)
	{
		InFile.read(&buffer, 1);
		for (char c = 0; c < 8; c++)
		{
			if ((i * 8 + c) < bitsetsize)
			{
				char bit = 1;
				if (buffer & (bit << c))
				{
					bits->set(i * 8 + c);
					//printf("%d", 1);
				}
				else
				{
					bits->set(i * 8 + c, false);
					//printf("%d", 0);
				}
			}
			else
			{
				//we are in the last byte in the file with buffer bits to be ignored.
			}
		}
	}
	InFile.close();
	//Save the largest prime in this file.
	this->setMaxValue();
	//std::vector<size_t> primes = this->GetPrimes();
	//auto s = primes.size();
}

void BitBlock::UnCache()
{
}

std::vector<size_t> BitBlock::GetPrimes()
{
	std::vector<size_t> listOfPrimes;
	//2,3,5 are removed from compressed list.  Need to add them back
	if (compressed && (this->GetIndex() == 0))
	{
		listOfPrimes.push_back(2);
		listOfPrimes.push_back(3);
		listOfPrimes.push_back(5);
	}
	for (size_t count = 0; count < this->GetSize(); count++)
	{
		bool isPrime = (*this)[count];
		if (isPrime)
		{
			listOfPrimes.push_back(this->GetIndex() * this->GetSize() + count+1);
		}
	}
	return listOfPrimes;
}

void BitBlock::Compress()
{
	this->compressBitSet();

}

void BitBlock::Uncompress()
{
	this->uncompressBitSet();
}

size_t BitBlock::GetMaxValue()
{
	return this->maxValue;
}
void BitBlock::setMaxValue()
{
	for (auto i = this->size; i > 0; --i)
	{
		if ((*this)[i - 1])
		{
			maxValue = index * size + i - 1;
			break;
		}
	}
	maxValue = 0;
				
}

size_t BitBlock::getBitsetSize(size_t dataSize) {
	size_t max2 = dataSize / 2 + dataSize % 2;         // don't need to save the even numbers
	size_t max3 = 2 * max2 / 3 + max2 % 3;   // don't need to save numbers divisible by 3
	size_t max5 = 4 * max3 / 5 + max3 % 5;   // don't need to save numbers divisible by 5
	return max5;
}

void BitBlock::compressBitSet()
{
	//if it is already compressed, do nothing
	if (this->compressed) return;
	size_t compSize = this->getBitsetSize(this->GetSize());
	boost::dynamic_bitset<> *compBits = new boost::dynamic_bitset<>(compSize);
	size_t count = 0;
	for (size_t i = 1; i < size; i++)
	{
		if (i % 2 == 0 || i % 3 == 0 || i % 5 == 0 )
		{
			// don't do anything
		}
		else
		{
			//note that bits are off by 1 (e.g. 1 bit is a location 0, 7 bit is at location 6)
			bool isPrime = this->bits->test(i-1);
			compBits->set(count, isPrime);
			count++;
		}
	}
	compressed = true;
	delete bits;
	bits = compBits;
	return;
}

void BitBlock::uncompressBitSet()
{
	size_t count = 0;  //first unmasked prime
	//Create a new bitset with everything set to 0 (false)
	boost::dynamic_bitset<> *newBits = new boost::dynamic_bitset<>(size);
	if (index == 0)
	{
		(*newBits)[1] = true; //2
		(*newBits)[2] = true; //3
		(*newBits)[4] = true; //5
	}
	for (size_t i = 1; i < size; i++)
	{
		if (i % 2 == 0 || i % 3 == 0 || i % 5 == 0 )
		{
			// don't do anything
		}
		else
		{
			//if (index == 0)
			//{
			//	newBits->set(i + 5, bits->test(count));
			//}
			//else

			newBits->set(i - 1, bits->test(count));
			count++;
		}
	}
	compressed = false;
	delete bits;
	bits = newBits;
	return;
}