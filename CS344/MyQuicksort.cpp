#include<iostream>
#include<cstdlib>
#include<time.h>
#include<cmath>
using namespace std;

template<typename T>
int part(T a[], int i, int j, int k) {
    int p = i;
    int q = i;
    int r = j - 1;
    swap(a[r], a[k]);
    int pivot = a[r];
    while(q < r)    {
        if(a[q] <= pivot)   {
            swap(a[p], a[q]);
            ++p;
        }
        ++q;
    }
    swap(a[p], a[q]);
    k = p;
    return k;
}

template<typename T>
void quicksort(int a[], int i, int j)  {
    if(j - i > 1)    {
        srand (time(NULL));

        //Choose a pivot
        //random
        int size_a = j - i;
        int pivot = (rand() % size_a) + i;

        //first
//        int pivot = i;




        //Partition around pivot                this will change depending on the partition algorithm used
        int k = part(a, i, j, pivot);

        //Sort recursively
        quicksort(a, i, k);
        quicksort(a, k + 1, j);
    }
}

 int main() {
    srand (time(NULL));
    int n = 9;
    int a[n] = {22, 37, 25, 60, 16, 42, 38, 46, 19};
    int input_size = sizeof(a)/sizeof(a[0]);

    //Print the presorted array
    cout << "Before: ";
    for(int i = 0; i < input_size; ++i)
        cout << a[i] << ' ';
    cout << endl << endl;

    //Sort the array
    quicksort(a, 0, input_size);
//    part(a, 0, input_size, 1);
//    part(a, 0, 4, 0);

    cout << endl;
    //Print the post sorted array
    cout << "After: ";
    for(int i = 0; i < input_size; ++i)
        cout << a[i] << ' ';
    cout << endl;


    return 0;
 }

