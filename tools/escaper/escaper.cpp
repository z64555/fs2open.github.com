
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

static char escapeCharacters[] = {'\\','"','\n'};
static const size_t numEscapeCharacters = sizeof(escapeCharacters) / sizeof(char);

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		std::cerr << "Usage: escaper <input> <output> <fieldname>";

		return -1;
	}

	std::ifstream inputStream(argv[1]);

	if (inputStream.bad())
	{
		std::cerr << "Failed to open file to read: " << argv[1];

		return -1;
	}

	std::string str;

	inputStream.seekg(0, std::ios::end);
	str.reserve((size_t) inputStream.tellg());
	inputStream.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(inputStream)),
				std::istreambuf_iterator<char>());

	inputStream.close();

	std::ofstream outputStream(argv[2]);

	if (outputStream.bad())
	{
		std::cerr << "Failed to open file to write: " << argv[2];

		return -1;
	}

	outputStream << "char* " << argv[3] << " = \"";

	size_t pos = 0;

	while (pos < str.length())
	{
		size_t found_pos = std::string::npos;

		for(int i = 0; i < numEscapeCharacters; i++)
		{
			found_pos = std::min(found_pos, str.find(escapeCharacters[i], pos));
		}

		if (found_pos != std::string::npos)
		{
			size_t delta = found_pos - pos;

			if (delta != 0)
			{
				outputStream.write(str.c_str() + pos, delta);
			}
				
			switch(str[found_pos])
			{
			case '\\':
				outputStream << "\\\\";
				break;
			case '\n':
				outputStream << "\\n\"" << std::endl << "\"";
				break;
			case '"':
				outputStream << "\\\"";
				break;
			default:
				std::cerr << "Invalid character encountered!" << std::endl;
			}

			pos = found_pos + 1;
		}
		else
		{
			outputStream.write(str.c_str() + pos, str.length() - pos);

			break;
		}
	}

	outputStream << "\";";

	outputStream.close();

	return 0;
}
