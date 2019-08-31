#pragma once
/**
	This class is for calculating and storing prime numbers. It uses the Sieve of Eratosthenes to mark 
	bits in a block of memory. As there are an infinite number of prime numbers, this class can apply the Sieve
	to an ever increasing list of blocks. BLock size is set to 4294967295 (0xFFFFFFFF) by default but can be recompiled
	by defining PRIMEMAX
	
	For version 0.1, it is limited to size_t (64 bits).  The plan is to extend thsi out to __int128 (128 bits) or use
	one of the "infinite" bit classes for integers in some futer version.

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
#define PRIMEMAX 1000 /*0xFFFFFFFF */
#endif

class __declspec(dllexport) Prime
{

public:
	Prime();
	~Prime();
	/** Tells the Object to save each chunk of bits as it gets fully set. */
	void SaveIncrementalFiles(bool b=true);
	/** Squareroot fuction for size_t */
	size_t Sqrt(size_t x);
	/** Finds all prime numbers up to the limit */
	void FindPrimes(size_t limit=PRIMEMAX);
	void findFirstBlockOFPrimes();
	/** Load previously stored prime numbers.  */
	void LoadFromFile(std::string baseName);
	/** Saves all calculated primes to one or more files.  */
	int SaveToFile(std::string baseName);
	/** returns a list of primes between two numbers. Note that there may be a maximum size of the 
	    std::vector.  A safe range is a difference of about 4294967295 (0xFFFFFFFF) */
	std::vector<size_t> GetPrimesAsVector(size_t lowVal, size_t highVal);
	/** returns a bitset of PRIMEMAX(0xFFFFFFFF by default) size where prime numbers are true.
	    if there are more than PRIMEMAX primes, you will need to call this function with 0,1,2,3 
	    to get each following set of primes */
	//std::bitset<PRIMEMAX> GetPrimesAsBitset(size_t index);
	/** returns the number of primes between lowVal and highVal. MAXPRIME (default #FFFFFFFF) used
	    if highVal not provided */
	size_t CountPrimes(size_t lowVal, size_t highVal=PRIMEMAX);
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


