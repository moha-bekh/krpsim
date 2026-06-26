#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <file> <result_to_test>\n";
        return 1;
    }

    std::cout << "krpsim_verif skeleton: verifier not implemented yet\n";
    return 0;
}
