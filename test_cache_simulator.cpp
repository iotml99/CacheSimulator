/**
 * @file test_cache_simulator.cpp
 * @author iotmlconsulting@gmail.com
 * @brief This file is the main interface for cache simulator, and executes different types of cache simulations.
 * @version 0.1
 * @date 2021-11-07
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <iostream>
#include <string>
#include <fstream>
#include <assert.h>
#include "cache_simulator.hpp"

using namespace std;

/**
 * @brief Simulate direct mapped chache based on parameters
 *
 * @param cache_size uint32_t, size of cache in bytes
 * @param block_size uint32_t, size of each block in bytes
 * @param replacement_policy uint32_t, replacement policy
 * @param traces_file string, file containing access traces
 */
void simulate_direct_mapped_cache(uint32_t cache_size, uint32_t block_size, uint32_t replacement_policy, string traces_file)
{

    DirectMappedCache Cache(cache_size, block_size, replacement_policy);

    std::ifstream file;
    uint32_t address;
    char operation;
    file.open(traces_file.c_str(), ios::in);
    if (!file)
    {
        cout << traces_file << " not found" << endl;
        exit(1);
    }
    for (std::string line; std::getline(file, line);)
    {
        sscanf(line.c_str(), "0x%x %c", &address, &operation);

#ifdef DEBUG
        cout << address << " " << operation << endl;
#endif
        if (operation == 'r')
        {
            Cache.read(address);
        }
        else
        {
            Cache.write(address);
        }
    }
    Cache.print_access_info();
}

/**
 * @brief Simulate fully associative chache based on parameters
 *
 * @param cache_size uint32_t, size of cache in bytes
 * @param block_size uint32_t, size of each block in bytes
 * @param replacement_policy uint32_t, replacement policy
 * @param traces_file string, file containing access traces
 */
void simulate_fullyassoc_cache(uint32_t cache_size, uint32_t block_size, uint32_t replacement_policy, string traces_file)
{

    FullyAssocCache Cache(cache_size, block_size, replacement_policy);

    std::ifstream file;
    uint32_t address;
    char operation;
    file.open(traces_file.c_str(), ios::in);
    if (!file)
    {
        cout << traces_file << " not found" << endl;
        exit(1);
    }
    for (std::string line; std::getline(file, line);)
    {
        sscanf(line.c_str(), "0x%x %c", &address, &operation);
#ifdef DEBUG
        cout << address << " " << operation << endl;
#endif
        if (operation == 'r')
        {
            Cache.read(address);
        }
        else
        {
            Cache.write(address);
        }
#ifdef DEBUG
        Cache.print_cache();
#endif
    }
    Cache.print_access_info();
}

/**
 * @brief Simulate fully associative chache based on parameters
 * @param associativity uint32_t, number of ways in each set
 * @param cache_size uint32_t, size of cache in bytes
 * @param block_size uint32_t, size of each block in bytes
 * @param replacement_policy uint32_t, replacement policy
 * @param traces_file string, file containing access traces
 */
void simulate_setassoc_cache(uint32_t associativity, uint32_t cache_size, uint32_t block_size, uint32_t replacement_policy, string traces_file)
{

    SetAssocCache Cache(cache_size, block_size, associativity, replacement_policy);

    std::ifstream file;
    uint32_t address;
    char operation;
    file.open(traces_file.c_str(), ios::in);
    if (!file)
    {
        cout << traces_file << " not found" << endl;
        exit(1);
    }
    for (std::string line; std::getline(file, line);)
    {
        sscanf(line.c_str(), "0x%x %c", &address, &operation);
#ifdef DEBUG
        cout << address << " " << operation << endl;
#endif
        if (operation == 'r')
        {
            Cache.read(address);
        }
        else
        {
            Cache.write(address);
        }
#ifdef DEBUG
        Cache.print_cache();
#endif
    }

    Cache.print_access_info();
}

/**
 * @brief Check if given number is a power of 2
 *
 * @param x number to be tested
 * @return true
 * @return false
 */
bool valid_pow2(uint32_t x)
{
    for (int i = 0; i < 31; i++)
    {
        int y = 1 << i;
        if (x == y)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Check if associativity is valid(2, 4, 8, 16, 32) for set associative cache
 *
 * @param x associativity to be tested
 * @return true
 * @return false
 */
bool valid_assoc(uint32_t x)
{
    for (int i = 1; i <= 5; i++)
    {
        int y = 1 << i;
        if (x == y)
        {
            return true;
        }
    }
    return false;
}

int main()
{
    uint32_t associativity, replacement_policy, cache_size, block_size;
    string traces_file;

    cout << "***** Cache Simulator Start *****" << endl;
    cout << "Enter cache size: ";
    cin >> cache_size;
    cout << "Enter block size: ";
    cin >> block_size;
    cout << "Enter Associativity: ";
    cin >> associativity;
    cout << "Enter replacement policy: ";
    cin >> replacement_policy;
    cout << "Enter traces file path: ";
    cin >> traces_file;

    if (!valid_pow2(cache_size))
    {
        cout << "Invalid cache size " << cache_size << endl;
        return 1;
    }

    if (!valid_pow2(block_size))
    {
        cout << "Invalid block size " << block_size << endl;
        return 1;
    }

    if (replacement_policy > PSEUDO_LRU)
    {
        cout << "Invalid replacement policy " << replacement_policy << endl;
    }

    cout << "***********************" << endl;
    cout << "Cache Settings for Simulation" << endl;

    cout << cache_size << endl;
    cout << block_size << endl;

    string replacement_policy_str[3] = {"Random", "LRU", "Pseudo LRU"};

    switch (associativity)
    {
    case DIRECT_MAPPED:
        cout << "Direct Mapped Cache" << endl;
        cout << replacement_policy_str[replacement_policy] << endl;
        simulate_direct_mapped_cache(cache_size, block_size, replacement_policy, traces_file);
        break;

    case FULLY_ASSOCIATIVE:
        cout << "Fully Associative Cache" << endl;
        cout << replacement_policy_str[replacement_policy] << endl;
        simulate_fullyassoc_cache(cache_size, block_size, replacement_policy, traces_file);
        break;

    default:
        if (!valid_assoc(associativity))
        {
            cout << "Invalid Associativity " << cache_size << endl;
            return 1;
        }
        cout << associativity << " Way Set Associative Cache" << endl;
        cout << replacement_policy_str[replacement_policy] << endl;
        simulate_setassoc_cache(associativity, cache_size, block_size, replacement_policy, traces_file);
        break;
    }
    cout << "*****************Simulation End**************" << endl;
    return 0;
}
