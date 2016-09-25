#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

typedef std::vector<int> Cont;
typedef Cont::iterator ContIter;

void dumpArray(Cont & nums)
{
    cout << "Array is ";
    for (ContIter iter = nums.begin(); iter != nums.end(); iter++)
    {
        cout << *iter << " ";
    }
    cout << endl;
}

void dandc(Cont & vals, int left, int right)
{
    //cout << "Left " << left << " Right " << right << endl;
    if (0 == (right - left))
    {
        return;
    }
    if (1 == (right - left))
    {
        if (vals[right] < vals[left])
        {
            swap(vals[right], vals[left]);
            //dumpArray(vals);
        }
        return;
    }
    int partSize((right - left) / 2);
    int partIndex = left + partSize;
    dandc(vals, left, partIndex);
    dandc(vals, partIndex + 1, right);

    // Divide and conquer
    Cont temp1(vals.begin() + left, vals.begin() + partIndex + 1);
    Cont temp2(vals.begin() + partIndex + 1, vals.begin() + right + 1);

    //cout << "temp1 "; dumpArray(temp1);
    //cout << "temp2 "; dumpArray(temp2);

    // Merge
    for (ContIter iter = vals.begin() + left; iter != vals.begin() + right + 1; iter++)
    {
        if (temp1.empty() && !temp2.empty())
        {
            *iter = temp2.front();
            temp2.erase(temp2.begin());
        }
        else if (temp2.empty() && !temp1.empty())
        {
            *iter = temp1.front();
            temp1.erase(temp1.begin());
        }
        else if (temp1.front() < temp2.front())
        {
            *iter = temp1.front();
            temp1.erase(temp1.begin());
        }
        else 
        {
            *iter = temp2.front();
            temp2.erase(temp2.begin());
        }
    }

    //dumpArray(vals);
}

int main(int argc, char * argv[])
{
    int numElems;
    cin >> numElems;

    Cont vals;

    for (int ii = 0; ii < numElems; ii++)
    {
        vals.push_back(0);
        cin >> vals.back();
    }
    dandc(vals, 0, vals.size() - 1);
    dumpArray(vals);

}
