#include <iostream>
#include <string>
#include <algorithm>
#include <list>
#include <deque>
#include <sstream>
#include <set>

using namespace std;

void dump(deque<int> & container)
{
    for (deque<int>::iterator iter = container.begin(); iter != container.end(); iter++)
    {
        cout << *iter << " ";
    }
    cout << endl;
}

void dump(list<deque<int> > & deques)
{
    for (list<deque<int> > ::iterator iter = deques.begin(); iter != deques.end(); iter++)
    {
        dump(*iter);
    }
}
//void dump(vector<int> & container)
//{
//    for (vector<int>::iterator iter = container.begin(); iter != container.end(); iter++)
//    {
//        cout << *iter << " ";
//    }
//    cout << endl;
//}

int main(int argc, char * argv[])
{
    string sizes;
    getline(cin, sizes);
    list<deque<int> > stacks;
    deque<int> heights;
    
    stringstream sstr(sizes);

    while (!sstr.eof())
    {
        int thissize;
        sstr >> thissize;

        stacks.push_back(deque<int>());
        heights.push_back(0);
        for (int ii = 0; ii < thissize; ii++)
        {
            int elem;
            cin >> elem;
            stacks.back().push_back(elem);
            heights.back() += elem;
        }
    }
    do
    {
//        cout << "Heights ";
//        dump(heights);
//        cout << "Stacks ";
//        dump(stacks);

        // collapse heights into set to remove dups
        set<int> currentHeights(heights.begin(), heights.end());
        if (1 == currentHeights.size())
        {
            cout << *currentHeights.begin() << endl;
            // all heights are the same, we're done
            break;
        }
        // find maximum height value
        int maxHeight(*max_element(currentHeights.begin(), currentHeights.end()));
//        cout << "maxHeight " << maxHeight << endl;
        // point iter to maximum height entry
        deque<int>::iterator iMaxHeight(find(heights.begin(), heights.end(), maxHeight));

        // get iterator to reference stack with maximum height
        list<deque<int> >::iterator iStack(stacks.begin());
        int ii(distance(heights.begin(), iMaxHeight));
//        cout << "Stack index to pop is " << ii << endl;
        advance(iStack, distance(heights.begin(), iMaxHeight));

        // reduce this stacks height
//        cout << "Value to remove is " << iStack->front() << endl;
        *iMaxHeight -= iStack->front();
//        cout << "New height for this stack is " << *iMaxHeight << endl;
        iStack->pop_front();

    }
    while (true);
    return 0;
}

