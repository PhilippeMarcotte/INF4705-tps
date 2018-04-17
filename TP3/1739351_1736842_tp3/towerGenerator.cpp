#include <list>
#include <fstream>
#include <functional>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <string>
#include <sstream>
#include <math.h>
#include <map>
#include <memory>
#include "blockingconcurrentqueue.h"

#include <thread>

using Int = long long;

int maxTowerHeight = 0;

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

        if(largeur < profondeur)
        { 
            smallestSide = (float)largeur;
        }
        else
        {
            smallestSide = (float)profondeur;
        }

        id = idGen++;
    }

    bool fitOn(const Block& bottom) const
    {
        return this->largeur < bottom.largeur && this->profondeur < bottom.profondeur;
    }
    
    static int idGen;
    int id;
    float hauteur = 0;
    float largeur = 0;
    float profondeur = 0;
    float criterion = 0;
    float smallestSide = 0;    
};

bool areaCriterion(Block& a, Block& b)
{
    return a.largeur * a.profondeur > b.largeur * b.profondeur;
}

bool areaAndShortestCriterion(Block& a, Block& b)
{
    return a.largeur * a.profondeur / a.hauteur > b.largeur * b.profondeur / b.hauteur;
}

bool squarestCriterion(Block& a, Block& b)
{
    return a.smallestSide * a.smallestSide > b.smallestSide * b.smallestSide;
}

bool squarestAndShortestCriterion(Block& a, Block& b)
{
    return a.smallestSide * a.smallestSide / a.hauteur > b.largeur * b.profondeur / b.hauteur;
}

struct Tower
{
    Tower()
    {
    }

    Tower(Block block)
    {
        push_back(block);
    }

