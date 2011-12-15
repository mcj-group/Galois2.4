/** Check worklist instantiations -*- C++ -*-
 * @file
 * @section License
 *
 * Galois, a framework to exploit amorphous data-parallelism in irregular
 * programs.
 *
 * Copyright (C) 2011, The University of Texas at Austin. All rights reserved.
 * UNIVERSITY EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES CONCERNING THIS
 * SOFTWARE AND DOCUMENTATION, INCLUDING ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR ANY PARTICULAR PURPOSE, NON-INFRINGEMENT AND WARRANTIES OF
 * PERFORMANCE, AND ANY WARRANTY THAT MIGHT OTHERWISE ARISE FROM COURSE OF
 * DEALING OR USAGE OF TRADE.  NO WARRANTY IS EITHER EXPRESS OR IMPLIED WITH
 * RESPECT TO THE USE OF THE SOFTWARE OR DOCUMENTATION. Under no circumstances
 * shall University be liable for incidental, special, indirect, direct or
 * consequential damages or loss of profits, interruption of business, or
 * related expenses which may arise from use of Software or Documentation,
 * including but not limited to those resulting from defects in Software and/or
 * Documentation, or loss or inaccuracy of data of any kind.
 *
 * @author Andrew Lenharth <andrewl@lenharth.org>
 */
template<typename T2>
struct checker {
  typedef typename T2::template retype<int>::WL T;
  T wl;
  typename T::template rethread<true>::WL wl2;
  typename T::template rethread<false>::WL wl3;

  checker() {
    int a[2] = {1,2};
    wl.push(0);
    wl.pop();

    wl2.push(0);
    wl2.pop();

    wl3.push(0);
    wl3.pop();
  }
};

#define WLCOMPILECHECK(name) checker<name<> > ck_##name;
#include "Galois/Runtime/WorkList.h"

int main() {
  return 0;
}
