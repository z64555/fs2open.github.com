/**
 * Originally written by z64555
 * Adapted to use stdlib by m!m
 */

// ToDo: Review wxWidgets API to file input and output
// Correct the code so that the program will properly output text to a target .h file
// Correct the code so that the program will properly read bytes in and output as hexidecimal string values.

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>

// Input: Filename of file to convert
// Function output: standard status codes
// Side-effect output: converted file
enum embedpng_errors
{
	error_none = 0,
	error_cantopenfile,
	error_cantoutputfile,
	error_invalidargs,
	error_ioerror,
};

static const size_t INPUT_BUFFER_SIZE = 1024;

typedef unsigned char ubyte;

int main( int argc, char* argv[] )
{
	if (argc != 4)
	{
		std::cerr << "Usage: embedpng <inout> <output> <fieldname>" << std::endl;

		return error_invalidargs;
	}

	std::string input_file(argv[1]);
	std::string output_file(argv[2]);
	std::string field_name(argv[3]);

	// Create the file_in stream, and verify integrety
	std::ifstream file_in(input_file.c_str(), std::ios::binary);
	if( file_in.bad() )
	{
		std::cerr << "Error opening file: " << input_file << std::endl;
		return error_cantopenfile;
	}

	// Get the size of the input stream
	size_t input_size;
	file_in.seekg(0, std::ios::end);

	if ((int) file_in.tellg() != -1)
	{
		input_size = (size_t) file_in.tellg();
	}
	else
	{
		std::cerr << "Failed to get size of input file: " << input_file << std::endl;
		file_in.close();
		return error_ioerror;
	}

	file_in.seekg(0, std::ios::beg);

	// Before conditioning, use this a the name of the .h
	std::ofstream file_out(output_file.c_str());
	if( file_out.bad() )
	{
		std::cerr << "Error opening file: " << output_file << std::endl;
		file_in.close();
		return error_cantoutputfile;
	}

	// Condition header_name to proper format ( _HEADER_H )
	std::string header_name;
	header_name.reserve(field_name.length() + 3); // Reserve three additional as we need those later
	header_name.assign(field_name);

	std::transform(header_name.begin(), header_name.end(), header_name.begin(), ::toupper); // HEADER (I hate C++)

	header_name.insert(0, 1, '_');	// _HEADER
	header_name.append("_H");	// _HEADER_H

	file_out << "#ifndef " << header_name << std::endl;
	file_out << "#define " << header_name << std::endl;
	file_out << std::endl;

	file_out << "#include <wx/mstream.h>" << std::endl;
	file_out << "#include <wx/image.h>" << std::endl;
	file_out << "#include <wx/bitmap.h>" << std::endl;
	file_out << std::endl;

	file_out << "static const unsigned char " << field_name << "[] = " << std::endl;
	file_out << "{" << std::endl;
	file_out << "\t";

	ubyte buffer[INPUT_BUFFER_SIZE];

	size_t i = 0;
	while(i < input_size)
	{
		size_t current_size = std::min(input_size - i, INPUT_BUFFER_SIZE);
		file_in.read((char*) buffer, current_size);

		for (size_t j = 0; j < current_size; j++)
		{
			file_out << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << int(buffer[j]);
			i++;

			if( (i % 16) == 0 )
			{
				// End of 16-byte row
				file_out << "," << std::endl;
				file_out << "\t";
			}
			else
			{
				// Somewhere within a column
				file_out << ", ";
			}
		}
	}

	file_out << std::endl;

	file_out << "};" << std::endl;
	file_out << std::endl;

	file_out << "wxBitmap& " << field_name << "_to_wx_bitmap()" << std::endl;
	file_out << "{" << std::endl;
	file_out << "\tstatic wxMemoryInputStream memIStream( " << field_name << ", sizeof( " << field_name << " ) );" << std::endl;
	file_out << "\tstatic wxImage image( memIStream, wxBITMAP_TYPE_PNG );" << std::endl;
	file_out << "\tstatic wxBitmap bmp( image );" << std::endl;
	file_out << "\treturn bmp;" << std::endl;
	file_out << "};" << std::endl;

	file_out << std::endl;

	file_out << std::endl;

	file_out << "#endif // " << header_name << std::endl;

	file_in.close();
	file_out.close();

	return error_none;
}

