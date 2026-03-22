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
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <system_error>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

using ShmType = score::shm::example::BoundedContainers<int, 10, 1024>;

static void print(const char* prefix, const score::shm::Vector<int>& v)
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

static void parent(const int request_pipe, const int response_pipe)
{
    constexpr char kShmName[] = "/score_shm_example";

    // Create and size shared memory
    const int shm_fd = shm_open(kShmName, O_RDWR | O_CREAT, 0600);
    if (shm_fd < 0)
    {
        throw std::system_error{errno, std::generic_category(), "Parent: cannot create shared memory"};
    }
    if (ftruncate(shm_fd, sizeof(ShmType)) < 0)
    {
        throw std::system_error{errno, std::generic_category(), "Parent: cannot resize shared memory"};
    }

    auto* shm_ptr = static_cast<ShmType*>(
        mmap(nullptr, sizeof(ShmType), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (shm_ptr == MAP_FAILED)
    {
        throw std::system_error{errno, std::generic_category(), "Parent: cannot mmap shared memory"};
    }
    std::cerr << "Parent: shm_ptr=" << shm_ptr << std::endl;

    // Placement-new the BoundedContainers into shared memory
    new (shm_ptr) ShmType{};
    shm_ptr->vector.PushBackOrAbort(19);
    shm_ptr->vector.PushBackOrAbort(17);
    shm_ptr->vector.PushBackOrAbort(23);
    shm_ptr->map.EmplaceOrAbort(19, 190);
    shm_ptr->map.EmplaceOrAbort(17, 170);
    shm_ptr->map.EmplaceOrAbort(23, 230);
    print("Parent", shm_ptr->vector);
    print("Parent", shm_ptr->map);

    // Signal child with SHM name
    if (write(request_pipe, kShmName, sizeof(kShmName)) < 0)
    {
        throw std::system_error{errno, std::generic_category(), "Parent: cannot write to pipe"};
    }

    // Wait for child to finish
    char dummy{};
    if (read(response_pipe, &dummy, 1) < 0)
    {
        throw std::system_error{errno, std::generic_category(), "Parent: cannot read from pipe"};
    }
    std::cerr << "Parent: child done." << std::endl;
    print("Parent", shm_ptr->vector);
    print("Parent", shm_ptr->map);

    shm_unlink(kShmName);
}

int main(int, char* argv[])
{
    int request_pipe[2]{};
    int response_pipe[2]{};
    if ((pipe(request_pipe) < 0) || (pipe(response_pipe) < 0))
    {
        throw std::system_error{errno, std::generic_category(), "Cannot create pipe"};
    }

    const pid_t child_pid = fork();
    if (child_pid < 0)
    {
        throw std::system_error{errno, std::generic_category(), "Cannot fork"};
    }

    if (child_pid == 0)
    {
        // Child: redirect pipes and exec
        close(request_pipe[1]);
        dup2(request_pipe[0], STDIN_FILENO);
        close(response_pipe[0]);
        dup2(response_pipe[1], STDOUT_FILENO);
        execl(argv[1], argv[1], nullptr);
        throw std::system_error{errno, std::generic_category(), "Cannot execl child"};
    }

    // Parent
    std::cerr << "Parent pid=" << getpid() << " child pid=" << child_pid << std::endl;
    close(request_pipe[0]);
    close(response_pipe[1]);
    parent(request_pipe[1], response_pipe[0]);

    int status{};
    waitpid(child_pid, &status, 0);
    if (WIFEXITED(status))
    {
        std::cerr << "Parent: child exited with status " << WEXITSTATUS(status) << std::endl;
    }

    return 0;
}
