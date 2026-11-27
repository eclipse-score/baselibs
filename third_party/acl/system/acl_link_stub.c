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

/*
 * Link-time stand-in for the system libacl.so.1.
 * It is only linked against and never loaded, because its soname is libacl.so.1 and it is not on the run path.
 * The dynamic loader resolves the real library on the target at run time.
 */

#include <acl/libacl.h>
#include <errno.h>
#include <stddef.h>
#include <sys/acl.h>

#define STUB_FAIL(value) \
    do                   \
    {                    \
        errno = ENOSYS;  \
        return (value);  \
    } while (0)

int acl_add_perm(acl_permset_t permset_d, acl_perm_t perm)
{
    (void)permset_d;
    (void)perm;
    STUB_FAIL(-1);
}

int acl_calc_mask(acl_t* acl_p)
{
    (void)acl_p;
    STUB_FAIL(-1);
}

int acl_clear_perms(acl_permset_t permset_d)
{
    (void)permset_d;
    STUB_FAIL(-1);
}

int acl_create_entry(acl_t* acl_p, acl_entry_t* entry_p)
{
    (void)acl_p;
    (void)entry_p;
    STUB_FAIL(-1);
}

int acl_free(void* obj_p)
{
    (void)obj_p;
    STUB_FAIL(-1);
}

int acl_get_entry(acl_t acl, int entry_id, acl_entry_t* entry_p)
{
    (void)acl;
    (void)entry_id;
    (void)entry_p;
    STUB_FAIL(-1);
}

acl_t acl_from_text(const char* buf_p)
{
    (void)buf_p;
    STUB_FAIL(NULL);
}

acl_t acl_get_fd(int fd)
{
    (void)fd;
    STUB_FAIL(NULL);
}

acl_t acl_get_file(const char* path_p, acl_type_t type)
{
    (void)path_p;
    (void)type;
    STUB_FAIL(NULL);
}

int acl_get_perm(acl_permset_t permset_d, acl_perm_t perm)
{
    (void)permset_d;
    (void)perm;
    STUB_FAIL(-1);
}

int acl_get_permset(acl_entry_t entry_d, acl_permset_t* permset_p)
{
    (void)entry_d;
    (void)permset_p;
    STUB_FAIL(-1);
}

void* acl_get_qualifier(acl_entry_t entry_d)
{
    (void)entry_d;
    STUB_FAIL(NULL);
}

// NOLINTNEXTLINE(readability-non-const-parameter): the signature must match libacl.
int acl_get_tag_type(acl_entry_t entry_d, acl_tag_t* tag_type_p)
{
    (void)entry_d;
    (void)tag_type_p;
    STUB_FAIL(-1);
}

int acl_set_fd(int fd, acl_t acl)
{
    (void)fd;
    (void)acl;
    STUB_FAIL(-1);
}

int acl_set_qualifier(acl_entry_t entry_d, const void* tag_qualifier_p)
{
    (void)entry_d;
    (void)tag_qualifier_p;
    STUB_FAIL(-1);
}

int acl_set_tag_type(acl_entry_t entry_d, acl_tag_t tag_type)
{
    (void)entry_d;
    (void)tag_type;
    STUB_FAIL(-1);
}

// NOLINTNEXTLINE(readability-non-const-parameter): the signature must match libacl.
char* acl_to_text(acl_t acl, ssize_t* len_p)
{
    (void)acl;
    (void)len_p;
    STUB_FAIL(NULL);
}

int acl_valid(acl_t acl)
{
    (void)acl;
    STUB_FAIL(-1);
}
