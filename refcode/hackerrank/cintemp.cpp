#include <iostream>
#include <vector>

using namespace std;

typedef vector<int> Cont;
typedef Cont::iterator ContIter;

void dump(Cont & container)
{
    for (ContIter iter = container.begin(); iter != container.end(); iter++)
    {
        cout << *iter << " ";
    }
    cout << endl;
}


int main(int argc, char * argv[])
{
    int numElems;
    cin >> numElems;
    Cont container;

    for (int ii = 0; ii < numElems; ii++)
    {
        container.push_back(0);
        cin >> container.back();
    }
    return 0;
}

