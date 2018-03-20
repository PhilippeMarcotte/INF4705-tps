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
#include <memory>
#include <math.h>


using Int = long long;

int maxTowerHeight = 0;
int nbBlocks = 0;

struct Block
{
    Block(std::string line)
    {
        std::stringstream ss(line);
        ss >> hauteur;
        ss >> largeur;
        ss >> profondeur;

        float smallestSide;
        if(largeur < profondeur)
        { 
            smallestSide = (float)largeur;
        }
        else
        {
            smallestSide = (float)profondeur;
        }

        criterion = smallestSide * smallestSide / hauteur;
    }

    bool fitOn(Block const& bottom) const
    {
        return this->largeur <= bottom.largeur && this->profondeur <= bottom.profondeur;
    }

    void setTabou()
    {
        tabouIt = rand()% 4 + 7;
    }

    bool updateTabou()
    {
        return !(--tabouIt);
    }

    int hauteur = 0;
    int largeur = 0;
    int profondeur = 0;
    float criterion = 0;
    int tabouIt = 0;
};

using Algo = const std::function<std::vector<std::vector<Block>>(std::vector<Block>&)>;

std::vector<Block> vorace(std::vector<Block>& blocks)
{
    std::sort(blocks.begin(), blocks.end(), 
        [](const Block& a, const Block& b) -> bool 
        {
            return a.criterion > b.criterion;
        });

    int towerHeight = 0;

    std::vector<Block> stackedBlock;
    for(auto it = blocks.begin(); it != blocks.end(); ++it)
    {
        int randomValue = rand() % 100;
        if( randomValue < 25 && std::next(it) != blocks.end())
        {
            it++;
        }

        auto blockToStack = blocks.end();
        if(!stackedBlock.empty())
        {
            if(it->fitOn(stackedBlock.back()))
            {
                blockToStack = it;
            }
        }
        else
        {
            blockToStack = it;
        }

        if (blockToStack != blocks.end())
        {
            towerHeight += blockToStack->hauteur;
            if (towerHeight >= maxTowerHeight) break;

            stackedBlock.push_back(*blockToStack);
            it = blocks.erase(it);
        }
    }

    return stackedBlock;
}

std::vector<std::vector<Block>> multiTowersVorace(std::vector<Block>& blocks)
{
    std::vector<std::vector<Block>> towers;
    while (!blocks.empty())
    {
        towers.push_back(vorace(blocks));
    }

    return towers;
}

