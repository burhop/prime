#include <memory>
#include "DataCacheManager.h"
#include "bitblock.h"
#include "iostream"


DataCacheManager::DataCacheManager(size_t blockcount,size_t maximumInMemory)
{
	this->blockCount = blockcount;
	this->maximumBlocksInMemory = maximumInMemory;
	//arrayOfBlocks = new BitBlock * [blockCount]();  //Allocate an array of pointers and initialize them to 0.
	arrayOfBlocks = new std::shared_ptr<BitBlock> [blockCount]();  //Allocate an array of pointers and initialize them to 0.
	//initializes the OMP lock, does not lock or unlock
	omp_init_lock(&theLock);
}

std::shared_ptr<BitBlock> DataCacheManager::MakeBitBlock(std::string file, bool cache =true,size_t size=0)
{
	return std::make_shared<BitBlock>(file, cache,size);
}
std::shared_ptr<BitBlock> DataCacheManager::MakeBitBlock(size_t size, size_t index)
{
	return std::make_shared<BitBlock>(size, index);
}
DataCacheManager::~DataCacheManager()
{
	delete[] arrayOfBlocks;
	arrayOfBlocks = nullptr;
}


bool DataCacheManager::BlockExists(size_t block)
{
	DataCacheManagerLock(this);
	return this->arrayOfBlocks[block]? true: false;
}

// A simple cache that removes the last accessed BitArray if we are out of space
void DataCacheManager::updateCache(size_t loc)
{
	DataCacheManagerLock(this);
	//#pragma omp critical(DataCacheManagerUpdate)
	{
		// if we are updating the Cache but the new block is not in memory do nothing.
		if (this->arrayOfBlocks[loc]->InMemory() == false)
		{
			return;
		}
		//remove it from the list, as we will be re adding it to the back.
		this->listOfBlocks.remove(loc);
		this->listOfBlocks.push_back(loc);
		cleanCache();
	}
}
void DataCacheManager::cleanCache()
{
	//##### issue!   ListOFBlocks is changing (it was size 12 to start and size 10 with bad data in line 61)
	auto blockCount = this->listOfBlocks.size();
	long count = blockCount - this->maximumBlocksInMemory;
	if (count < 1)return;
	auto tempList = this->listOfBlocks;
	//We will try to remove 'count' blocks
	for (auto const& i :tempList)
	{
		long use_count = this->arrayOfBlocks[i].use_count();
		if (use_count < 2)
		{   
			this->listOfBlocks.remove(i);
			arrayOfBlocks[i]->UnCache();
			count--;
		}
		//if we have removed 'count' blocks, we are done
		if (count < 1) break;
	}
	// It possible to get here and not have removed 'count' blocks
	// That should be a rare case when everything in the cache is being used in another thread
}

std::shared_ptr<BitBlock> DataCacheManager::get(size_t loc)
{
	DataCacheManagerLock(this);
//#pragma omp critical(DataCacheManager)
	{
		// if our smart pointer exists but it is not in memory, load it
		if (arrayOfBlocks[loc] != 0 && !arrayOfBlocks[loc]->InMemory())
		{
			(arrayOfBlocks[loc])->Cache();
			//since we are loading a new block of primes, we need to update our list of loaded blocks and possibly unload if we are at the max
			updateCache(loc);
		}
	}
	return this->arrayOfBlocks[loc];
}

void DataCacheManager::set(std::shared_ptr<BitBlock> obj, size_t idx)
{
	DataCacheManagerLock(this);
//#pragma omp critical(DataCacheManager2)
	{
		this->arrayOfBlocks[idx] = obj;
		this->updateCache(idx);
	}
}

void DataCacheManager::PrintStatus()
{
	DataCacheManagerLock(this);
//#pragma omp critical(print3)
	{

		std::cout << "DataCache maximum blocks " << this->maximumBlocksInMemory << std::endl;
		std::cout << "list of Blocks that are in memory ";
		for (auto const& i : this->listOfBlocks) {
			std::cout << i << " ";
		}
		std::cout << std::endl;
		std::cout << "Blocks used ___________________" << std::endl;
		size_t count = 0;
		for (size_t i = 0; i < this->blockCount; ++i)
		{
			if (arrayOfBlocks[i] != 0 && arrayOfBlocks[i]->InMemory())
			{
				std::cout << "block " << i << " has refcount : " << arrayOfBlocks[i].use_count() << std::endl;
				count++;
			}
			else if (arrayOfBlocks[i] != 0)
			{
				//std::cout << "block" << i << " not in memory" << std::endl;
			}
		}
		std::cout << "blocks in memory: " << count << std::endl;
	}
}