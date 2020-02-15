#pragma once
#include <string>
#include <vector>
#include <boost/dynamic_bitset.hpp>

class BitBlock
{
	/** Links a bitblock to a file. There is an option to load it to memory (cache = true by default) or leave it on disk. (false) */
	BitBlock(std::string file, bool cache=true);
	/** Creates an empty bitblock */
	BitBlock(size_t size, size_t index);
	~BitBlock();
	/** Gets the size of the bitblock (number of bits */
	size_t GetSize();
	/** Gets the starting index of the bitblock */
	size_t GetIndex();
	bool operator[] (size_t);
	/** Saves a file to disk.  This will also set the file name */
	void SaveFile(std::string);
	/** Loads the file into memory if it was not already loaded */
	void LoadFile();

	/** removes the data from memory.  Access can still happen but it will be from the disk */
	void UnCache();
	/** Gets all the prime numbers stored in the block as a size_t vector */
	std::vector<size_t> GetPrimes();


private:
	size_t size;
	size_t index;
	std::string filename;
	bool cached;
	std::vector<size_t>* cachedPrimes;
	boost::dynamic_bitset<> *bits;
	//Compiler worries this vector might get passed out of the DLL. Turn this warning off
#pragma warning( push )   
#pragma warning( disable : 4251)
	std::vector<boost::dynamic_bitset<>*> vectorOfBitSets;       // Place to save our bitsets
#pragma warning(pop)
	/**  Gets the initial property types from the file */
	//void getFileProps(size_t& size, size_t& count);
	void setFile(size_t& size, size_t& count, bool loadbits);
	size_t getBitsetSize(size_t dataSize);

};

