#include <list>
#include <vector>
#include <fstream>
#include <functional>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <stack>
#include <string>
#include <sstream>
#include <memory>
#include <math.h>
#include <map>


using Int = long long;

int maxTowerHeight = 0;
int nbBlocks = 0;

struct Block
{
    Block()
    {

    }
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
        id = idGen++;
    }

    bool fitOn(const Block& bottom) const
    {
        return this->largeur <= bottom.largeur && this->profondeur <= bottom.profondeur;
    }
    
    static int idGen;
    int id;
    int hauteur = 0;
    int largeur = 0;
    int profondeur = 0;
    float criterion = 0;
};

struct Tower
{
    Tower()
    {
    }

    Tower(std::list<Block> blocks)
    {
        this->blocks = blocks;
        for(auto blockIt = blocks.begin(); blockIt != blocks.end(); ++blockIt)
        {
            height += blockIt->hauteur;
        }
    }

    bool insert(std::list<Block>::iterator it, Block block)
    {
        if (tabous.find(block.id) != tabous.end())
        {
            return false;
        }

        blocks.insert(it, block);
        height += block.hauteur;
        return true;
    }

    bool push_back(Block block)
    {
        return insert(blocks.end(), block);
    }

    std::list<Block>::iterator erase(std::list<Block>::iterator it)
    {
        height -= it->hauteur;
        return blocks.erase(it);
    }
    
    void setTabou(int blockId)
    {
        tabous[blockId] = rand()% 4 + 7;
    }

    bool updateTabou()
    {
        for(auto it = tabous.begin(); it != tabous.end();)
        {
            if(!--it->second)
            {
                it = tabous.erase(it++);
            }
            else
            {
                ++it;
            }
        }
    }

    void print()
    {
        std::cout << blocks.size() << std::endl;
        for (auto block = blocks.begin(); block != blocks.end(); block++)
            std::cout << block->hauteur << " " << block->largeur << " " << block->profondeur << std::endl;
    }

    std::map<int, int> tabous;
    std::list<Block> blocks;
    int height = 0;
};

int Block::idGen = 0;

std::list<Block> vorace(std::list<Block>& blocks)
{
    blocks.sort( 
        [](const Block& a, const Block& b) -> bool 
        {
            return a.criterion > b.criterion;
        });

    int towerHeight = 0;

    std::list<Block> stackedBlock;
    for(auto it = blocks.begin(); it != blocks.end();)
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
            it = blocks.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    return stackedBlock;
}

using Towers = std::list<Tower>;

Towers* multiTowersVorace(std::list<Block>& blocks)
{
    Towers* towers = new Towers();
    while (!blocks.empty())
    {
        towers->emplace_back(vorace(blocks));
    }

    return towers;
}

Tower* findSmallestTower(Towers* towers)
{
    int smallestHeight = INT32_MAX;
    Tower* smallestTower;
    for(auto tower = towers->begin(); tower != towers->end(); ++tower)
    {
        if(tower->height < smallestHeight)
        {
            smallestTower = &*tower;
            smallestHeight = tower->height;
        }
    }

    return smallestTower;
}

std::vector<Block> trimTower(Tower* tower, Block blockToInsert, std::vector<Block> blocksToRemove)
{
    int heightToRemove = std::accumulate(blocksToRemove.begin(), blocksToRemove.end(), 0, [](int sum, Block block){
        return sum + block.hauteur;
    });
    int towerHeight = tower->height + blockToInsert.hauteur - heightToRemove;
    if (towerHeight > maxTowerHeight)
    {
        int sizeToRemove = towerHeight - maxTowerHeight;
        while (sizeToRemove > 0)
        {
            Block tallestBlock ;
            for(auto blockIt = tower->blocks.begin(); blockIt != tower->blocks.end(); ++blockIt)
            {
                bool ignoreBlock = std::find_if(blocksToRemove.begin(), blocksToRemove.end(), [blockIt](Block block) 
                                        { 
                                            return blockIt->id == block.id;
                                        }) != blocksToRemove.end();
                if(blockIt->hauteur > tallestBlock.hauteur && !ignoreBlock)
                {
                    tallestBlock = *blockIt;
                }
            }        
            
            blocksToRemove.push_back(tallestBlock);
            heightToRemove += tallestBlock.hauteur;
            sizeToRemove -= tallestBlock.hauteur;
        }
    }

    return blocksToRemove;
}

