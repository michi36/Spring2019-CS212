#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "StringSplitter.h"
#include <bitset>
using namespace std;

const vector<vector<int>> CGM{
	{1, 1, 0, 1},
	{1, 0, 1, 1},
	{1, 0, 0, 0},
	{0, 1, 1, 1},
	{0, 1, 0, 0},
	{0, 0, 1, 0},
	{0, 0, 0, 1}
};

const vector<vector<int>> PCH{
	{1, 0, 1, 0, 1, 0, 1},
	{0, 1, 1, 0, 0, 1, 1},
	{0, 0, 0, 1, 1, 1, 1}
};

vector<int> decodingMultiply(const vector<int>& bit)
{
	vector<int> decoded{};

	for (auto row : PCH)
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

vector<int> encodingMultiply(const vector<int>& bit)
{
	vector<int> encoded{};

	for (auto row : CGM)
	{
		int total = 0;
		for (int i = 0; i < bit.size(); i++)
		{
			int temp = row[i] * bit[i];
			total += temp;
		}
		encoded.push_back(total % 2);
	}

	return encoded;
}

char convertCharEncode(const short& ch)
{
	vector<int> bits{};
	string bit = bitset<4>(ch).to_string();

	for (int i = 0; i < 4; i++)
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

	bits = encodingMultiply(bits);

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

	ofstream output{ "sample1.txt.coded" };
	for (auto line : text)
	{
		for (auto ch : line)
		{
			short low = (0b1111) & ch;
			short high = (0b1111) & (ch >> 4);

			char converted_high = convertCharEncode(high);
			char converted_low = convertCharEncode(low);

			output << converted_high << converted_low;
		}

		char new_line = 10;
		output << new_line;
	}
	output.close();

	return;
}

vector<int> convertCharDecode(const string& binary)
{
	vector<int> bits{};

	for (int i = 0; i < binary.length(); i++)
	{
		if (binary[i] == '0')
		{
			bits.push_back(0);
		}
		else
		{
			bits.push_back(1);
		}
	}

	bits = decodingMultiply(bits);

	return bits;
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

	ofstream output{ "sample.decoded.txt" };
	for (auto line : text)
	{
		for (int i = 0; i < line.length(); i += 2)
		{
			string binary = bitset<7>(line[i]).to_string();
			short low = (0b1111) & line[i];
			short high = (0b1111) & (line[i] >> 4);

			convertCharDecode(binary);

			char converted_high = convertCharEncode(high);
			char converted_low = convertCharEncode(low);

			cout << "Hello";
		}

		output << endl;
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