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
#ifndef THIRD_PARTY_ACL_SYSTEM_INCLUDE_ACL_LIBACL_H
#define THIRD_PARTY_ACL_SYSTEM_INCLUDE_ACL_LIBACL_H

/* Declaration of the Linux-specific libacl function used by score/os, matching the libacl.so.1 ABI. */

#include <sys/acl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int acl_get_perm(acl_permset_t permset_d, acl_perm_t perm);

#ifdef __cplusplus
}
#endif

#endif /* THIRD_PARTY_ACL_SYSTEM_INCLUDE_ACL_LIBACL_H */
