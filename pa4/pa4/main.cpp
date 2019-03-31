#include "CsvParser.h"
#include "TreeNode.h"
#include "StringSplitter.h"
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>

TreeNode* buildTreeFromFile();
void writeTreeToFile(TreeNode* tree);
void writeTreeToFileHelper(ofstream& output, TreeNode* node, string edge_name);
void predictOutcome(TreeNode* tree);
void predictOutcomeHelper(TreeNode* tree, const vector<string>& row, const vector<string>& header, string& predicted_outcome);
TreeNode* readTreeFromFile();
void readTreeFromFileHelper(TreeNode* node, const vector<string>& tree_vector, int& next_child_index, const int& children);

double calculateEntrophy(const unordered_map<string, int>& outcome_level);
unordered_map<string, int> buildFrequencyDistribution(const vector<string>& data);
vector<string> getObservations(const vector<vector<string>>& matrix, int column);
vector<vector<string>> reduceMatrix(const vector<vector<string>>& matrix, int column, string predictor);
int findMaxGain(const vector<vector<string>>& matrix, int outcome_column, double entrophy);
TreeNode* buildTree(const vector<vector<string>>& matrix, const vector<string>& predictors, const int& outcome_column);

int main(void)
{
	TreeNode* tree = nullptr;
	int response;
	
	while (true)
	{
		cout << "*** MENU ***" << endl;
		cout << "1. Build Decision Tree From File\n";
		cout << "2. Write Tree To File\n";
		cout << "3. Predict Outcome\n";
		cout << "4. Read Tree From File\n";

		cin >> response;
		cin.ignore();

		switch (response)
		{
		case 1: tree = buildTreeFromFile();
			break;
		case 2: writeTreeToFile(tree);
			break;
		case 3: predictOutcome(tree);
			break;
		case 4: tree = readTreeFromFile();
			break;
		default: cout << "Invalid input\n";
		}
	}
	
	return 0;
}

TreeNode* buildTreeFromFile()
{
	string input;
	vector<string> header;
	vector<vector<string>> data{};
	vector<vector<string>> data2{};
	int outcome_variable_index = 0;

	cout << "Enter CSV file name: ";
	getline(cin, input);

	CsvStateMachine parser{ input };
	data = parser.processFile();
	header = data[0];

	cout << "What is the outcome variable?: ";
	getline(cin, input);

	// Look for the outcome variable index.
	for (auto column : header)
	{
		if (column == input)
		{
			break;
		}
		outcome_variable_index++;
	}

	// Move from index 1 to end to new vector of vectors.
	move(data.begin() + 1, data.end(), back_inserter(data2));

	// Build the tree.
	return buildTree(data2, header, outcome_variable_index);
}

void writeTreeToFile(TreeNode* tree)
{
	// Check if tree is null.
	if (tree == nullptr)
	{
		cout << "Tree does not exist" << endl;
		return;
	}

	// Write the root to file.
	ofstream output{ "tree_file.txt" };
	output << "NULL" << "|";
	output << tree->value << "|";
	output << tree->children.size();
	output << endl;

	// First, write all the root's children to
	// the file.
	for (auto child : tree->children)
	{
		output << child.first << "|";
		output << child.second->value << "|";
		output << child.second->children.size();
		output << endl;
	}

	// After writing all the children, pass all
	// the children.
	for (auto child : tree->children)
	{
		if (child.second != nullptr)
		{
			writeTreeToFileHelper(output, child.second, child.first);
		}
	}
	output.close();

	cout << "Tree written to tree_file.txt" << endl;
}

void writeTreeToFileHelper(ofstream& output, TreeNode* node, string edge_name)
{
	// Same as before, write all children
	for (auto child : node->children)
	{
		output << child.first << "|";
		output << child.second->value << "|";
		output << child.second->children.size();
		output << endl;
	}

	// Pass all children
	for (auto child : node->children)
	{
		if (child.second != nullptr)
		{
			writeTreeToFileHelper(output, child.second, child.first);
		}
	}
}

