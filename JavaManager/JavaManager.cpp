#include <iostream>
#include <string>
#include <vector>
#include "filemanager.hpp"
#include "config.hpp"
#include "argparse/argparse.hpp"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
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

	if (auto fn = program.present<int>("-jv")) {
		cout << *fn;
	}

	if (auto fn = program.present("-c")) {
		Config config(*fn);
	}
	else{
		Config config();
	}

	system("pause");

	return 0;
}
