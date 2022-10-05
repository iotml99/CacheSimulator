/**
 * @file direct_mapped.cpp
 * @author iotmlconsulting@gmail.com
 * @brief This file implements create, read and write methods for direct mapped cache simulation.
 * @version 0.1
 * @date 2021-11-07
 * @copyright Copyright (c) 2021
 *
 */

#include "cache_simulator.hpp"

/**
 * @brief Construct a new Direct Mapped Cache:: Direct Mapped Cache object
 *
 * @param cache_size uint32_t, size of cache in bytes
 * @param block_size uint32_t, size of each block in bytes
 * @param replacement_policy uint32_t replacement policy to be used
 */
DirectMappedCache::DirectMappedCache(uint32_t cache_size, uint32_t block_size, uint32_t replacement_policy) : Cache(cache_size, block_size)
{
    // Create a vector of cache blocks
    this->cache_blocks.reserve(num_blocks);
    for (auto i = 0; i < this->num_blocks; i++)
    {
        CacheBlock *block = new CacheBlock(block_size);
        this->cache_blocks.push_back(block);
    }
}

/**
 * @brief Destroy the Direct Mapped Cache:: Direct Mapped Cache object
 *
 */
DirectMappedCache::~DirectMappedCache()
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
void DirectMappedCache ::read(uint32_t address)
{
    this->access_info.cache_access++;
    this->access_info.read_access++;

    // Calculate index of cache block
    int block_address = address >> this->line_bits;
    int index = block_address % this->num_blocks;
    // Calculate address tag
    int addr_tag = address >> (this->line_bits + this->index_bits);

    bool previously_accessed = this->is_accessed(block_address);

    if (!previously_accessed)
    {
        this->access_info.compulsory_misses++;
        this->access_info.cache_misses++;
        this->access_info.read_misses++;
    }

    CacheBlock *block = this->cache_blocks[index];

    /* If cache block is empty */
    if (block->valid == false)
    {
        block->valid = true;
        block->tag = addr_tag;
    }
    // Block is valid
    // Check if tags match
    else if (block->tag == addr_tag)
    {
        /* No replacement needed */
    }
    else
    {
        /* Current block needs to be replaced*/

        if (previously_accessed)
        {
            this->access_info.conflict_misses++;
            this->access_info.read_misses++;
            this->access_info.cache_misses++;
        }
        if (block->dirty)
        {
            // Write back to the main memory
            this->access_info.dirty_blocks_evicted++;
            block->dirty = false;
        }
        // Update tag
        block->tag = addr_tag;
    }
}

/**
 * @brief Write byte of memory into main memory. If not present in cache, copy the block into cache.
 *
 * @param address uint32_t, address of byte to be written
 */
void DirectMappedCache ::write(uint32_t address)
{
    this->access_info.cache_access++;
    this->access_info.write_access++;

    // Calculate index of cache block
    int block_address = address >> this->line_bits;
    int index = block_address % this->num_blocks;
    // Calculate address tag
    int addr_tag = address >> (this->line_bits + this->index_bits);

    bool previously_accessed = this->is_accessed(block_address);

    if (!previously_accessed)
    {
        this->access_info.compulsory_misses++;
        this->access_info.cache_misses++;
        this->access_info.write_misses++;
    }

    CacheBlock *block = this->cache_blocks[index];

    // If block is empty
    if (block->valid == false)
    {
        block->valid = true;
        block->tag = addr_tag;
        // Write data into block
        block->dirty = true;
    }
    else if (block->tag == addr_tag)
    {
        // Block exists in the memory
        block->dirty = true;
    }
    else
    {
        // Replace this block
        if (previously_accessed)
        {
            this->access_info.conflict_misses++;
            this->access_info.cache_misses++;
            this->access_info.write_misses++;
        }
        if (block->dirty)
        {
            // Write back to the main memory
            this->access_info.dirty_blocks_evicted++;
            block->dirty = false;
        }
        // Update tag
        block->tag = addr_tag;
        // Write data
        block->dirty = true;
    }
}