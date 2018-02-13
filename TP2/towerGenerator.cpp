#include <vector>
#include <fstream>
#include <functional>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <stack>
#include <string>
#include <sstream>


using Int = long long;

struct Block
{
    Block(std::string line){
        std::stringstream ss(line);
        ss >> hauteur;
        ss >> largeur;
        ss >> profondeur;

        float rapport;
        if(largeur < profondeur)
        { 
            rapport = (float)largeur/(float)profondeur;
        }
        else
        {
            rapport = (float)profondeur/(float)largeur;
        }

        criterion = (float)largeur * (float)profondeur * rapport + (float) hauteur;
        std::cout << criterion << std::endl;
    }

    bool operator <(Block const& rhs) {
        return this->largeur < rhs.largeur && this->profondeur < rhs.profondeur;
    }

    int hauteur;
    int largeur;
    int profondeur;
    float criterion;
};

using Algo = const std::function<std::vector<Block>(std::vector<Block>&)>&;

std::vector<Block> vorace(std::vector<Block>& blocks)
{
    std::sort(blocks.begin(), blocks.end(), 
        [](const Block& a, const Block& b) -> bool 
        {
            if(a.criterion != b.criterion)
            {
                return a.criterion > b.criterion;
            }
                
            return rand() % 2;
        });

    std::vector<Block> stackedBlock;
    for(auto it = blocks.begin(); it != blocks.end(); it++)
    {
        if(!stackedBlock.empty())
        {
            if(*it < stackedBlock.back())
            {
                stackedBlock.push_back(*it);
            }
        }
        else
        {
            stackedBlock.push_back(*it);
        }
    }

    return stackedBlock;
}

std::vector<Block> progdyn(std::vector<Block>& blocks)
{
    return std::vector<Block>();
}

std::vector<Block> tabou(std::vector<Block>& blocks)
{
    return std::vector<Block>();
}

void run(Algo algo, std::vector<Block>& blocks, bool print_res, bool print_time, bool print_height) {
    using namespace std::chrono;
    auto start = steady_clock::now();
    const std::vector<Block> stackedBlock = algo(blocks);
    auto end = steady_clock::now();

    if (print_time) {
        duration<double> s = end-start;
        std::cout << std::fixed << s.count() << std::endl;
    }

    if (print_res) {
        std::cout << std::fixed;
        for (auto it = stackedBlock.rbegin(); it != stackedBlock.rend(); it++)
            std::cout << it->hauteur << " " << it->largeur << " " << it->profondeur << std::endl;
    }

    if (print_height) {
        int height = 0;
        for (auto it = stackedBlock.begin(); it != stackedBlock.end(); it++)
            height += it->hauteur;

        std::cout << "Hauteur de la tour: " << height << std::endl;
    }
}

int main(int argc, char *argv[]) {
    struct {
        std::string algo;
        std::string file_path;
        bool print_res{false};
        bool print_time{false};
        bool print_height{false};
    } prog_args;

    // Read program arguments
    for (int i=1; i<argc; i++) {
        std::string arg(argv[i]);
        if (arg == "-a") {
            prog_args.algo = argv[i+1]; i++;
        } else if (arg == "-e") {
            prog_args.file_path = argv[i+1]; i++;
        } else if (arg == "-p") {
            prog_args.print_res = true;
        } else if (arg == "-t") {
            prog_args.print_time = true;
        } else if (arg == "-h") {
            prog_args.print_height = true;
        }
    }

    // Read numbers into vector
    std::vector<Block> blocks;
    {
        std::ifstream infile(prog_args.file_path);
        std::string line;
        while (std::getline(infile, line)){
            Block block = Block(line);
            blocks.push_back(block);
        }
    }

    // Apply correct algorithm
    if (prog_args.algo == "vorace")
        run(vorace, blocks, prog_args.print_res, prog_args.print_time, prog_args.print_height);
    else if(prog_args.algo == "progdyn")
        run(progdyn, blocks, prog_args.print_res, prog_args.print_time, prog_args.print_height);
    else if(prog_args.algo == "tabou")
        run(tabou, blocks, prog_args.print_res, prog_args.print_time, prog_args.print_height);
}