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
#ifndef THIRD_PARTY_ACL_SYSTEM_INCLUDE_SYS_ACL_H
#define THIRD_PARTY_ACL_SYSTEM_INCLUDE_SYS_ACL_H

/* Declarations of the POSIX.1e ACL functions used by score/os, matching the libacl.so.1 ABI. */

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int acl_type_t;
typedef int acl_tag_t;
typedef unsigned int acl_perm_t;

typedef struct __acl_ext* acl_t;
typedef struct __acl_entry_ext* acl_entry_t;
typedef struct __acl_permset_ext* acl_permset_t;

#define ACL_READ (0x04)
#define ACL_WRITE (0x02)
#define ACL_EXECUTE (0x01)

#define ACL_UNDEFINED_TAG (0x00)
#define ACL_USER_OBJ (0x01)
#define ACL_USER (0x02)
#define ACL_GROUP_OBJ (0x04)
#define ACL_GROUP (0x08)
#define ACL_MASK (0x10)
#define ACL_OTHER (0x20)

#define ACL_TYPE_ACCESS (0x8000)
#define ACL_TYPE_DEFAULT (0x4000)

#define ACL_FIRST_ENTRY 0
#define ACL_NEXT_ENTRY 1

extern int acl_add_perm(acl_permset_t permset_d, acl_perm_t perm);
extern int acl_calc_mask(acl_t* acl_p);
extern int acl_clear_perms(acl_permset_t permset_d);
extern int acl_create_entry(acl_t* acl_p, acl_entry_t* entry_p);
extern int acl_free(void* obj_p);
extern acl_t acl_from_text(const char* buf_p);
extern int acl_get_entry(acl_t acl, int entry_id, acl_entry_t* entry_p);
extern acl_t acl_get_fd(int fd);
extern acl_t acl_get_file(const char* path_p, acl_type_t type);
extern int acl_get_permset(acl_entry_t entry_d, acl_permset_t* permset_p);
extern void* acl_get_qualifier(acl_entry_t entry_d);
extern int acl_get_tag_type(acl_entry_t entry_d, acl_tag_t* tag_type_p);
extern int acl_set_fd(int fd, acl_t acl);
extern int acl_set_qualifier(acl_entry_t entry_d, const void* tag_qualifier_p);
extern int acl_set_tag_type(acl_entry_t entry_d, acl_tag_t tag_type);
extern char* acl_to_text(acl_t acl, ssize_t* len_p);
extern int acl_valid(acl_t acl);

#ifdef __cplusplus
}
#endif

#endif /* THIRD_PARTY_ACL_SYSTEM_INCLUDE_SYS_ACL_H */
