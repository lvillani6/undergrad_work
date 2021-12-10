#include<iostream>
using namespace std;

template<typename T>
sequential_search(T a[], int start, int stop, T e)  {
    if(a[start] == e)
        return start;
    else    {
        if(start == stop)
            return -1;
        else
            sequential_search(a, start+1, stop, e);
    }
}
 int main() {
    int a[5] = {24, 75, 23, 86, 94};
    int e;
    cout << "Enter element to search for: " << endl;
    cin >> e;
    int i = sequential_search(a, 0, 4, e);
    if(i == -1)
        cout << "Element not found.\n";
    else
        cout << "Element is in position: " << i << endl;
 }

