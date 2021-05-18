#pragma once
#include <string>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <omp.h>
#include <iostream>

class BitBlock
{
public:
	/** Links a bitblock to a file. There is an option to load it to memory (cache = true by default) or leave it on disk. (false) */
	BitBlock(std::string file, bool cache=true,size_t size=0);
	/** Creates an empty bitblock */
	BitBlock(size_t size, size_t index);
	~BitBlock();
	/** Gets the size of the bitblock (number of bits) */
	size_t GetSize();
	/** Get First value in block */
	size_t GetFirstValue();
	/** Get last value in block */
	size_t GetLastValue();
	/** Gets the starting index of the bitblock (e.g. 2,3,5 will be in the first block so index 0). The next block or file will be in index 1 */
	size_t GetIndex();
	boost::dynamic_bitset<>::reference operator[] (size_t);

	//const bool& operator[] (size_t) const;
	
	void set(size_t index, bool val);

	/** Same as opperator[] kept for campitibility for bitarray */
	bool test(size_t index);
	/** Saves a file to disk.  This will also set the file name */
	void SaveFile(std::string);
	/** Loads the file into memory if it was not already loaded */
	void LoadFile();

	/** removes the data from memory. If the data was not previously saved to disk, an expception will be thrown. */
	void UnCache();
	/** Reads file data into disk for faster access */
	void Cache();
	/** Gets all the prime numbers stored in the block as a size_t vector */
	std::vector<size_t> GetPrimes();
	/** Compresses the data */
	void Compress();
	/** Uncomresses the data */
	void Uncompress();
	size_t GetMaxValue();
	/** If the data is loaded in memory or on disk */
	bool InMemory();
private:
	//If size=1000, it means we can get a bit for 1000 numbers.  However, the actuall number of bits in the class and file may be less.
	size_t size;
	size_t index;
	size_t maxValue;
	std::string filename;
	bool cached;
	bool compressed;
	bool savedToDisk;  //if this block has been saved to disk already
	std::vector<size_t>* cachedPrimes;
	boost::dynamic_bitset<>* bits = nullptr;
	boost::dynamic_bitset<> bDummy{ 2,2 };
	omp_lock_t theLock;
	//Compiler worries this vector might get passed out of the DLL. Turn this warning off
#pragma warning( push )   
#pragma warning( disable : 4251)
	//std::vector<boost::dynamic_bitset<>*> vectorOfBitSets;       // Place to save our bitsets
#pragma warning(pop)
	void setFile(size_t& size, size_t& count, bool loadbits);
	//Gets the actually number of bit in the bitset if it is compressed.
	size_t getBitsetSize(size_t dataSize);

	void compressBitSet();
	void uncompressBitSet();
	void setMaxValue();
	boost::dynamic_bitset<>::reference getAtIndex(size_t loc);
	void loadFile();
	void saveFile(std::string);

	friend class lock;
};

//just a simple lock class so losing scope will unlock
class lock
{
private:
	BitBlock* block;
public:
	lock(BitBlock* b)
	{
		block = b;

		//if (_DEBUG)
		//{
		//	//int num= omp_get_thread_num();
		//	///std::cout << num << std::endl;
		//	//Debug purposes. See if the lock is set
		//	if (omp_test_lock(&block->theLock))
		//	{
		//		//omp_unset_lock(&block->theLock);
		//	}
		//	else
		//	{
		//		assert(false);
		//	}
		//}
		//else
		//{

			omp_set_lock(&(block->theLock));
		//}
	}

	~lock()
	{
		omp_unset_lock(&(block->theLock));
	}
};