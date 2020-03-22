#pragma once
#include <string>
#include <vector>
#include <boost/dynamic_bitset.hpp>

class BitBlock
{
public:
	/** Links a bitblock to a file. There is an option to load it to memory (cache = true by default) or leave it on disk. (false) */
	BitBlock(std::string file, bool cache=true);
	/** Creates an empty bitblock */
	BitBlock(size_t size, size_t index);
	~BitBlock();
	/** Gets the size of the bitblock (number of bits */
	size_t GetSize();
	/** Gets the starting index of the bitblock */
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
	/** Tells the class to expand the bit array.  This makes it faster but uses more memory */

	/** removes the data from memory.  Access can still happen but it will be from the disk */
	void UnCache();
	/** Gets all the prime numbers stored in the block as a size_t vector */
	std::vector<size_t> GetPrimes();
	/** Compresses the data */
	void Compress();
	/** Uncomresses the data */
	void Uncompress();
	size_t GetMaxValue();
private:
	//If size=1000, it means we can get a bit for 1000 numbers.  However, the actuall number of bits in the class and file may be less.
	size_t size;
	size_t index;
	size_t maxValue;
	std::string filename;
	bool cached;
	bool compressed;
	std::vector<size_t>* cachedPrimes;
	boost::dynamic_bitset<> *bits;
	boost::dynamic_bitset<> bDummy{ 2,2 };
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
};

