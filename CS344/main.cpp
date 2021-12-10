#include <cstdlib>
#include <vector>
#include <iostream>
#include <string.h>

#include "Hashtable.hpp"

using namespace std;

void printHeader(string msg);

int main(void) {
    
    Hashtable<int> table;
    
    printHeader("Check table size and if table is Empty");
    cout << "Size: " << table.getSize() << endl;
    cout << "Is Empty: " << table.isEmpty() << endl;
    table.print();
    
    
    printHeader("Add 1000 to key 3:");
    cout << "List length increased: " << table.insert(3, 1000) << endl;
    table.print();
    
    
    printHeader("Replace 1000 with 90 on key 3:");
    cout << "List length increased: " << table.insert(3, 90) << endl;
    table.print();
    
    
    printHeader("Add 1000 to key 4:");
    cout << "List length increased: " << table.insert(4, 1000) << endl;
    table.print();
    
    
    printHeader("Print load factor:");
    cout << "Load Factor: " << table.getLoadFactor() << endl;
    
    
    printHeader("Add 111 to key 1, "
                "Add 222 to key 500, "
                "Add 4321 to key 1234 "
            "(table capacity increases here):");
    cout << "List length increased: " << table.insert(1, 111) << endl;
    cout << "List length increased: " << table.insert(500, 222) << endl;
    cout << "List length increased: " << table.insert(1234, 4321) << endl;
    table.print();
    
    
    printHeader("Print load factor:");
    cout << "Load Factor: " << table.getLoadFactor() << endl;
    
    
    printHeader("Check table size and if table is Empty");
    cout << "Size: " << table.getSize() << endl;
    cout << "Is Empty: " << table.isEmpty() << endl;
    table.print();
    
    
    printHeader("Index table at key 4 (should be 1000)");
    cout << "table[4] = " << table[4] << endl;
    
    
    printHeader("Index table at key 4 to 22 (table[4] = 22)");
    table[4] = 22;
    table.print();
    
    
    printHeader("Index table at key 5 to -50 (table[5] = -50)");
    table[5] = -37;
    table.print();
    
    
    printHeader("Find key 500 (should return pointer)");
    cout << table.find(500) << endl;
    

    printHeader("Attempt to erase key 7 (should not do anything)");
    table.erase(7);
    table.print();
    
    
    printHeader("Attempt to erase key 500 (should erase value 222)");
    table.erase(500);
    table.print();
    
    
    printHeader("Find key 500 (should return null pointer)");
    cout << table.find(500) << endl;
    
    
    printHeader("Attempt to clear table");
    table.clear();
    table.print();
    
    
    printHeader("Check if table is empty (should be true):");
    cout << "Table is empty: " << table.isEmpty() << endl; 
    
    
    return 0;
}


void printHeader(string msg) {
    cout << endl << endl << endl << msg << endl;
}
