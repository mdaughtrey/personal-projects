#include <iostream>
#include <algorithm>
#include <deque>

using namespace std;

void dump(deque<int> & stk)
{
    for (deque<int>::iterator iter = stk.begin(); iter != stk.end(); iter++)
    {
        cout << *iter << " ";
    }
    cout << endl;
}

int main(int argc, char * argv[])
{
    int numElems;
    cin >> numElems;
    deque<int> indices;
    deque<int> heights;

    int ii;
    int maxArea(0);
    for (ii = 0; ii < numElems; ii++)
    {
        int height;
        cin >> height;
        if (indices.empty() || height > heights.front())
        {
            indices.push_front(ii);
            heights.push_front(height);
            continue;
        }

        if (height < heights.front())
        {
            int lastIndex;
            while (!heights.empty() && height < heights.front())
            {
                maxArea = max(maxArea,
                        heights.front() * (ii - indices.front()));
                heights.pop_front();
                lastIndex = indices.front();
                indices.pop_front();
            }
            heights.push_front(height);
            indices.push_front(lastIndex);
        }
    }

    while (!heights.empty())
    {
        maxArea = max(maxArea,
                heights.front() * (ii - indices.front()));
        heights.pop_front();
        indices.pop_front();
    }

    cout << maxArea << endl;
    return 0;
}
