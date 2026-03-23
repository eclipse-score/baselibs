/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

#include "bounded_containers.h"

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <iostream>
#include <system_error>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

using ShmType = score::nothrow::example::BoundedContainers<int, 10, 1024>;

static void print(const char* prefix, const score::nothrow::Vector<int>& v)
{
    for (std::size_t i = 0U; i < v.size(); ++i)
    {
        std::cerr << prefix << ": v[" << i << "]=" << v[i] << std::endl;
    }
}

static void print(const char* prefix, const ShmType::MapType& map)
{
    std::cerr << prefix << ": map(size=" << map.size() << ")" << std::endl;
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        std::cerr << prefix << ": map[" << it->first << "]=" << it->second << std::endl;
    }
}

static void child(const int request_pipe, const int response_pipe)
{
    char shm_name[64]{};
    if (read(request_pipe, shm_name, sizeof(shm_name)) < 0)
    {
        throw std::system_error{errno, std::generic_category(), "Child: cannot read from pipe"};
    }
    std::cerr << "Child: received shm name \"" << shm_name << "\"" << std::endl;

    // Map shared memory
    const int shm_fd = shm_open(shm_name, O_RDWR, 0600);
    if (shm_fd < 0)
    {
        throw std::system_error{errno, std::generic_category(), "Child: cannot open shared memory"};
    }

    auto* shm_ptr = static_cast<ShmType*>(
        mmap(nullptr, sizeof(ShmType), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (shm_ptr == MAP_FAILED)
    {
        throw std::system_error{errno, std::generic_category(), "Child: cannot mmap shared memory"};
    }
    std::cerr << "Child: shm_ptr=" << shm_ptr << std::endl;

    // Read parent's data
    print("Child", shm_ptr->vector);
    print("Child", shm_ptr->map);

    // Append elements and sort
    std::cerr << "Child: appending four elements." << std::endl;
    shm_ptr->vector.PushBackOrAbort(7);
    shm_ptr->vector.PushBackOrAbort(5);
    shm_ptr->vector.PushBackOrAbort(3);
    shm_ptr->vector.PushBackOrAbort(2);
    print("Child", shm_ptr->vector);

    std::cerr << "Child: sorting..." << std::endl;
    std::sort(shm_ptr->vector.begin(), shm_ptr->vector.end());
    print("Child", shm_ptr->vector);

    // Mutate map data in-place (no new inserts => no allocator pressure).
    std::cerr << "Child: updating map values..." << std::endl;
    for (auto it = shm_ptr->map.begin(); it != shm_ptr->map.end(); ++it)
    {
        it->second += 1;
    }
    print("Child", shm_ptr->map);

    // Signal parent by closing response pipe
    std::cerr << "Child: closing response pipe." << std::endl;
    close(response_pipe);
}

int main()
{
    child(STDIN_FILENO, STDOUT_FILENO);
    return 0;
}
