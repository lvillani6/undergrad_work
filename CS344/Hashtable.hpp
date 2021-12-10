#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <vector>
#include <iostream>

using namespace std;


template<class T>
class Hashtable {
    private:
        // the raw vector table containing all buckets with all current
        // key/element pairs
        vector<vector<pair<int, T>>> values;
        
        // the number of elements within the table (NOT the table capacity)
        int size;        
        
        
        
        
        
        // UTILITY FUNCTION:
        // adds a key element pair to a raw vector table (argument 3)...
        // returns the pointer to the element (that was inserted) in the table
        // and returns whether or not the insert operation increased the number
        // of elements within the table (will be false if a previous element
        // replaced another element)
        pair<T*,bool> addToHashtable(int key, T element, vector<vector<pair<int, T>>> & hashtable) {
            
            int index = key % hashtable.size();
            vector<pair<int, T>> * bucket = & hashtable[index];
            
            for(int i = 0; i < bucket->size(); i++){
                pair<int, T> *p = &(*bucket)[i];
                if(p->first == key) {
                    (*bucket)[i] = pair<int, T>(key, element);
                    T* ptr = &((*bucket)[i].second);
                    pair<T*, bool> p2 = pair<T*, bool>(ptr, false); 
                    return p2; // element replaced another in list
                }
            }
            
            bucket->push_back(pair<int, T>(key, element));
            T* ptr = &((*bucket)[bucket->size()-1].second);
            pair<T*, bool> p2 = pair<T*,bool>(ptr, true);
            return p2; // element increase size of list (return ptr)
        }
        
        
        
        // UTILITY FUNCTION:
        // checks the load factor at a specific time...
        // if the load factor is >= 1, then the table capacity will increase to
        // 2n-1 (n is the previous table capacity) and all elements previously
        // hashed within the table will be rehashed into the newly sized table
        void checkM(){
            if(size - values.size() == 0) {
                vector<vector<pair<int, T>>> temp;
                temp.resize(values.size() * 2 + 1);
                
                for(int i = 0; i < values.size(); i++){
                    vector<pair<int, T>> * bucket = &values[i];
                    for(int j = 0; j < bucket->size(); j++) {
                        pair<int,T> *p = &(*bucket)[j];
                        
                        addToHashtable(p->first, p->second, temp);
                    }
                }
                values = temp;
            }
        }
        
        
        
        // UTILITY FUNCTION:
        // returns the pointer to the element (that was inserted) in the table
        // and returns whether or not the insert operation increased the number
        // of elements within the table (will be false if a previous element
        // replaced another element)
        pair<T*,bool> insert_ret_ptr(int key, T element) {
            pair<T*,bool> outcome = addToHashtable(key, element, values);
            
            if(outcome.second){
                size++;
                checkM();
            }
            
            return outcome;
        }
        
        
        
        // UTILITY FUNCTION:
        // returns the hashed value of a arbitrary key 
        // (NOTE: values.size() is current capacity NOT the number of elements
        // in the table)
        int hash(int key){
            return key % values.size();
        }
        
        
    public:
        Hashtable() {
            size = 0;
            values.resize(3); // initialize table capacity to 3
        }
        
        
        
        
        int getSize() {
            return size;
        }
        
        
        
        
        bool isEmpty() {
            return size == 0;
        }
        
        
        
        
        float getLoadFactor() {
            return  (float) size / (float) values.size();
        }
        
        
        
        
        
        T* find(int key) {
            int index = hash(key);
            vector<pair<int, T>> *bucket = &values[index];
     
            for(int i = 0; i < bucket->size(); i++){
                pair<int, T> *p = &(*bucket)[i];
                if(p->first == key)
                    return &(p->second);
            }
            
            return nullptr;
        }
        
        
        
        
        void erase(int key) {
            int index = hash(key);
            vector<pair<int, T>> *bucket = &values[index];

            for(int i = 0; i < bucket->size(); i++){
                pair<int, T> *p = &(*bucket)[i];
                if(p->first == key) {
                    bucket->erase(bucket->begin() + i);
                    size--;
                    return;
                }
            }
        }
        
        
        
        
        
        
        bool insert(int key, T element) {
            return insert_ret_ptr(key, element).second;
        }
        
        
        
        
        
        
        T &operator[] (int key) {
            int index = hash(key);
            vector<pair<int, T>> *bucket = &values[index];
            
            for(int i = 0; i < bucket->size(); i++){
                pair<int, T> *p = &(*bucket)[i];
                if(p->first == key)
                    return p->second;
            }
            
            // create blank element a give a reference to it
            T elem;
            return *(insert_ret_ptr(key, elem).first);
        }
        
        
        
        
        
        void clear() {
            size = 0;
            values.clear();
            values.resize(3);
        }
        
        
        
        
        // prints HashTable
        void print() {
            cout << "[ size = " << getSize() << endl;
            for(int i = 0; i < values.size(); i++){
                vector<pair<int, T>> *bucket = &values[i];
                for(int j = 0; j < bucket->size(); j++) {
                    pair<int,T> *p = &(*bucket)[j];
                    cout << p->first << " : " <<  p->second << endl;
                }
            }
            cout << "-----]" << endl;
        }
};


#endif /* HASHTABLE_HPP */
