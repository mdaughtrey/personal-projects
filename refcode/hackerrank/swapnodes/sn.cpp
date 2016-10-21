#include <iostream>
#include <vector>
#include <map>
#include <utility>

typedef struct node
{
    int data;
    int left;
    int right;
} Node;

using namespace std;

typedef vector<Node> Nodes;
typedef Nodes::iterator NodesIter;
typedef map<int, int> Depths;
typedef Depths::iterator DepthsIter;

void dumpInOrder(Nodes & nodes, int root)
{
    if (-1 != nodes[root].left)
    {
        dumpInOrder(nodes, nodes[root].left);
    }
    cout << nodes[root].data << " ";
    if (-1 != nodes[root].right)
    {
        dumpInOrder(nodes, nodes[root].right);
    }
}

void dumpArray(Nodes & nodes)
{
    cout << "Nodes length " << nodes.size() << endl;
    NodesIter iter(nodes.begin());
    iter++;
    for ( ; iter != nodes.end(); iter++)
    {
        cout << iter->data << ": " << iter->left << "," << iter->right << endl;
    }
}

void getDepths(Nodes & nodes, int pos, Depths & depths, int depth)
{
    if (depths.find(depth) == depths.end())
    {
        depths[depth] = pos;
    }
    else
    {
        depths[depth] = min(depths[depth], pos);
    }
    if (-1 != nodes[pos].left)
    {
        getDepths(nodes, nodes[pos].left, depths, depth + 1);
    }
    if (-1 != nodes[pos].right)
    {
        getDepths(nodes, nodes[pos].right, depths, depth + 1);
    }
}

int main(int argc, char * argv[])
{
    int numNodes;
    cin >> numNodes;
    Nodes nodes;
    nodes.resize(numNodes +1);
    for (int ii = 1; ii <= numNodes; ii++)
    {
        Node node { ii };
        cin >> node.left >> node.right;
        nodes[node.data] = node;
    }

//    dumpInOrder(nodes, 1);
//    dumpArray(nodes);
//    cout << endl;

    Depths depths;
    getDepths(nodes, 1, depths, 1);
//    for (DepthsIter iter = depths.begin(); iter != depths.end(); iter++)
//    {
//        cout << iter->first << " " << iter->second << endl;
//    }

    int numSwaps;
    cin >> numSwaps;
    for (int ii = 0; ii < numSwaps; ii++)
    {
        int level;
        cin >> level;
        int thisLevel(level);

        while (depths.find(thisLevel) != depths.end())
        {
            int startAt(depths[thisLevel]);
            int stopAt;
            if (depths.find(thisLevel + 1) == depths.end())
            {
                stopAt = nodes.size();
            }
            else
            {
                stopAt = depths[thisLevel + 1];
            }

            for (int ii = startAt; ii < stopAt; ii++)
            {
                swap(nodes[ii].left, nodes[ii].right);
            }
            thisLevel += level;
        }
        dumpInOrder(nodes, 1);
        cout << endl;
    }
}
