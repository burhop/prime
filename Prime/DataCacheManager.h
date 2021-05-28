#pragma once
#include <list>	
#include <memory>
#include "bitblock.h"

class DataCacheManager
{
	//struct proxy
	//{
	//	DataCacheManager* cache;
	//	size_t pos;

	//	proxy(DataCacheManager* us, size_t position)
	//		: cache(us), pos(position)
	//	{}

	//	// Invoked when proxy is used to modify the value.
	//	void operator = (const std::shared_ptr<BitBlock>& block)
	//	{
	//		cache->set(block, pos);

	//	}

	//	// Invoked when proxy is used to read the value.
	//	operator const std::shared_ptr<BitBlock>& () const
	//	{
	//		return cache->get(pos);
	//	}
	//};

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
	//BitBlock*& operator[] (size_t);
	/** Sets or accesses the bit at the location provided. If the data is on disk, it will be cached in memory as it assumes you will access more bits in this block */

	//std::shared_ptr<BitBlock>& operator[] (size_t);
	/** checks the pointer for a block to see if it exists.  The operator[] should not be used just to check existence as it will load the data*/
	bool BlockExists(size_t block);
	//proxy operator [] (size_t index)
	//{
	//	return DataCacheManager::proxy(this, index);
	//}
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
class DataCacheManagerLock
{
private:
	DataCacheManager* mgr;
public:
	DataCacheManagerLock(DataCacheManager* mgr)
	{
		this->mgr = mgr;
#ifdef _DEBUG
		//{
		//	if (omp_test_lock(&(mgr->theLock)))
		//	{
		//		omp_unset_lock(&(mgr->theLock));
		//	}
		//	else
		//	{
		//		//In single thread mode should not get here
		//		assert(false);
		//	}
		//	//int num = omp_get_thread_num();
		//	//std::cout << num << std::endl;

		//	//Set the lock and checks the result
		//}
#endif
		omp_set_lock(&(mgr->theLock));
		//}
	}

	~DataCacheManagerLock()
	{
		omp_unset_lock(&(mgr->theLock));
	}
};