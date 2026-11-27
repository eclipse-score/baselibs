# *******************************************************************************
# Copyright (c) 2026 Contributors to the Eclipse Foundation
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

# Single source of truth for which acl-2.4.0 files are vendored, shared between
# acl.BUILD (the `:acl` cc_library) and BUILD's `:config_drift_test` (which checks
# that config.h still covers every autoconf-style macro these files reference).

ACL_SRCS = [
    # POSIX_CFILES (Makemodule.am).
    "libacl/acl_add_perm.c",
    "libacl/acl_calc_mask.c",
    "libacl/acl_clear_perms.c",
    "libacl/acl_copy_entry.c",
    "libacl/acl_copy_ext.c",
    "libacl/acl_copy_int.c",
    "libacl/acl_create_entry.c",
    "libacl/acl_delete_def_file.c",
    "libacl/acl_delete_entry.c",
    "libacl/acl_delete_perm.c",
    "libacl/acl_dup.c",
    "libacl/acl_free.c",
    "libacl/acl_from_text.c",
    "libacl/acl_get_entry.c",
    "libacl/acl_get_fd.c",
    "libacl/acl_get_file.c",
    "libacl/acl_get_file_at.c",
    "libacl/acl_get_perm.c",
    "libacl/acl_get_permset.c",
    "libacl/acl_get_qualifier.c",
    "libacl/acl_get_tag_type.c",
    "libacl/acl_init.c",
    "libacl/acl_set_fd.c",
    "libacl/acl_set_file.c",
    "libacl/acl_set_file_at.c",
    "libacl/acl_set_permset.c",
    "libacl/acl_set_qualifier.c",
    "libacl/acl_set_tag_type.c",
    "libacl/acl_size.c",
    "libacl/acl_to_text.c",
    "libacl/acl_valid.c",
    # LIBACL_CFILES.
    "libacl/acl_check.c",
    "libacl/acl_cmp.c",
    "libacl/acl_entries.c",
    "libacl/acl_equiv_mode.c",
    "libacl/acl_error.c",
    "libacl/acl_extended_fd.c",
    "libacl/acl_extended_file.c",
    "libacl/acl_extended_file_at.c",
    "libacl/acl_extended_file_nofollow.c",
    "libacl/acl_from_mode.c",
    "libacl/acl_to_any_text.c",
    # INTERNAL_CFILES.
    "libacl/__acl_apply_mask_to_mode.c",
    "libacl/__acl_from_xattr.c",
    "libacl/__acl_reorder_obj_p.c",
    "libacl/__acl_to_any_text.c",
    "libacl/__acl_to_xattr.c",
    "libacl/__libobj.c",
    # libmisc compat shims needed by the "_at" files above (this toolchain's glibc
    # doesn't provide getxattrat()/setxattrat(), so config.h leaves HAVE_GETXATTRAT/
    # HAVE_SETXATTRAT undefined and xattrat_compat.h maps them to these instead).
    # xattrat.c provides the raw syscall-based getxattrat()/setxattrat() that the
    # *_compat.c shims try first, falling back to proc-self-fd-based emulation
    # (via ENOSYS) on kernels that predate the getxattrat/setxattrat syscalls.
    "libmisc/xattrat.c",
    "libmisc/getxattrat_compat.c",
    "libmisc/setxattrat_compat.c",
    "libmisc/proc-self-fd.c",
    # Helpers used by acl_from_text.c/acl_to_any_text.c (quoting/unquoting of
    # non-printable user and group names) and acl_get_qualifier.c (uid/gid lookup).
    "libmisc/quote.c",
    "libmisc/unquote.c",
    "libmisc/uid_gid_lookup.c",
    "libmisc/high_water_alloc.c",
]

ACL_HDRS = [
    # HFILES (Makemodule.am): private headers shared between the .c files above.
    "libacl/libobj.h",
    "libacl/libacl.h",
    "libacl/byteorder.h",
    "libacl/__acl_from_xattr.h",
    "libacl/__acl_to_xattr.h",
    # Internal (noinst) headers pulled in by the .c files above.
    "include/acl_ea.h",
    "include/misc.h",
    "include/visibility-hidden.h",
    "include/xattrat.h",
    "include/xattrat_compat.h",
    "libmisc/proc-self-fd.h",
]
