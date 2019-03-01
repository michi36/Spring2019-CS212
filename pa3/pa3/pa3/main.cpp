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

int main(void)
{
	string input;
	CityGraph graph{};
	unordered_map<string, int> vertex_seen{};

	cout << "***Route Planner***" << endl;

	// Constructs graph
	vertex_seen = constructGraph(graph);
	
	cout << "Enter destination file: ";
	getline(cin, input);

	// Get the delivery route
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

	// Construct a sub graph based on the delivery route
	CityGraph sub_graph;
	queue<string> route{};
	constructSubGraph(graph, sub_graph, destinations);

	// Find least seen vertex
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

	// Construct minimal spanning tree on the sub graph based on the
	// least seen vertex
	vector<Edge> mst = sub_graph.computeMinimumSpanningTree(start_location);

	// Again, record how many times each vertex is seen.
	// This will be used to find the best possible route
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

	// Get the MST based on the new starting location
	mst.clear();
	mst = sub_graph.computeMinimumSpanningTree(start_location, route, vertex_seen);

	// Add the weights to find the total transit time
	int time = 0;
	for (auto edge : mst)
	{
		// Decrement everytime we see a vertex
		// By the end, everything in hashtable should be zero
		vertex_seen[edge.source->getKey()]--;
		vertex_seen[edge.sink->getKey()]--;

		// If the vertex we start from has been visited more (or equal)
		// than the vertex we are going, we know the edge between the start
		// and end vertices will be backtracked.
		// Add the weight of the backtrack
		if (vertex_seen[edge.source->getKey()] >= vertex_seen[edge.sink->getKey()])
		{
			time += edge.weight;
		}

		// This adds the weight as if we traversed once
		time += edge.weight;
	}

	// Subtract the last edge weight because we will end at
	// the last visited vertex and we will NOT backtrack
	time -= mst[mst.size() - 1].weight;

	// Print total transit time
	cout << "Total transit time: " << time << " minutes" << endl;

	// Print route
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
		// Add vertex if haven't been seen yet
		if (visited[vertex] < 1)
		{
			sub_graph.addVertex(vertex);
			visited[vertex]++;
		}

		// Find the shortest paths from the starting vertex to all other vertex
		unordered_map<string, int> paths = graph.computeShortestPath(vertex);
		for (auto second_vertex : destinations)
		{
			// If vertices are the same, skip
			if (second_vertex == vertex)
			{
				continue;
			}

			// Add vertex if haven't been seen yet
			if (visited[second_vertex] < 1)
			{
				sub_graph.addVertex(second_vertex);
				visited[second_vertex]++;
			}

			// Connect vertices
			sub_graph.connectVertex(vertex, second_vertex, paths[second_vertex], true);
		}
	}
}