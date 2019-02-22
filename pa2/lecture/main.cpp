/*
	NAME: Misael Hinojosa
	STUDENT ID: 0134-86600
	COMPLETION TIME: 13 hours
	COLLABS: None
*/
#include "CampusGraph.h"
#include "CsvParser.h"
#include "StringSplitter.h"
#include <iostream>

void constructGraph(CampusGraph& graph, vector<vector<string>> data);
void pathTaken(CampusGraph graph, unordered_map<string, string> mapKey, unordered_map<string, string> abbreviations);

int main(void)
{
	// Example of how to parse a CSV file file for graph building
	CsvStateMachine csm{ "distances.csv" };
	vector<vector<string>> data = csm.processFile();
	unordered_map<string, string> mapKey{};
	unordered_map<string, string> abbreviations{};

	ifstream input_file{ "mapKey.txt" };
	if (input_file.is_open() == true)
	{
		while (input_file.good() == true)
		{
			string line;
			getline(input_file, line);
			if (line.length() > 0)
			{
				vector<string>  temp = StringSplitter::split(line, " ");
				mapKey[temp[0]] = temp[1];
			}
		}
	}
	input_file.close();
	input_file.open("abriv.txt");
	if (input_file.is_open() == true)
	{
		while (input_file.good() == true)
		{
			string line;
			getline(input_file, line);
			if (line.length() > 0)
			{
				vector<string> temp = StringSplitter::split(line, ",");
				abbreviations[temp[0]] = temp[1];
			}
		}
	}
	input_file.close();


	CampusGraph graph{};
	constructGraph(graph, data);

	pathTaken(graph, mapKey, abbreviations);

	return 0;
}

void constructGraph(CampusGraph& graph, vector<vector<string>> data)
{
	unordered_map<string, int> visited_nodes{};

	for (auto vec : data)
	{
		// if we haven't seen the node, add to graph and mark as seen
		if (visited_nodes[vec[0]] < 1)
		{ 
			graph.addVertex(vec[0]);
			visited_nodes[vec[0]]++;
		}
		
		if (visited_nodes[vec[1]] < 1)
		{
			graph.addVertex(vec[1]);
			visited_nodes[vec[1]]++;
		}
		
		graph.connectVertex(vec[0], vec[1], stoi(vec[2]));
		
	}

	return;
}

void pathTaken(CampusGraph graph, unordered_map<string, string> mapKey, unordered_map<string, string> abbreviations)
{
	string start_location, end_location;
	unordered_map<string, int> distances{};
	queue<string> order{};
	int time = 0;

	cout << "***HSU Transit Time Calculator***" << endl;
	cout << "Enter starting location: ";
	getline(cin, start_location);
	cout << "Enter destination: ";
	getline(cin, end_location);

	distances = graph.computShortestPath(mapKey[start_location], mapKey[end_location], order);
	
	if (distances.find(mapKey[end_location]) != distances.end())
	{
		time = distances[mapKey[end_location]];
		cout << "Estimated travel time: " << time / 60 << " minutes and " << time % 60 << " seconds" << endl;
		cout << "On you way from " << abbreviations[mapKey[start_location]] << " to " << abbreviations[mapKey[end_location]] << ", you will pass by: ";
		order.pop();
		while (order.empty() == false)
		{
			if (order.size() == 1)
			{
				cout << "and " << abbreviations[order.front()] << endl;
			}
			else
			{
				cout << abbreviations[order.front()] << ", ";
			}
			order.pop();
		}
	}
	else
	{
		cout << "Invalid location(s)" << endl;
		return;
	}
}