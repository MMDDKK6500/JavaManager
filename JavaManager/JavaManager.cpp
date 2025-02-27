#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "filemanager.hpp"
#include "config.hpp"
#include "argparse/argparse.hpp"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {

	fs::path javaPath;

	std::unique_ptr<Config> config;

	argparse::ArgumentParser program("JavaManager");

	program.add_argument("-jv", "--java-version")
		.help("Which java version should be used (if not passed will use latest installed)")
		.scan<'i', int>();
	
	program.add_argument("-c", "--config")
		.help("Where the config file is (if not passed will use default path)");

	program.add_argument("javaArgs")
		.remaining();

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		return 1;
	}

	if (auto fn = program.present("-c")) {
		config = std::make_unique<Config>(*fn);
	}
	else {
		config = std::make_unique<Config>();
	}

	if (auto fn = program.present<int>("-jv")) {
		//check if provided java version is installed
		if (config->versionsInstalled.find(*fn) == config->versionsInstalled.end()) {
			std::cerr << "Java version " << *fn << " is not installed" << std::endl;
			return 1;
		}
	}

	try {
		auto javaArgs = program.get<std::vector<std::string>>("javaArgs");
		std::cout << javaArgs.size() << " java arguments provided" << std::endl;
		for (auto i = 0; i < javaArgs.size(); i++) {
			if (i == 0) {
				//check if first argument is a valid java executable from the bin folder of the java version
				if (auto fn = program.present<int>("-jv")) {
					javaPath = config->versionsInstalled[*fn];
				}
				else {
					javaPath = config->versionsInstalled[config->defaultVersion];
				}
				

				//TODO: cross-platform this
				javaPath += "\\bin\\" + javaArgs[i] + ".exe";

				//Check if the path exists
				//cout << fs::current_path() << "\n";
				if (!fs::exists(javaPath)) {
					std::cerr << "Invalid java executable provided" << std::endl;
					return 1;
				}

			}
		}

		
		//use CreateProcessW to execute javaPath with javaArgs
		std::cout << "Executing " << javaPath << std::endl;
		
		//prepare args
		std:string javaExe = javaArgs[0] + ".exe";
		std::wstring widestr = std::wstring(javaExe.begin(), javaExe.end());
		const wchar_t* command = widestr.c_str();
		//TODO: Implement the args

		//
		const wchar_t* workingDir = fs::current_path().c_str();


		STARTUPINFO sia = { 0 };
		sia.cb = sizeof(sia);
		PROCESS_INFORMATION pi = { 0 };

		// Create the child process
		BOOL success = CreateProcessW(
			javaPath.c_str(),  // Path to executable
			const_cast<wchar_t*>(command),          // Command line arguments
			NULL,                                   // Process attributes
			NULL,                                   // Thread attributes
			FALSE,                                  // Inherit handles
			0,                                      // Creation flags
			NULL,                                   // Environment
			workingDir,                             // Working directory
			&sia,                                   // Startup info
			&pi);                                   // Process information

		if (success)
		{
			// Wait for the process to exit
			WaitForSingleObject(pi.hProcess, INFINITE);

			// Process has exited - check its exit code
			DWORD exitCode;
			GetExitCodeProcess(pi.hProcess, &exitCode);

			// At this point exitCode is set to the process' exit code

			// Handles must be closed when they are no longer needed
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		else
		{
			// CreateProcess failed
			DWORD error = GetLastError();
			std::cerr << "CreateProcess failed with error " << error << std::endl;
		}
	}
	catch (std::logic_error& e) {
		std::cout << "No java arguments provided" << std::endl;
	}

	return 0;
}
