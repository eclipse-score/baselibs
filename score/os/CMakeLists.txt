# *******************************************************************************
# Copyright (c) 2025 Contributors to the Eclipse Foundation
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************

# OS abstraction library with core functionality
add_library(score_os
    # Core OS functionality
    unistd.cpp
    stdio.cpp
    stdio_impl.cpp
    errno.cpp
    errno_logging.cpp
    sys_poll.cpp
    sys_poll_impl.cpp
    
    # File system operations
    dirent.cpp
    dirent_impl.cpp
    fcntl.cpp
    fcntl_impl.cpp
    file_stream.cpp
    glob.cpp
    glob_impl.cpp
    inotify.cpp
    inotify_impl.cpp
    ioctl.cpp
    ioctl_impl.cpp
    libgen.cpp
    libgen_impl.cpp
    makedev.cpp
    mman.cpp
    mount.cpp
    mount_impl.cpp
    stat.cpp
    stat_impl.cpp
    statvfs.cpp
    statvfs_impl.cpp
    
    # Process and system operations
    getopt.cpp
    getopt_impl.cpp
    grp.cpp
    grp_impl.cpp
    sched.cpp
    sched_impl.cpp
    select.cpp
    select_impl.cpp
    semaphore.cpp
    semaphore_impl.cpp
    sigevent.cpp
    sigevent_error.cpp
    sigevent_impl.cpp
    spawn.cpp
    spawn_impl.cpp
    stdlib.cpp
    stdlib_impl.cpp
    string.cpp
    string_impl.cpp
    sys_uio.cpp
    sys_uio_impl.cpp
    sys_wait.cpp
    sys_wait_impl.cpp
    time.cpp
    time_impl.cpp
    uname.cpp
    uname_impl.cpp
    
    # Network operations
    arpa_inet.cpp
    ifaddrs.cpp
    ifaddrs_impl.cpp
    netdb.cpp
    net_if.cpp
    socket.cpp
    socket_impl.cpp
    
    # Security and capabilities
    acl.cpp
    acl_impl.cpp
    capability.cpp
    
    # Hardware/CPU specific
    cpuid.cpp
    
    # Message queues
    mqueue.cpp
    
    # POSIX functionality
    posix/ftw.cpp
    
    # Platform-specific functionality
    linux_pthread.cpp
    pthread.cpp
    
    # Linux-specific implementations (conditional based on availability)
    # linux/capability.cpp  # Requires libcap-dev
    linux/endmntent.cpp
    linux/getmntent.cpp
    # linux/ifaddrs.cpp     # May require specific headers
    linux/linuxsched.cpp
    linux/pthread.cpp
    linux/setmntent.cpp
    # linux/shm.cpp         # May require specific headers
    linux/statfs.cpp
    linux/sysinfo.cpp
    # linux/timerfd.cpp     # May require specific headers
    # linux/timex.cpp       # May require specific headers
    linux/unistd.cpp
    # linux/utils/network_interface.cpp  # May require specific headers
    
    # Utils directory - Linux implementations only
    utils/abortable_blocking_reader.cpp
    utils/acl/access_control_list.cpp
    utils/acl/i_access_control_list.cpp
    utils/detect_os.cpp
    utils/inotify/inotify_event.cpp
    utils/inotify/inotify_instance.cpp
    utils/inotify/inotify_instance_facade.cpp
    utils/inotify/inotify_instance_impl.cpp
    utils/inotify/inotify_watch_descriptor.cpp
    utils/interprocess/interprocess_conditional_variable.cpp
    utils/interprocess/interprocess_mutex.cpp
    utils/interprocess/interprocess_notification.cpp
    utils/machine.cpp
    utils/machine_seam.cpp
    utils/machine_seam_impl.cpp
    utils/mqueue.cpp
    utils/path.cpp
    utils/path_linux.cpp
    utils/semaphore.cpp
    utils/signal.cpp
    utils/signal_impl.cpp
    utils/spinlock.cpp
    utils/tcp_keep_alive_linux.cpp
    utils/thread.cpp
    utils/thread_linux.cpp
)

# Use quote includes like Bazel to avoid header name conflicts with system headers
# -iquote ensures that #include "header.h" searches SCORE headers first
# while #include <header.h> searches system headers first
target_compile_options(score_os
    PRIVATE
        -iquote ${CMAKE_CURRENT_SOURCE_DIR}
        -iquote ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Also apply -iquote to dependent targets
target_compile_options(score_os
    INTERFACE
        -iquote ${CMAKE_CURRENT_SOURCE_DIR}
        -iquote ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Add compile definitions to ensure proper system header inclusion
target_compile_definitions(score_os
    PRIVATE
        _GNU_SOURCE=1
        _POSIX_C_SOURCE=200809L
        _DEFAULT_SOURCE=1
)

# Do not use standard include directories for OS headers to avoid conflicts
# Only provide the parent directory for external code that needs score/os/header.h
# Use -iquote for score headers to avoid shadowing system headers
target_compile_options(score_os 
    PRIVATE 
        -iquote ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        -iquote ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_include_directories(score_os 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)



target_link_libraries(score_os
    PUBLIC
        score_compiler_flags
        score_language_futurecpp
        score_bitmask_operators
        pthread
        acl
        $<LINK_ONLY:score_static_reflection_visitor>
)

# OS Fake/Mock library for testing
add_library(score_os_fake
    utils/inotify/inotify_instance_mock.cpp
    utils/inotify/inotify_instance_mock.h
)

# Use -iquote for score headers to avoid shadowing system headers
target_compile_options(score_os_fake
    PRIVATE 
        -iquote ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        -iquote ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_include_directories(score_os_fake
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)



target_link_libraries(score_os_fake
    PUBLIC
        score_compiler_flags
        score_os
)

# Create alias targets
add_library(score::os ALIAS score_os)
add_library(score::os_fake ALIAS score_os_fake)

# Install target
install(TARGETS score_os score_os_fake
    EXPORT score-baselibsTargets
)
