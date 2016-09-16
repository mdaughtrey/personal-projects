#include <iostream>
#include <vector>
#include <stack>

using namespace std;

int main(int argc, char * argv[])
{
    int numElems;
    cin >> numElems;
    stack<pair<int, int> > elems; 

    for (int ii = 0; ii < numElems; ii++)
    {
        int query;
        int value;
        cin >> query;
        if (1 == query)
        {
            cin >> value;
            if (!elems.empty())
            {
                elems.push(make_pair(value, max(value, elems.top().second)));
            }
            else
            {
                elems.push(make_pair(value, value));
            }
        }
        else if (2 == query)
        {
            elems.pop();
        }
        else if (3 == query)
        {
            cout << elems.top().second << endl;;
        }
    }
    return 0;
}

