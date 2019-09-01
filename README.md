# prime
This project is for calculating and storing prime numbers. It uses the [Sieve of Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes) to mark bits in a block of memory. As there are an infinite number of prime numbers, this class can apply the Sieve to an ever increasing list of blocks. Block size is set to 4294967295 (0xFFFFFFFF) by default but can be recompiled by defining **PRIMEMAX**

For version 0.1, it is limited to size_t (64 bits). The plan is to extend this out to "__int128" (128 bits) or use one of the "infinite" bit classes for integers in some future version.

Storage of the prime numbers, at least the initial numbers, is inefficient as a list. Storage of a bit for each number (true for prime) is initially more efficient. This list of bits can be cut in 1/2 by only storing odd numbers. It can be further cut by 1/3 by eliminating numbers divisible by 3, 1/5 by eliminating numbers divisible by 5 and so on. Since future cuts beyond 5 give diminishing returns, version 0.1 stops here. I hypothesize that a list of numbers will eventually be the most efficient for higher ranges. I invite anyone with a more efficient method let me know of your solution so I can further compress the data.

P.S. The most efficient way to store these numbers is to just store the algorithm. However, we want to be able to look up the numbers in roughly constant time.

Some useful links:

[The Nth Prime Page](https://primes.utm.edu/nthprime/index.php#piofx)
[How Many Primes Are There?](https://primes.utm.edu/howmany.html)

**Some next steps are:**

 1.  Some Test cases to keep the code stable.
 2. Expanding to larger ints.
 3. Support for unlimited size ints
 4. Adding boost and getting rid of compile time specified data structures (e.g. bitset)
 5. More efficient use of memory to support unlimited prime calculations (right now, all the bitsets are kept in memory)
 6. More efficient storage of the numbers (ping me if you have ideas)

While C++ is efficient, Python is more fun for trying out clever ideas. Some next "fun" steps are:

 1. Python code to read the output files.  
 2. Python image creation based off prime numbers 3.) Maybe a "primes as a service" implementation :-)


**Motivation:**
This is just a fun project.  I got a scholarship to the University of Florida for some work I did with prime numbers in high school and I have continued to play with them over the years. 
