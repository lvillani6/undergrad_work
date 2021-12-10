#include<iostream>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<algorithm>

using namespace std;

int integer_count = 0;

//5
class Integer    {
   public:
       //default constructor
       Integer()    {
           value = 0;
        }
       //constructor
       Integer(int x)   {
           value = x;
        }
       //copy constructor
       Integer(const Integer& other )   {
           value = other.getValue();
           ++integer_count;
        }
       //value method
       int getValue() const {
           return value;
        }

       const bool operator<(const Integer& i2)  {
           ++integer_count;
           if(getValue() < i2.getValue())
            return true;
           else
            return false;
       }

       Integer operator=(const Integer& i2) {
           ++integer_count;
           value = i2.getValue();
           return *this;
       }
   private:
       int value;
};


//1
template<typename T>
void iterative_insertion_sort(T a[], int i, int j)  {
    for(int k = i + 1; k <= j - 1; ++k)  {
        //insert a[k] into a[i...k)
        T x = a[k];
        int p = k - 1;
        while((p >= i) && (x < a[p]))   {
            a[p+1] = a[p];
            --p;
        }
        a[p+1] = x;
    }
}

template<class T>
int one_scan_part(T a[], int i, int j, int p) {
  T pivot = a[p];

  swap(a[p], a[j - 1]);

  int left = 0;
  int right = j - i - 1;

  T* b = new T[j-i];

  for(int k = i; k < j; k++ ){
    if (a[k] < pivot){
      b[left] = a[k];
      left++;
    } else {
      b[right] = a[k];
      right--;
    }
  }

  for(int k = 0; k < j-i; k++) {
    a[i+k] = b[k];
  }

  delete[] b;

  return left + i;
}

template<typename T>
int in_place_part(T a[], int i, int j, int k) {
    int p = i;
    int q = i;
    int r = j - 1;
    swap(a[r], a[k]);
    T pivot = a[r];
    while(q < r)    {
        if(a[q] < pivot)   {
            swap(a[p], a[q]);
            ++p;
        }
        ++q;
    }
    swap(a[p], a[q]);
    k = p;
    return k;
}

//2
template<typename T>
void first_element_quicksort(T a[], int i, int j)  {
    if(j - i > 1)    {
        //first element
        int pivot = i;

        //Partition around pivot
        int k = one_scan_part(a, i, j, pivot);

        //Sort recursively
        first_element_quicksort(a, i, k);
        first_element_quicksort(a, k + 1, j);
    }
}

//3
template<typename T>
void random_onescan_quicksort(T a[], int i, int j)  {
    if(j - i > 1)    {
        srand (time(NULL));
        //Choose a pivot
        //random
        int size_a = j - i;
        int pivot = (rand() % size_a) + i;

        //Partition around pivot
        int k = one_scan_part(a, i, j, pivot);

        //Sort recursively
        random_onescan_quicksort(a, i, k);
        random_onescan_quicksort(a, k + 1, j);
    }
}

//4
template<typename T>
void random_inplace_quicksort(T a[], int i, int j)  {
    if(j - i > 1)    {
        srand (time(NULL));

        //Choose a pivot
        //random
        int size_a = j - i;
        int pivot = (rand() % size_a) + i;

        //Partition around pivot
        int k = in_place_part(a, i, j, pivot);

        //Sort recursively
        random_inplace_quicksort(a, i, k);
        random_inplace_quicksort(a, k + 1, j);
    }
}

template<class T>
void selection_sort(T a[], int start, int stop) {
    int i = start; int j = stop;
    while(stop - start > 1) {
        auto itr_max = std::max_element(a + start, a + stop);
        std::swap(*itr_max, a[stop - 1]);
        --stop;
    }
}

template <class T>
void my_merge(T a[], int start, int middle, int stop) {
    int i1  = start;
    int i2 = middle;

    T* result = new T[stop - start];

    int j = 0;
    while(i1 < middle && i2 < stop) {
        if ( a[i1] < a[i2] ) {
            result[j] = a[i1];
            ++i1;
        } else{
            result[j] = a[i2];
            ++i2;
        }
        ++j;
    }
    copy(a + i1, a + middle, result + j);
    copy(a + i2, a + stop, result + j);
    copy(result, result + (stop - start), a + start);


    delete [] result;
}

