#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

using namespace std;

/*
 * Complete the function below.
  */
typedef vector<string> Friend;
typedef vector<string>::iterator FriendIter;
typedef vector<pair<int, int> > Circle;
typedef Circle::iterator CircleIter;
typedef vector<Circle> Circles;
typedef Circles::iterator CirclesIter;

void dumpCircles(Circles & circles)
{
    int circleCount(0);
    for (CirclesIter iter = circles.begin(); iter != circles.end(); iter++)
    {
        if (!iter->empty())
        {
            circleCount++;
        }
        for (CircleIter iter2 = iter->begin(); iter2 != iter->end(); iter2++)
        {
            cout << iter2->first << "," << iter2->second << " ";
        }
        cout << endl;
    }
    cout << circleCount << endl;
}

int lookAround(Friend & friends, int ii, int jj, Circles & circles, Circle & discovered)
{
    cout << "looking for a friend at " << ii << ", " << jj << endl;
    dumpCircles(circles);
    if (ii < 0 || jj < 0 || ii > friends.size() - 1 || jj > friends[0].length() - 1)
    {
        //        cout << "out of bounds" << endl;
        return 0; // out of bounds
    }
    //    cout << "bounds ok" << endl;
    if (friends[ii][jj] == 'N')
    {
        //        cout << "not a friend" << endl;
        return 0;    // no friends here
    }
    //    cout << "I am someones friend" << endl;
    if (std::find(discovered.begin(),
                discovered.end(),
                make_pair(ii, jj)) != discovered.end())
    {
        cout << "already a friend" << endl;
        return 0; // already a friend
    }
    //    cout << "I am a new friend" << endl;
    circles.back().push_back(make_pair(ii, jj));
    discovered.push_back(make_pair(ii, jj));
    int newFriends(
            + lookAround(friends, ii, jj - 1, circles, discovered) // look left
            + lookAround(friends, ii, jj + 1, circles, discovered) // look right
            + lookAround(friends, ii - 1, jj, circles, discovered) // look up
            + lookAround(friends, ii + 1, jj, circles, discovered)); // look down
    return newFriends;
}

typedef vector<string>::iterator FriendIter;
int friendCircles(vector < string > friends) {
    Circles circles;
    circles.push_back(Circle());
    Circle discovered;
    for (int ii = 0; ii < friends.size(); ii++)
    {
        for (int jj = 0; jj < friends[ii].length(); jj++)
        {
            if (std::find(discovered.begin(), discovered.end(), make_pair(ii, jj))
                    != discovered.end())
            {
                continue;
            }
            cout << "looking from main, " << ii << ", " << jj << endl;
           lookAround(friends, ii, jj, circles, discovered);
           if (!circles.back().empty())
           {
            circles.push_back(Circle());
           }
        }
    }
    cout << "found " << circles.size() << " circles" << endl;
    dumpCircles(circles);
    return 0;
}

int main() {
    ofstream fout(getenv("OUTPUT_PATH"));
    int res;

    int _friends_size = 0;
    cin >> _friends_size;
    cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n'); 
    vector<string> _friends;
    string _friends_item;
    for(int _friends_i=0; _friends_i<_friends_size; _friends_i++) {
        getline(cin, _friends_item);
        _friends.push_back(_friends_item);
    }

    res = friendCircles(_friends);
    fout << res << endl;

    fout.close();
    return 0;
}

