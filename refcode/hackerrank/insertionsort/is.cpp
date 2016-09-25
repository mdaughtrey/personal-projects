#include <iostream>
#include <list>
#include <vector>

using namespace std;

typedef vector<int> Cont;
typedef Cont::iterator ContIter;
typedef Cont::reverse_iterator ContRevIter;

void dumpArray(Cont & nums)
{
    cout << "Array is ";
    for (ContIter iter = nums.begin(); iter != nums.end(); iter++)
    {
        cout << *iter << " ";
    }
    cout << endl;
}

int main(int argc, char * argv[])
{
    Cont nums;
    int numElems;
    cin >> numElems;

    for (int ii = 0; ii < numElems; ii++)
    {
        nums.push_back(0);
        cin >> nums.back();
    }
    //dumpArray(nums);

    for (int ii = 1; ii < nums.size(); ii++)
    {
        int idx(0);
        for (int jj = ii - 1; jj >= 0; jj--)
        {
            if (nums[jj] <= nums[ii])
            {
                idx = jj + 1;
                break;
            }
        }
        int value(nums[ii]);
        nums.erase(nums.begin() + ii);
        nums.insert(nums.begin() + idx, value);
    }
    dumpArray(nums);
}
