/**
 * @file cache_simulator.hpp
 * @author iotmlconsulting@gmail.com
 * @brief This file declares classes and methods used.
 * @version 0.1
 * @date 2021-11-07
 * @copyright Copyright (c) 2021
 *
 */

#ifndef CACHE_SIMULATOR_HPP
#define CACHE_SIMULATOR_HPP

#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <iterator>
#include <bits/stdc++.h>

using namespace std;

/** Cache Types **/
typedef enum
{
    FULLY_ASSOCIATIVE,
    DIRECT_MAPPED,
    SET_ASSOCIATIVE_2 = 2,
    SET_ASSOCIATIVE_4 = 4,
    SET_ASSOCIATIVE_6 = 6,
    SET_ASSOCIATIVE_8 = 8,
    SET_ASSOCIATIVE_16 = 16,
    SET_ASSOCIATIVE_32 = 32,
} CacheMap_t;

/** Replacement Policy **/
typedef enum
{
    RANDOM,
    LRU,
    PSEUDO_LRU,
} CacheReplacement_t;

/**
 * @brief This class represents individual blocks in the cache memory
 *
 */
class CacheBlock
{
private:
public:
    /* data */
    uint32_t tag;
    bool dirty;
    bool valid;
    uint8_t *data;

    CacheBlock(uint32_t block_size);
    ~CacheBlock();
};

/**
 * @brief This class represents access information for a cache memory
 *
 */
class AccessInfo
{
private:
public:
    /* data */
    uint32_t cache_access;
    uint32_t read_access;
    uint32_t write_access;
    uint32_t cache_misses;
    uint32_t compulsory_misses;
    uint32_t capacity_misses;
    uint32_t conflict_misses;
    uint32_t read_misses;
    uint32_t write_misses;
    uint32_t dirty_blocks_evicted;

    AccessInfo(/* args */);
    ~AccessInfo();
    void print();
};

/**
 * @brief This class represents replacement policy of cache memory
 *
 */
class CacheReplace
{
private:
    /* data */
    uint32_t num_sets;
    uint32_t ways;
    CacheReplacement_t replacement_policy;
    vector<vector<int>> meta_data;

public:
    CacheReplace(uint32_t policy, uint32_t num_sets, uint32_t ways);
    ~CacheReplace();
    int get_victim_index(uint32_t set_index, vector<CacheBlock *> data);
    void mark_accessed(uint32_t set_index, vector<CacheBlock *> &data, int index);
    void print_metadata(uint32_t set_index);
};

/**
 * @brief This class represents a generic cache memory
 *
 */
class Cache
{
private:
    /* data */

protected:
    /* data */
    uint32_t cache_size;
    uint32_t block_size;
    uint32_t num_blocks;
    uint32_t line_bits;
    uint32_t index_bits;
    AccessInfo access_info;
    CacheReplace *cache_repl;

    list <uint32_t> accessed_blocks;

public:
    Cache(uint32_t cache_size, uint32_t block_size);
    ~Cache();
    void read(uint32_t address);
    void write(uint32_t address);
    void print_access_info();
    void print_cache();
    bool is_accessed(uint32_t block_address);
};

/**
 * @brief This class represents a fully associative cache memory
 *
 */
class FullyAssocCache : public Cache
{
private:
    /* data */
    vector<CacheBlock *> cache_blocks;
    bool all_blocks_valid;

public:
    FullyAssocCache(uint32_t cache_size, uint32_t block_size, uint32_t replacemen_policy);
    ~FullyAssocCache();
    void read(uint32_t address);
    void write(uint32_t address);
    void print_cache();
};

/**
 * @brief This class represents a direct mapped cache memory
 *
 */
class DirectMappedCache : public Cache
{
private:
    /* data */
    vector<CacheBlock *> cache_blocks;

public:
    DirectMappedCache(uint32_t cache_size, uint32_t block_size, uint32_t replacement_policy);
    ~DirectMappedCache();
    void read(uint32_t address);
    void write(uint32_t address);
};

/**
 * @brief This class represents a set associative cache memory
 *
 */
class SetAssocCache : public Cache
{
private:
    vector<vector<CacheBlock *>> cache_blocks;
    uint32_t num_sets;
    uint32_t num_ways;

public:
    SetAssocCache(uint32_t cache_size, uint32_t block_size, uint32_t num_sets, uint32_t replacemen_policy);
    ~SetAssocCache();
    void read(uint32_t address);
    void write(uint32_t address);
    void print_cache();
};

#endif
