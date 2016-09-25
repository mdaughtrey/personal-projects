#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <algorithm>
#include <list>
#include <deque>

#define VERBOSE
using namespace std;

typedef list<unsigned long> Cont;
typedef Cont::iterator ContIter;

#ifdef VERBOSE
void dump(Cont & container)
{
    for (ContIter iter = container.begin(); iter != container.end(); iter++)
    {
        cout << *iter << " ";
    }
    cout << endl;
}
#endif // VERBOSE

void strategy1()
{
    int numElems;
    cin >> numElems;
    Cont container;

    for (int ii = 0; ii < numElems; ii++)
    {
        container.push_back(0);
        cin >> container.back();
    }
    for (int day = 0; ; day++)
    {
        list<ContIter> toDelete;
        ContIter iThis(container.begin());
        ContIter iPrev(iThis);
        iThis++;
        while (iThis != container.end())
        {
            if (*iThis > *iPrev)
            {
                toDelete.push_back(iThis);
            }
            iThis++;
            iPrev++;
        }
        if (toDelete.empty())
        {
            cout << day << endl;
            break;
        }
//        cout << "toDelete ";
 //       dump(toDelete);
        for (list<ContIter>::reverse_iterator iDel = toDelete.rbegin(); iDel != toDelete.rend(); iDel++)
        {
            container.erase(*iDel);
        }
 //       dump(container);
    } 
}

typedef vector<unsigned long> Vec;
typedef Vec::iterator VecIter;
typedef deque<pair<int, int> > Killers;
typedef Killers::iterator KillersIter;

void dumpKillers(Killers & killers)
{
    for (KillersIter iter = killers.begin(); iter != killers.end(); iter++)
    {
        cout << "Toxicity " << iter->first << " TTL " << iter->second << endl;
    }
}

void strategy2()
{
    int numElems;
    cin >> numElems;
    Vec plants;

    Killers killers; //  toxicity, days to live
    int maxDays(-1);
    //
    // Read in all the plant values
    //
    for (int ii = 0; ii < numElems; ii++)
    {
        plants.push_back(0);
        cin >> plants.back();
    }

    killers.push_front(make_pair(plants[0], -1));

    for (int ii = 1; ii < numElems; ii++)
    {
        int myTTL(1);
        while (!killers.empty())
        {
            //
            // Compare this plant's toxicity to the previous known killer
            //
            int thisTox(plants[ii]);
            int killerTox(killers.front().first);
            cout << "thisTox " << thisTox << " killerTox " << killerTox << endl;

            // If we're stronger than newest killer, remove it from the kill
            // list. Our TTL is the killers TTL + 1
            if (thisTox < killerTox) 
            {
                myTTL = max(killers.front().second + 1, myTTL);
                cout << "myTTL " << myTTL << endl;
                killers.pop_front();
            //    maxDays = max(maxDays, myTTL);
//                cout << "maxDays is now " << maxDays << endl;
            }
            else
            {
                // We will die in 1 day, don't need to run through the
                // rest of the kill list
//                maxDays = max(maxDays, 0);
                break;
            }
            dumpKillers(killers);
        }
        if (killers.empty())
        {
            // The newest plant is stronger than all others,
            // it's the sole occupant of the kill list
            myTTL = -1;
        }
        killers.push_front(make_pair(plants[ii], myTTL));
        maxDays = max(maxDays, myTTL);
        dumpKillers(killers);
        
    }
    maxDays = max(maxDays, 0);
    cout << maxDays << endl;
}

int main(int argc, char * argv[])
{
//    strategy1();
    strategy2();
    return 0;
}

