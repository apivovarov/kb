#include <iostream>

int main(int, char **) {
  int a = 8;
  std::size_t buf_size = 80;
  char buf[buf_size];
  snprintf(buf, buf_size, "num: %d", a);
  std::cout << "Hello, from c2!, " << buf << std::endl;
  return 0;
}
