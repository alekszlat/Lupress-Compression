#include "Node.h"
Node::Node(char c, int freq)
{
	this->ch = c;
	this->frequincy = freq;
	this->left = nullptr;
	this->right = nullptr;
}
