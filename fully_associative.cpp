/**
 * @file fully_associative.cpp
 * @author iotmlconsulting@gmail.com
 * @brief This file implements create, read and write methods for fully associative cache simulation.
 * @version 0.1
 * @date 2021-11-07
 * @copyright Copyright (c) 2021
 *
 */

#include "cache_simulator.hpp"

/**
 * @brief Construct a new Fully Assoc Cache:: Fully Assoc Cache object
 *
 * @param cache_size uint32_t, size of cache in bytes
 * @param block_size uint32_t, size of each block in bytes
 * @param replacement_policy uint32_t replacement policy to be used
 */
FullyAssocCache::FullyAssocCache(uint32_t cache_size, uint32_t block_size, uint32_t replacement_policy) : Cache(cache_size, block_size)
{
    // Create a vector of cache blocks
    this->cache_blocks.reserve(num_blocks);
    this->all_blocks_valid = false;
    for (auto i = 0; i < this->num_blocks; i++)
    {
        CacheBlock *block = new CacheBlock(block_size);
        this->cache_blocks.push_back(block);
    }
    /** Initialize replacement with 1 set and num_blocks ways */
    cache_repl = new CacheReplace(replacement_policy, 1, this->num_blocks);
}

/**
 * @brief Destroy the Fully Assoc Cache:: Fully Assoc Cache object
 *
 */
FullyAssocCache::~FullyAssocCache()
{
    while (!this->cache_blocks.empty())
    {
        CacheBlock *block = this->cache_blocks.back();
        this->cache_blocks.pop_back();
        delete block;
    }
}

/**
 * @brief Read byte of memory from main memory. If not present in cache, copy the block into cache.
 *
 * @param address uint32_t, address of byte to be read
 */
void FullyAssocCache ::read(uint32_t address)
{

    this->access_info.cache_access++;
    this->access_info.read_access++;

    // Calculate address tag
    uint32_t addr_tag = address >> (this->line_bits + this->index_bits);
    int block_address = address >> this->line_bits;

    bool previously_accessed = this->is_accessed(block_address);

    if (!previously_accessed)
    {
        this->access_info.compulsory_misses++;
        this->access_info.cache_misses++;
        this->access_info.read_misses++;
    }

    int empty_block_index = -1;
    int found_block_index = -1;

    /* Check if tag is already present, also find index of first empty block */
    for (int i = 0; i < this->cache_blocks.size(); i++)
    {
        CacheBlock *block = this->cache_blocks[i];
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
            int victim_index = cache_repl->get_victim_index(0, this->cache_blocks);

            CacheBlock *victim_block = this->cache_blocks[victim_index];
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
            CacheBlock *block = this->cache_blocks[empty_block_index];
            block->valid = true;
            block->tag = addr_tag;
            found_block_index = empty_block_index;
        }
    }

    // Mark this block as accessed
    this->cache_repl->mark_accessed(0, this->cache_blocks, found_block_index);
}

/**
 * @brief Write byte of memory into main memory. If not present in cache, copy the block into cache.
 *
 * @param address uint32_t, address of byte to be written
 */
void FullyAssocCache ::write(uint32_t address)
{
    this->access_info.cache_access++;
    this->access_info.write_access++;

    // Calculate address tag
    uint32_t addr_tag = address >> (this->line_bits + this->index_bits);
    int block_address = address >> this->line_bits;

    bool previously_accessed = this->is_accessed(block_address);

    if (!previously_accessed)
    {
        this->access_info.compulsory_misses++;
        this->access_info.cache_misses++;
        this->access_info.write_misses++;
    }

    int empty_block_index = -1;
    int found_block_index = -1;

    /* Check if tag is already present, also find index of first empty block */
    for (int i = 0; i < this->cache_blocks.size(); i++)
    {
        CacheBlock *block = this->cache_blocks[i];
        if (block->valid == false && empty_block_index == -1)
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
        /* block already present in cache, set dirty bit */
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
            int victim_index = cache_repl->get_victim_index(0, this->cache_blocks);
            CacheBlock *victim_block = this->cache_blocks[victim_index];
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
            CacheBlock *block = this->cache_blocks[empty_block_index];
            block->valid = true;
            block->tag = addr_tag;
            found_block_index = empty_block_index;
        }
    }

    // Set the dirty bit and mark the block as recently accessed
    CacheBlock *block = this->cache_blocks[found_block_index];
    block->dirty = true;
    this->cache_repl->mark_accessed(0, this->cache_blocks, found_block_index);
}

/**
 * @brief print the cache memory metadata
 *
 */
void FullyAssocCache ::print_cache()
{
    for (int i = 0; i < this->num_blocks; i++)
    {
        CacheBlock *block = this->cache_blocks[i];
        cout << i << " V " << block->valid << " D " << block->dirty << " T " << block->tag << endl;
    }
}