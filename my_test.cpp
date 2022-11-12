#include<iostream>
#include "bst.h"


int main()
{
	BinarySearchTree<int, double> bst;
	bst.insert(std::make_pair(2, 1.0));
	bst.insert(std::make_pair(1, 1.0));
	bst.insert(std::make_pair(3, 1.0));
	bst.remove(2);

	return 0;

}
