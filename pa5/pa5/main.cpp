/*
	NAME: Misael Hinojosa
	STUDENT ID: 0134-86600
	COMPLETION TIME: 3 hours
	COLLABS: None
*/
#include <iostream>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <queue>
#include "StringSplitter.h"
#include "Word.h"

string autoCorrect(const string& word, const vector<string>& dictionary, const string& context_line);
string autoCorrectWithPQ(const string& word, const vector<string>& dictionary, const string& context_line);

int calculateEditDistance(
	const string& first,
	const string& second,
	int first_index,
	int second_index,
	vector<vector<int>>& mem)
{
	int cost = 0;

	//ensure index is in bounds
	if (first_index >= first.length())
	{
		//at is point, we can no longer transform and delete doesn't 
		//make sense because we're smaller
		return second.length() - second_index;
	}
	else if (second_index >= second.length())
	{
		return first.length() - first_index;
	}

	//is there currently alignment at the indices?
	if (first[first_index] == second[second_index])
	{
		return calculateEditDistance(
			first,
			second,
			first_index + 1,
			second_index + 1,
			mem);
	}
	else
	{
		//before we make recursive calls, check mem
		//value greater than -1 means that we've calculated this before
		int insert_cost = mem[first_index][second_index + 1];
		int delete_cost = mem[first_index + 1][second_index];
		int transform_cost = mem[first_index + 1][second_index + 1];

		//recursive calls must be made if we have bad memory
		if (insert_cost == -1)
		{
			insert_cost = calculateEditDistance(
				first,
				second,
				first_index,
				second_index + 1,
				mem);
		}
		if (delete_cost == -1)
		{
			delete_cost = calculateEditDistance(
				first,
				second,
				first_index + 1,
				second_index,
				mem);
		}
		if (transform_cost == -1)
		{
			transform_cost = calculateEditDistance(
				first,
				second,
				first_index + 1,
				second_index + 1,
				mem);
		}
		cost = 1 + min(min(insert_cost, delete_cost), transform_cost);
		mem[first_index][second_index] = cost;
		return cost;
	}
	return -1;
}

int calculateEditDistance(
	const string& first,
	const string& second,
	int first_index = 0,
	int second_index = 0)
{
	vector<vector<int>> mem{};
	mem.resize(first.length() + 1);
	for (int i = 0; i < mem.size(); i++)
	{
		mem[i].resize(second.length() + 1);
		for (int j = 0; j < second.length() + 1; j++)
		{
			mem[i][j] = -1;
		}
	}
	int result = calculateEditDistance(first, second, first_index, second_index, mem);
	return result;
}

int calculateEditDistanceBU(const string& first, const string& second)
{
	vector<vector<int>> matrix{};

	//build result matrix
	matrix.resize(first.length() + 1);
	for (auto& row : matrix)
	{
		row.resize(second.length() + 1);
	}

	//fill in first row  
	for (int i = 0; i < matrix[0].size(); i++)
	{
		matrix[0][i] = i;
	}

	//fill in first column
	for (int i = 0; i < matrix.size(); i++)
	{
		matrix[i][0] = i;
	}

	//compute rest of matrix
	for (int i = 1; i < matrix.size(); i++)
	{
		for (int j = 1; j < matrix[i].size(); j++)
		{
			//find least cost of our 3 choices
			int top_cost = matrix[i - 1][j] + 1;
			int left_cost = matrix[i][j - 1] + 1;
			int diagonal_cost = matrix[i - 1][j - 1];

			//add 1 if characters are not the same
			if (first[i - 1] != second[j - 1])
			{
				diagonal_cost++;
			}

			int best_choice = min(top_cost, min(left_cost, diagonal_cost));

			//store result in current cell
			matrix[i][j] = best_choice;
		}
	}

	return matrix[matrix.size() - 1][matrix[0].size() - 1];
}

int main(void)
{
	vector<string> dictionary{};
	vector<string> text{};
	string text_file, destination_file;

	// Grab dictionary.
	ifstream input_file{ "words.txt" };
	if (input_file.is_open() == true)
	{
		while (input_file.good() == true)
		{
			string line;
			getline(input_file, line);
			if (line.length() > 0)
			{
				dictionary.push_back(line);
			}
		}
	}
	input_file.close();

	cout << "Enter input file: ";
	getline(cin, text_file);
	cout << "Enter desination file: ";
	getline(cin, destination_file);

	// Grab file to correct.
	input_file.open(text_file);
	if (input_file.is_open() == true)
	{
		while (input_file.good() == true)
		{
			string line;
			getline(input_file, line);
			if (line.length() > 0)
			{
				text.push_back(line);
			}
		}
	}
	input_file.close();

	ofstream output{ destination_file };
	for (auto line : text)
	{
		// For every line in the text, split into single words.
		vector<string> temp = StringSplitter::split(line, " ");

		// For each word, check if the word is spelled correctly.
		for (auto word : temp)
		{
			string only_word;
			char punctuation;
			bool isChanged = false;

			// Check for punctuation at the end.
			if (word[word.length() - 1] == '!' || word[word.length() - 1] == '?'
				|| word[word.length() - 1] == '.' || word[word.length() - 1] == ',')
			{
				only_word = word.substr(0, word.length() - 1);
				punctuation = word[word.length() - 1];
				isChanged = true;
			}

			if (isChanged == true)
			{
				//string temp = autoCorrect(only_word, dictionary, line);
				string temp = autoCorrectWithPQ(only_word, dictionary, line);
				output << temp + punctuation << " ";
			}
			else
			{
				//output << autoCorrect(word, dictionary, line) << " ";
				output << autoCorrectWithPQ(word, dictionary, line) << " ";
			}
		}
		output << "\n";
	}
	output.close();

	return 0;
}

