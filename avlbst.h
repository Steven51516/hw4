#ifndef RBBST_H
#define RBBST_H

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
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
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
		bool zig_zig(AVLNode<Key, Value>* g, AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
		void LRotate(AVLNode<Key, Value>* N);
		void RRotate(AVLNode<Key, Value>* N);
		void RemoveFix(AVLNode<Key, Value>* n, int diff);
		void insert_fix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
		AVLNode<Key, Value>* internalFind(const Key& k);
		void insert_helper (const std::pair<const Key, Value> &new_item);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */

template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
		//if empty tree
		if(this->root_ == NULL)
		{
			insert_helper(new_item);
			return;
		}
		//if the key already exists, change the value
		bool already_in = false;
		if(this->internalFind(new_item.first)!= NULL) already_in = true;

		insert_helper(new_item);

		if(already_in) return;
		

		AVLNode<Key, Value>* n = this->internalFind(new_item.first);
		//if b(p) = 1 or -1
		if(n->getParent()->getBalance()==-1 || n->getParent()->getBalance()== 1 )
		{
			n->getParent()->setBalance(0);
			return;
		}
		//if b(p) = 0, update b(p) and call insert-fix(p, n)
		else if(n->getParent()->getBalance()==0)
		{
			if(n->getParent()->getRight()==n) n->getParent()->setBalance(1);
			else n->getParent()->setBalance(-1);
			insert_fix(n->getParent(), n);
		}
		
}

//insert function simialr to BST but create an AVLNode instead of Node
template<class Key, class Value>
void AVLTree<Key, Value>::insert_helper (const std::pair<const Key, Value> &new_item)
{
	  Key key = new_item.first;
    Value val = new_item.second;
    AVLNode<Key, Value>* temp = internalFind(key);
    if(temp!=NULL)
    {
        temp->setValue(val);
        return;
    }
    else if(this->root_ == NULL)
    {
        this->root_ = new AVLNode<Key, Value>(key, val, NULL);
	
        return;
    }
    temp = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* prev;
    while(1)
    {			
        prev=temp;
        if(temp->getKey()<key) temp=temp->getRight();
        else temp=temp->getLeft();
        if(temp==NULL) break;
    }
    AVLNode<Key, Value>* n = new AVLNode<Key, Value>(key, val, prev);
    if(prev->getKey()<key) prev->setRight(n);
    else prev->setLeft(n);
}


template<class Key, class Value>
void AVLTree<Key, Value>::insert_fix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
	//if p is null or parent(p) is null, return
	if(p==NULL) return;
	AVLNode<Key, Value>* g = p->getParent();
	if(g==NULL) return;
	// if p is left child of g
	if(g->getLeft()==p)
	{
		g->updateBalance(-1);
		if(g->getBalance()==0) return;
		else if(g->getBalance()==-1) insert_fix(g, p);\
		else
		{
			//If zig-zig then rotateRight(g); b(p) = b(g) = 0
			if(zig_zig(g, p, n))
			{
				RRotate(g);
				p->setBalance(0);
				g->setBalance(0);

			}
			//If zig-zag then rotateLeft(p); rotateRight(g); 
			else
			{
				LRotate(p);
				RRotate(g);
				if(n->getBalance()==0) 
				{
					p->setBalance(0);
					g->setBalance(0);
				}
				else if(n->getBalance()==1)
				{
					p->setBalance(-1);
					n->setBalance(0);
					g->setBalance(0);
				}
				else if(n->getBalance()==-1)
				{
					p->setBalance(0);
					g->setBalance(1);
					n->setBalance(0);
				}
			}
		}
	}
	//similar operations if p is right child of g
	else
	{
		g->updateBalance(1);
		if(g->getBalance()==0) return;
		else if(g->getBalance()==1) insert_fix(g, p);
		else
		{
			if(zig_zig(g, p, n))
			{
				LRotate(g);
				p->setBalance(0);
				g->setBalance(0);

			}
			else
			{
				RRotate(p);
				LRotate(g);
				if(n->getBalance()==0) 
				{
					p->setBalance(0);
					g->setBalance(0);
				}
				else if(n->getBalance()==-1)
				{
					p->setBalance(1);
					n->setBalance(0);
					g->setBalance(0);
				}
				else if(n->getBalance()==1)
				{
					p->setBalance(0);
					g->setBalance(-1);
					n->setBalance(0);
				}
			}
	 }
	}
}

//helper function that returns an AVLNode according to key
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::internalFind(const Key& k)
{
	  AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->root_);
    while(temp != NULL)
    {
        if(temp->getKey()==k) return temp;
        else if(k < temp->getKey()) temp = temp->getLeft();
        else temp = temp->getRight();
    }
    return NULL;
}

