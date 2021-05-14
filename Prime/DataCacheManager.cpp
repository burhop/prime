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
	//for (size_t i = 0; i < blockCount;++i)
	//{
	//	if (arrayOfBlocks[i] != 0) delete arrayOfBlocks[i];
	//}
	delete[] arrayOfBlocks;
	arrayOfBlocks = nullptr;
}

//std::shared_ptr<BitBlock>& DataCacheManager::operator[](size_t loc)
//{
//	#pragma omp critical(DataCacheManager)
//	{
//		if (arrayOfBlocks[loc] != 0 && !arrayOfBlocks[loc]->InMemory())
//		{
//			(arrayOfBlocks[loc])->Cache();
//			updateCache(loc);
//		}
//	}
//	return this->arrayOfBlocks[loc];
//}

bool DataCacheManager::BlockExists(size_t block)
{
	return this->arrayOfBlocks[block]? true: false;
}

// A simple cache that removes the last accessed BitArray if we are out of space
//Note that the calling function may need to be is in an OMP Critical section
void DataCacheManager::updateCache(size_t loc)
{
//#pragma omp critical(DataCacheManagerUpdate)
	{
		// if we are updating the Cache but the new block is not in memory do nothing.
		if (this->arrayOfBlocks[loc]->InMemory() == false)
		{
			return;
		}
		//	std::cout << "Cashe Size: " << listOfBlocks.size() << std::endl;
		{
			//remove it from the list, as we will be re adding it to the back.
			this->listOfBlocks.remove(loc);
			this->listOfBlocks.push_back(loc);

			//Do we have too many in memory?  If so, remove the last used (if it is free).
			if (this->listOfBlocks.size() > this->maximumBlocksInMemory)
			{
				size_t idx = this->listOfBlocks.front();

				//If We have the only pointer to this block, we can unload it. If others do, leave it there for now and try the next oldest
				for (size_t val : listOfBlocks)
				{
					long use_count = this->arrayOfBlocks[val].use_count();
					if (use_count < 2 && val != loc)
					{
						this->listOfBlocks.remove(val);
						arrayOfBlocks[val]->UnCache();
						break;
					}
				}
				// If all blocks are being referenced outside of this class, we could end up with more in the list than the maximumBlocksInMemory.
			}
		}
	}
}

std::shared_ptr<BitBlock> DataCacheManager::get(size_t loc)
{
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
//#pragma omp critical(DataCacheManager2)
	{
		this->arrayOfBlocks[idx] = obj;
		this->updateCache(idx);
	}
}

void DataCacheManager::PrintStatus()
{
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