string autoCorrect(const string& word, const vector<string>& dictionary, const string& context_line)
{
	// Check if the word is in the dictionary.
	if (find(dictionary.begin(), dictionary.end(), word) != dictionary.end())
	{
		return word;
	}
	else
	{
		// Word is not in the dictionary.
		vector<string> words{};
		unordered_map<string, int> table{};
		bool hasFile = false;

		// Check if the misspelled word has files.
		ifstream word_file{ word + ".dat" };
		if (word_file.is_open() == true)
		{
			// Set to true
			hasFile = true;

			// Place the wards into a vector.
			while (word_file.good() == true)
			{
				string line;
				getline(word_file, line);

				if (line.length() > 0)
				{
					vector<string> temp = StringSplitter::split(line, " ");
					words.push_back(temp[0]);
				}
			}
			word_file.close();
		}
		else
		{
			// Word has no file
			// Calculate edit distance of every word.
			for (auto w : dictionary)
			{
				table[w] = calculateEditDistanceBU(word, w);
			}

			// Find the least costly edit distance.
			int counter = 1;
			while (words.size() < 10)
			{
				for (auto kvp : table)
				{
					if (kvp.second == counter)
					{
						words.push_back(kvp.first);
					}

					if (words.size() > 9)
					{
						break;
					}
				}
				counter++;
			}
		}

		int response;
		int counter = 2;
		cout << "Unknown Word: " << word << endl;
		cout << "\tConext: " << context_line << endl;
		cout << "Corrected word: " << endl;
		cout << "1. None of the words below are correct" << endl;

		// Print the words.
		for (auto w : words)
		{
			cout << counter << ". " << w << endl;
			counter++;
		}

		// Recieve user input.
		do
		{
			cout << "Enter selection: ";
			cin >> response;
			cin.ignore();
		} while (response > 12);

		string correct_word;
		ofstream output{};

		if (response == 1)
		{
			// None of the words are correct.
			// Record correct word from user and save the
			// it to a file.
			correct_word;
			cout << "Enter the correct word: ";
			getline(cin, correct_word);
			
			output.open(word + ".dat");
			output << correct_word << " 0" << "\n";
			for (auto w : words)
			{
				output << w << " " << table[w] << "\n";
			}
		}
		else
		{
			// Grab the correct word and if the word doesn't
			// have a file already, save it.
			correct_word = words[response - 2];

			if (hasFile == false)
			{
				output.open(word + ".dat");
				output << correct_word << " 0" << "\n";
				for (auto w : words)
				{
					if (w != correct_word)
					{
						output << w << " " << table[w] << "\n";
					}
				}
			}
		}
		output.close();

		return correct_word;
	}
}

string autoCorrectWithPQ(const string& word, const vector<string>& dictionary, const string& context_line)
{
	// Check dictionary.
	if (find(dictionary.begin(), dictionary.end(), word) != dictionary.end())
	{
		return word;
	}
	else
	{
		// Use a PQ for this solution.
		priority_queue<Word, vector<Word>, greater<Word>> words{};
		vector<Word> top_words{};

		// Check if word has a saved file.
		ifstream word_file{ word + ".dat" };
		if (word_file.is_open() == true)
		{
			// For each word, create Word object and push into PQ.
			while (word_file.good() == true)
			{
				string line;
				getline(word_file, line);
				if (line.length() > 0)
				{
					vector<string> temp = StringSplitter::split(line, " ");
					words.push(Word(temp[0], stoi(temp[1])));
				}
			}
			word_file.close();
		}
		else
		{
			// Find distance for each word in dictionary relative to
			// the misspelled word.
			// Push into PQ.
			for (auto w : dictionary)
			{
				int distance = calculateEditDistanceBU(word, w);
				words.push(Word(w, distance));
			}
		}

		int response;
		int counter = 2;
		cout << "Unknown Word: " << word << endl;
		cout << "\tConext: " << context_line << endl;
		cout << "Corrected word: " << endl;
		cout << "1. None of the words below are correct" << endl;

		// Print out the closest words, push them into a vector,
		// and then pop the PQ.
		for (int i = 0; i < 10; i++)
		{
			cout << i + 2 << ". " << words.top().word << endl;
			top_words.push_back(words.top());
			words.pop();
		}

		do
		{
			cout << "Enter selection: ";
			cin >> response;
			cin.ignore();
		} while (response > 12);

		string correct_word;
		ofstream output{};
		if (response == 1)
		{
			// Get user input and save to file.
			cout << "Enter the correct word: ";
			getline(cin, correct_word);

			output.open(word + ".dat");
			output << correct_word << " 0" << "\n";
			for (auto w : top_words)
			{
				output << w.word << " " << w.weight << "\n";
			}
		}
		else
		{
			// Get the correct word from vector, then save
			// to file.
			correct_word = top_words[response - 2].word;

			output.open(word + ".dat");
			output << correct_word << " 0\n";

			for (auto w : top_words)
			{
				if (correct_word != w.word)
				{
					output << w.word << " " << w.weight << "\n";
				}
			}
		}
		output.close();

		return correct_word;
	}
}