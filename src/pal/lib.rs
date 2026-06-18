// *******************************************************************************
// Copyright (c) 2026 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// <https://www.apache.org/licenses/LICENSE-2.0>
//
// SPDX-License-Identifier: Apache-2.0
// *******************************************************************************

//! Minimal POSIX adaptation layer.

mod affinity;
mod errno;

pub use affinity::{get_affinity, set_affinity, CpuSet};
pub use errno::errno;

pub use libc::{
    c_int, c_ulong, c_void, pid_t, pthread_attr_destroy, pthread_attr_getstacksize, pthread_attr_init,
    pthread_attr_setstacksize, pthread_attr_t, pthread_create, pthread_getschedparam, pthread_join, pthread_self,
    pthread_setschedparam, pthread_t, sched_get_priority_max, sched_get_priority_min, sched_param, SCHED_FIFO,
    SCHED_OTHER, SCHED_RR,
};

#[cfg(target_os = "linux")]
pub use libc::{
    pthread_attr_getinheritsched, pthread_attr_getschedparam, pthread_attr_getschedpolicy,
    pthread_attr_setinheritsched, pthread_attr_setschedparam, pthread_attr_setschedpolicy, PTHREAD_EXPLICIT_SCHED,
    PTHREAD_INHERIT_SCHED,
};

#[cfg(target_os = "nto")]
pub const PTHREAD_INHERIT_SCHED: c_int = 0;
#[cfg(target_os = "nto")]
pub const PTHREAD_EXPLICIT_SCHED: c_int = 1;

#[cfg(target_os = "nto")]
extern "C" {
    pub fn pthread_attr_getinheritsched(attr: *const pthread_attr_t, inheritsched: *mut c_int) -> c_int;
    pub fn pthread_attr_setinheritsched(attr: *mut pthread_attr_t, inheritsched: c_int) -> c_int;
    pub fn pthread_attr_getschedparam(attr: *const pthread_attr_t, param: *mut sched_param) -> c_int;
    pub fn pthread_attr_setschedparam(attr: *mut pthread_attr_t, param: *const sched_param) -> c_int;
    pub fn pthread_attr_getschedpolicy(attr: *const pthread_attr_t, policy: *mut c_int) -> c_int;
    pub fn pthread_attr_setschedpolicy(attr: *mut pthread_attr_t, policy: c_int) -> c_int;
}
