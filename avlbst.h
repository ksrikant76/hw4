#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    int height(AVLNode<Key, Value>* node) const;
    AVLNode<Key, Value>* rebalance(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);
    void updateBalance(AVLNode<Key, Value>* node); 
};

template<class Key, class Value>
int AVLTree<Key, Value>::height(AVLNode<Key, Value>* node) const {
    if (node == nullptr) return 0;
    int leftHeight = height(node->getLeft());
    int rightHeight = height(node->getRight());
    return std::max(leftHeight, rightHeight) + 1;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node) {
    if (node == nullptr) return node;

    int bal = node->getBalance();

    //left subtree taller
    if (bal > 1) {
        //find left child's balance (and update it) to see whether LL or LR rotation needed
        AVLNode<Key, Value>* L = node->getLeft();
        int leftBal = height(L->getLeft()) - height(L->getRight());
        L->setBalance(static_cast<int8_t>(leftBal));

        if (leftBal < 0) { //LR initial rotation
            rotateLeft(L);
        }
        //needed for both LL and LR rotations
        node = rotateRight(node);
    }

    //right subtree taller
    else if (bal < -1) {
        //find right child's balance (and update it) to see whether RR or RL rotation needed
        AVLNode<Key, Value>* R = node->getRight();
        int rightBal = height(R->getLeft()) - height(R->getRight());
        R->setBalance(static_cast<int8_t>(rightBal));

        if (rightBal > 0) { //RL initial rotation
            rotateRight(R);
        }
        //needed for both RR and RL rotations
        node = rotateLeft(node);
    }

    //update balance of node and its children
    updateBalance(node);
    updateBalance(node->getLeft());
    updateBalance(node->getRight());

    return node;
}

//left rotation at node, returns new root
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node) {
    //no rotation needed
    if (node == nullptr) return node;
    AVLNode<Key, Value>* R = node->getRight();
    if (R == nullptr) return node;

    //get RL and P
    AVLNode<Key, Value>* RL = R->getLeft();
    AVLNode<Key, Value>* P = node->getParent();

    //connect R to P
    R->setParent(P);
    if (P == nullptr) {
        this->root_ = R;
    } 
    else {
        if (P->getLeft() == node) P->setLeft(R);
        else P->setRight(R);
    }

    //put node as left child of R
    R->setLeft(node);
    node->setParent(R);

    //put RL as right child of node 
    node->setRight(RL);
    if (RL != nullptr) RL->setParent(node);

    return R;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node) {
    //no rotation needed
    if (node == nullptr) return node;
    AVLNode<Key, Value>* L = node->getLeft();
    if (L == nullptr) return node;

    //get LR and P
    AVLNode<Key, Value>* LR = L->getRight();
    AVLNode<Key, Value>* P = node->getParent();

    //connect L to P
    L->setParent(P);
    if (P == nullptr) {
        this->root_ = L;
    } 
    else {
        if (P->getLeft() == node) P->setLeft(L);
        else P->setRight(L);
    }

    //put node as right child of L
    L->setRight(node);
    node->setParent(L);

    //put LR as left child of node 
    node->setLeft(LR);
    if (LR != nullptr) LR->setParent(node);

    return L;
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalance(AVLNode<Key, Value>* node) {
    if (node == nullptr) return;
    int bal = height(node->getLeft()) - height(node->getRight());
    node->setBalance(static_cast<int8_t>(bal));
}

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO

    //empty tree
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        static_cast<AVLNode<Key, Value>*>(this->root_)->setBalance(0);
        return;
    }

    //find where to insert
    AVLNode<Key,Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key,Value>* parent = nullptr;
    while (current != nullptr) {
        parent = current;
        if (new_item.first < current->getKey()) {
            current = current->getLeft();
        }
        else if (new_item.first > current->getKey()) {
            current = current->getRight();
        }
        else {
            //keys are equal (key already exists)
            current->setValue(new_item.second);
            return;
        }
    }

    //create node and attach it to parent
    AVLNode<Key, Value>* node = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    node->setBalance(0);
    if (new_item.first < parent->getKey()) {
        parent->setLeft(node);
    }
    else {
        parent->setRight(node);
    }

    //rebalance from parent, and walk back up the tree
    AVLNode<Key, Value>* fix = parent;
    while (fix != nullptr) {
        //set new balance of fix
        int bal = height(fix->getLeft()) - height(fix->getRight());
        fix->setBalance(static_cast<int8_t>(bal));

        //if unbalanced, rebalance at fix
        if (bal > 1 || bal < -1) {
            fix = rebalance(fix);
        }
        
        fix = fix->getParent();
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (node == nullptr) return; //key doesn't exist in tree

    //if node has two children, swap with predecessor
    if (node->getLeft() != nullptr && node->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(node));
        if (pred != nullptr) {
            nodeSwap(node, pred);
        }
    }

    //now, node must have at most one child
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* child = nullptr;
    if (node->getLeft() != nullptr) {
        child = node->getLeft();
    }
    else {
        child = node->getRight();
    }

    //if there is a child, connect to parent
    if (child != nullptr) {
        child->setParent(parent);
    }

    //if parent exists, set child appropriately
    if (parent != NULL) {
        if (parent->getLeft() == node) {
            parent->setLeft(child);
        }
        else {
            parent->setRight(child);
        } 
    }
    else { //no parent, node is root
        this->root_ = child;
    }
    delete node;

    //rebalance from parent, and walk back up the tree
    AVLNode<Key, Value>* fix = parent;
    while (fix != nullptr) {
        //set new balance of fix
        int bal = height(fix->getLeft()) - height(fix->getRight());
        fix->setBalance(static_cast<int8_t>(bal));

        //if unbalanced, rebalance at fix
        if (bal > 1 || bal < -1) {
            fix = rebalance(fix);
        }
        
        fix = fix->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
