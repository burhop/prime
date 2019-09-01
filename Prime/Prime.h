#pragma once
/**
	This class is for calculating and storing prime numbers. It uses the Sieve of Eratosthenes to mark 
	bits in a block of memory. As there are an infinite number of prime numbers, this class can apply the Sieve
	to an ever increasing list of blocks. BLock size is set to 4294967280 (0xFFFFFFF0) by default but can be recompiled
	by defining PRIMEMAX.  Note that PRIMEMAX must be devisible by 2,3, and 5 to keep blocks alligned.
	
	For version 0.1, it is limited to size_t (64 bits).  The plan is to extend thsi out to __int128 (128 bits) or use
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
	@version 0.1 8/20/2019
*/
#include <string>
#include <vector>
#include <bitset>
// The maximum size of a block of memory. Default is max for VS C++ default x64 compiler. 
#ifndef PRIMEMAX
#define PRIMEMAX  3000000 // 0xFFFFFFF0 //highest number divisiable by 30
#endif

#if( PRIMEMAX % 120 != 0 )
#error "Bitset block size PRIMEMAX must be divisible by 30 (2*3*5) "
#endif
/**
	This class is for calculating and storing prime numbers. It uses the Sieve of Eratosthenes to mark
	bits in a block of memory. As there are an infinite number of prime numbers, this class can apply the Sieve
	to an ever increasing list of blocks. BLock size is set to 4294967280 (0xFFFFFFF0) by default but can be recompiled
	by defining PRIMEMAX.  Note that PRIMEMAX must be devisible by 2,3, and 5 to keep blocks alligned.

	For version 0.1, it is limited to size_t (64 bits).  The plan is to extend thsi out to __int128 (128 bits) or use
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
	@version 0.1 8/20/2019
*/
class __declspec(dllexport) Prime
{

public:
	/** Constructor */
	Prime();
	/** Destructor */
	~Prime();
	/** Tells the Object to save each chunk of bits as it gets fully set. This is useful if you are searching 
	    for a large set of prime numbers and want to be sure you don't lose data to a catastrphic failure.
	@param b If true, saves each set of bits to a file as it is completed.*/
	void SaveIncrementalFiles(bool b=false);
	/** Efficient integer based square root fuction for size_t integers. It will be <= the floating point square root
	@return An size_t integer that is less than or equal to the floating point square root function */
	size_t Sqrt(size_t x);
	/** Calculates all the prime numbers for one or more data blocks. 
		@param numberOfBlocks the number of blocks of memory to search though */
	void FindPrimes(size_t numberOfBlocks=1);
	/** Load previously stored prime numbers in a set of .prm files. 
		@param baseName This is the baseName of the file. Each base name gets appended with 1,2,3, ... n and 
		the extenion '.prm'  (e.g. basename1.prm, basename2.prm) */
	void LoadFromFile(std::string baseName);
	/** Saves all calculated primes to one or more files.  */
	int SaveToFile(std::string baseName);
	/** returns a list of primes between two numbers. Note that there may be a maximum size of the 
	    std::vector.  A safe range should be less than 4294967295 (0xFFFFFFFF) 
		@param lowVal The lower value of the range
		@param highVal The upper value of the range*/
	std::vector<size_t> GetPrimesAsVector(size_t lowVal, size_t highVal);
	/* returns a bitset of PRIMEMAX(0xFFFFFFFF by default) size where prime numbers are true.
	    if there are more than PRIMEMAX primes, you will need to call this function with 0,1,2,3 
	    to get each following set of primes */
	//std::bitset<PRIMEMAX> GetPrimesAsBitset(size_t index);
	/** returns the number of primes between lowVal and highVal. 
		@param lowVal The lower boundary of the range of numbers
		@param highVal The upper boundary of the range of numbers
		@return The number of prime numbers between and including the two numbers
		*/
	size_t CountPrimes(size_t lowVal, size_t highVal);
	/** the size of the data block the library was compiled with. */
	static const size_t BitBlockSize = PRIMEMAX;

private:
	static const size_t max = (size_t)PRIMEMAX;           // 4294967295;
	static const size_t max2 = max / 2 + max % 2;         // don't need to save the even numbers
	static const size_t max3 = 2 * max2 / 3 + max2 % 3;   // don't need to save numbers divisible by 3
	static const size_t max5 = 4 * max3 / 5 + max3 % 5;   // don't need to save numbers divisible by 5
	size_t searchDisttance = 0;							  // Fartherest this class instance has search so far.
	//std::string baseFileName="primes";
	bool saveIcrementalFiles = false;
	//Compiler worries this vector might get passed out of the DLL. Turn this warning off
#pragma warning( push )   
#pragma warning( disable : 4251)
	std::vector<std::bitset<max>*> vectorOfBitSets;       // Place to save our bitsets
#pragma warning(pop)

	//Private Functions
	void findFirstBlockOFPrimes();
	size_t NextPrime(std::bitset<Prime::max >* bSet, size_t index);
	void writeSparseBitSetToStream(const std::string fileName, std::bitset<Prime::max> *bitss);
	void writeBitSetToStream(const std::string fileName, std::bitset<max5>* bitss);
	void readSparseBitSetFromStream(const char * my_file, std::bitset<Prime::max>* b);
	void readBitSetFromStream(const std::string my_file, std::bitset<Prime::max5>* b);
	void clearBitsetVector();
	std::bitset<Prime::max>* primeSieve();
	size_t countPrimes(std::bitset<Prime::max>* b);
	std::bitset<Prime::max>* primeSieve2(std::vector<std::bitset<Prime::max>*> vec);
	void compressBitSet(std::bitset<Prime::max>* iBitSet, std::bitset<Prime::max5>* b);
	void uncompressBitSet(std::bitset<Prime::max5>* iBitSet, std::bitset<Prime::max>* b);
};


