#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int Apply (int (*fun) (int), int arg)
{
    return fun(arg);
}

class Range {

public:

    // The range from begin(inclusive) to end (exclusive)
    Range (int begin, int end)
    {
        if (begin > end) throw std::invalid_argument ("Beginning should be lesser than or equal to end!");
        _begin = begin;
        _end = end;
    }

    class Iterator
        : public std::iterator<std::forward_iterator_tag, int>
    {

        public:
            Iterator (int n, int e) : cur(n), end(e) {}

            Iterator& operator=(const Iterator &other) { cur = other.cur; end = other.end; return *this;}

            bool operator== (const Iterator& other) { return cur == other.cur; }

            bool operator!= (const Iterator& other) { return cur != other.cur; }

            Iterator& operator++() { if (cur < end) cur++; return *this;}
            Iterator& operator++(int) { Iterator t(*this); ++(*this); return t;}

            int operator*() { return cur;}
        private:
            int cur, end;
    };

    Iterator begin() { return Iterator(_begin, _end);}
    Iterator end() { return Iterator(_end, _end);}

private:
    int _begin, _end;

};

int main()
{
    Range r(1,10);

    for (auto it : r) cout << it << " ";
    cout << endl;
    for_each (r.begin(), r.end(), [] (int n) { cout << 2*n + 1 << " ";});
    cout << endl;

    cout << all_of (r.begin(), r.end(), [] (int n) -> int { return n < 0; }) << endl;
    cout << any_of (r.begin(), r.end(), [] (int n) -> int { return n == 8; }) << endl;

    // Anonymous functions can be used where function pointers are expected
    cout << Apply ([] (int n) -> int { return n+2;}, 5) << endl;
    return 0;
}
