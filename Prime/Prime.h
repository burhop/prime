#pragma once
#include <string>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <thread>
#include <bitblock.h>
/**
	This class is for calculating and storing prime numbers. It uses the Sieve of Eratosthenes to mark
	bits in a block of memory. As there are an infinite number of prime numbers, this class can apply the Sieve
	to an ever increasing list of blocks.  Note that PRIMEMAX must be devisible by 2,3, and 5 to keep blocks alligned.

	For version 0.3, it is limited to size_t (64 bits).  The plan is to extend thsi out to __int128 (128 bits) or use
	one of the "infinite" bit classes for integers in some futer versio0.

	Storage of the prime numbers, at least the initial numbers, is inefficient as a list.  Storage of a bit for each
	number (true for prime) is initially more efficient.  This list of bits can be cut in 1/2 by only storing odd numbers.
	It can be further cut by 1/3 by eliminating numbers divisible by 3, 1/5 by eliminating numbers devisible by 5 and so on.
	Since future cuts beyond 5 give diminishing returns, version 0.1 stops here. I hypothosize that a list of numbers will
	eventually be the most effiecient for higher ranges. I invite anyone with a more efficient method let me know of
	your solution so I can futher compress the data.

	P.S. The most efficient way to store these numbers is to just store the algorithm.  However, we want to be able to
	find the numbers in roughly constant time.

	https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes

	@author Mark Burhop
	@version 0.4 29/02/2020
*/
class __declspec(dllexport)  Prime
{

public:
	/** Constructor. By default block size will be 0xFFFFFFF0 */
	Prime(size_t bitsetSize=0xFFFFFFF0);
	/** Continously calculates primes.  For version 0.3, we are limited by size_t */
	int ContinousRun(std::string baseName);
	/** Destructor */
	~Prime();
	/** returns the size of the bit blocks.  That is, how many bit each block of data stores.  This was the value passed to the constuctor*/
	size_t GetBitBlockSize();
	/** Tells the Object to save each chunk of bits as it gets fully set. This is useful if you are searching 
	    for a large set of prime numbers and want to be sure you don't lose data to a catastrphic failure.
	@param b If true, saves each set of bits to a file as it is completed.
	*/
	void SaveIncrementalFiles(bool b=true);
	/** Efficient integer based square root fuction for size_t integers. It will be <= the floating point square root
	@return An size_t integer that is less than or equal to the floating point square root function */
	size_t Sqrt(size_t x);
	/** Calculates all the prime numbers for one or more data blocks. Throws an exception if int is negative.
		@param numberOfBlocks the number of blocks of memory to search though
		*/
	void FindPrimes(int numberOfBLocks);
	/** Calculates all the prime numbers for one or more data blocks. Throws an exception if int is negative.
		@param numberOfBlocks the number of blocks of memory to search though
		*/
	void FindPrimes(size_t numberOfBlocks=1);
	/** Load previously stored prime numbers in a set of .prm files. This will throw an exception if the 
	    number of bits in in file is different from the number of bits for this instance of the class.  It 
		will also throw exceptions if there are problems accessing the file or it finds bad data 
		@param baseName This is the baseName of the file. Each base name gets appended with 1,2,3, ... n and 
		the extenion '.prm'  (e.g. basename1.prm, basename2.prm) 
		*/
	void LoadFromFile(std::string baseName);
	/** Get the maximum value this instance has searched */
	size_t GetMaxValue();
	/** Get the maximum prime value (largest prime) this instance has found */
	size_t GetMaxPrime();
	/** Get the total number of primes that have been found */
	size_t GetMaxCount();
	/** Deletes existing prime files. Since the goal of this class is to find prime numbers, this is really only
	    useful for testing the code.
		@param basename  The basename of the set of files to delete.
		*/
	void DeleteExistingPrimeFiles(std::string baseName);
	/** Saves all calculated primes to one or more files. Each block is saved to a different file
	    by apending 1,2,3,...n to the baseName followed by .prm
		@param basename  The basename of the set of files to save.
		*/
	int SaveToFile(std::string baseName);
	/** returns a list of primes between two numbers. Note that there may be a maximum size of the 
	    std::vector.  A safe range should be less than 4294967295 (0xFFFFFFFF) members.
		At this time, the retrn value is meaningless.
		@param lowVal The lower value of the range
		@param highVal The upper value of the range
		@return meaningless.  May be used in future versions.
		*/
	std::vector<size_t> GetPrimesAsVector(size_t lowVal, size_t highVal);
	/** returns the number of primes between lowVal and highVal. 
		@param lowVal The lower boundary of the range of numbers
		@param highVal The upper boundary of the range of numbers
		@return The number of prime numbers between and including the two numbers
		*/
	size_t CountPrimes(size_t lowVal, size_t highVal);
	/** compresses the data so that less memory is used*/
	void Compress();
	/** uncompresses the data */
	void Uncompress();

private:
	unsigned int threadCount = std::thread::hardware_concurrency();
	size_t bitBlockSize=0;
	size_t max =  0;    // Can be up to 4294967280;
	size_t max2 = 0;    // don't need to save the even numbers
	size_t max3 = 0;    // don't need to save numbers divisible by 3
	size_t max5 = 0;    // don't need to save numbers divisible by 5
	size_t searchDisttance = 0;					   // Fartherest this class instance has search so far.
	std::string baseFileName="primes";
	bool saveIcrementalFiles = false;
	//Compiler worries this vector might get passed out of the DLL. Turn this warning off
#pragma warning( push )   
#pragma warning( disable : 4251)
	std::vector<BitBlock*> vectorOfBitSets;       // Place to save our data
#pragma warning(pop)

	//Private Functions
	void findFirstBlockOFPrimes();
	size_t NextPrime(boost::dynamic_bitset<>* bSet, size_t index);
	void writeBitSetToStream(const std::string fileName, BitBlock* bitss);
	//void readSparseBitSetFromStream(const char * my_file, std::vector<BitBlock*> b);
	//void readBitSetFromStream(const std::string my_file, std::vector<BitBlock*> b);
	void clearBitsetVector();
	BitBlock* primeSieve(std::vector<BitBlock*> vec);
	//void compressBitSet(boost::dynamic_bitset<>* iBitSet, std::vector<BitBlock*> b);
	//void uncompressBitSet(boost::dynamic_bitset<>* iBitSet, std::vector<BitBlock*> b);
	int saveToFile(std::string baseName, size_t count);
};


