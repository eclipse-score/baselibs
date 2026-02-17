/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator.h"
#include "score/analysis/tracing/common/shared_list/shared_list.h"

#include <unistd.h>
#include <iostream>
// Shared memory settings
constexpr const char* SHM_NAME = "/shared_list_";
constexpr size_t SHM_SIZE = 4096;

int main()
{
    std::list<int> std_list = {100, 200, 300};
    // Create shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        return 1;
    }

    // Allocate shared memory size
    if (ftruncate(shm_fd, SHM_SIZE) == -1)
    {
        perror("ftruncate");
        return 1;
    }

    // Map shared memory
    void* addr = mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    std::shared_ptr<score::analysis::tracing::IFlexibleCircularAllocator> flexible_allocator_ =
        std::make_shared<score::analysis::tracing::FlexibleCircularAllocator>(addr, SHM_SIZE);

    std::cout << " Available Memory Before List Allocation "
              << std::to_string(flexible_allocator_->GetAvailableMemory()) << std::endl;
    auto* list = static_cast<score::analysis::tracing::shared::List<int>*>(
        flexible_allocator_->Allocate(sizeof(score::analysis::tracing::shared::List<int>)).value());

    const auto my_list = new (list) score::analysis::tracing::shared::List<int>(flexible_allocator_);

    for (auto it = std_list.begin(); it != std_list.end(); ++it)
    {
        auto emplace_result = my_list->emplace_back(*it);
        if (!emplace_result.has_value())
        {
            my_list->clear();
            std::ignore = flexible_allocator_->Deallocate(my_list, sizeof(my_list));
        }
    }

    // Add elements
    my_list->push_back(10);
    my_list->push_back(20);
    my_list->push_back(30);
    std::cout << " Available Memory After Pushing three elements "
              << std::to_string(flexible_allocator_->GetAvailableMemory()) << std::endl;

    // Access the elements by at()
    auto res = my_list->at(1);
    if (res.has_value())
    {
        std::cout << "Element at index 1: " << res.value() << std::endl;
    }
    else
    {
        std::cout << "Error accessing index 1!" << std::endl;
    }

    // Iterate using iterators
    std::cout << "List elements: ";
    for (auto it = my_list->begin(); it != my_list->end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    // Cleanup (but keep shared memory for other processes)
    close(shm_fd);

    return 0;
}