// return a bool representing whether the three nodes are zig_zig
template<class Key, class Value>
bool AVLTree<Key, Value>::zig_zig(AVLNode<Key, Value>* g, AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
	if(g->getRight()==p && p->getRight()==n) return true;
	else if(g->getLeft()==p && p->getLeft()==n) return true;
	return false;
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
		AVLNode<Key, Value>* n = this->internalFind(key);
		if(n==NULL) return;
		//If n has 2 children, swap positions with in-order predecessor
		if(n->getLeft()!=NULL&&n->getRight()!=NULL)
		{
			nodeSwap(n, static_cast<AVLNode<Key, Value>*>(this->predecessor(n)));
		}
		AVLNode<Key, Value>* p = n->getParent();
		
		int diff = 1048;
		if(p!=NULL)
		{
			if(p->getLeft()==n) diff=1;
			else diff=-1;
		}
		//Delete n
		BinarySearchTree<Key, Value>::remove_helper(n);
		RemoveFix(p, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>:: RemoveFix(AVLNode<Key, Value>* n, int diff)
{
	if(n==NULL) return;
	//Compute next recursive call's arguments now before altering the tree
	AVLNode<Key, Value>* p = n->getParent();
	int ndiff = 1048;
	if(p!=NULL)
	{
		if(p->getLeft()==n) ndiff=1;
		else ndiff=-1;
	}
	//case for diff = -1
	if(diff==-1)
	{
		//Case 1: b(n) + diff == -2
		if(n->getBalance()+diff==-2)
		{
			AVLNode<Key, Value>* c = n->getLeft();
			//Case 1a: b(c) == -1 - zig-zig case
			if(c->getBalance()==-1)
			{
				RRotate(n);
				n->setBalance(0);
				c->setBalance(0);
				RemoveFix(p, ndiff);
			}
			//Case 1b: b(c) == 0 - zig-zig case
			else if(c->getBalance()==0)
			{
				RRotate(n);
				n->setBalance(-1);
				c->setBalance(1);
				return;
			}
			//Case 1c: b(c) == +1  zig-zag case
			else if(c->getBalance()==1)
			{
				AVLNode<Key, Value>* g = c->getRight();
				LRotate(c);
				RRotate(n);
				if(g->getBalance()==1)
				{
					n->setBalance(0);
					c->setBalance(-1);
					g->setBalance(0);
				}
				else if(g->getBalance()==0)
				{
					n->setBalance(0);
					c->setBalance(0);
				}
				else if(g->getBalance()==-1)
				{
					n->setBalance(1);
					c->setBalance(0);
					g->setBalance(0);

				}
				RemoveFix(p, ndiff);
			}

		}
		//Case 2: b(n) + diff == -1
		else if(n->getBalance()+diff==-1)
		{
			n->setBalance(-1);
			return;
		}
		//Case 3: b(n) + diff == 0
		else if(n->getBalance()+diff==0)
		{
			n->setBalance(0);
			RemoveFix(p,ndiff);
		}
	}
	//mirroring the case for diff -1
	else if(diff==1)
	{
		if(n->getBalance()+diff==2)
		{
			AVLNode<Key, Value>* c = n->getRight();
			if(c->getBalance()==1)
			{
				LRotate(n);
				n->setBalance(0);
				c->setBalance(0);
				RemoveFix(p, ndiff);
			}
			else if(c->getBalance()==0)
			{
				LRotate(n);
				n->setBalance(1);
				c->setBalance(-1);
				return;
			}
			else if(c->getBalance()==-1)
			{
				AVLNode<Key, Value>* g = c->getLeft();
				RRotate(c);
				LRotate(n);
				if(g->getBalance()==-1)
				{
					n->setBalance(0);
					c->setBalance(1);
					g->setBalance(0);
				}
				else if(g->getBalance()==0)
				{
					n->setBalance(0);
					c->setBalance(0);
				}
				else if(g->getBalance()==1)
				{
					n->setBalance(-1);
					c->setBalance(0);
					g->setBalance(0);

				}
				RemoveFix(p, ndiff);
			}

		}
		else if(n->getBalance()+diff==1)
		{
			n->setBalance(1);
			return;
		}
		else if(n->getBalance()+diff==0)
		{
			n->setBalance(0);
			RemoveFix(p,ndiff);
		}

	}
}


//helper function for left rotation
template<class Key, class Value>
void AVLTree<Key, Value>::LRotate(AVLNode<Key, Value>* N)
{
	AVLNode<Key, Value>* P = N->getParent();
	AVLNode<Key, Value>* R = N->getRight();
	if(P != NULL)
	{
		if(P->getLeft() == N) P->setLeft(R);
		else P->setRight(R);
		R->setParent(P);
	}
	//case when N is root node
	else
	{
		this->root_ = R;
		R->setParent(NULL);
	}
	N->setRight(R->getLeft());
	if(N->getRight() != NULL) N->getRight()->setParent(N);
	R->setLeft(N);
	R->getLeft()->setParent(R);
}


//helper function for right rotation
template<class Key, class Value>
void AVLTree<Key, Value>::RRotate(AVLNode<Key, Value>* N)
{
	AVLNode<Key, Value>* P = N->getParent();
	AVLNode<Key, Value>* L = N->getLeft();
	if(P != NULL)
	{
		if(P->getLeft() == N) P->setLeft(L);
		else P->setRight(L);
		L->setParent(P);
	}
	//case when N is root node
	else
	{
		this->root_ = L;
		L->setParent(NULL);
	}
	N->setLeft(L->getRight());
	if(N->getLeft() != NULL) N->getLeft()->setParent(N);
	L->setRight(N);
	L->getRight()->setParent(L);
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
