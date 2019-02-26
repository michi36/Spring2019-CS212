#include "CityGraph.h"
#include "CsvParser.h"
#include "StringSplitter.h"
#include <iostream>

void constructGraph(CityGraph& graph);
void constructSubGraph(CityGraph graph, CityGraph& sub_graph, vector<string> destinations);

int main(void)
{
	string input;
	CityGraph graph{};
	constructGraph(graph);

	cout << "Enter destination file: ";
	getline(cin, input);

	vector<string> destinations{};
	ifstream input_file{ input };
	if (input_file.is_open() == true)
	{
		while (input_file.good() == true)
		{
			string line;
			getline(input_file, line);
			destinations.push_back(line);
		}
	}
	input_file.close();

	CityGraph sub_graph;
	constructSubGraph(graph, sub_graph, destinations);

	vector<Edge> mst2 = sub_graph.computeMinimumSpanningTree(destinations[0]);
	int time2 = 0;
	for (auto edge : mst2)
	{
		time2 += edge.weight;
	}
	cout << "Total transit time: " << time2  << " minutes" << endl;

	return 0;
}

void constructGraph(CityGraph& graph)
{
	string map_name;
	unordered_map<string, int> visited{};

	cout << "Enter maps file: ";
	getline(cin, map_name);

	ifstream input_file{ map_name };
	if (input_file.is_open() == true)
	{
		while (input_file.good() == true)
		{
			string line;
			getline(input_file, line);
			vector<string> temp = StringSplitter::split(line, " ");
			if (visited[temp[0]] < 1)
			{
				visited[temp[0]]++;
				graph.addVertex(temp[0]);
			}
			if (visited[temp[1]] < 1)
			{
				visited[temp[1]]++;
				graph.addVertex(temp[1]);
			}

			graph.connectVertex(temp[0], temp[1], stoi(temp[2]), true);
		}
	}
	input_file.close();
}

void constructSubGraph(CityGraph graph, CityGraph& sub_graph, vector<string> destinations)
{
	unordered_map<string, int> visited{};
	for (auto vertex : destinations)
	{
		if (visited[vertex] < 1)
		{
			sub_graph.addVertex(vertex);
			visited[vertex]++;
		}

		unordered_map<string, int> paths = graph.computeShortestPath(vertex);
		for (auto second_vertex : destinations)
		{
			if (second_vertex == vertex)
			{
				continue;
			}

			if (visited[second_vertex] < 1)
			{
				sub_graph.addVertex(second_vertex);
				visited[second_vertex]++;
			}

			sub_graph.connectVertex(vertex, second_vertex, paths[second_vertex], true);
		}
	}
}