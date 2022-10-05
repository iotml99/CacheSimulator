/**
 * @file direct_mapped.cpp
 * @author iotmlconsulting@gmail.com
 * @brief This file implements create, read and write methods for set associative cache simulation.
 * @version 0.1
 * @date 2021-11-07
 * @copyright Copyright (c) 2021
 *
 */

#include "cache_simulator.hpp"

/**
 * @brief Set the Assoc Cache:: Set Assoc Cache object
 *
 * @param cache_size size of cache memory in bytes
 * @param block_size size of each block in bytes
 * @param num_ways number of ways in each set
 * @param replacement_policy replacement policy for choosing victim blocks
 */
SetAssocCache::SetAssocCache(uint32_t cache_size, uint32_t block_size, uint32_t num_ways, uint32_t replacement_policy) : Cache(cache_size, block_size)
{
    /** Initialize memory with a vector of size num_sets X num_ways **/
    this->num_ways = num_ways;
    this->num_sets = this->num_blocks / this->num_ways;
    this->cache_blocks.reserve(num_sets);
    for (auto i = 0; i < this->num_sets; i++)
    {
        this->cache_blocks[i].reserve(num_ways);
        for (auto j = 0; j < this->num_ways; j++)
        {
            CacheBlock *block = new CacheBlock(block_size);
            this->cache_blocks[i].push_back(block);
        }
    }
    cache_repl = new CacheReplace(replacement_policy, this->num_sets, this->num_ways);
}

/**
 * @brief Destroy the Set Assoc Cache::~ Set Assoc Cache object
 *
 */
SetAssocCache::~SetAssocCache()
{
}

/**
 * @brief Read byte of memory from main memory. If not present in cache, copy the block into cache.
 *
 * @param address uint32_t, address of byte to be read
 */
void SetAssocCache ::read(uint32_t address)
{
    this->access_info.cache_access++;
    this->access_info.read_access++;

    // Calculate address tag and mapped set index
    int set_index = (address >> this->line_bits) % this->num_sets;
    int addr_tag = address >> (this->line_bits + this->index_bits);
    int empty_block_index = -1;
    int found_block_index = -1;

    int block_address = address >> this->line_bits;

    bool previously_accessed = this->is_accessed(block_address);

    if (!previously_accessed)
    {
        this->access_info.compulsory_misses++;
        this->access_info.cache_misses++;
        this->access_info.read_misses++;
    }

    /* Check if tag is already present, also find index of first empty block */
    for (int i = 0; i < this->cache_blocks[set_index].size(); i++)
    {
        CacheBlock *block = this->cache_blocks[set_index][i];
        if (block->valid == false and empty_block_index == -1)
        {
            empty_block_index = i;
        }
        if (block->tag == addr_tag)
        {
            /* Found the correct block in memory */
            found_block_index = i;
            break;
        }
    }

    if (found_block_index != -1)
    {
        /* block already present in cache, do nothing */
    }
    else
    {

        if (empty_block_index == -1)
        {
            /* If no empty block is found, existing block is to be evicted based on replacement policy */

            if (previously_accessed)
            {
                this->access_info.cache_misses++;
                this->access_info.read_misses++;
                this->access_info.capacity_misses++;
            }
            int victim_index = cache_repl->get_victim_index(set_index, this->cache_blocks[set_index]);
            CacheBlock *victim_block = this->cache_blocks[set_index][victim_index];
            if (victim_block->dirty)
            {
                // Write back to the main memory
                this->access_info.dirty_blocks_evicted++;
                victim_block->dirty = false;
            }
            victim_block->tag = addr_tag;
            found_block_index = victim_index;
        }
        else
        {
            /* If empty block is present read from main memory and replace empty block */

            CacheBlock *block = this->cache_blocks[set_index][empty_block_index];
            block->valid = true;
            block->tag = addr_tag;
            found_block_index = empty_block_index;
        }
    }
    // Mark the accessed block
    this->cache_repl->mark_accessed(set_index, this->cache_blocks[set_index], found_block_index);
}

/**
 * @brief Write byte of memory into main memory. If not present in cache, copy the block into cache.
 *
 * @param address uint32_t, address of byte to be written
 */
void SetAssocCache ::write(uint32_t address)
{
    // Calculate address tag and mapped set index
    this->access_info.cache_access++;
    this->access_info.write_access++;
    int set_index = (address >> this->line_bits) % this->num_sets;
    int addr_tag = address >> (this->line_bits + this->index_bits);
    int empty_block_index = -1;
    int found_block_index = -1;

    int block_address = address >> this->line_bits;

    bool previously_accessed = this->is_accessed(block_address);

    if (!previously_accessed)
    {
        this->access_info.compulsory_misses++;
        this->access_info.cache_misses++;
        this->access_info.write_misses++;
    }

    /* Check if tag is already present, also find index of first empty block */
    for (int i = 0; i < this->cache_blocks[set_index].size(); i++)
    {
        CacheBlock *block = this->cache_blocks[set_index][i];
        if (block->valid == false and empty_block_index == -1)
        {
            empty_block_index = i;
        }
        if (block->tag == addr_tag)
        {
            /* Found the correct block in memory */
            found_block_index = i;
            break;
        }
    }

    if (found_block_index != -1)
    {
        /* block already present in cache, do nothing */
    }
    else
    {

        if (empty_block_index == -1)
        {
            /* If no empty block is found, existing block is to be evicted based on replacement policy */

            if (previously_accessed)
            {
                this->access_info.cache_misses++;
                this->access_info.write_misses++;
                this->access_info.capacity_misses++;
            }
            int victim_index = cache_repl->get_victim_index(set_index, this->cache_blocks[set_index]);
            CacheBlock *victim_block = this->cache_blocks[set_index][victim_index];
            if (victim_block->dirty)
            {
                // Write back to the main memory
                this->access_info.dirty_blocks_evicted++;
                victim_block->dirty = false;
            }
            victim_block->tag = addr_tag;
            found_block_index = victim_index;
        }
        else
        {
            /* If empty block is present read from main memory and replace empty block */
            CacheBlock *block = this->cache_blocks[set_index][empty_block_index];
            block->valid = true;
            block->tag = addr_tag;
            found_block_index = empty_block_index;
        }
    }
    // Mark block as dirty and recently accessed
    this->cache_blocks[set_index][found_block_index]->dirty = true;
    this->cache_repl->mark_accessed(set_index, this->cache_blocks[set_index], found_block_index);
}

/**
 * @brief Print metadata from cache memory
 *
 */
void SetAssocCache::print_cache()
{
    for (int i = 0; i < this->num_sets; i++)
    {
        cout << "**** Set " << i << endl;
        for (int j = 0; j < this->num_ways; j++)
        {
            CacheBlock *block = this->cache_blocks[i][j];
            cout << i << " V " << block->valid << " D " << block->dirty << " T " << block->tag << endl;
        }
    }
}