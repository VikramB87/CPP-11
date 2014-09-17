#include <iostream>
using namespace std;

class TestMove {
public:
    TestMove (int n)
    {
        cout << "In ctor" << endl;
        val = new int(n);
    }

    TestMove (const TestMove &t)
    {
        cout << "In copy ctor" << endl;
        if (t.val == nullptr) val = nullptr;
        else val = new int(*t.val);
    }

    TestMove (TestMove &&t)
    {
        cout << "In move ctor" << endl;
        if (&t == this) return;
        val = t.val;
        t.val = nullptr;
    }

    TestMove& operator=(TestMove &&other)
    {
        cout << "In = operator (move)" << endl;
        if (&other == this) return *this;
        val = other.val;
        other.val = nullptr;
    }

    TestMove& operator=(const TestMove &other)
    {
        cout << "In = operator (copy)" << endl;
        if (&other == this) return *this;
        if (!other.val) val = nullptr;      // Note the implicit conversion to bool
        else val = new int(*other.val);
        return *this;
    }

    int get (void) const { return val ? *val : 0; } // Note the implicit conversion to bool
    void set (int x) { if (val) *val = x; else val = new int(x); }

    void show (void) const
    {
        if (!val) cout << "null" << endl;
        else cout << *val << endl;
    }

private:
    int *val;
};

void fun1(const TestMove &t)
{
    cout << "In fun1" << endl;
}

void fun2(TestMove t)
{
    cout << "In fun2" << endl;
}

void fun3(TestMove &&t)
{
    cout << "In fun3" << endl;
    t.show();
}


TestMove fun4(const TestMove &t)
{
    TestMove r(t.get() + 10);
    cout << "In fun4" << endl;
    return r;           // Calls move ctor for temporary object
}

int main()
{
    TestMove a(10);     // Calls ctor
    TestMove b = a;     // Calls copy ctor
    TestMove b1(b);     // Calls copy ctor
    TestMove b2(20);
    b2 = b1;            // Call copy assignment operator

    b2.show ();         // Prints 10
    TestMove c = std::move(b1); // Calls move ctor
    // TestMove c(std::move(b1)); // Also Calls move ctor
    b1.show ();         // Is null; ownership is transferred to c
    c.show ();          // Prints 10

    fun1 (c);           // Call-by-ref, nothing called
    fun2 (c);           // Call-by-value - copy ctor called
    fun3 (std::move(c)); // Move ctor may be skipped (copy-elision?)
    c.show ();          // 10 if move ctor is skipped else 0

    TestMove d = fun4(b2);
    d.show ();          // 20
    return 0;
}

