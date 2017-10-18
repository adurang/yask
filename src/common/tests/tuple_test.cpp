/*****************************************************************************

YASK: Yet Another Stencil Kernel
Copyright (c) 2014-2017, Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

* The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

*****************************************************************************/

// Test the YASK tuples.

// enable assert().
#define DEBUG

#include "tuple.hpp"
using namespace std;
using namespace yask;

typedef Tuple<int> IntTuple;

int main(int argc, char** argv) {

    ostream& os = cout;
    
    IntTuple t1;
    t1.setFirstInner(true);
    t1.addDimBack("x", 3);
    t1.addDimBack("y", 4);
    assert(t1.getNumDims() == 2);
    assert(t1[0] == 3);
    assert(t1[1] == 4);
    assert(t1["x"] == 3);
    assert(t1["y"] == 4);

    os << "space: " << t1.makeDimValStr() << ", is ";
    if (!t1.isFirstInner()) os << "NOT ";
    os << "first-inner layout.\n";

    IntTuple t2(t1);
    assert(t2 == t1);
    t2["x"] = 2;
    assert(t2 < t1);
    t2["x"] = 4;
    assert(t2 > t1);

    IntTuple t3(t1);
    assert(t3 == t1);
    t3.addDimFront("a", 1);
    assert(t3 > t1);            // more dims.

    IntTuple t4;
    t4.addDimBack("x", 3);
    t4.addDimBack("z", 4);
    assert(t4 > t1);
    
    os << "loop test...\n";
    size_t j = 0;
    for (int y = 0; y < t1["y"]; y++) {
        for (int x = 0; x < t1["x"]; x++, j++) {

            IntTuple ofs;
            ofs.addDimBack("x", x);
            ofs.addDimBack("y", y);

            auto i = t1.layout(ofs);
            os << " offset at " << ofs.makeDimValStr() << " = " << i << endl;

            IntTuple ofs2 = t1.unlayout(i);
            assert(ofs == ofs2);
            assert(i == j);
        }
    }

    os << "sequential visit test...\n";
    j = 0;
    t1.visitAllPoints
        ([&](const IntTuple& ofs, size_t k) {

            auto i = t1.layout(ofs);
            os << " offset at " << ofs.makeDimValStr() << " = " << i << endl;

            assert(i == j);
            assert(i == k);
            j++;
            return true;
        });
    assert(int(j) == t1.product());

    os << "parallel visit test...\n";
    j = 0;
    t1.visitAllPointsInParallel
        ([&](const IntTuple& ofs, size_t k) {

            auto i = t1.layout(ofs);
#pragma omp critical
            {
                os << " offset at " << ofs.makeDimValStr() << " = " << i << endl;
                j++;
            }

            assert(i == k);
            return true;
        });
    assert(int(j) == t1.product());
    
    os << "End of YASK tuple test.\n";
    return 0;
}