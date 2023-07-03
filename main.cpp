// main function

// Core includes
#include <iostream>
#include <Windows.h>
#include <memory>

// WT Lib
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>

// import of dbase module containing dbase class
import dbase;

int main(int argc, char * argv[])
{
	// SETUP

	setlocale(LC_ALL, "ru_RU.UTF-8");

	// VARS

	std::string connectionString;
	std::unique_ptr<jinx::DBASE> base;	

	std::string dbname;
	std::string dbuser;
	std::string dbpasswd;
	bool cmdArgs = false;
	bool isInitSuccess = false;

	// BODY

	std::cout << " ///////////////// DATABASE CONNECTION ///////////////// ";

	std::cout << "\n> Lauched as: ";

	// Вызов должен быть DBaseWork.exe -db <DB NAME> -u <USER NAME> -p <PWD>, порядок аргументов важен

	for (int i = 0; i < argc; ++i)
	{
		std::cout << argv[i] << ' ';
	}

	std::cout << "\n\n";

	if (argc == 7)
	{
		// checks ------------

		bool check = false;

		if (strcmp(argv[1], "-db") == 0)
		{
			check = true;
		}

		cmdArgs = check;
		check = false;

		if (strcmp(argv[3], "-u") == 0)
		{
			check = true;
		}

		cmdArgs = check;
		check = false;

		if (strcmp(argv[5], "-p") == 0)
		{
			check = true;
		}

		cmdArgs = check;
		check = false;

		// !checks ---------

		if (cmdArgs)
		{

			dbname = argv[2];
			dbuser = argv[4];
			dbpasswd = argv[6];

		}
	}
	else
	{
		cmdArgs = false;
	}

	while (!isInitSuccess)
	{
		std::cout << "\n\n";

		// Manual input if not cmd args (or invalid args)
		if (!cmdArgs)
		{
			std::cout << "> DB NAME: ";
			std::cin >> dbname;
			std::cout << "> USER: ";
			std::cin >> dbuser;
			std::cout << "> PASSWORD: ";
			std::cin >> dbpasswd;
		}

		std::cout << "\n> Connecting to database: " << dbname << " as user: " << dbuser << '\n';

		connectionString = 
			"host=127.0.0.1 " 
			"port=5432 " 
			"dbname=" + dbname + " " 
			"user=" + dbuser + " " 
			"password=" + dbpasswd;

	try
	{
		base = std::make_unique<jinx::DBASE>(connectionString);

		/*
		"Alligator Pubs"
		"REX Pages"
		"Wolf Industries"
		*/

		std::cout << "\nLookup publishers: \n\n";

		base->find_shops("Alligator Pubs");

		base->find_shops("REX Pages");

		base->find_shops("Wolf Industries");

		std::cout << std::endl;

		// EXIT

		isInitSuccess = true;
	}
	catch (const Wt::Dbo::Exception& e)
	{
		std::cout << "> DBO: Error connecting to database!\nError: " << e.what() << std::endl;
		isInitSuccess = false;
		cmdArgs = false;
	}

	} // while (!isInitSuccess)	

	// EXIT
	std::cout << std::endl;
	system("pause");
	return EXIT_SUCCESS;
}