std::vector<Block> findBestTowerToInsertInto(Towers* towers, Block blockToInsert, Tower* smallestTower, 
                                             Towers::iterator* bestTowerToInsertIt, std::list<Block>::iterator* bestTowerInsertionIt)
{
    std::vector<Block> smallestBlocksToRemove;
    std::vector<Block> blocksToRemove;
    for(auto towerToInsertIt = towers->begin(); towerToInsertIt != towers->end(); ++towerToInsertIt)
    {
        if(&*towerToInsertIt == &*smallestTower) continue;
        
        auto towerInsertionIt = towerToInsertIt->blocks.begin();
        for (auto blockIt = towerToInsertIt->blocks.rbegin(); blockIt != towerToInsertIt->blocks.rend(); ++blockIt)
        {
            if(blockToInsert.fitOn(*blockIt))
            {
                towerInsertionIt = blockIt.base();
                break;
            }
        }

        // Check if the blocks, that the new block did not fit on, fit on the new block. If not, tabou it.
        auto blockToRemoveIt = towerInsertionIt;
        for(blockToRemoveIt; blockToRemoveIt != towerToInsertIt->blocks.end() && !blockToRemoveIt->fitOn(blockToInsert); blockToRemoveIt++)
        {
            blocksToRemove.push_back(*blockToRemoveIt);
        }

        blocksToRemove = trimTower(&*towerToInsertIt, blockToInsert, blocksToRemove);

        if (blocksToRemove.size() < smallestBlocksToRemove.size())
        {
            smallestBlocksToRemove = blocksToRemove;
            *bestTowerToInsertIt = towerToInsertIt;
            *bestTowerInsertionIt = towerInsertionIt;
        }
        
        blocksToRemove.clear();
    }

    return smallestBlocksToRemove;
}

void tabou(Towers* towers, bool print_res)
{
    int maxHeight = 0;
    Towers bestSolution = *towers;
    Towers* currentSolution = towers;

    // Start iterating and stop when no better tower is found during a 100 iterations    
    for(int iteration = 0; iteration < 100; iteration++)
    {
        Tower* smallestTower = findSmallestTower(currentSolution);
        std::cout << "Smallest Tower : \n" << std::endl;
        smallestTower->print();
        auto towerToInsertIt = currentSolution->begin();
        std::list<Block>::iterator towerInsertionIt;
        Block blockToMove = smallestTower->blocks.back();

        std::vector<Block> blocksToRemove = 
            findBestTowerToInsertInto(currentSolution, blockToMove, smallestTower, &towerToInsertIt, &towerInsertionIt);
        
        towerToInsertIt->blocks.insert(towerInsertionIt, blockToMove);

        smallestTower->blocks.pop_back();

        // Update the already tabous blocks
        for(auto tower = towers->begin(); tower != towers->end(); ++tower)
        {
            tower->updateTabou();
        }
        
        for(auto blockIt = towerToInsertIt->blocks.begin(); blockIt != towerToInsertIt->blocks.end();)
        { 
            bool shouldRemove = std::find_if(blocksToRemove.begin(), blocksToRemove.end(), 
                [blockIt](Block block) 
                { 
                    return blockIt->id == block.id; 
                }) != blocksToRemove.end();
            
            if (shouldRemove)
            {
                towerToInsertIt->setTabou(blockIt->id);
                blockIt = towerToInsertIt->erase(blockIt++);
            }
            else
            {
                ++blockIt;
            }
        }

        // Check if the chosen possible tower is taller than the tallest tower encountered
        if(currentSolution->size() < bestSolution.size())
        {
            bestSolution = *currentSolution;
            if (print_res) 
            {
                std::cout << std::fixed;
                for (auto tower = currentSolution->begin(); tower != currentSolution->end(); tower++)
                {
                    tower->print();
                }
            }
            else
            {
                std::cout << currentSolution->size() << std::endl;
            }
        }
        std::cout << iteration << std::endl;        
    }
}

void run(std::list<Block>& blocks, bool print_res, bool print_time) {
    using namespace std::chrono;
    auto start = steady_clock::now();
    Towers* towers = multiTowersVorace(blocks);
    if (print_res) 
    {
        std::cout << std::fixed;
        for (auto tower = towers->begin(); tower != towers->end(); tower++)
        {
            tower->print();
        }
    }
    else
    {
        std::cout << towers->size() << std::endl;
    }
    tabou(towers, print_res);
    auto end = steady_clock::now();

    if (print_time) 
    {
        duration<double> s = end-start;
        std::cout << std::fixed << s.count() << std::endl;
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
    std::list<Block> blocks;
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

    run(blocks, prog_args.print_res, prog_args.print_time);
}