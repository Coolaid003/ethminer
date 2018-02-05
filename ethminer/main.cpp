/*
	This file is part of cpp-ethereum.

	cpp-ethereum is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-ethereum is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file main.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 * Ethereum client.
 */

#include <thread>
#include <fstream>
#include <iostream>
#include <signal.h>
#include "MinerAux.h"
#include "BuildInfo.h"

using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace boost::algorithm;

#ifdef _WIN32
void signalHandler(int sig)
{
	dev::con::ResetTerminalColor();
	exit(0);
}
#endif

void help()
{
	cout
		<< "Usage ethminer [OPTIONS]" << endl
		<< "Options:" << endl << endl;
	MinerCLI::streamHelp(cout);
	cout
		<< "General Options:" << endl
		<< "    -v,--verbosity <0 - 9>  Set the log verbosity from 0 to 9 (default: 8)." << endl
		<< "    -V,--version  Show the version and exit." << endl
		<< "    -h,--help  Show this help message and exit." << endl
		;
	exit(0);
}

void version()
{
	cout << "ethminer version " << ETH_PROJECT_VERSION << endl;
	cout << "Build: " << ETH_BUILD_PLATFORM << "/" << ETH_BUILD_TYPE << endl;
	exit(0);
}

int main(int argc, char** argv)
{
#ifdef _WIN32
	// Signal handlers to reset terminal color on exit
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
#endif

	// Set env vars controlling GPU driver behavior.
	setenv("GPU_MAX_HEAP_SIZE", "100");
	setenv("GPU_MAX_ALLOC_PERCENT", "100");
	setenv("GPU_SINGLE_ALLOC_PERCENT", "100");

	MinerCLI m(MinerCLI::OperationMode::Farm);

	try
	{
		for (int i = 1; i < argc; ++i)
		{
			// Mining options:
			if (m.interpretOption(i, argc, argv))
				continue;

			// Standard options:
			string arg = argv[i];
			if ((arg == "-v" || arg == "--verbosity") && i + 1 < argc)
				g_logVerbosity = atoi(argv[++i]);
			else if (arg == "-h" || arg == "--help")
				help();
			else if (arg == "-V" || arg == "--version")
				version();
			else
			{
				cerr << "Invalid argument: " << arg << endl;
				exit(-1);
			}
		}
	}
	catch (BadArgument ex)
	{
		std::cerr << "Error: " << ex.what() << "\n";
		exit(-1);
	}

	try
	{
		m.execute();
	}
	catch (std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << "\n";
		return 1;
	}

	return 0;
}
