#include "serialization.h"
#include "examples.h"

#include <fstream>

using namespace std;

int main(int argc, char const *argv[])
{
    {
        ofstream output("data.bin", ios_base::binary);
        Archive<ofstream> oa(output);

        SimpleClass simple_1(
            23,
            vector<int>({1, 2, 3, 4}), 
            3.14, "new string",
            SimpleStruct(
                24, 2.6, 3.15, 'e',
                deque<int>({1, 2, 8}),
                forward_list<double>({5.0, 3.15, 6.26})), 
            list<string>({"three", "two", ""}));

        oa << simple_1;
    }

    {
        ifstream input("data.bin", ios_base::binary);
        Archive<ifstream> ia(input);
        SimpleClass simple_2;
        ia >> simple_2;
    }

    return 0;
}
