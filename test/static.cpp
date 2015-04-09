// std_tr1__type_traits__is_pod.cpp 

#include "Galois/config.h"
#include "Galois/Runtime/ll/PtrLock.h"
#include "Galois/Runtime/ll/SimpleLock.h"
#include "Galois/Runtime/ll/StaticInstance.h"

#include GALOIS_CXX11_STD_HEADER(type_traits)
#include <iostream> 

using namespace Galois::Runtime;
using namespace Galois::Runtime::LL;

int main() 
{ 
  std::cout << "is_pod PtrLock<int> == " << std::boolalpha 
	    << std::is_pod<PtrLock<int> >::value << std::endl; 
  std::cout << "is_pod DummyPtrLock<int> == " << std::boolalpha 
	    << std::is_pod<DummyPtrLock<int> >::value << std::endl; 

  std::cout << "is_pod SimpleLock == " << std::boolalpha 
	    << std::is_pod<SimpleLock >::value << std::endl; 
  std::cout << "is_pod DummyLock == " << std::boolalpha 
	    << std::is_pod<DummyLock >::value << std::endl; 

  std::cout << "is_pod StaticInstance<int> == " << std::boolalpha 
	    << std::is_pod<StaticInstance<int> >::value << std::endl; 
  std::cout << "is_pod StaticInstance<std::iostream> == " << std::boolalpha 
	    << std::is_pod<StaticInstance<std::iostream> >::value << std::endl; 

  std::cout << "is_pod volatile int == " << std::boolalpha 
   	    << std::is_pod<volatile int>::value << std::endl; 
  std::cout << "is_pod int == " << std::boolalpha 
   	    << std::is_pod<int>::value << std::endl; 

  // std::cout << "is_pod<throws> == " << std::boolalpha 
  // 	    << std::is_pod<throws>::value << std::endl; 
  
  return (0); 
} 
