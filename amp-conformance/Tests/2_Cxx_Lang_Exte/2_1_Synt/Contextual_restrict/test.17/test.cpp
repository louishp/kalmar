// Copyright (c) Microsoft
// All rights reserved
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.
/// <tags>P0</tags>
/// <summary>This test checks that a function modifier can be used as the name of a restricted function</summary>

#include <amptest.h>
#include <iostream>

using namespace Concurrency;
using namespace std;

void restrict(index<1>& idx, Concurrency::array<int, 1>& a) restrict(amp)
{
   a(0) = 1;
}

int restrict(int a) restrict(cpu)
{
   return a + 1;
}


// Main entry point
int main(int argc, char **argv) 
{
    bool passed = true;
    int x = 10;

    cout << "Test: Use restrict as a restricted cpu function name" << endl;
    if(restrict(x) == x + 1)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        passed = false;
        cout << "Failed!" << endl;
    }

    cout << "Test: Use restrict as a restricted amp function name" << endl;
    Concurrency::extent<1> ex(1);
    Concurrency::array<int, 1> arr(ex);

    parallel_for_each(arr.get_extent(), [&](index<1> idx) restrict(amp) { restrict(idx, arr);});

    vector<int> v = arr;
    if(v[0] == 1)
    {
        cout << "Passed!" << endl;
    }
    else
    {
        passed = false;
        cout << "Failed!" << endl;
    }

    return passed ? 0 : 1;
}
