#include <stack>

#include "avltree.h"
#include "middleware.h"
#include "response.h"

int AVLTree::getHeight(Node *node){
    if(!node) return 0;
    return node->height;
}

int AVLTree::getBalanceFactor(Node *node){
    if(!node) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

Node *AVLTree::rotateRight(Node *A){
    Node* B = A->left;
    Node* C = B->right;

    A->left = C;
    B->right = A;

    A->height = 1 + std::max(getHeight(A->left),getHeight(A->right));
    B->height = 1 + std::max(getHeight(B->left),getHeight(B->right));
    
    return B;
}

Node *AVLTree::rotateLeft(Node *A){
    Node* B = A->right;
    Node* C = B->left;

    A->right = C;
    B->left = A;

    A->height = 1 + std::max(getHeight(A->left),getHeight(A->right));
    B->height = 1 + std::max(getHeight(B->left),getHeight(B->right));

    return B;
}

Node *AVLTree::insert(Node *node, std::string route, Response (*responseFunction)(Request&), Middleware &middleware){
    if(!node) return new Node(route, responseFunction, middleware);

    if(node->route > route){
        node->left = insert(node->left, route, responseFunction, middleware);
    }
    else if(node->route < route){
        node->right = insert(node->right, route, responseFunction, middleware);
    }
    else{
        std::cerr << "Two definitions for the same route not permitted : "<<node->route<<std::endl;
        throw std::runtime_error("Two definitions for the same route");
        return node;
    }

    node->height = 1 + std::max(getHeight(node->left),getHeight(node->right));

    int bf = getBalanceFactor(node);
    if(bf > 1 && route < node->left->route){
        return rotateRight(node);
    }
    else if(bf > 1 && route > node->left->route){
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    else if(bf < -1 && route > node->right->route){
        return rotateLeft(node);
    }
    else if(bf < -1 && route < node->right->route){
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

bool AVLTree::startsWith(const std::string &str, const std::string &prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }

    return str.substr(0, prefix.length()) == prefix;
}

AVLTree::AVLTree():root(NULL){}

void AVLTree::insert(std::string route,Response (*responseFunction)(Request&)){
    Middleware middleware;
    root = insert(root, route, responseFunction, middleware);
}

void AVLTree::insert(std::string route, Response (*responseFunction)(Request &), Middleware &middleware){
    root = insert(root, route, responseFunction, middleware);
}

Response AVLTree::inorder(Node *node, Request &requestObject)
{
    if(node==NULL) return Middleware::nextObj;
    std::stack<Node*> st;
    Node* current = node;
    while(!st.empty() || current){
        while(current){
            st.push(current);
            current = current->left;
        }
        current = st.top();
        st.pop();
        if(startsWith(requestObject.getRequestRoute(), current->route)){
            Response res = current->middleware.execute(requestObject);
            if(res != Middleware::nextObj) return res;
        }
        current = current->right;
    }

    return Middleware::nextObj;
}


/*
I am sure this is not the best way of doing this, but it works. I am open for suggestions. Should it been a prefix tree, it would be easier to traverse and find the middleware.

If we set a middleware for a parent route, it is possible that while it may be present in the left subtree, thus when we move to the right, we need to search if the prefix route is to the left
Also after we find the route node, it is possible that the parent lies to the left subtree of the node, thus we need to traverse inorder in the left subtree of the route node

*/
Node *AVLTree::search(Request &requestObject){
    Node* curr = root;
    while(curr){
        if(startsWith(requestObject.getRequestRoute(), curr->route)){
            Response res = curr->middleware.execute(requestObject);
            if(res != Middleware::nextObj){
                return new Node(res);
            }
        }
        if(curr->route == requestObject.getRequestRoute()){
            Response res = inorder(curr->left, requestObject);
            if(res != Middleware::nextObj){
                return new Node(res);
            }
            return curr;
        }
        else if(curr->route > requestObject.getRequestRoute()){
            curr = curr->left;
        }
        else{
            Response res = inorder(curr->left, requestObject);
            if(res != Middleware::nextObj){
                return new Node(res);
            }
            curr = curr->right;
        }
    }
    return nullptr;
}
