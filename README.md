# prime
This project is for calculating and storing prime numbers. It uses the [Sieve of Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes) to mark bits in a block of memory. As there are an infinite number of prime numbers, this class can apply the Sieve to an ever increasing list of blocks. 

For version 0.2, it is limited to size_t (64 bits). The plan is to extend this out to "__int128" (128 bits) or use one of the "infinite" bit classes for integers in some future version.

Storage of the prime numbers, at least the initial numbers, is inefficient as a list. Storage of a bit for each number (true for prime) is initially more efficient. This list of bits can be cut in 1/2 by only storing odd numbers. It can be further cut by 1/3 by eliminating numbers divisible by 3, 1/5 by eliminating numbers divisible by 5 and so on. Since future cuts beyond 5 give diminishing returns, version 0.2 stops here.  I invite anyone with a more efficient method let me know of your solution so I can further compress the data.

P.S. The most efficient way to store these numbers is to just store the algorithm. However, we want to be able to look up the numbers in roughly constant time.

Some useful links:

[The Nth Prime Page](https://primes.utm.edu/nthprime/index.php#piofx)

[How Many Primes Are There?](https://primes.utm.edu/howmany.html)

**Some next steps are:**

 1. Expanding to larger ints.
 2. Support for unlimited size ints
 5. More efficient use of memory to support unlimited prime calculations (right now, all the bitsets are kept in memory)
 6. More efficient storage of the numbers (ping me if you have ideas)

While C++ is efficient, Python is more fun for trying out clever ideas. Some next "fun" steps are:

 1. Python code to read the output files.  THere is a rough version of this now.
 2. Python image creation based off prime numbers.  See below for an example of an [Ulam Spiral](https://en.wikipedia.org/wiki/Ulam_spiral)
 
 3.) Maybe a "primes as a service" implementation :-)


**Motivation:**
This is just a fun project.  I got a scholarship to the University of Florida for some work I did with prime numbers in high school. It took me days to calculate a set of prime numbers that today can be calculated in a few minutes.

**Changes since Last Version:**

For version 0.2, the following changes were made:

1. Use of [Boost bitset](https://www.boost.org/doc/libs/1_36_0/libs/dynamic_bitset/dynamic_bitset.html) so that the size of the bitset does not require recompile of the DLL/so
2. Addition of [C++ Catch](https://github.com/catchorg/Catch2).  Creation of new test case project and a number of test cases.
3. A new project called "go" that just keeps running and running looking for new primes. You can stop it and restarted it and it will remember any blocks of primes it already completed.
3. Python files for reading in the prime files and creating some prime bases images.  THis is still rough and in development but I inlcude it here in case anyone wants a parcer for the files.

![Spiral](https://github.com/burhop/prime/blob/master/python/primeSpiral.png)
