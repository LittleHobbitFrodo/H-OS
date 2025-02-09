//
//	utilities/build.cpp
//		part of the build system belonging to the H-OS project
//

#include <iostream>
#include <fstream>
#include "./lib/mdc/mdc.hpp"


int main(int argc, const char* const * const argv) {

	mdc::section sec = mdc::section("section");

	{
		mdc::section &sub = sec.add("subsection");
		std::cout << "subsection created" << std::endl;

		sub.add<int>("value", 69);

		std::cout << "value created" << std::endl;
	}

	std::cout << "section \'" << sec.name() << "\":\t";
	try {
		const mdc::section &sub = sec.at<mdc::section>("subsection");

		std::cout << "section retrieved!" << std::endl;

		try {
			const mdc::value &val = sub.at<mdc::value>("value");

			std::cout << val << std::endl;
		} catch (...) {
			std::cout << "could not retrieve value" << std::endl;
		}



	} catch (mdc::exception) {
		std::cout << "mdc::exception" << std::endl;
		exit(1);
	} catch (std::exception) {
		std:: cout << "std::exception" << std::endl;
		exit(2);
	}





}