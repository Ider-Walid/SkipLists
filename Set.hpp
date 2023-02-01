#include <iostream>
#include <exception>
#include <ostream>
#include <array>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>
#include <fstream>



#ifndef Set_hpp
#define Set_hpp

using namespace std;


#define N 2000
#define M 100
#define Q 0.3

#define gamma 0.5772156649


template <class T >
class Set {
private:



    const int h_limit = 120; // Height Limit
    vector<T> values;
    double q; // = 1 - p
    
    struct Node {
        T val; 
        int height;
        Node** next;
        
        Node(T v, int h) {
            val = v;
            height = h;
            next = new Node*[h];
            while (--h >= 0) next[h] = nullptr;
        }
        
        ~Node() {
           delete [] next;
        }

    friend ostream & operator << (ostream &out,  Node * nd)
    {   
        while (nd != nullptr){
            for (int i=0 ; i<nd->height ; i++){
                out<< nd->val<< " ";
            }
            out <<endl;
            nd=nd->next[0];
        }
        return out;
    }

    T getVal() const {return val;}
    };
    
    int geometricalHeight() {
        geometric_distribution<int> distribution(1-q);
        random_device rd;
        default_random_engine generator(rd());
        return distribution(generator)+1;
    }
    
    
    Node* searchBiggerValue(T value, Node** prev) {
        Node* it = head;
        int level = h_limit-1;
        while (true) {
            Node* next = it->next[level];
            if (next && next->val < value) {
                it = next;
            } else {
                if (prev)  prev[level] = it;
                
                if (level == 0) {
                    return next;
                } else {
                    --level;
                }
            }
        }
    }
    
    Node* head;
    
public:
    Set(int n , double qq) {
        head = new Node(0, h_limit);
        q=qq;
        values.resize(n);
        iota(values.begin(), values.end(), 1);
        for (int i =1 ; i<n+1; i++){
            this->insert(i);
        }
    }

    ~ Set (){}; // destructor

    double getQ() const {return q;};//Getter for q

    int getN() const {return values.size();};//Getter for n

    Node* getHead() const {return head;}

    vector<T> getValues() const {return values;}

    Set & operator =( const Set & S ){
    q=S.getQ();
    values=S.getValues();
    return *this;
    } // assigment
    
    bool contains(T value) {
        Node* node = searchBiggerValue(value, nullptr);
        return node != nullptr && node->val == value;
    }
    
    void insert(T x) {
        Node* prev[h_limit];
        searchBiggerValue(x, prev);
        
        Node* node = new Node(x, geometricalHeight());  
        for (int i = 0; i < node->height; ++i) {
            node->next[i] = prev[i]->next[i];
            prev[i]->next[i] = node;
        }    
    }

    void insert_set(T x){
        typename vector<T>::const_iterator it = values.begin();
        if (it==values.end()) //Empty Set
            values.push_back(x);
        else
            while (x > *it)
                it++;
            values.insert(it,x);
    }
    
    bool remove(T x) {
         typename vector<T>::const_iterator it = values.begin();
        while (*it != x && it != values.end())
            it++;
        while (*it==x)
            values.erase(it);
            it++;
            

        Node* prev[h_limit];
        Node* val_to_remove = searchBiggerValue(x, prev);
        if (val_to_remove == nullptr || val_to_remove->val != x) {
            return false;
        }
        
        for (int i = 0; i < val_to_remove->height; ++i) {
            prev[i]->next[i] = val_to_remove->next[i];
        }
        
        delete val_to_remove;
        return true;}


int totalSearchCost(Node* node ,int size) const;

    // Calculate the number of pointers in a skip list
int number_pointers(Node* head) const {
    // Start with a count of 0 pointers
    int count = 0;

    // Traverse the levels of the skip list
    for (int level = head->height - 1; level >= 0; --level) {
        // Set the current node to the next node at this level
        Node* curr = head->next[level];

        // Increment the pointer count for each pointer in the list
        while (curr) {
            curr = curr->next[level];
            count++;
        }
    }

    // Return the total number of pointers
    return count;
}

int find_max_height(Node* head) const {
        int max_height = 0;
        // Traverse the skiplist
        while (head != nullptr) {
            // Update the maximum height if the current node's height is greater
            if (head->height > max_height) {
                max_height = head->height;
            }
            // Move to the next node in the skiplist
            head = head->next[0];
        }

        return max_height;
    }


};


template <class T >
int Set<T>::totalSearchCost(Node* node, int size) const
{
    // If the set is empty, there is no search cost.
    if (node == nullptr)
    {
        return 0;
    }

    // Find the first element of height h in the skip list.
    int k = 0;
    while (node->next[0] != nullptr && node->next[0]->val < size)
    {
        node = node->next[0];
        k = node->val;
    }

    // Calculate the total search cost of the sub skiplist with elements 1 to k - 1.
    int cost1 = this->totalSearchCost(node->next[0], k - 1);

    // Calculate the total search cost of the sub skiplist with elements k + 1 to n.
    int cost2 = 0;
    while (node->next[0] != nullptr)
    {
        node = node->next[0];
        cost2++;
    }

    return size + cost1 + cost2;
}



#endif