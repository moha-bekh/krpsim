#include <iostream>

int main(int argc, char** argv)
{
  if (argc == 1) {
    // dev mode witch mock configs
    return 0;
  }

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <file> <delay>\n";
    return 1;
  }

  std::cout << "krpsim skeleton: solver not implemented yet\n";
  return 0;
}