std::vector<Block> tabou(std::vector<Block>& unstackedBlocks)
{
    srand(time(NULL));
    int maxHeight = 0;
    std::unique_ptr<std::vector<Block>> bestTower;
    std::unique_ptr<std::vector<Block>> currentTower = std::make_unique<std::vector<Block>>(std::vector<Block>());
    std::unique_ptr<std::vector<Block>> currentTabous = std::make_unique<std::vector<Block>>(std::vector<Block>());

    std::random_shuffle(unstackedBlocks.begin(), unstackedBlocks.end());

    // Take the first random block
    currentTower->push_back(unstackedBlocks.back());
    unstackedBlocks.pop_back();

    // Start iterating and stop when no better tower is found during a 100 iterations    
    for(int iteration = 0; iteration < 100; iteration++)
    {
        int maxPossibleHeight = 0;
        auto bestPossibleInsertIt = currentTower->begin();
        auto bestPossibleBlockIt = unstackedBlocks.begin();
        int bestPossibleNbrOfBlockToTabou = 0;

        // Shuffle before selecting first 10% blocks as to make the selection random
        //std::random_shuffle(unstackedBlocks.begin(), unstackedBlocks.end());

        // Compute iterator that ends the first 10% blocks
        auto random10PercentEnd = unstackedBlocks.end();
        //std::advance(random10PercentEnd, int(ceil(unstackedBlocks.size() * 0.1)));

        // Iterate first 10% unstacked blocks to compute possibilities
        for(auto unstackedBlockIt = unstackedBlocks.begin(); unstackedBlockIt != random10PercentEnd; unstackedBlockIt++)
        {
            // Assume that the block will have to be inserted at the tower's base 
            auto insertedBlockIt = currentTower->begin();

            // Check where the block should be inserted
            for(auto stackedBlockIt = currentTower->rbegin(); stackedBlockIt != currentTower->rend(); ++stackedBlockIt)
            {
                // If the block fit on one of the tower's block insert it on it (right after)
                if(unstackedBlockIt->fitOn(*stackedBlockIt))
                {
                    // Update the position of the new block
                    insertedBlockIt = stackedBlockIt.base();
                    break;
                }
            }
            
            // Check if the blocks, that the new block did not fit on, fit on the new block. If not, tabou it.
            auto blockToTabouIt = insertedBlockIt;
            int nbrOfBlockToTabou = 0;
            for(blockToTabouIt; blockToTabouIt != currentTower->end() && !blockToTabouIt->fitOn(*unstackedBlockIt); blockToTabouIt++, nbrOfBlockToTabou++);

            int height = 0;
            // Compute the height of the possible tower
            for(auto stackedBlockedIt = currentTower->begin(); stackedBlockedIt != insertedBlockIt; ++stackedBlockedIt)
            {
                height += stackedBlockedIt->hauteur;
            }

            height += unstackedBlockIt->hauteur;

            for(auto stackedBlockedIt = blockToTabouIt; stackedBlockedIt != currentTower->end(); ++stackedBlockedIt)
            {
                height += stackedBlockedIt->hauteur;
            }

            // Check if it is the best possible tower encountered
            if(height > maxPossibleHeight)
            {
                maxPossibleHeight = height;
                
                // Remember the position of the block as to remove it if this possible tower is chosen
                bestPossibleInsertIt = insertedBlockIt;
                bestPossibleBlockIt = unstackedBlockIt;
                bestPossibleNbrOfBlockToTabou = nbrOfBlockToTabou;
            }
        }

        bestPossibleInsertIt = currentTower->insert(bestPossibleInsertIt, *bestPossibleBlockIt);

        // Update the already tabous blocks
        for(auto blockTabouIt = currentTabous->begin(); blockTabouIt != currentTabous->end(); blockTabouIt++)
        {
            if(blockTabouIt->updateTabou())
            {
                unstackedBlocks.push_back(*blockTabouIt);
                blockTabouIt = currentTabous->erase(blockTabouIt);
                if(blockTabouIt == currentTabous->end()) break;
            }
        }
        
        // Set tabou of the new tabous and add them to the current tabous
        for(auto blockTabouIt = std::next(bestPossibleInsertIt); blockTabouIt != std::next(bestPossibleInsertIt, bestPossibleNbrOfBlockToTabou + 1); ++blockTabouIt)
        {
            blockTabouIt->setTabou();
            currentTabous->push_back(*blockTabouIt);
            blockTabouIt = currentTower->erase(blockTabouIt);
        }

        // Remove the added block from the unstacked blocks
        unstackedBlocks.erase(bestPossibleBlockIt);

        // Check if the chosen possible tower is taller than the tallest tower encountered
        if(maxPossibleHeight > maxHeight)
        {
            maxHeight = maxPossibleHeight;
            bestTower = std::make_unique<std::vector<Block>>(*currentTower.get());

            // Reset the iteration because we encountered a better tower
            iteration = 0;
        }        
    }

    return *(bestTower.get());
}

void run(Algo algo, std::vector<Block>& blocks, bool print_res, bool print_time) {
    using namespace std::chrono;
    auto start = steady_clock::now();
    const std::vector<std::vector<Block>> towers = algo(blocks);
    auto end = steady_clock::now();

    if (print_time) 
    {
        duration<double> s = end-start;
        std::cout << std::fixed << s.count() << std::endl;
    }

    if (print_res) 
    {
        std::cout << std::fixed;
        for (auto tower = towers.begin(); tower != towers.end(); tower++)
        {
            std::cout << towers.size() << std::endl;
            for (auto block = tower->begin(); block != tower->end(); block++)
                std::cout << block->hauteur << " " << block->largeur << " " << block->profondeur << std::endl;
        }
    }
    else
    {
        std::cout << towers.size() << std::endl;
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

    srand(time(NULL));

    // Read program arguments
    for (int i=1; i<argc; i++) {
        std::string arg(argv[i]);
        if (arg == "-e") {
            prog_args.file_path = argv[i+1]; i++;
        } else if (arg == "-p") {
            prog_args.print_res = true;
        } else if (arg == "-t") {
            prog_args.print_time = true;
        }
    }

    // Read numbers into vector
    std::vector<Block> blocks;
    {
        std::ifstream infile(prog_args.file_path);

        std::string line;

        std::getline(infile, line);
        nbBlocks = std::stoi(line);

        std::getline(infile, line);
        maxTowerHeight = std::stoi(line);
        
        while (std::getline(infile, line)){
            Block block = Block(line);
            blocks.push_back(block);
        }
    }

    // Apply correct algorithm
    run(multiTowersVorace, blocks, prog_args.print_res, prog_args.print_time);
}