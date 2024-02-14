#include <iostream>
#include <functional>
#include <stack>


int main() {
  // Bind function with arguments
  // std::stack<int> s;
  // s.push(10);
  // std::function<void()> storedFunction = std::bind(&std::stack<int>::push, &s, 10);

  // // Execute the stored function later
  // storedFunction();
  // std::cout << s.top() << std::endl;

  std::stack<int> s;
  s.push(10);

  std::function<void()> storedFunction = std::bind(static_cast<void (std::stack<int>::*)(const int&)>(&std::stack<int>::push), &s, 69);

  // Execute the stored function later
  storedFunction();
  std::cout << s.top() << std::endl;
  s.pop();
  std::cout << s.top() << std::endl;
  storedFunction();
  std::cout << s.top() << std::endl;


  return 0;
}
