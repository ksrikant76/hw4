#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

//returns the height of a subtree if heights are equal,
//otherwise returns -1 
int pathLengths(Node* root) {
  //empty subtree, height 0
  if (root == nullptr) return 0;

  //no children, height 1
  if (root->left == nullptr && root->right == nullptr) return 1;

  //two children
  if (root->left != nullptr && root->right != nullptr) {
    int left = pathLengths(root->left);
    int right = pathLengths(root->right);

    //return -1 if either side fails or both sides aren't equal
    if (left == -1 || right == -1 || left != right) return -1;
    
    return left + 1;
  }

  //one child
  Node* child = (root->left != nullptr) ? root->left : root->right;
  int ch = pathLengths(child);
  if (ch == -1) return -1;
  return ch + 1;
}


bool equalPaths(Node * root)
{
    // Add your code below
  return (pathLengths(root) != -1);
}

