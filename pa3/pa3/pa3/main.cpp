/*
	NAME: Misael Hinojosa
	STUDENT ID: 0134-86600
	COMPLETION TIME: 5 hours
	COLLABS: None
*/
#include "CityGraph.h"
#include "CsvParser.h"
#include "StringSplitter.h"
#include <iostream>
#include <stack>

unordered_map<string, int> constructGraph(CityGraph& graph);
void constructSubGraph(CityGraph graph, CityGraph& sub_graph, const vector<string>& destinations);
void addingEdges(const vector<Edge>& mst, int& time, unordered_map<string, int> vertex_seen);
void addingEdgesWithRoute(const vector<Edge>& mst, int& time, unordered_map<string, int> vertex_seen, queue<string>& route);

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
	vertex_seen.clear();
	for (auto edge : mst)
	{
		vertex_seen[edge.sink->getKey()]++;
		vertex_seen[edge.source->getKey()]++;
	}

	// Find all vertices that equal one
	destinations.clear();
	for (auto vertex : vertex_seen)
	{
		if (vertex.second == 1)
		{
			destinations.push_back(vertex.first);
		}
	}

	int time = 0;
	int least_time = 0;
	for (auto start_position : destinations)
	{
		vector<Edge> temp = sub_graph.computeMinimumSpanningTree(start_position);
		addingEdges(temp, time, vertex_seen);

		if (least_time == 0)
		{
			least_time = time;
			start_location = start_position;
		}

		if (time < least_time)
		{
			start_location = start_position;
			least_time = time;
		}
		time = 0;
	}

	// Get the MST based on the new starting location
	mst.clear();
	mst = sub_graph.computeMinimumSpanningTree(start_location);

	time = 0;
	addingEdgesWithRoute(mst, time, vertex_seen, route);
	
	// Print total transit time
	cout << "Total transit time: " << time << " minutes" << endl;

	// Print route
	cout << "Route:" << endl;
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

void constructSubGraph(CityGraph graph, CityGraph& sub_graph, const vector<string>& destinations)
{
	unordered_map<string, int> visited{};
	for (auto vertex : destinations)
	{
		// Add vertex if haven't been seen yet
		if (visited[vertex] < 1)
		{
			sub_graph.addVertex(vertex);
			
		}
		visited[vertex]++;

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
				
			}
			visited[second_vertex]++;

			// Connect vertices
			sub_graph.connectVertex(vertex, second_vertex, paths[second_vertex], true);
		}
	}
}

void addingEdges(const vector<Edge>& mst, int& time, unordered_map<string, int> vertex_seen)
{
	// Add the weights to find the total transit time
	// root_vertex is where the graph will branch off to a subgraph
	// and come back to the root_vertex
	// Because we know it will come back, double the time added
	// is_branched will keep track if we are branched
	bool is_branched = false;
	string root_vertex = "";
	for (auto edge : mst)
	{
		// Decrement the starting vertex
		vertex_seen[edge.source->getKey()]--;

		// If we return to the root_vertex, set is_branched to false
		// and root_vertex to an empty string
		if (edge.source->getKey() == root_vertex)
		{
			root_vertex = "";
			is_branched = false;
		}

		// If branched, add double the time
		if (is_branched == true)
		{
			time += (2 * edge.weight);
		}
		// We know that a vertex less than 2 will not be seen again,
		// and because of that, that edge will only be passed once
		else if (vertex_seen[edge.source->getKey()] < 2)
		{
			time += edge.weight;
		}
		// At this point, we know that the source vertex has been touched
		// at least 2 times. We will see this vertex again, so add double
		// time, set is_branched to true, and record vertex name
		else
		{
			is_branched = true;
			time += (2 * edge.weight);
			root_vertex = edge.source->getKey();
		}
	}
}

void addingEdgesWithRoute(const vector<Edge>& mst, int& time, unordered_map<string, int> vertex_seen, queue<string>& route)
{
	// Look at function addingEdges for explaination for
	// adding the edges
	// Will use a stack to record the backtracking

	stack<string> backtracking{};
	bool is_branched = false;
	string root_vertex = "";
	for (auto edge : mst)
	{
		vertex_seen[edge.source->getKey()]--;

		if (edge.source->getKey() == root_vertex)
		{
			root_vertex = "";
			is_branched = false;

			// We've returned to the vertex from where
			// we branched off. Add contents of stack
			// to the route
			while (backtracking.empty() == false)
			{
				route.push(backtracking.top());
				backtracking.pop();
			}
		}

		if (is_branched == true)
		{
			time += (2 * edge.weight);

			// Add source vertex to route
			route.push(edge.source->getKey());

			// If the sink had a value of one, it will
			// never be seen again, add to route
			if (vertex_seen[edge.sink->getKey()] == 1)
			{
				route.push(edge.sink->getKey());
			}
		}
		else if (vertex_seen[edge.source->getKey()] < 2)
		{
			time += edge.weight;

			// Source vertex will never be seen again,
			// add to route
			route.push(edge.source->getKey());
		}
		else
		{
			is_branched = true;
			time += (2 * edge.weight);
			root_vertex = edge.source->getKey();

			// We reached a part of the graph where it will
			// branch off and backtrack back. Add source vertex
			// to route and sink vertex to stack
			route.push(edge.source->getKey());
			backtracking.push(edge.sink->getKey());
		}
	}

	// The last sink vertex seen wasn't added from algorithm above.
	// Add the last sink vertex now
	route.push(mst[mst.size() - 1].sink->getKey());
}