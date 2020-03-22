// go.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "Prime.h"

int main()
{
	Prime* p = new Prime(0xFFFFFFF0);
	//Prime* p = new Prime(600000000);
	std::cout << "Reading existing results (if they exist...\n";
	try
	{
		p->SetVerbose(true);
		p->ContinousRun("RunRunRun");
	}
	catch(std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Error.  Possible bad .prm file." << std::endl;
	}

}
