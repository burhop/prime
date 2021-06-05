#pragma once
#include <list>	
#include <memory>
#include "bitblock.h"

class DataCacheManager
{
public:
	/**
	A class for managing blocks of prime numbers.  The blocks are BitBlocks and tracked via reference counting.
	Right tnow you specify the number of blocks that will be managed and the maximum that can be in memory at any time. */
	DataCacheManager(size_t blockcount, size_t maximumInMemory);

	/** Creates a BitBlock and associates it to a file on disk. If cache is true, the file contents are loaded into memory for fast access. 
	    If size==0, the size is extracted from the file. If you know the size, it can be provided to avoid the fileio. */
	std::shared_ptr<BitBlock> MakeBitBlock(std::string file, bool cache,size_t size);
	/** Creates an empty BitBlock of a given size and stores a pointer to it at the index location provided*/
	std::shared_ptr<BitBlock> MakeBitBlock(size_t size, size_t index);
	~DataCacheManager();
	/** checks the pointer for a block to see if it exists.  */
	bool BlockExists(size_t block);

	std::shared_ptr<BitBlock> get(size_t);
	void set(std::shared_ptr<BitBlock>, size_t);
	//Prints the status to stdio.  This is mainly for debug to see how memory is being managed
	void PrintStatus();
private:
	//Total number of blocks that can be managed. Correspeonds to array size.
	size_t blockCount;	
	size_t maximumBlocksInMemory;
	std::list<size_t> listOfBlocks;
	//std::shared_ptr<BitBlock*> p;
	std::shared_ptr<BitBlock>* arrayOfBlocks;
	omp_lock_t theLock;  //allow locking of DataCacheManager to reading of data while changing of data
	//BitBlock** arrayOfBlocks;

	void updateCache(size_t idx);
	void cleanCache();
	friend class DataCacheManagerLock;

};

/** just a simple lock class for OpenMP so losing scope will unlock */
class DataCacheManagerLock
{
private:
	DataCacheManager* dcm;
public:
	DataCacheManagerLock(DataCacheManager* mgr)
	{
		dcm = mgr;
		omp_set_lock(&(mgr->theLock));
	}

	~DataCacheManagerLock()
	{
		omp_unset_lock(&(dcm->theLock));
	}
};