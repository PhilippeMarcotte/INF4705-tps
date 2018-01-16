#include <vector>
#include <fstream>
#include <functional>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <iterator>


using Int = long long;
using Algo = const std::function<void(std::vector<Int>&)>&;

void stdsort(std::vector<Int>& numbers) {
    std::sort(numbers.begin(), numbers.end());
}

//Rosetta code http://rosettacode.org/wiki/Sorting_algorithms/Merge_sort#C.2B.2B
template<typename RandomAccessIterator>
void mergesort_(RandomAccessIterator first, RandomAccessIterator last)
{
    if (last - first > 1)
    {
    RandomAccessIterator middle = first + (last - first) / 2;
    mergesort_(first, middle);
    mergesort_(middle, last);
    std::inplace_merge(first, middle, last, std::less<typename std::iterator_traits<RandomAccessIterator>::value_type>());    
    }
}

void mergesort(std::vector<Int>& numbers)
{
  mergesort_(numbers.begin(), numbers.end());
}

//Rosetta code http://rosettacode.org/wiki/Sorting_algorithms/Insertion_sort#C.2B.2B
template <typename RandomAccessIterator>
void insertion_sort_(RandomAccessIterator begin, RandomAccessIterator end) {
  for (auto i = begin; i != end; ++i) {
    std::rotate(std::upper_bound(begin, i, *i, std::less<typename std::iterator_traits<RandomAccessIterator>::value_type>()), i, i + 1);
  }
}
 
void insertion_sort(std::vector<Int>& numbers) {
    insertion_sort_(numbers.begin(), numbers.end());
}

int threshold = 0;
void mergesort_threshold(std::vector<Int>& numbers) {
    auto first = numbers.begin();
    auto last = numbers.end();
    
    if (last - first < threshold)
    {
        insertion_sort(numbers);
    }
    else
    {
        RandomAccessIterator middle = first + (last - first) / 2;
        mergesort_(first, middle);
        mergesort_(middle,last);
    }
    // RECOMBINER
    //return void
}

void run(Algo algo, std::vector<Int>& numbers, bool print_res, bool print_time) {
    using namespace std::chrono;
    auto start = steady_clock::now();
    algo(numbers);
    auto end = steady_clock::now();

    if (print_time) {
        duration<double> s = end-start;
        std::cout << std::fixed << s.count() << std::endl;
    }

    if (print_res) {
        std::cout << std::fixed;
        for (auto n : numbers)
            std::cout << n << std::endl;
    }
}

int main(int argc, char *argv[]) {
    struct {
        std::string algo;
        std::string file_path;
        bool print_res{false};
        bool print_time{false};
        int threshold{0};
    } prog_args;

    // Read program arguments
    for (int i=1; i<argc; i++) {
        std::string arg(argv[i]);
        if (arg == "-a") {
            prog_args.algo = argv[i+1]; i++;
        } else if (arg == "-e") {
            prog_args.file_path = argv[i+1]; i++;
        } else if (arg == "-p") {
            prog_args.print_res = true;
        } else if (arg == "-t") {
            prog_args.print_time = true;
        }
    }

    // Read numbers into vector
    std::vector<Int> numbers;
    {
        std::fstream ex_file(prog_args.file_path);
        Int n;
        while (ex_file >> n)
            numbers.push_back(n);
    }

    // Apply correct algorithm
    if (prog_args.algo == "stdsort")
        run(stdsort, numbers, prog_args.print_res, prog_args.print_time);
    else if(prog_args.algo == "mergesort")
        run(mergesort, numbers, prog_args.print_res, prog_args.print_time);
    else if(prog_args.algo == "insertion_sort")
        run(insertion_sort, numbers, prog_args.print_res, prog_args.print_time);
}