#ifndef AVLTREE_H
#define AVLTREE_H
#include "node.h"
#include <string>

/*
In hindsight, a prefix tree would be a better choice, but it would consume more nodes 
*/

class AVLTree{
private:
    Node* root;

    int getHeight(Node* node);

    int getBalanceFactor(Node* node);

    Node* rotateRight(Node* A);

    Node* rotateLeft(Node* A);

    Node* insert(Node *node, std::string route, Response (*responseFunction)(Request&), Middleware &middleware);

    bool startsWith(const std::string& str, const std::string& prefix);

    Response inorder(Node* node, Request &requestObject);


public:
    AVLTree();

    void insert(std::string route, Response (*responseFunction)(Request&));
    void insert(std::string route, Response (*responseFunction)(Request&), Middleware &middleware);
    
    Node* search(Request &requestObject);
};


#endif