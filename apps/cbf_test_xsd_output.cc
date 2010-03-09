//! controller.hxx (and controller.cxx) is autogenerated by XSD (see docs)
#include <schemas.hxx>

#include <memory>
#include <stdexcept>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
	if (argc < 2)
		throw std::runtime_error("Error:\n  Missing argument: Please specify an XML document describing a controller.\n  Usage: test_xsd_output <filename>");
	try
	{
		//! Try to create a ControllerType from the passed XML document
		std::auto_ptr<ControllerType> controller (
			Controller(
				argv[1], 
				xml_schema::flags::dont_validate
			)
		);

		std::cout << *controller.get() << std::endl;


#if 0
		std::auto_ptr<ResourceType> resource(
			Resource(
				argv[2], xml_schema::flags::dont_validate
			)
		);
#endif
	}
	catch (const xml_schema::exception& e)
	{
		std::cerr << "Something went wrong instantiating the controller(s): Reason: \n" << e << std::endl;

		//! Rethrow the exception, so a debugger might catch it :)
		throw;
	}
}