template <class T>
void mergesort(T a[], int start, int stop) {

    if ( stop - start > 1 ) {
        int middle = (start + stop) / 2;

        mergesort(a, start, middle);
        mergesort(a, middle, stop);

        my_merge(a, start, middle, stop);
    }

}

void print_Intarr(Integer a[],int n)   {
    for(int i = 0; i < n; ++i)  {
        cout << a[i].getValue() << ' ';
    }
    cout << endl;
}

int main()  {
    int n = 10000;

    Integer a[n];
    for(int i = 0; i < n; ++i)  {
        a[i] = i + 1;
    }

    Integer b[n];
    for(int i = 0; i < n; ++i)  {
        b[i] = n - i;
    }

    Integer r1[n];
    Integer r2[n];
    Integer r3[n];
    Integer r4[n];
    Integer r5[n];

    srand(time(NULL));
    for(int i = 0; i < n; ++i)  {
        r1[i] = rand();
        r2[i] = rand();
        r3[i] = rand();
        r4[i] = rand();
        r5[i] = rand();
    }

    int random_count = 0;

    integer_count = 0;
//    selection_sort(a, 0, n);
//    iterative_insertion_sort(a, 0, n);
//    mergesort(a, 0, n);
//    first_element_quicksort(a, 0, n);
//    random_onescan_quicksort(a, 0, n);
//    random_inplace_quicksort(a, 0, n);
//    sort(a, a + n);
    cout << integer_count << endl;

    integer_count = 0;
//    selection_sort(b, 0, n);
//    iterative_insertion_sort(b, 0, n);
//    mergesort(b, 0, n);
//    first_element_quicksort(b, 0, n);
//    random_onescan_quicksort(b, 0, n);
//    random_inplace_quicksort(b, 0, n);
//    sort(b, b + n);
    cout << integer_count << endl;

    integer_count = 0;
//    selection_sort(r1, 0, n);
//    iterative_insertion_sort(r1, 0, n);
//    mergesort(r1, 0, n);
//    first_element_quicksort(r1, 0, n);
//    random_onescan_quicksort(r1, 0, n);
//    random_inplace_quicksort(r1, 0, n);
//    sort(r1, r1 + n);
    random_count += integer_count;

    integer_count = 0;
//    selection_sort(r2, 0, n);
//    iterative_insertion_sort(r2, 0, n);
//    mergesort(r2, 0, n);
//    first_element_quicksort(r2, 0, n);
//    random_onescan_quicksort(r2, 0, n);
//    random_inplace_quicksort(r2, 0, n);
//    sort(r2, r2 + n);
    random_count += integer_count;

    integer_count = 0;
//    selection_sort(r3, 0, n);
//    iterative_insertion_sort(r3, 0, n);
//    mergesort(r3, 0, n);
//    first_element_quicksort(r3, 0, n);
//    random_onescan_quicksort(r3, 0, n);
//    random_inplace_quicksort(r3, 0, n);
//    sort(r3, r3 + n);
    random_count += integer_count;

    integer_count = 0;
//    selection_sort(r4, 0, n);
//    iterative_insertion_sort(r4, 0, n);
//    mergesort(r4, 0, n);
//    first_element_quicksort(r4, 0, n);
//    random_onescan_quicksort(r4, 0, n);
//    random_inplace_quicksort(r4, 0, n);
//    sort(r4, r4 + n);
    random_count += integer_count;

    integer_count = 0;
//    selection_sort(r5, 0, n);
//    iterative_insertion_sort(r5, 0, n);
//    mergesort(r5, 0, n);
//    first_element_quicksort(r5, 0, n);
//    random_onescan_quicksort(r5, 0, n);
//    random_inplace_quicksort(r5, 0, n);
//    sort(r5, r5 + n);
    random_count += integer_count;

    cout << random_count/5 << endl;

    return 0;
}
