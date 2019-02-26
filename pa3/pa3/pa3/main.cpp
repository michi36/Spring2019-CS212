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

	cout << "***Route Planner***" << endl;

	// constructs graph
	constructGraph(graph);
	
	cout << "Enter destination file: ";
	getline(cin, input);

	// get the delivery route
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

	// construct a sub graph based on the delivery route
	CityGraph sub_graph;
	queue<string> route{};
	constructSubGraph(graph, sub_graph, destinations);

	// construct minimal spanning tree on the sub graph and record route
	vector<Edge> mst = sub_graph.computeMinimumSpanningTree(destinations[1], route);

	// add the weights to find the total transit time
	int time = 0;
	for (auto edge : mst)
	{
		time += edge.weight;
	}
	cout << "Total transit time: " << time << " minutes" << endl;
	cout << "Route:" << endl;

	// print route
	while (route.empty() == false)
	{
		cout << route.front() << " -> ";
		route.pop();
		cout << route.front() << endl;

		if (route.size() == 1)
		{
			route.pop();
		}
	}

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
		// add vertex if haven't been seen yet
		if (visited[vertex] < 1)
		{
			sub_graph.addVertex(vertex);
			visited[vertex]++;
		}

		// find the shortest paths from the starting vertex to all other vertex
		unordered_map<string, int> paths = graph.computeShortestPath(vertex);
		for (auto second_vertex : destinations)
		{
			// if vertices are the same, skip
			if (second_vertex == vertex)
			{
				continue;
			}

			// add vertex if haven't been seen yet
			if (visited[second_vertex] < 1)
			{
				sub_graph.addVertex(second_vertex);
				visited[second_vertex]++;
			}

			// connect vertices
			sub_graph.connectVertex(vertex, second_vertex, paths[second_vertex], true);
		}
	}
}