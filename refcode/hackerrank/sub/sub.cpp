#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int longest(0);

int recurse(vector<string> & wordset, std::string word, int length)
{
    if (word.empty())
    {
        return length;
    }
    for (int ii = 0; ii < word.length(); ii++)
    {
        string sliced(word);
        sliced.erase(ii, 1);
        if (std::find(wordset.begin(),
            wordset.end(),
            sliced) != wordset.end())
        {
            length++;
            length += recurse(wordset, sliced, length);
        }
    }
    return length;
}

int longestChain(vector<string> & words, int chainLength)
{
    int longest(0);
    for (vector<string>::iterator iter = words.begin();
        iter != words.end();
        iter++)
    {
        cout << "New word " << *iter << endl;
        if (1 == iter->length())
        {
            continue;
        }
        longest = max(longest, recurse(words, *iter, 0));
    }
    return longest;
}

int main(int argc, char * argv[])
{
    int numlines;
    cin >> numlines;
    vector<string> words;
    for (int ii = 0; ii < numlines; ii++)
    {
        string word;
        cin >> word;
        words.push_back(word);
    }
    int chainLength(0);
    cout << longestChain(words, chainLength) << endl;
}
