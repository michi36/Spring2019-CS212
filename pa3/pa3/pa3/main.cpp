/*
	NAME: Misael Hinojosa
	STUDENT ID: 0134-86600
	COMPLETION TIME: 2 hours
	COLLABS: None
*/
#include "CityGraph.h"
#include "CsvParser.h"
#include "StringSplitter.h"
#include <iostream>

unordered_map<string, int> constructGraph(CityGraph& graph);
void constructSubGraph(CityGraph graph, CityGraph& sub_graph, vector<string> destinations);
void findStartingPoint(string& start_location);

int main(void)
{
	string input;
	CityGraph graph{};
	unordered_map<string, int> vertex_seen{};

	cout << "***Route Planner***" << endl;

	// constructs graph
	vertex_seen = constructGraph(graph);
	
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

	// find least seen vertex
	int least = 0;
	string start_location;
	for (auto vertex: destinations)
	{
		if (least == 0)
		{
			least = vertex_seen[vertex];
			start_location = vertex;
		}
		if (vertex_seen[vertex] < least)
		{
			least = vertex_seen[vertex];
			start_location = vertex;
		}
	}

	// construct minimal spanning tree on the sub graph and record route
	vector<Edge> mst = sub_graph.computeMinimumSpanningTree(start_location);

	vertex_seen.clear();
	for (auto edge : mst)
	{
		vertex_seen[edge.sink->getKey()]++;
		vertex_seen[edge.source->getKey()]++;
	}

	for (auto vertex : destinations)
	{
		if (least == 0)
		{
			least = vertex_seen[vertex];
			start_location = vertex;
		}
		if (vertex_seen[vertex] < least)
		{
			least = vertex_seen[vertex];
			start_location = vertex;
		}
	}
	mst.clear();

	mst = sub_graph.computeMinimumSpanningTree(start_location, route, vertex_seen);

	// add the weights to find the total transit time
	int time = 0;
	string previous = "";
	int backtracking = 0;
	for (auto edge : mst)
	{
		if (edge.source->getKey() == previous)
		{
			time += backtracking;
		}
		time += edge.weight;
		previous = edge.source->getKey();
		backtracking = edge.weight;
	}
	cout << "Total transit time: " << time << " minutes" << endl;

	// print route
	cout << "Route:" << endl;
	while (route.empty() == false)
	{
		string first = route.front();
		route.pop();
		string second = route.front();

		if (first == second)
		{
			route.pop();
			second = route.front();
		}
		cout << first << " -> ";
		cout << second << endl;

		if (route.size() == 3)
		{
			route.pop();
			route.pop();
			route.pop();
		}
	}

	return 0;
}

unordered_map<string, int> constructGraph(CityGraph& graph)
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
			vector<string> temp = StringSplitter::split(line, ",");
			if (visited[temp[0]] < 1)
			{
				graph.addVertex(temp[0]);
			}
			if (visited[temp[1]] < 1)
			{
				graph.addVertex(temp[1]);
			}
			visited[temp[0]]++;
			visited[temp[1]]++;

			graph.connectVertex(temp[0], temp[1], stoi(temp[2]), true);
		}
	}
	input_file.close();

	return visited;
}

void constructSubGraph(CityGraph graph, CityGraph& sub_graph, vector<string> destinations)
{
	unordered_map<string, int> visited{};
	unordered_map<string, int> seen{};
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