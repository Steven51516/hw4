#include "equal-paths.h"
using namespace std;
#include <cmath>
#include <algorithm>

// You may add any prototypes of helper functions here

int height(Node * root)
{
	  if (root == NULL) return 0;
    else return max(height(root->left), height(root->right))+1;
}
bool equalPaths(Node * root)
{
    if(root == NULL) return 1;
		if(root->left == NULL) return equalPaths(root->right);
		else if(root->right == NULL) return equalPaths(root->left);
		else return equalPaths(root->left) && equalPaths(root->right) && abs(height(root->left) - height(root->right)) == 0;
}