void predictOutcome(TreeNode* tree)
{
	string file_name, outcome_variable;
	vector<string> header;
	vector<vector<string>> data{};
	vector<vector<string>> data2{};
	int outcome_column = 0;

	cout << "Enter CSV file to open: ";
	getline(cin, file_name);

	//CsvStateMachine parser{ file_name };
	//CsvStateMachine parser{ "easy data set.csv" };
	CsvStateMachine parser{ "loan stats with single outcomes_80.csv" };
	data = parser.processFile();
	header = data[0];

	cout << "Enter the outcome variable: ";
	getline(cin, outcome_variable);

	move(data.begin() + 1, data.end(), back_inserter(data2));

	string predicted_outcome = "";
	vector<string> predicted{};
	for (auto row : data2)
	{
		predictOutcomeHelper(tree, row, header, predicted_outcome);
		predicted.push_back(predicted_outcome);
		predicted_outcome.clear();
	}

	ofstream output_file{ "stats.csv" };
	for (auto column : header)
	{
		if (column.length() > 0)
		{
			output_file << column << ",";
		}
	}
	output_file << "Prediction" << endl;

	int row_index = 0;
	for (auto row : data2)
	{
		for (auto variable : row)
		{
			if (variable.length() > 0)
			{
				output_file << variable << ",";
			}
		}
		output_file << predicted[row_index];
		output_file << endl;
		row_index++;
	}

	output_file.close();
	cout << endl;
}

void predictOutcomeHelper(TreeNode* node, const vector<string>& row, const vector<string>& header, string& predicted_outcome)
{
	if (node == nullptr)
	{
		return;
	}

	if (node->children.size() == 0)
	{
		predicted_outcome = node->value;
		return;
	}

	int column_index = 0;
	for (auto column : header)
	{
		if (column == node->value)
		{
			break;
		}
		column_index++;
	}


	string variable_value = row[column_index];

	TreeNode* next = new TreeNode{};
	next = node->children[variable_value];
	predictOutcomeHelper(next, row, header, predicted_outcome);
}

TreeNode* readTreeFromFile()
{
	vector<string> tree_vector;
	TreeNode* root = new TreeNode{};
	ifstream input_file{ "tree_file.txt" };

	if (input_file.is_open() == true)
	{
		while (input_file.good() == true)
		{
			string line;
			getline(input_file, line);
			if (line.length() > 0)
			{
				vector<string> temp = StringSplitter::split(line, "|");
				tree_vector.push_back(temp[0]);
				tree_vector.push_back(temp[1]);
				tree_vector.push_back(temp[2]);
			}
		}
	}

	// Maybe I over complicated this but it's the best I got.
	// Get all the information from the first line of the data.
	// current_child_index will keep track what child we are
	// on during the parent node cycle.
	// next_child_index keeps track of the index of the first
	// child of next parent.
	// Since the vector is setup that three pieces of information
	// belong to the same node, increment by 3's.
	root->value = tree_vector[1];
	int num_of_children = stoi(tree_vector[2]);
	int current_child_index = 3;
	int next_child_index = num_of_children * current_child_index + 3;

	for (int i = 0; i < num_of_children; i++)
	{
		// Create new node and attach to the root node.
		TreeNode* child = new TreeNode{};
		root->children[tree_vector[current_child_index]] = child;
		child->value = tree_vector[current_child_index + 1];
		int children = stoi(tree_vector[current_child_index + 2]);

		readTreeFromFileHelper(child, tree_vector, next_child_index, children);

		current_child_index += 3;
	}

	return root;
}

void readTreeFromFileHelper(TreeNode* parent, const vector<string>& tree_vector, int& next_child_index, const int& children)
{
	// Set current_child_index equal to next child_index.
	// current_child_index is for this node's child index.
	// Set next_child_index new value by multiplying the
	// number of children this parent has by three
	// and adding that to the value of next_child_index.
	int current_child_index = next_child_index;
	next_child_index += (children * 3);

	for (int i = 0; i < children; i++)
	{
		// Create child node and attach to parent
		TreeNode* child = new TreeNode{};
		parent->children[tree_vector[current_child_index]] = child;
		child->value = tree_vector[current_child_index + 1];
		int childs = stoi(tree_vector[current_child_index + 2]);

		// Pass only with node has a child
		if (childs > 0)
		{
			readTreeFromFileHelper(child, tree_vector, next_child_index, childs);
		}

		current_child_index += 3;
	}
}

