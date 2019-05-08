/*
	NAME: Misael Hinojosa
	STUDENT ID: 0134-86600
	COMPLETION TIME: 6 hours
	COLLABS: None
*/

#include <iostream>
#include <fstream>
#include "StringSplitter.h"
#include <bitset>

using namespace std;

const vector<vector<int>> ENCODINGMATRIX{
	{1, 1, 0, 1},
	{1, 0, 1, 1},
	{1, 0, 0, 0},
	{0, 1, 1, 1},
	{0, 1, 0, 0},
	{0, 0, 1, 0},
	{0, 0, 0, 1}
};

const vector<vector<int>> PARITYMATRTIX{
	{1, 0, 1, 0, 1, 0, 1},
	{0, 1, 1, 0, 0, 1, 1},
	{0, 0, 0, 1, 1, 1, 1}
};

const vector<vector<int>> DECODINGMATRIX{
	{0, 0, 1, 0, 0, 0, 0},
	{0, 0, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 1}
};

vector<int> encodingMultiply(const vector<int>& bit)
{
	// Multiply out the vectors.
	vector<int> encoded{};

	for (auto row : ENCODINGMATRIX)
	{
		int total = 0;
		for (int i = 0; i < bit.size(); i++)
		{
			int temp = row[i] * bit[i];
			total += temp;
		}

		// Mod 2 to keep the bits.
		encoded.push_back(total % 2);
	}

	return encoded;
}

char encodingChar(const short& ch)
{
	// Convert the short into its binary representation.
	vector<int> bits{};
	string bit = bitset<4>(ch).to_string();

	for (int i = 0; i < bit.length(); i++)
	{
		if (bit[i] == '0')
		{
			bits.push_back(0);
		}
		else
		{
			bits.push_back(1);
		}
	}

	// Returns encoded representation.
	bits = encodingMultiply(bits);

	// Get the string representation of the encoded
	// char.
	// Place a 0 at the beginning of the string to
	// keep the byte.
	string binary = "0";
	for (int i = 0; i < bits.size(); i++)
	{
		binary = binary + to_string(bits[i]);
	}

	return stoi(binary, nullptr, 2);
}

void encodeFile(string file_name)
{
	vector<string> text{};
	ifstream input_file{ file_name };
	if (input_file.is_open() == true)
	{
		while (input_file.good() == true)
		{
			string line;
			getline(input_file, line);
			text.push_back(line);
		}
	}
	input_file.close();

	ofstream output{ file_name + ".coded" };
	for (auto line : text)
	{
		for (auto ch : line)
		{
			// Grab the high and low bits of the char.
			short high = (0b1111) & (ch >> 4);
			short low = (0b1111) & ch;

			// Convert the high and low bits into its
			// encoded char.
			char converted_high = encodingChar(high);
			char converted_low = encodingChar(low);

			// Save to file.
			output << converted_high << converted_low;
		}

		// Adds new line to the file.
		char new_line = 10;
		output << new_line;
	}
	output.close();

	return;
}

void parityCheck(vector<int>& bits)
{
	// error_position keeps track of the where
	// the possible error might be.
	int error_position = 0;
	int multiplier = 1;

	for (auto row : PARITYMATRTIX)
	{
		// Multiply the respected row and column.
		int total = 0;
		for (int i = 0; i < bits.size(); i++)
		{
			int temp = row[i] * bits[i];
			total += temp;
		}

		// If total = 0, then the error_position
		// won't be affected.
		// Else, multipy the parity_bit by the multiplier
		// and 2 * multiplier to properly keep muliplying
		// the parity_bit.
		short parity_bit = total % 2;
		error_position += parity_bit * multiplier;
		multiplier *= 2;
	}

	// If error was detected, ask user if a fix should
	// be attempted.
	if (error_position != 0)
	{
		string response;

		cout << "There was an error detected." << endl;
		cout << "Would you like to fix the error(y/n)?: ";
		getline(cin, response);

		if (response == "Y" || response == "y")
		{
			// Minus one to get the proper bit position
			// that will be flipped.
			if (bits[error_position - 1] == 1)
			{
				bits[error_position - 1] = 0;
			}
			else
			{
				bits[error_position - 1] = 1;
			}
		}
	}
}

vector<int> decodingMultiply(const vector<int>& bit)
{
	// Multiply vectors.
	vector<int> decoded{};

	for (auto row : DECODINGMATRIX)
	{
		int total = 0;
		for (int i = 0; i < bit.size(); i++)
		{
			int temp = row[i] * bit[i];
			total += temp;
		}
		decoded.push_back(total % 2);
	}

	return decoded;
}

char decodingChar(const short& ch)
{
	// String representation of the short in binary.
	vector<int> bits{};
	string bit = bitset<7>(ch).to_string();

	for (int i = 0; i < bit.length(); i++)
	{
		if (bit[i] == '0')
		{
			bits.push_back(0);
		}
		else
		{
			bits.push_back(1);
		}
	}

	// Check if bits are correct.
	parityCheck(bits);

	// Decode the bits.
	bits = decodingMultiply(bits);

	// Get string representation of bits.
	string binary = "";
	for (int i = 0; i < bits.size(); i++)
	{
		binary = binary + to_string(bits[i]);
	}

	return stoi(binary, nullptr, 2);
}

void decodeFile(string file_name)
{
	vector<string> text{};

	ifstream input_file{ file_name };
	if (input_file.is_open() == true)
	{
		while (input_file.good() == true)
		{
			string line;
			getline(input_file, line);
			text.push_back(line);
		}
	}
	input_file.close();

	file_name = file_name.substr(0, file_name.size() - 10);

	// counter wll be used to ensure an extra empty line
	// will not be outputted to the file.
	int counter = text.size();
	ofstream output{ file_name + ".decoded.txt" };
	for (auto line : text)
	{
		// Every two indices make up the original letter.
		// For some reason, the loop would execute when there
		// was an empty string.
		// If I left line.length() - 1 in the for loop,
		// it would execute even though 0 < -1 should never
		// execute. Using a variable solved that problem.
		int length = line.length() - 1;
		for (int i = 0; i < length; i += 2)
		{
			// i represents the high bits and i + 1
			// represents the low bits.
			short high = (0b1111111) & (line[i]);
			short low = (0b1111111) & (line[i + 1]);

			// Pass each to get the decoded bits.
			char upper_bits = decodingChar(high);
			char lower_bits = decodingChar(low);

			// Create the original (possible error) char.
			char converted = '\0';
			converted = upper_bits << 4;
			converted = converted | lower_bits;

			// Save to file.
			output << converted;
		}

		// Subtract counter.
		counter--;

		// Add newline while counter is greater than 0.
		if (counter > 0)
		{
			output << '\n';
		}
	}
	output.close();
}

int main(int argc, char* argv[])
{
   if (argc != 3)
   {
      cout << "Expected format: PA6.exe <encode / decode> <file_name>" << endl;
      return 0;
   }

   if (string{ argv[1] } == "encode")
   {
      //encode ABC.txt to ABC.txt.coded
      encodeFile(string{ argv[2] });

      cout << "File encoded." << endl;
   }
   else if (string{ argv[1] } == "decode")
   {
      //decode ABC.txt.coded to ABC.decoded.txt
      decodeFile(string{ argv[2] });
      cout << "File decoded." << endl;
   }
   else
   {
      cout << "Unexpected command." << endl;
   }

}