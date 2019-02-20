#ifndef INTERSECTION_NODE_H
#define INTERSECTION_NODE_H

#include "GraphNode.h"

class IntersectionNode : public GraphNode<string, string>
{
private:
	string _name;
	int weight;
	IntersectionNode* _north = nullptr;
	IntersectionNode* _east = nullptr;
	IntersectionNode* _south = nullptr;
	IntersectionNode* _west = nullptr;

public:
	IntersectionNode(IntersectionNode* node)
	{

	}
};




#endif