#include "bitblock.h"
#include <iostream>
#include <fstream> 
#include <boost/dynamic_bitset.hpp>
#include <boost/filesystem.hpp>
#include <cstdio>
#include <filesystem>
#include <cstdio>
#include <omp.h>

// [[ omp::sequence(directive(parallel), directive(for)) ]] 

BitBlock::BitBlock(std::string file, bool cache, size_t inputSize)
{
	//Multiple threads may access this object at the same time. Be able to lock it.
	omp_init_lock(&theLock);  //doesn't lock, just creates one. Initially it is unlocked
	filename = file;
	savedToDisk = false;
	cached = cache;
	cachedPrimes = nullptr;
	maxValue = 0;
	size = inputSize;  //if 0, the file will be opened to get the value. If the value is alrady known, no point in wasting time for fileIO

	//Load part or all of the file depending on the cache setting.  Do not uncompress as this takes time.
	LoadFile(cache,false);
	if (size == 0)
	{
		throw "Error loading file. Size could not be determined. Bad file?";
	}
}

BitBlock::BitBlock(size_t s, size_t i)
{
	//User doesn't care about name. Just create a unique one (probably in /tmp or $TEMP) and use that. Use Boost ast std version is security issue
	filename  = boost::filesystem::unique_path().string();
	savedToDisk = false;
	//Multiple threads may access this object at the same time. Be able to lock it.
	omp_init_lock(&theLock);//doesn't lock, just creates one. Initially it is unlocked
	size=s;
	index=i;  

	compressed = false;
	cachedPrimes = nullptr;
	maxValue = 0;
	//auto bitsetsize = getBitsetSize(size);
	//block off the full size
	bits = new boost::dynamic_bitset(size);
	bits->set();
	//Initially start in memory, so cached =true even though ti is blank
	cached = true;
}

BitBlock::~BitBlock()
{
	omp_destroy_lock(&theLock);
	//delete memory
	delete bits;
}

size_t BitBlock::GetSize()
{
	//lock ompLock(this);
	return this->size;
}

size_t BitBlock::GetIndex()
{
	//lock ompLock(this);
	return index;
}
size_t BitBlock::GetFirstValue()
{
	//lock ompLock(this);
	return index * size;
}
size_t BitBlock::GetLastValue()
{
	//lock ompLock(this);
	return (index + 1)* size - 1;
}

//TODO  Test is if is faster to do this or just have the whole array in memory

// you can't use bool here because the setter won't work. This is because bitsets don't return a bool but a proxy
boost::dynamic_bitset<>::reference BitBlock::operator[](size_t loc)
{
	lock ompLock(this);
//#pragma omp critical
	{
		return getAtIndex(loc);
	}
}
boost::dynamic_bitset<>::reference BitBlock::getAtIndex(size_t loc)
{
	{
		//because of parallel code, cached could be true and bits is null
		if (!this->cached || bits==nullptr)
		{
			throw std::exception("Data must be loaded into memory for access.  Call Cache() or LoadFile().");
		}
		if (compressed)
		{
			// if it is the first block, 2,3,5 are missing.
			if (this->index == 0)
			{
				if (loc == 1 || loc == 2 || loc == 4) return bDummy[1];
			}
			size_t index = loc + 1;
			if (index % 2 && index % 3 && index % 5)
			{
				size_t idx = index + index / 6 + index / 10 + index / 15 - index / 2 - index / 3 - index / 5 - index / 30 - 1;
				return (*bits)[idx];
			}
			return bDummy[0];
		}
		//bool x = (*bits)[loc];
		//size_t number = loc + 1;
		return (*bits)[loc];
	}
}


