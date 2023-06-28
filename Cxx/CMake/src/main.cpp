#include <iostream>

int main(int, char **) {
  int a = 6;
  std::size_t buf_size = 100;
  char buf[buf_size];
  snprintf(buf, buf_size, "num: %d", a);
  std::cout << "Hello, from c1!, " << buf << std::endl;
  return 0;
}
