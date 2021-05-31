// go.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "Prime.h"
//#include <Windows.h>
//#include <exception>

int main()
{
	//__try
	{
		try
		{

			Prime* p = nullptr;
			std::cout << "Reading existing results (if they exist)...\n";
			try {
				p = new Prime("Prime100000020-", 0, 800);
			}
			catch (std::exception)
			{
				//no existing files found we need to start new
				std::cout << "No existing data found. Starting prime search from 0\n";

				// Would be good to calculate this automatically based on available memory on the system.  Mine is 12 cores with 32 Gig so we'll let that be the default.

				//p = new Prime(0xFFFFFFF0,0,26);  //4,294,967,280 or 4.2 billion or 536MB  If we save 26 and have 12 cores running, we are using about 20Gig of memory just for data  
				//p = new Prime(100000020,0,800);  //100 million and 20   or 12.5MB.  So each 80 we keep in memory about 1 gig. 800 would be about 10Gig
				p = new Prime(100000020, 0, 800);


				//size_t is FFFFFFFFFFFFFFFF == 18, 446, 744, 073, 709, 551, 615  on win10-64 intel

			}
			try {
				p->SetBaseName("Prime100000020-");
				p->SetVerbose(true);
				p->SaveIncrementalFiles(true);
				p->FindPrimes((size_t)0xFFFFFFF0);  //largest number we can do with size_t
				std::cout << " Exited FindPrimes\n";
			}
			catch (std::exception & e)
			{
				std::cout << e.what();
			}
			std::cout << "FindPrimes exiting\n";
			std::cout << "Max Prime found is " << p->GetMaxPrime() << std::endl;
			return 0;
		}
		catch (std::exception & ex)
		{
			std::cout << ex.what() << std::endl;
		}
		catch (...)
		{
			std::cout << "Error.  Possible bad .prm file." << std::endl;
		}
	}
	//__except(EXCEPTION_EXECUTE_HANDLER)
	//{
	//	std::cout << "Some Weird Exception\n";
	//}

}