void BitBlock::set(size_t loc, bool val)
{
	lock ompLock(this);
	if (!this->cached)
	{		
		throw std::exception("Data must be loaded into memory for access. Start with a new object or Call Cache() or LoadFile().");

	}
	if (this->compressed)
	{
		if (this->index == 0)
		{
			//this rpresents 2,3,and 5 which are not saved in compressed data.  They will always be true;
			if (loc == 1 || loc == 2 || loc == 4) return;
		}
		size_t index = loc + 1;
		if (index % 2 && index % 3 && index % 5)
		{
			size_t idx = index + index / 6 + index / 10 + index / 15 - index / 2 - index / 3 - index / 5 - index / 30 - 1;
			this->bits->set(idx, val);
		}
		else
		{
			//do nothing as it is divisible by 2, 3, or 5.  It will always return 0 from the compressed data.
		}
	}

	this->bits->set(loc, val);
}
bool BitBlock::test(size_t index)
{
	lock ompLock(this);
	if (!this->cached)
	{
		throw std::exception("Data must be loaded into memory for access.  Call Cache() or LoadFile().");
	}
	if (this->compressed)
	{
		throw std::exception("test() function only works on uncompressed data. Please uncompress the bitblock or use the [] operator.");
	}
	return this->bits->test(index);
}
void BitBlock::SaveFile(std::string filename)
{
	lock ompLock(this);
	std::cout << "saving " << filename;
	saveFile(filename);
	std::cout << " ...saved\n";
}
void BitBlock::saveFile(std::string filename)
{
	if (this->savedToDisk)
	{
		return;
	}
	// if it has not been saved to disk AND it is not in memory, we have an error.
	if (!this->cached)
	{
		throw std::exception("Error.  No Data in this BitBlock to save.");
	}

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
	savedToDisk = true;
	return;
}
void BitBlock::LoadFile(bool fullFile=true, bool unCompress=false)
{
	lock ompLock(this);
	loadFile(fullFile,unCompress);
}
void BitBlock::loadFile(bool fullFile, bool unCompress)
{
	//First see if it was already loaded. We don't need to load again.
	if (this->cached && this->bits != nullptr)
	{
		return;
	}
	//lock ompLock(this);
	if (filename.empty())
		throw std::exception("File is empty");
	
	//If the file exists, we've already loaded its header (size!=0) and we don't want the data there is nothing to do
	if (this->size!=0 && fullFile == false && std::filesystem::exists(this->filename))
	{
		return;
	}

	//load into a compressed dataset.  This is what is saved in the file.
	compressed = true;
	std::ifstream InFile;

	//First read in our object data
	InFile.open(filename, std::ios::out | std::ios::binary);
	InFile.read((char*)&size, sizeof(size_t));
	InFile.read((char*)&index, sizeof(size_t));
	if (size < 1)
	{
		std::string message = std::string("bad File: ") + filename;
		throw std::exception(message.c_str());
	}
	// If we don't need to read the rest of the file now, skip it.
	if (fullFile == false)
	{
		InFile.close();
		return;
	}

	// We want to cache the file so bring it into memory as a bitset

	//Allocate memory 
	size_t bitsetsize = getBitsetSize(size);   
	//do it on the heap to avoid stack issues
	if (this->bits != nullptr)
	{
		assert(false);
		throw "Atempting to load file data into an existing structure. This will result in a Memory Leak";
	}

//#pragma omp critical 
	{
		this->bits = new boost::dynamic_bitset(bitsetsize);
	}

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
				}
				else
				{
					bits->set(i * 8 + c, false);
				}
			}
			else
			{
				//we are in the last byte in the file with buffer bits to be ignored.
			}
		}
	}
	InFile.close();
	this->cached = true;
	if (unCompress)
	{
		this->uncompressBitSet();
	}
	//Save the largest prime in this file.
	this->setMaxValue();

	return;
}

//Clears the memory for the bitset.  Note that it is not thread safe so be careful if you have multiple threads with access to this object.
void BitBlock::UnCache()
{
	lock ompLock(this);

	//if it is not in memory already, nothing to do
	if (cached == false) return;

	//If we have never saved this data to permanent storage, better do it now.
	if (savedToDisk == false)
	{
		saveFile(filename);
	}
	delete this->bits;
	this-> bits = nullptr;
	delete this->cachedPrimes;
	this->cachedPrimes = nullptr;
	this->cached = false;
}
void BitBlock::Cache()
{
	lock ompLock(this);
	//since multithreaded, could have paused at lock due to other call cacheing.
	if (filename.empty())
		throw std::exception("No filename for this data exists. Don't know where to load data from.");
	//Turn on the cached flag and load the data
#pragma omp critical 
	{
		//this->cached = true;
		this->loadFile(true,true);
		//this->uncompressBitSet();
	}
}

std::vector<size_t> BitBlock::GetPrimes()
{
	lock ompLock(this);
	//We can't have multiple threads building this at the same time.


//#pragma omp critical(GetPrimes)
	{
		//lock ompLock(this);
		if (!cachedPrimes)
		{
			//Calculate the primes

			if (!this->cached)
			{
				throw std::exception("Data must be loaded into memory for access.  Call Cache() or LoadFile().");
			}
			std::vector<size_t> listOfPrimes;
			//2,3,5 are removed from compressed list.  Need to add them back
			if (compressed && (this->index == 0))
			{
				listOfPrimes.push_back(2);
				listOfPrimes.push_back(3);
				listOfPrimes.push_back(5);
			}
			for (size_t count = 0; count < this->size; count++)
			{
				bool isPrime = getAtIndex(count);
				if (isPrime)
				{
					listOfPrimes.push_back(this->index * this->size + count + 1);
				}
			}
			this->cachedPrimes = new std::vector<size_t>(listOfPrimes);
		}
	}
	return *cachedPrimes;
}

void BitBlock::Compress()
{

	lock ompLock(this);
	this->compressBitSet();

}

void BitBlock::Uncompress()
{
	lock ompLock(this);
	if (this->cached)
	{
		this->uncompressBitSet();
	}
	else
	{
		//We first need to load the data.  Second option means to uncompress so no need to do this a second time
		this->loadFile(true, true);
	}
}

size_t BitBlock::GetMaxValue()
{
	lock ompLock(this);
	return this->maxValue;
}
bool BitBlock::InMemory()
{
	lock ompLock(this);
	return this->cached;
}
void BitBlock::setMaxValue()
{
	///lock ompLock(this);
	for (auto i = this->size; i > 0; --i)
	{
		if (getAtIndex(i - 1))
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
	size_t compSize = this->getBitsetSize(this->size);
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
	if (!this->compressed) return;
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
			newBits->set(i - 1, bits->test(count));
			count++;
		}
	}
	compressed = false;
	delete bits;
	bits = newBits;
	return;
}