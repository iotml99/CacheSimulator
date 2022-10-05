/**
 * @file cache_base.cpp
 * @author iotmlconsulting@gmail.com
 * @brief This file contains common functionalities of different types of caches.
 * @version 0.1
 * @date 2021-11-07
 * @copyright Copyright (c) 2021
 *
 */

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "cache_simulator.hpp"

/***********************Cacheblock************************/
CacheBlock::CacheBlock(uint32_t block_size)
{
    this->data = new uint8_t[block_size];
    this->valid = false;
    this->dirty = false;
}

CacheBlock::~CacheBlock()
{
    delete[] this->data;
}

/***************************End****************************/

/************************CacheReplace*********************/

/**
 * @brief Construct a new Cache Replace:: Cache Replace object
 *
 * @param policy Replacement policy to be used
 * @param num_sets number of sets
 * @param ways number of ways in each set
 */
CacheReplace::CacheReplace(uint32_t policy, uint32_t num_sets, uint32_t ways)
{
    /** Seed for random number generation **/
    srand(time(0));

    // Parameters
    this->replacement_policy = (CacheReplacement_t)policy;
    this->num_sets = num_sets;
    this->ways = ways;

    // Initialize metadata for pseudo LRU replacement
    if (this->replacement_policy == PSEUDO_LRU)
    {
        this->meta_data = vector<vector<int>>(this->num_sets);
        for (int i = 0; i < num_sets; i++)
        {
            /** Binary tree representation needs 2n-1 array elements for n ways**/
            this->meta_data[i] = vector<int>(2 * ways - 1);
            /* first 2n+1-n = n+1 items are 0 */
            for (int j = 0; j < ways - 1; j++)
            {
                this->meta_data[i][j] = 0;
            }
            for (int j = ways - 1; j < 2 * ways - 1; j++)
            {
                this->meta_data[i][j] = -1;
            }
        }
    }
}

/**
 * @brief Destroy the Cache Replace:: Cache Replace object
 *
 */
CacheReplace::~CacheReplace()
{
}

/**
 * @brief Get the victim index of block to be evicted based on replacement policy
 *
 * @param set_index set from where victim block is to be selected
 * @return int index of victim block
 */
int CacheReplace ::get_victim_index(uint32_t set_index, vector<CacheBlock *> data)
{
    int victim_index = -1;
    switch (this->replacement_policy)
    {
    case RANDOM:
        // For random replacement, choose a random number between 0 and ways-1
        victim_index = rand() % this->ways;
        break;

    case LRU:
        // For LRU replacement, index 0 is always selected
        // as it holds the least recently used block
        victim_index = 0;
        break;

    case PSEUDO_LRU:
    {
        // With pseudo LRU find victim index by traversing the binary tree
        // If current node has value 0 go to left subtree, if 1 go to right subtree
        // Return index of the leaf node encountered.

        // Start at root
        int current_index = 0;
        auto set_meta_data = this->meta_data[set_index];
        while (current_index < this->ways - 1)
        {
            // Check current node value
            if (set_meta_data[current_index] == 0)
            {
                // Go to left subtree
                current_index = 2 * current_index + 1;
            }
            else
            {
                // Go to right subree
                current_index = 2 * current_index + 2;
            }
        }

        // Calculate victim index based on leaf node index
        victim_index = current_index - this->ways + 1;
    }
    break;

    default:
        break;
    }
    return victim_index;
}

/**
 * @brief Mark a given block as recently accessed
 *
 * @param set_index set index where the block belongs
 * @param data vector representing content of cache set
 * @param block_index index of recently accessed block in the set
 */
void CacheReplace ::mark_accessed(uint32_t set_index, vector<CacheBlock *> &data, int block_index)
{
    if (this->replacement_policy == LRU)
    {
        // Move block to the end of vector
        auto it = data.begin();
        CacheBlock *accessed_block = data[block_index];
        data.erase(it + block_index);
        data.push_back(accessed_block);
    }
    if (this->replacement_policy == PSEUDO_LRU)
    {
        // Reverse value of all ancestors of leaf node representing current block
        CacheBlock *accessed_block = data[block_index];
        this->meta_data[set_index][block_index + this->ways - 1] = accessed_block->tag;
        int current_index = this->ways - 1 + block_index;
        do
        {
            current_index = (current_index - 1) / 2;
            this->meta_data[set_index][current_index] = this->meta_data[set_index][current_index] ? 0 : 1;
        } while (current_index > 0);
    }
}

/**
 * @brief Utility function to print meta data for a given set
 *
 * @param set_index index of set
 */
void CacheReplace ::print_metadata(uint32_t set_index)
{
    auto data = this->meta_data[set_index];
    cout << "Meta Data set " << set_index << data.size() << " " << this->meta_data[set_index].size() << endl;
    for (int i = 0; i < data.size(); i++)
    {
        cout << i << " " << data[i] << endl;
    }
}

/***************************End**************************/

/************************AccessInfo*********************/

/**
 * @brief Create and AccessInfo object. Initialize all counters to 0
 *
 */
AccessInfo::AccessInfo(/* args */)
{
    this->cache_access = 0;
    this->read_access = 0;
    this->write_access = 0;
    this->cache_misses = 0;
    this->compulsory_misses = 0;
    this->capacity_misses = 0;
    this->conflict_misses = 0;
    this->read_misses = 0;
    this->write_misses = 0;
    this->dirty_blocks_evicted = 0;
}

/**
 * @brief Destroy the Access Info:: Access Info object
 *
 */
AccessInfo::~AccessInfo()
{
}

/**
 * @brief Print access information of given cache
 *
 */
void AccessInfo::print()
{
    cout << "****************************" << endl;
    cout << "Cache Access :" << this->cache_access << endl;
    cout << "Read Access :" << this->read_access << endl;
    cout << "Write Access :" << this->write_access << endl;
    cout << "Cache Misses :" << this->cache_misses << endl;
    cout << "Compulsory Misses :" << this->compulsory_misses << endl;
    cout << "Capacity Misses :" << this->capacity_misses << endl;
    cout << "Conflict Misses :" << this->conflict_misses << endl;
    cout << "Read Misses :" << this->read_misses << endl;
    cout << "Write Misses :" << this->write_misses << endl;
    cout << "Dirty Blocks evicted :" << this->dirty_blocks_evicted << endl;
}

/*****************************End********************************/

/****************************Cache Base Class********************/

/**
 * @brief Construct a new Cache:: Cache object
 *
 * @param cache_size Size of cache memory in bytes
 * @param block_size Size of block in bytes
 */
Cache::Cache(uint32_t cache_size, uint32_t block_size)
{
    this->cache_size = cache_size;
    this->block_size = block_size;
    this->num_blocks = cache_size / block_size;
    for (int i = 0; i < 32; i++)
    {
        if ((1 << i) == this->block_size)
        {
            this->line_bits = i;
            break;
        }
    }
    for (int i = 0; i < 32; i++)
    {
        if ((1 << i) == this->num_blocks)
        {
            this->index_bits = i;
            break;
        }
    }
}

/**
 * @brief Destroy the Cache:: Cache object
 *
 */
Cache::~Cache()
{
}

/**
 * @brief Print access information for this cache
 *
 */
void Cache ::print_access_info()
{
    this->access_info.print();
}


bool Cache::is_accessed(uint32_t block_address){
    for(auto it = this->accessed_blocks.begin(); it != this->accessed_blocks.end(); ++it)
    {
        if(*it == block_address){
            return true;
        }
    }
    this->accessed_blocks.push_back(block_address);
    return false;
}

/*****************************End************************/