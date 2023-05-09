
// this program test c++ 14 Heterogeneous look up 
// see 
// and also along with std::set extract in c++17
// which is the only way to take a move-only object out of a set
#include <cstdint>
#include <iostream>
#include <memory>
#include <set>
#include <vector>

struct Foo {
  int x;
  Foo (int x):x (x) {}
  int value() {return x;}
  ~Foo () { std::cout << "Foo " << x << " destroyed\n"; }
};

// Heterogeneous look up step 2 : define your own operator< that compatible to your type
// a typical usage in std::set is
// step 1: std::set<move_only_type, std::less<>> , leave std::less<> empty
// step 2: define operator < that compatible to move_only_type
 bool operator<(const std::pair<int,std::unique_ptr<Foo>>& lhs, const std::pair<int, Foo*>& rhs)
 {
    if(lhs.first == rhs.first){
        return lhs.second.get() < rhs.second;
    }
    return lhs.first < rhs.first;
 }

 bool operator<(const std::pair<int, Foo*>& lhs, const std::pair<int,std::unique_ptr<Foo>>& rhs)
 {
    if(lhs.first == rhs.first){
        return lhs.second < rhs.second.get();
    }
    return lhs.first < rhs.first;
 }
int main () {
  // create a set
  using Entry = std::pair<int, std::unique_ptr<Foo>>;
  // Heterogeneous look up step 1 : use std::less<> without any template parameter
  std::set<Entry, std::less<>> s;

  // insert some elements 
  s.insert (std::make_pair (1, std::make_unique<Foo> (10)));
  s.insert (std::make_pair (2, std::make_unique<Foo> (20)));
  s.insert (std::make_pair (3, std::make_unique<Foo> (30)));
  s.insert (std::make_pair (4, std::make_unique<Foo> (40)));
  s.insert (std::make_pair (5, std::make_unique<Foo> (50)));

  // the first element pair{1, 10} should released
  {
    std::vector<Entry> freelist;
    auto iter = s.begin();
    {
      auto p = s.extract(iter);
      freelist.push_back(std::move(p.value()));
      std::cout << "leaving inner {}" << std::endl;
    }
    std::cout << "left inner {}" << std::endl;
    std::cout << "leaving outter {}" << std::endl;
  }
  std::cout << "left outter {}" << std::endl; 
  std::cout << "---------------------------------------" << std::endl;

  // the 2-4th elements should released
  {
    std::vector<Entry> freelist;
    auto end = s.lower_bound(std::make_pair(4, reinterpret_cast<Foo*>(UINTPTR_MAX)));
    auto it = s.begin();
    while(it != end){
        freelist.emplace_back(std::move(s.extract(it++).value()));
    }
    std::cout << "freelist item：" << std::endl;
    for(auto& entry : freelist){
        std::cout << entry.first << ", " << entry.second->value() << "\n";
    }
    std::cout << "leaving scope of freelist" << std::endl;
  }
  std::cout << "left scope of freelist" << std::endl;
  std::cout << "---------------------------------------" << std::endl;

  // print the rest of the elements 
  for (const auto& p : s) {
    std::cout << p.first << ", " << p.second->x << "\n";
  }
  // the last element is released
  return 0;
}

