#include <iostream>
#include <boost/program_options.hpp>

#ifndef VERSIONNUMBER
#define VERSIONNUMBER "0.1.0"
#endif

int main(int argc, const char* argv[]) {
	boost::program_options::options_description desc("Options");
	desc.add_options()
	("version", "Print version information and quit")
	("help,h", "Print basic usage information to stdout and quit")
	("somestring", boost::program_options::value<std::string>(), "A string to print")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 0;
	}
	if (vm.count("version")) {
		std::cout << "sago_multi_scrambler_puzzle2 " << VERSIONNUMBER << "\n";
		return 0;
	}
	if (vm.count("somestring")) {
		std::string somestring = vm["somestring"].as<std::string>();
		std::cout << "Called with a parameter value of: " << somestring << "\n";
	}
	else {
		std::cout << "This program demonstrates boost::program_options. Try \"" << argv[0] << " --help\" or \"" << argv[0] << " --somestring hello\"\n";
	}
	return 0;
}
