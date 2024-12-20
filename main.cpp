#include <iostream>
#include <vector>
#include <sstream>

#include "./Templates/TypeInfo.h"
#include "./Templates/Imitation.h"


#include "./Templates/Switch.h"


std::string parse_arg(const std::string &arg)
{
    std::string res = "";
    for (auto sim = arg.begin() + arg.find('=') + 1; sim != arg.end(); ++sim)
    {
        if (!std::isspace(*sim))
        {
            res.push_back(*sim);
        }
    }
    return res;

}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cerr << "Not enough arguments provided!\n";
        exit(-1);
    }

    std::string in_file = "in.bin", type1, type2, type3, arg;

    // parsing arguments
    for (int i = 1; i < argc; i++)
    {
        arg = std::string(argv[i]);
        if (arg.starts_with("--p-type"))
        {
            type1 = parse_arg(arg);
        }
        else if (arg.starts_with("--v-type"))
        {
            type2 = parse_arg(arg);
        }
        else if (arg.starts_with("--v-flow-type"))
        {
            type3 = parse_arg(arg);
        }
        else if (arg.starts_with("--in-file"))
        {
            in_file = parse_arg(arg);
        }
        else
        {
            std::cerr << "Unknown command line argument: " << arg << '\n';
        }
    }

    if (type1.empty() || type2.empty() || type3.empty())
    {
        std::cerr << "Not all types provided!\n";
    }

    std::ifstream is(in_file, std::ios::binary);
    uint32_t rows, cols;
    is.read((char*)&rows, 4);
    is.read((char*)&cols, 4);
    std::vector<std::vector<char>> field(rows, std::vector<char>(cols));
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            is.read(&field[i][j], sizeof(char));
        }
    }

    Binder binder;
    binder.bind(type1, type2, type3, rows, cols, std::move(field));
    return 0;
}
