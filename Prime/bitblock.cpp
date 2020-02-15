#include "bitblock.h"
#include <iostream>
#include <fstream> 
#include <boost/dynamic_bitset.hpp>
#include <cstdio>

BitBlock::BitBlock(std::string file, bool cache)
{
	filename = file;
	cached = cache;
	//Load part or all of the file depending on the cache setting
	LoadFile();
}

BitBlock::BitBlock(size_t s, size_t i)
{
	size=s;
	index=i;
	auto bitsetsize = getBitsetSize(size);
	bits = new boost::dynamic_bitset(bitsetsize);
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
bool BitBlock::operator[](size_t index)
{

	if (index % 2 && index % 3 && index % 5)
	{
		size_t idx = index - index / 2 - index / 3 - index / 5 - index / 30 + index / 5 + index / 6 + index / 10 + index / 15;
		return (*bits)[idx];
	}
	return false;
}

void BitBlock::SaveFile(std::string filename)
{
	std::ofstream outfile;
	outfile.open(filename, std::ios::out | std::ios::binary);
	outfile.write((char*)&size, sizeof(size_t));
	outfile.write((char*)&index, sizeof(size_t));

	char buffer = 0;
	for (size_t i = 0; i < size; i += 8)
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


	std::ifstream InFile;

	//First read in our object data
	InFile.open(filename, std::ios::out | std::ios::binary);
	InFile.read((char*)&size, sizeof(size_t));
	InFile.read((char*)&index, sizeof(size_t));

	// If we don't need to read the rest of the file now, skip it.
	if (cached == false) return;

	// We want to cache the file so bring it into memory as a bitset

	//Allocate memory 
	size_t bitsetsize = getBitsetSize(size);   // don't need to save the even numbers
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
}

void BitBlock::UnCache()
{
}

std::vector<size_t> BitBlock::GetPrimes()
{
	std::vector<size_t> listOfPrimes;
	for (size_t count = 0; count < this->GetSize(); count++)
	{
		bool isPrime = (*bits)[count];
		if (isPrime)
		{
			listOfPrimes.push_back(this->GetIndex() * this->GetSize() + count);
		}
	}
	return listOfPrimes;
}

size_t BitBlock::getBitsetSize(size_t dataSize) {
	size_t max2 = dataSize / 2 + dataSize % 2;         // don't need to save the even numbers
	size_t max3 = 2 * max2 / 3 + max2 % 3;   // don't need to save numbers divisible by 3
	size_t max5 = 4 * max3 / 5 + max3 % 5;   // don't need to save numbers divisible by 5
	return max5;
}

//size_t BitBlock::getDataSize(size_t bitsettSize) {
//	
//	size_t max2 = dataSize / 2 + dataSize % 2;         // don't need to save the even numbers
//	size_t max3 = 2 * max2 / 3 + max2 % 3;   // don't need to save numbers divisible by 3
//	size_t max5 = 4 * max3 / 5 + max3 % 5;   // don't need to save numbers divisible by 5
//	return max5;
//}
