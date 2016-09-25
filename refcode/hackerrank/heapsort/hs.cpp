#include <iostream>
#include <vector>
#include <list>

#include <../node.h>
#include <../myutil.h>

using namespace std;

typedef vector<int> Cont;
typedef Cont::iterator ContIter;

typedef std::list<Node *> Nodes;
typedef Nodes::iterator NodesIter;

int main(int argc, char * argv[])
{
    int numElems;
    cin >> numElems;
    Cont container;

    for (int ii = 0; ii < numElems; ii++)
    {
        container.push_back(0);
        cin >> container.back();
    }

    //Nodes * nodes(myutil::heapify<Cont, Node>(container));
    Node * root(container.front());
    Node * nodes(myutil::heapify(root, container, 1);
    myutil::dumpNodes(nodes);
    return 0;
}