    Tower(std::list<Block> blocks) : Tower()
    {
        for(auto blockIt = blocks.begin(); blockIt != blocks.end(); ++blockIt)
        {
            height += blockIt->hauteur;
            this->blocks.push_back(*blockIt);
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

    void pop_back()
    {
        height -= blocks.back().hauteur;
        blocks.pop_back();
    }
    
    void setTabou(int blockId)
    {
        tabous[blockId] = rand()% 1000 + 1000;
    }

    bool isBlockTabou(int blockId)
    {
        return tabous.find(blockId) != tabous.end();
    }

    bool updateTabou()
    {
        for(auto it = tabous.begin(); it != tabous.end();)
        {
            if(!--(it->second))
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
        for (auto blockIt = blocks.rbegin(); blockIt != blocks.rend(); blockIt++)
        {
            Block block = *blockIt;
            std::cout << block.hauteur << " " << block.largeur << " " << block.profondeur << std::endl;
        }
    }

    std::list<Block>::iterator begin()
    {
        return blocks.begin();
    }

    std::list<Block>::iterator end()
    {
        return blocks.end();
    }

    std::list<Block>::reverse_iterator rbegin()
    {
        return blocks.rbegin();
    }

    std::list<Block>::reverse_iterator rend()
    {
        return blocks.rend();
    }

    Block back()
    {
        return blocks.back();
    }

    bool empty()
    {
        return blocks.empty();
    }

    void sort(bool(*predicate)(Block&, Block&))
    {
        blocks.sort(predicate);
    }

    std::map<int, int> tabous;
    int height = 0;
    private:
    std::list<Block> blocks;
};

int Block::idGen = 0;

std::list<Block> vorace(std::list<Block>& blocks, bool(*predicate)(Block&, Block&))
{
    blocks.sort(predicate);

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

Towers& multiTowersVorace(std::list<Block>& blocks, bool(*predicate)(Block&, Block&))
{
    Towers* towers = new Towers();
    while (!blocks.empty())
    {
        towers->emplace_back(vorace(blocks, predicate));
    }

    return *towers;
}

Towers& worstCaseStart(std::list<Block>& blocks)
{
    Towers* towers = new Towers();
    while (!blocks.empty())
    {
        towers->emplace_back(blocks.back());
        blocks.pop_back();
    }

    return *towers;
}

int findSmallestTower(Towers& towers)
{
    int smallestHeight = INT32_MAX;
    int i;
    int smallestTower = 0;
    Towers::iterator tower;
    for(tower = towers.begin(), i =0; tower != towers.end(); ++tower, ++i)
    {
        if(tower->height < smallestHeight)
        {
            smallestTower = i;
            smallestHeight = tower->height;
        }
    }

    return smallestTower;
}

std::list<Block> trimTower(Towers::iterator& tower, Block& blockToInsert, std::list<Block>& blocksToRemove)
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
            for(auto blockIt = tower->begin(); blockIt != tower->end(); ++blockIt)
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
            sizeToRemove -= tallestBlock.hauteur;
        }
    }

    return blocksToRemove;
}

Tower findBestTowerToInsertInto(Towers& towers, Block& blockToInsert, Towers::iterator& smallestTower, 
                                             int& bestTowerToInsert, int& bestTowerInsertion)
{
    int smallestNbrOfBlocsToRemove = INT32_MAX;
    int bestSolutionSquareHeight = INT32_MAX;
    Tower smallestBlocksToRemove;
    std::list<Block> blocksToRemove;
    Towers::iterator towerToInsertIt;
    int towerToInsert;
    for(towerToInsertIt = towers.begin(), towerToInsert = 0; towerToInsertIt != towers.end(); ++towerToInsertIt, ++towerToInsert)
    {
        if(&*towerToInsertIt == &*smallestTower) continue;
        if(towerToInsertIt->isBlockTabou(blockToInsert.id)) continue;

        auto towerInsertionIt = towerToInsertIt->begin();
        int towerInsertion = 0;
        for (auto blockIt = towerToInsertIt->rbegin(); blockIt != towerToInsertIt->rend(); ++blockIt, ++towerInsertion)
        {
            if(blockToInsert.fitOn(*blockIt))
            {
                towerInsertionIt = blockIt.base();
                break;
            }
        }

        // Check if the blocks, that the new block did not fit on, fit on the new block. If not, tabou it.
        auto blockToRemoveIt = towerInsertionIt;
        for(blockToRemoveIt; blockToRemoveIt != towerToInsertIt->end() && !blockToRemoveIt->fitOn(blockToInsert); blockToRemoveIt++)
        {
            blocksToRemove.push_back(*blockToRemoveIt);
        }

        blocksToRemove = trimTower(towerToInsertIt, blockToInsert, blocksToRemove);

        if (blocksToRemove.size() <= smallestNbrOfBlocsToRemove)
        {
            Tower blocksToRemoveTower(blocksToRemove);
            bool isNewSolutionReallyBetter = true;
            
            if (blocksToRemove.size() == smallestNbrOfBlocsToRemove)
            {
                int towerToInsertSquareHeight = towerToInsertIt->height - blocksToRemoveTower.height + blockToInsert.hauteur;
                towerToInsertSquareHeight *= towerToInsertSquareHeight;

                int towerToRemoveSquareHeight = blocksToRemoveTower.height * blocksToRemoveTower.height;

                int smallestTowerSquareHeight = smallestTower->height - blockToInsert.hauteur;
                smallestTowerSquareHeight *= smallestTowerSquareHeight;

                int solutionSquareHeight = towerToInsertSquareHeight + towerToRemoveSquareHeight + smallestTowerSquareHeight;

                if (solutionSquareHeight > bestSolutionSquareHeight)
                {
                    bestSolutionSquareHeight = solutionSquareHeight;
                }
                else
                {
                    isNewSolutionReallyBetter = false;
                }
            }

            if (isNewSolutionReallyBetter)
            {
                smallestBlocksToRemove = blocksToRemove;
                bestTowerToInsert = towerToInsert;
                bestTowerInsertion = towerInsertion;
                smallestNbrOfBlocsToRemove = blocksToRemove.size();
            }
        }
        
        blocksToRemove.clear();
    }

    return smallestBlocksToRemove;
}

void tabou(Towers& currentSolution, bool(*predicate)(Block&, Block&))
{
    auto smallestTowerIt = std::next(currentSolution.begin(), findSmallestTower(currentSolution));
    int towerToInsert;
    int towerInsertion;
    Block blockToMove = smallestTowerIt->back();

    Tower blocksToRemove = 
        findBestTowerToInsertInto(currentSolution, blockToMove, smallestTowerIt, towerToInsert, towerInsertion);
    
    auto towerToInsertIt = std::next(currentSolution.begin(), towerToInsert);
    auto towerInsertionIt = std::next(towerToInsertIt->begin(), towerInsertion);
    towerToInsertIt->insert(towerInsertionIt, std::move(blockToMove));

    smallestTowerIt->pop_back();
    if (smallestTowerIt->empty())
    {
        currentSolution.erase(smallestTowerIt);
    }

    for(auto tower = currentSolution.begin(); tower != currentSolution.end(); ++tower)
    {
        tower->updateTabou();
    }
    
    for(auto blockIt = blocksToRemove.begin(); blockIt != blocksToRemove.end(); ++blockIt)
    { 
        auto removeIt = std::find_if(towerToInsertIt->begin(), towerToInsertIt->end(), 
            [blockIt](Block block) 
            { 
                return blockIt->id == block.id; 
            });
        towerToInsertIt->setTabou(blockIt->id);
        towerToInsertIt->erase(removeIt);
    }

    if (!blocksToRemove.empty())
    {
        blocksToRemove.sort(predicate);
        
        currentSolution.push_back(blocksToRemove);
    }
}

static moodycamel::BlockingConcurrentQueue<Towers> queue;
void run(std::list<Block> blocks, bool(*predicate)(Block&, Block&)) 
{
    Towers& solution = multiTowersVorace(blocks, predicate);
    int smallestSolutionSize = INT32_MAX;

    while(true)
    {
        if(solution.size() < smallestSolutionSize)
        {
            queue.enqueue(solution);
            smallestSolutionSize = solution.size();
        }
        tabou(solution, predicate);
    }
}

void runOnlyVorace(std::list<Block> blocks, bool(*predicate)(Block&, Block&)) 
{
    int smallestSolutionSize = INT32_MAX;
    while(true)
    {
        std::list<Block> copy = blocks;
        Towers& solution = multiTowersVorace(copy, predicate);
        if(solution.size() < smallestSolutionSize)
        {
            queue.enqueue(solution);
            smallestSolutionSize = solution.size();
        }
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
        int nbBlocks = std::stoi(line);

        std::getline(infile, line);
        maxTowerHeight = std::stoi(line);
        
        while (std::getline(infile, line)){
            Block block = Block(line);
            blocks.push_back(block);
        }
    }

    bool(* predicates [])(Block&, Block&) = {areaCriterion, areaAndShortestCriterion, squarestCriterion, squarestAndShortestCriterion};
    for (int i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
        std::thread(run, blocks, predicates[i]).detach();
    }

    int sizeBestSolution = INT32_MAX;
    // Check if the chosen possible tower is taller than the tallest tower encountered
    while(true)
    {
        Towers solution;
        queue.wait_dequeue(solution);
        if(solution.size() < sizeBestSolution)
        {
            sizeBestSolution = solution.size();
            if (prog_args.print_res) 
            {
                for (auto tower = solution.begin(); tower != solution.end(); tower++)
                {
                    tower->print();
                }
                std::cout << "fin" << std::endl;
            }
            else
            {
                std::cout << solution.size() << std::endl;
            }
        }
    }
}