// Calculate entrophy based on a frequency distribution of all
// outcome levels
double calculateEntrophy(const unordered_map<string, int>& outcome_level)
{
	int denominator = 0;

	// Determine denominator
	for (auto i : outcome_level)
	{
		denominator += i.second;
	}

	// Calculate entrophy
	double entrophy = 0.0;
	for (auto item : outcome_level)
	{
		double ratio = (double)item.second / denominator;
		double logged = log2(ratio);
		entrophy += -ratio * logged;
	}

	return entrophy;
}

// Builds frequency distribution based on supplied vector of data
unordered_map<string, int> buildFrequencyDistribution(const vector<string>& data)
{
	unordered_map<string, int> distribution{};

	for (auto item : data)
	{
		distribution[item]++;
	}

	return distribution;
}

// Allows us to grab a specific column of data from our 2d matrix
vector<string> getObservations(const vector<vector<string>>& matrix, int column)
{
	vector<string> result{};

	for (int i = 0; i < matrix.size(); i++)
	{
		result.push_back(matrix[i][column]);
	}

	return result;
}

// For the given matrix, reduce matrix such that all results match
// the supplied predictor variable on the supplied column
vector<vector<string>> reduceMatrix(const vector<vector<string>>& matrix, int column, string predictor)
{
	vector<vector<string>> result{};
	int row_counter = 0;

	for (int i = 0; i < matrix.size(); i++)
	{
		if (matrix[i][column] == predictor)
		{
			result.push_back(matrix[i]);
		}
	}

	return result;
}

int findMaxGain(const vector<vector<string>>& matrix, int outcome_column, double entrophy)
{
	if (matrix.size() == 0)
	{
		return -1;
	}
	
	vector<double> information_gain{};

	// Calculate information gain for each predictor variable
	for (int column = 0; column < matrix[0].size(); column++)
	{
		// Skip outcome column
		if (column == outcome_column)
		{
			information_gain.push_back(-1);
			continue;
		}

		vector<string> observations = getObservations(matrix, column);
		unordered_map<string, int> observation_levels = buildFrequencyDistribution(observations);
		double local_entrophy = 0.0;

		for (auto level : observation_levels)
		{
			auto reduced_matrix = reduceMatrix(matrix, column, level.first);
			auto reduced_observations = getObservations(reduced_matrix, outcome_column);
			local_entrophy += ((double)level.second / observations.size()) * calculateEntrophy(buildFrequencyDistribution(reduced_observations));
			
		}
		information_gain.push_back(entrophy - local_entrophy);
	}

	// Return column with most gain
	int most_gain = 0;
	for (int i = 1; i < information_gain.size(); i++)
	{
		if (information_gain[i] > most_gain)
		{
			most_gain = i;
		}
	}

	return most_gain;
}

TreeNode* buildTree(const vector<vector<string>>& matrix,
	const vector<string>& predictors,
	const int& outcome_column)
{
	vector<string> observations = getObservations(matrix, outcome_column);
	double entrophy = calculateEntrophy(buildFrequencyDistribution(observations));

	// Base case: 0 entrophy
	if (entrophy < 0.01)
	{
		TreeNode* node = new TreeNode{};
		node->value = matrix[0][outcome_column];
		return node;
	}

	int col = findMaxGain(matrix, outcome_column, entrophy);

	// Create new node
	TreeNode* node = new TreeNode{};
	node->value = predictors[col];

	// Attach as children all levels of outcome
	vector<string> selected_observations = getObservations(matrix, col);
	auto selected_levels = buildFrequencyDistribution(selected_observations);
	for (auto level : selected_levels)
	{
		auto reduced_matrix = reduceMatrix(matrix, col, level.first);
		node->children[level.first] = buildTree(reduced_matrix, predictors, outcome_column);
	}

	return node;
}