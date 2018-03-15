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
    }

    bool operator <(Block const& rhs) {
        return this->largeur < rhs.largeur && this->profondeur < rhs.profondeur;
    }

    bool fitOn(Block const& bottom)
    {
        return this->largeur < rhs.largeur && this->profondeur < rhs.profondeur;
    }

    void setTabou()
    {
        tabouIt = rand()% 4 + 7;
    }

    bool updateTabou()
    {
        return !(--tabouIt);
    }

    int hauteur;
    int largeur;
    int profondeur;
    float criterion;
    int tabouIt;
};

using Algo = const std::function<std::vector<Block>(std::vector<Block>&)>&;

std::vector<Block> vorace(std::vector<Block>& blocks)
{
    srand (time(NULL));
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
    for(auto it = blocks.begin(); it != blocks.end(); ++it)
    {
        int randomValue = rand() % 100;
        if( randomValue < 25 && std::next(it) != blocks.end())
        {
            it++;
        }

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
    std::sort(blocks.begin(), blocks.end(), 
        [](const Block& a, const Block& b) -> bool 
        {
            return a.largeur * a.profondeur > b.largeur * b.profondeur;
        });

    std::vector<Block> stackedBlocks[blocks.size()];
    int maxStackedHeights[blocks.size()];

    for(int i = 0; i < blocks.size(); i++)
    {
        maxStackedHeights[i] = blocks.at(i).hauteur;
    }

    for(int i = 1; i < blocks.size(); i++)
    {
        for(int j = 0; j < i; j++)
        {
            if(blocks.at(i) < blocks.at(j) && maxStackedHeights[i] < maxStackedHeights[j] + blocks.at(i).hauteur)
            {
                stackedBlocks[i].clear();
                stackedBlocks[i] = stackedBlocks[j];
                stackedBlocks[i].push_back(blocks.at(i));

                maxStackedHeights[i] = maxStackedHeights[j] + blocks.at(i).hauteur;
            }
        }
    }

    int maxStackedHeight = 0;
    std::vector<Block> stackedBlock;
    for(int i = 0; i < blocks.size(); i++)
    {
        if(maxStackedHeights[i] > maxStackedHeight)
        {
            stackedBlock = stackedBlocks[i];
            maxStackedHeight = maxStackedHeights[i];
        }
    }

    std::cout << maxStackedHeight << std::endl;

    return stackedBlock;
}

std::vector<Block> tabou(std::vector<Block>& unstackedBlocks)
{
    int maxHeight = 0;
    std::vector<Block> bestTower;
    std::vector<Block> currentTower;
    std::vector<Block> currentTabous;

    std::random_shuffle(unstackedBlocks.begin(), unstackedBlocks.end());

    // Take the first random block
    currentTower.push_back(unstackedBlocks.pop_back());

    // Start iterating and stop when no better tower is found during a 100 iterations    
    for(int iteration = 0; iteration < 100; iteration++)
    {
        std::vector<Block> bestPossibleTower;
        std::vector<Block> bestPossibleTabou;
        int maxPossibleHeight = 0;
        auto bestPossibleInsertedBlockIt = unstackedBlocks.begin();

        // Iterate every unstacked block to compute every possibilities
        for(auto unstackedBlockIt = unstackedBlocks.begin(); unstackedBlockIt != unstackedBlocks.end(); unstackedBlockIt++)
        {
            std::vector<Block> possibleTower = currentTower;
            std::vector<Block> possibleTabous;

            // Assume that the block will have to be inserted at the tower's base 
            auto insertedBlockIt = possibleTower.begin();

            // Check where the block should be inserted
            for(auto stackedBlockedIt = possibleTower.rbegin(); stackedBlockedIt != possibleTower.rend(); stackedBlockedIt++)
            {
                Block unstackedBlock = *unstackedBlockIt;
                Block stackedBlock = *stackedBlockedIt;

                // If the block fit on one of the tower's block insert it on it (right after)
                if(unstackedBlock.fitOn(stackedBlock))
                {
                    // Update the position of the new block
                    insertedBlockIt = possibleTower.insert(stackedBlockedIt + 1, unstackedBlock);
                    break;
                }
            }

            // If the position of the block never changed, it has to be the new tower's base
            if(insertedBlockIt == possibleTower.begin())
            {
                insertedBlockIt = possibleTower.insert(possibleTower.begin(), unstackedBlock);
            }

            Block insertedBlock = *insertedBlockIt;

            // Check if the blocks, that the new block did not fit on, fit on the new block. If not, tabou it.
            for(auto blockToTabouIt = insertedBlockIt + 1; blockToTabouIt != possibleTower.end(); blockToTabouIt++)
            {
                Block blockToTabou = *blockToTabouIt;

                if(!blockToTabou.fitOn(insertedBlock))
                {
                    // Put in tabou
                    possibleTabous.push_back(blockToTabou);

                    // Remove from the tower
                    blockTabouIt = possibleTower.erase(blockTabouIt);
                }
                else
                {
                    break;
                }
            }

            int height = 0;
            // Compute the height of the possible tower
            for(auto stackedBlockedIt = possibleTower.begin(); stackedBlockedIt != possibleTower.end(); stackedBlockedIt++)
            {
                height += stackedBlockedIt->hauteur;
            }

            // Check if it is the best possible tower encountered
            if(height > maxPossibleHeight)
            {
                bestPossibleTower = possibleTower;
                bestPossibleTabou = possibleTabous;
                maxPossibleHeight = height;
                
                // Remember the position of the block as to remove it if this possible tower is chosen
                bestPossibleInsertedBlockIt = unstackedBlockIt;
            }
        }

        // Check if the chosen possible tower is taller than the tallest tower encountered
        if(maxPossibleHeight > maxHeight)
        {
            maxHeight = maxPossibleHeight;
            bestTower = bestPossibleTower;

            // Reset the iteration because we encountered a better tower
            iteration = 0;
        }

        currentTower = maxPossibleHeight;

        // Update the already tabous blocks
        for(auto blockTabouIt = currentTabous.begin(); blockTabouIt != currentTabous.end(); blockTabouIt++)
        {
            if(blockTabouIt->updateTabou())
            {
                unstackedBlocks.push_back(*blockTabouIt);
                blockTabouIt = currentTabous.erase(blockTabouIt);
            }
        }

        // Set tabou of the new tabous and add them to the current tabous
        for(auto blockTabouIt = bestPossibleTabou.begin(); blockTabouIt != bestPossibleTabou.end(); blockTabouIt++)
        {
            blockTabouIt->setTabou();
            currentTabous.push_back(*blockTabouIt);
        }

        // Remove the added block from the unstacked blocks
        unstackedBlocks.erase(bestPossibleInsertedBlockIt);
    }

    return bestTower;
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
        for (auto it = stackedBlock.begin(); it != stackedBlock.end(); it++)
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