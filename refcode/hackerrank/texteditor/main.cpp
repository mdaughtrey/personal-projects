#include <iostream>
#include <vector>
#include <string>
#include <stack>

using namespace std;

typedef vector<int> Cont;
typedef Cont::iterator ContIter;

int main(int argc, char * argv[])
{
    int numElems;
    cin >> numElems;
    Cont container;
    string line;
    stack<string> undo;

    for (int ii = 0; ii < numElems; ii++)
    {
        int command;
        cin >> command;
        string sData;
        int iData;
        switch (command)
        {
            case 1: // append

                cin >> sData;
                undo.push(line);
                line += sData;
                break;

            case 2: // delete
                cin >> iData;
                undo.push(line);
                line = line.substr(0, line.length() - iData);
                break;

            case 3: // print
                cin >> iData;
                cout << line[iData - 1] << endl;;
                break;

            case 4: // undo
                line = undo.top();
                undo.pop();
                break;
        }

    }
    return 0;
}

