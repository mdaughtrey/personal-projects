// C++ program to find maximum rectangular area in 
// linear time 
#include<iostream> 
#include<fstream>
#include<stack> 
#include<random>
#include<list>
#include<iterator>
#include<algorithm>
#include<deque>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
using namespace std; 



// The main function to find the maximum rectangular 
// area under given histogram with n bars 
int getMaxArea(int hist[], int n) 
{ 
	// Create an empty stack. The stack holds indexes 
	// of hist[] array. The bars stored in stack are 
	// always in increasing order of their heights. 
	stack<int> s; 

	int max_area = 0; // Initalize max area 
	int tp; // To store top of stack 
	int area_with_top; // To store area with top bar 
					// as the smallest bar 

	// Run through all bars of given histogram 
	int i = 0; 
	while (i < n) 
	{ 
		// If this bar is higher than the bar on top 
		// stack, push it to stack 
		if (s.empty() || hist[s.top()] <= hist[i]) 
			s.push(i++); 

		// If this bar is lower than top of stack, 
		// then calculate area of rectangle with stack 
		// top as the smallest (or minimum height) bar. 
		// 'i' is 'right index' for the top and element 
		// before top in stack is 'left index' 
		else
		{ 
			tp = s.top(); // store the top index 
			s.pop(); // pop the top 

			// Calculate the area with hist[tp] stack 
			// as smallest bar 
			area_with_top = hist[tp] * (s.empty() ? i : 
								i - s.top() - 1); 

			// update max area, if needed 
			if (max_area < area_with_top) 
				max_area = area_with_top; 
		} 
	} 

	// Now pop the remaining bars from stack and calculate 
	// area with every popped bar as the smallest bar 
	while (s.empty() == false) 
	{ 
		tp = s.top(); 
		s.pop(); 
		area_with_top = hist[tp] * (s.empty() ? i : 
								i - s.top() - 1); 

		if (max_area < area_with_top) 
			max_area = area_with_top; 
	} 

	return max_area; 
} 


typedef std::vector<unsigned char> Row;
typedef std::list<Row> Hist;
typedef Hist::iterator HistIter;
typedef std::stack<int> Sint;

// Driver program to test above function 
int main(int argc, char * argv[]) 
{ 
    bool debug = false;
    std::string filename;
    std::ofstream debugout;
    for (int ii = 1; ii < argc; ii++)
    {
    //    std::cout << "Arg " << (int)ii << " is " << argv[ii] << std::endl;
        if (argv[ii][0] == '-')
        {
            if (std::string("debug") == &(argv[ii][1]))
            {
                std::stringstream dbgFilename;
                dbgFilename << "/tmp/debuglog_" << getpid() << ".txt";
                debugout.open(dbgFilename.str().c_str());
            }
        }
        else
        {
            filename = argv[ii];
        }
    }
    debugout << "Processing " << filename << std::endl;
    unsigned int numcols, numrows;
    std::ifstream is;
    is.open(filename, std::ios::binary);
    if (!is) std::cout << "bad open" << std::endl;
    is.read((char*)&numrows, sizeof(numrows));
    is.read((char*)&numcols, sizeof(numcols));

    Hist hist;
    std::deque<int> lens;
//    std::deque<int> rows;

    // store all the rows with white pixels
    for (int nr=0; nr<numrows; nr++)
    {
        Row row(numcols, 0);
        is.read((char*)row.data(), row.size());
        //std::cout << nr << ": ";
        int ii;
        for (ii = 0; ii < row.size() && row[ii]; ii++);
//        if (debugout) debugout << "Row " << nr << " length " << ii << std::endl;
//        if (!ii) continue;
//        if((0 == lens.size()) || ii > lens.front())
//        {
//            if (debugout) debugout << "Pushed " << nr << std::endl;
         lens.push_back(ii);
//            rows.push_back(nr);
//        }
    }

    int maxArea(0);
    std::stack<int> histStack;
    histStack.push(0);
    int maxLeft;
    int maxRight;
    int maxHeight;

    for (int rr = 0; rr < lens.size(); rr++)
    {
        int thislen = lens[rr];
        while (!histStack.empty())
        {
            int top = histStack.top();
//            std::cout << "top " << (int)top << " thislen " << (int)thislen << std::endl;
            if (thislen > lens[top])
            {
                histStack.push(rr);
                break;
            }
            top = histStack.top();
            histStack.pop();
            int left = histStack.empty() ? 0 : histStack.top();
            int right = (lens[top] == thislen) ? rr : rr -1;
            int area = (right - left) * lens[top];
            //std::cout << "area " << area << " maxArea " << maxArea << std::endl;

            if (area > maxArea)
            {
                maxArea = area;
                maxLeft = left;
                maxRight = right;
                maxHeight = lens[top];
                if (debugout) debugout << "maxLeft " << maxLeft << " maxRight " << maxRight << " maxHeight " << maxHeight << std::endl;
            }
        }
        if (histStack.empty())
        {
            histStack.push(rr);
        }
    }
    //std::cout << maxLeft + rows.front() << " " << maxRight + rows.front() << " " << maxHeight << std::endl;
    if (debugout) debugout << maxLeft << " " << maxRight << " " << maxHeight << std::endl;
    std::cout << maxLeft << " " << maxRight << " " << maxHeight << std::endl;
	return 0; 
} 
