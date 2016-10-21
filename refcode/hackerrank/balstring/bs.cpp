#include <iostream>
#include <string>
#include <map>
#include <stack>

using namespace std;

int main(int argc, char * argv[])
{
    map<char, char> matching;
    matching['}'] = '{';
    matching[']'] = '[';
    matching[')'] = '(';
    int numStrings;
    cin >> numStrings;
    for (int ii = 0; ii < numStrings; ii++)
    {
        string aStr;
        cin >> aStr;
        if (aStr.length() % 2 == 1)
        {
            cout << "NO" << endl;
            continue;
        }
        stack<char> aStack;
        bool matched(true);
        for (string::iterator iter = aStr.begin(); iter != aStr.end() && matched; iter++)
        {
            switch (*iter)
            {
                case '{':
                case '[':
                case '(':
                    aStack.push(*iter);
                    break;

                case '}':
                case ']':
                case ')':
                    if (aStack.empty() || aStack.top() != matching[*iter])
                    {
                        matched = false;
                    }
                    else
                    {
                        aStack.pop();
                    }
                    break;

            }
        }
        if (!aStack.empty())
        {
            matched = false;
        }
        if (matched)
        {
            cout << "YES" << endl;
        }
        else
        {
            cout << "NO" << endl;
        }
    }
}
