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

use crate::parameters::{SchedulerPolicy, ThreadParameters};
use core::cell::OnceCell;
use core::mem::MaybeUninit;
use core::ptr::null_mut;
use core::sync::atomic::{fence, Ordering};
use std::panic::{catch_unwind, AssertUnwindSafe};
use std::sync::Arc;

/// `pthread` attributes object.
struct Attributes {
    attr_handle: pal::pthread_attr_t,
}

impl Attributes {
    /// Create `pthread` attributes object.
    fn new() -> Self {
        let mut attr = MaybeUninit::uninit();
        // SAFETY: initializes `attr`, pointer is ensured to be valid.
        let rc = unsafe { pal::pthread_attr_init(attr.as_mut_ptr()) };
        assert!(rc == 0, "pthread_attr_init failed, rc: {rc}");

        // SAFETY: `attr` is initialized by a `pthread_attr_init` call.
        let attr_handle = unsafe { attr.assume_init() };
        Self { attr_handle }
    }

    /// Pointer to mutable internal handle.
    fn ptr(&mut self) -> *mut pal::pthread_attr_t {
        // SAFETY:
        // Handle is initialized during object construction.
        // Pointer is ensured to be valid.
        &mut self.attr_handle
    }

    /// Set inherit scheduling attributes.
    fn inherit_scheduling_attributes(&mut self, inherit: bool) {
        let inherit_native = if inherit {
            pal::PTHREAD_INHERIT_SCHED
        } else {
            pal::PTHREAD_EXPLICIT_SCHED
        };

        // SAFETY: value is ensured to be valid.
        let rc = unsafe { pal::pthread_attr_setinheritsched(self.ptr(), inherit_native) };
        assert!(rc == 0, "pthread_attr_setinheritsched failed, rc: {rc}");
    }

    /// Set thread priority.
    fn priority(&mut self, priority: i32) {
        let mut params = MaybeUninit::uninit();
        // Create `sched_param` struct.
        // SAFETY: initializes `params`, pointer is ensured to be valid.
        let rc = unsafe { pal::pthread_attr_getschedparam(self.ptr(), params.as_mut_ptr()) };
        assert!(rc == 0, "pthread_attr_getschedparam failed, rc: {rc}");

        // Store and modify `sched_param` struct.
        // SAFETY: `params` is initialized by a `pthread_attr_getschedparam` call.
        let mut params = unsafe { params.assume_init() };
        params.sched_priority = priority;

        // Set modified `sched_param`.
        // SAFETY:
        // `params` is initialized by a `pthread_attr_getschedparam` call.
        // Pointer is ensured to be valid.
        let rc = unsafe { pal::pthread_attr_setschedparam(self.ptr(), &params) };
        assert!(rc == 0, "pthread_attr_setschedparam failed, rc: {rc}");
    }

    /// Set scheduler policy.
    fn scheduler_policy(&mut self, scheduler_policy: SchedulerPolicy) {
        let policy = scheduler_policy as i32;
        // SAFETY: `policy` value is ensured to be valid.
        let rc = unsafe { pal::pthread_attr_setschedpolicy(self.ptr(), policy) };
        assert!(rc == 0, "pthread_attr_setschedpolicy failed, rc: {rc}");
    }

    /// Set stack size.
    fn stack_size(&mut self, stack_size: usize) {
        // SAFETY: `stack_size` type is valid, invalid value will cause abort.
        let rc = unsafe { pal::pthread_attr_setstacksize(self.ptr(), stack_size) };
        assert!(rc == 0, "pthread_attr_setstacksize failed, rc: {rc}");
    }

    /// Get reference to inner handle.
    fn get(&self) -> &pal::pthread_attr_t {
        &self.attr_handle
    }
}

impl Drop for Attributes {
    fn drop(&mut self) {
        // SAFETY: after drop handle is no longer needed and can be destructed.
        let rc = unsafe { pal::pthread_attr_destroy(self.ptr()) };
        debug_assert!(rc == 0, "pthread_attr_destroy failed, rc: {rc}");
    }
}

struct ThreadData<F: FnOnce()> {
    f: F,
}

/// `pthread` thread object.
struct Thread {
    thread_handle: pal::pthread_t,
}

impl Thread {
    fn new<F>(attributes: Attributes, f: F) -> Self
    where
        F: FnOnce() + Send + 'static,
    {
        let mut thread_handle = MaybeUninit::uninit();

        // SAFETY:
        // It is safe to use `extern "C"`.
        // Provided callback is a wrapper containing panic handling.
        extern "C" fn start_routine<F: FnOnce()>(data: *mut pal::c_void) -> *mut pal::c_void {
            // SAFETY:
            // `data` is ensured to be valid - it is boxed right before `pthread_create`.
            // On `pthread_create` failure it is deallocated during error handling.
            let data: Box<ThreadData<F>> = unsafe { Box::from_raw(data.cast()) };
            (data.f)();
            null_mut()
        }

        let data = Box::into_raw(Box::new(ThreadData { f }));
        // SAFETY:
        // Initializes `thread_handle`.
        // Validity of all pointers is ensured.
        let rc = unsafe {
            pal::pthread_create(
                thread_handle.as_mut_ptr(),
                attributes.get(),
                start_routine::<F>,
                data.cast(),
            )
        };
        if rc != 0 {
            // Reobtain and drop `ThreadData`.
            // SAFETY:
            // `data` is ensured to be valid - it is boxed right before `pthread_create`.
            // On `pthread_create` success it is deallocated after `start_routine` finishes.
            let _ = unsafe { Box::from_raw(data) };
            panic!("pthread_create failed, rc: {rc}");
        }

        Self {
            // SAFETY: `thread_handle` is initialized by a `pthread_create` call.
            thread_handle: unsafe { thread_handle.assume_init() },
        }
    }
}

/// A specialized [`Result`] type for threads.
/// Indicates the manner in which a thread exited.
pub type Result<T> = core::result::Result<T, Box<dyn core::any::Any + Send + 'static>>;

/// Packet containing thread result.
///
/// No need for a mutex because:
/// - state is set during thread.
/// - the caller will never read this packet until the thread has exited.
struct Packet<T>(OnceCell<Result<T>>);

impl<T> Packet<T> {
    fn new() -> Self {
        Self(OnceCell::new())
    }
}

// SAFETY:
// Due to the usage of `OnceCell` manual implementation of `Sync` is required.
// The caller will never read this packet until the thread has exited.
// This is based on `std::thread` implementation.
unsafe impl<T: Send> Sync for Packet<T> {}

impl<T> Drop for Packet<T> {
    fn drop(&mut self) {
        // Make sure that panic was handled.
        if let Some(result) = self.0.get() {
            if result.is_err() {
                panic!("unhandled panic occurred in a thread")
            }
        }
    }
}

/// Inner representation for [`JoinHandle`].
pub struct JoinInner<T> {
    thread: Thread,
    packet: Arc<Packet<T>>,
}

impl<T> JoinInner<T> {
    fn new(thread: Thread, packet: Arc<Packet<T>>) -> Self {
        Self { thread, packet }
    }

    /// Wait for the associated thread to finish.
    ///
    /// This function will return immediately if the associated thread has already finished.
    pub fn join(mut self) -> Result<T> {
        // Perform native join.
        self.join_internal();

        // Obtain `packet` from `Arc`.
        // This can only be done once thread finished.
        fence(Ordering::Acquire);
        let packet = Arc::get_mut(&mut self.packet).expect("thread not yet finished");
        let result = packet.0.take().expect("thread result uninitialized");

        // Prevent destructor from running - thread is already joined.
        core::mem::forget(self);

        result
    }

    fn join_internal(&self) {
        // Perform join.
        let thread_handle = self.thread.thread_handle;
        // SAFETY: `thread_handle` is ensured to be valid.
        let rc = unsafe { pal::pthread_join(thread_handle, null_mut()) };
        assert!(rc == 0, "pthread_join failed, rc: {rc}");
    }
}

impl<T> Drop for JoinInner<T> {
    fn drop(&mut self) {
        self.join_internal();
    }
}

/// An owned permission to join on a thread (block on its termination).
///
/// Thread is joined on [`Self::join`] and [`drop`] (if still joinable).
pub struct JoinHandle<T>(JoinInner<T>);

impl<T> JoinHandle<T> {
    /// Wait for the associated thread to finish.
    ///
    /// This function will return immediately if the associated thread has already finished.
    pub fn join(self) -> Result<T> {
        self.0.join()
    }
}

/// Spawn a new thread, returning [`JoinHandle`] for it.
pub fn spawn<F, T>(f: F, thread_parameters: ThreadParameters) -> JoinHandle<T>
where
    F: FnOnce() -> T,
    F: Send + 'static,
    T: Send + 'static,
{
    // Construct attributes based on provided parameters.
    let mut attributes = Attributes::new();

    if let Some(scheduler_parameters) = thread_parameters.scheduler_parameters {
        attributes.inherit_scheduling_attributes(false);
        attributes.scheduler_policy(scheduler_parameters.policy());
        attributes.priority(scheduler_parameters.priority());
    }
    if let Some(stack_size) = thread_parameters.stack_size {
        attributes.stack_size(stack_size);
    }

    // Construct a wrapper containing affinity configuration and panic handling.
    let packet = Arc::new(Packet::new());
    let packet_clone = packet.clone();
    let thread_wrapper = move || {
        let result = catch_unwind(AssertUnwindSafe(|| {
            // Set affinity.
            if let Some(cpu_set) = thread_parameters.affinity {
                pal::set_affinity(cpu_set);
            }

            // Execute function.
            f()
        }));

        // Set thread result.
        if packet_clone.0.set(result).is_err() {
            panic!("thread result is already set");
        }
    };

    // Create a `Thread` and place it in a `JoinHandle`.
    let thread = Thread::new(attributes, thread_wrapper);
    JoinHandle(JoinInner::new(thread, packet))
}

#[cfg(all(test, not(miri)))]
mod tests {
    use crate::parameters::{SchedulerParameters, SchedulerPolicy, ThreadParameters};
    use crate::thread::{spawn, Attributes};
    use core::mem::MaybeUninit;
    use pal::get_affinity;
    use std::sync::mpsc::channel;

    fn attr_inherit_scheduling_attributes(attrs: &Attributes) -> bool {
        let mut native = MaybeUninit::uninit();
        let rc = unsafe { pal::pthread_attr_getinheritsched(attrs.get(), native.as_mut_ptr()) };
        assert!(rc == 0, "pthread_attr_getinheritsched failed, rc: {rc}");

        match unsafe { native.assume_init() } {
            pal::PTHREAD_INHERIT_SCHED => true,
            pal::PTHREAD_EXPLICIT_SCHED => false,
            _ => panic!("unknown inherit scheduling attributes value"),
        }
    }

    fn attr_priority(attrs: &Attributes) -> i32 {
        let mut param_native = MaybeUninit::uninit();
        let rc = unsafe { pal::pthread_attr_getschedparam(attrs.get(), param_native.as_mut_ptr()) };
        assert!(rc == 0, "pthread_attr_getschedparam failed, rc: {rc}");

        unsafe { param_native.assume_init().sched_priority }
    }

    fn attr_policy(attrs: &Attributes) -> SchedulerPolicy {
        let mut policy_native = MaybeUninit::uninit();
        let rc = unsafe { pal::pthread_attr_getschedpolicy(attrs.get(), policy_native.as_mut_ptr()) };
        assert!(rc == 0, "pthread_attr_getschedpolicy failed, rc: {rc}");

        SchedulerPolicy::try_from(unsafe { policy_native.assume_init() }).unwrap()
    }

    fn attr_stack_size(attrs: &Attributes) -> usize {
        let mut stack_size = MaybeUninit::uninit();
        let rc = unsafe { pal::pthread_attr_getstacksize(attrs.get(), stack_size.as_mut_ptr()) };
        assert!(rc == 0, "pthread_attr_getstacksize failed, rc: {rc}");

        unsafe { stack_size.assume_init() }
    }

    #[test]
    fn attributes_new_succeeds() {
        // Also checks `Drop` on exit.
        let _ = Attributes::new();
    }

    #[test]
    fn attributes_inherit_scheduling_attributes_succeeds() {
        let mut attrs = Attributes::new();

        attrs.inherit_scheduling_attributes(true);
        assert!(attr_inherit_scheduling_attributes(&attrs));

        attrs.inherit_scheduling_attributes(false);
        assert!(!attr_inherit_scheduling_attributes(&attrs));
    }

    #[test]
    fn attributes_priority_succeeds() {
        let mut attrs = Attributes::new();

        attrs.scheduler_policy(SchedulerPolicy::Fifo);
        attrs.priority(50);
        assert_eq!(attr_priority(&attrs), 50);
    }

    #[test]
    #[should_panic(expected = "pthread_attr_setschedparam failed, rc:")]
    fn attributes_priority_wrong_scheduler_policy() {
        let mut attrs = Attributes::new();
        attrs.priority(50);
    }

    #[test]
    fn attributes_scheduler_policy_succeeds() {
        let mut attrs = Attributes::new();

        attrs.scheduler_policy(SchedulerPolicy::Fifo);
        assert_eq!(attr_policy(&attrs), SchedulerPolicy::Fifo);
    }

    #[test]
    fn attributes_stack_size_succeeds() {
        let mut attrs = Attributes::new();

        let expected_stack_size = 1024 * 1024;
        attrs.stack_size(expected_stack_size);
        assert_eq!(attr_stack_size(&attrs), expected_stack_size);
    }

    #[test]
    #[should_panic(expected = "pthread_attr_setstacksize failed, rc:")]
    fn attributes_stack_size_too_small() {
        let mut attrs = Attributes::new();
        attrs.stack_size(4 * 1024);
    }

    #[test]
    fn spawn_joined_succeeds() {
        let thread_parameters = ThreadParameters::default();
        let (tx, rx) = channel();
        let join_handle = spawn(
            move || {
                tx.send(654321).unwrap();
                123
            },
            thread_parameters,
        );
        let result = join_handle.join();

        assert!(result.is_ok_and(|x| x == 123));
        assert_eq!(rx.recv().unwrap(), 654321)
    }

    #[test]
    fn spawn_not_joined_succeeds() {
        let thread_parameters = ThreadParameters::default();
        let (tx, rx) = channel();
        let _ = spawn(
            move || {
                tx.send(654321).unwrap();
            },
            thread_parameters,
        );

        assert_eq!(rx.recv().unwrap(), 654321)
    }

    #[test]
    fn spawn_joined_panics() {
        let thread_parameters = ThreadParameters::default();
        let join_handle = spawn(
            move || {
                panic!("internal panic");
            },
            thread_parameters,
        );
        let result = join_handle.join();
        assert!(result.is_err());
    }

    #[test]
    #[should_panic(expected = "unhandled panic occurred in a thread")]
    fn spawn_not_joined_panics() {
        let thread_parameters = ThreadParameters::default();
        let _ = spawn(
            move || {
                panic!("internal panic");
            },
            thread_parameters,
        );
    }

    fn current_sched_params() -> (SchedulerPolicy, i32) {
        let thread = unsafe { pal::pthread_self() };
        let mut policy = MaybeUninit::uninit();
        let mut param = MaybeUninit::uninit();
        let rc = unsafe { pal::pthread_getschedparam(thread, policy.as_mut_ptr(), param.as_mut_ptr()) };
        assert!(rc == 0, "pthread_getschedparam failed, rc: {rc}");

        let policy_native = unsafe { policy.assume_init() };
        let scheduler_policy = match policy_native {
            pal::SCHED_OTHER => SchedulerPolicy::Other,
            pal::SCHED_FIFO => SchedulerPolicy::Fifo,
            pal::SCHED_RR => SchedulerPolicy::RoundRobin,
            _ => panic!("unknown scheduler type"),
        };

        let priority = unsafe { param.assume_init().sched_priority };

        (scheduler_policy, priority)
    }

    #[test]
    #[ignore = "test requires cap_sys_nice=ep"]
    fn spawn_scheduler_params_succeeds() {
        let exp_scheduler_parameters = SchedulerParameters::new(SchedulerPolicy::Fifo, 10);
        let thread_parameters = ThreadParameters::new().scheduler_parameters(exp_scheduler_parameters);
        let (tx, rx) = channel();
        let join_handle = spawn(
            move || {
                let sched_params = current_sched_params();
                tx.send(sched_params).unwrap();
            },
            thread_parameters,
        );
        let result = join_handle.join();
        assert!(result.is_ok());

        let (scheduler_policy, priority) = rx.recv().unwrap();
        assert_eq!(scheduler_policy, exp_scheduler_parameters.policy());
        assert_eq!(priority, exp_scheduler_parameters.priority());
    }

    #[test]
    fn spawn_affinity_succeeds() {
        let exp_affinity = vec![0];
        let thread_parameters = ThreadParameters::new().affinity(&exp_affinity);
        let (tx, rx) = channel();
        let join_handle = spawn(
            move || {
                let affinity = get_affinity();
                tx.send(affinity).unwrap();
            },
            thread_parameters,
        );
        let result = join_handle.join();
        assert!(result.is_ok());

        assert_eq!(rx.recv().unwrap().iter().copied().collect::<Vec<_>>(), exp_affinity);
    }

    #[test]
    #[should_panic(expected = "CPU ID provided to affinity exceeds max supported size, provided: 1234, max: 1023")]
    fn spawn_affinity_out_of_range() {
        let thread_parameters = ThreadParameters::new().affinity(&[1234]);
        let _ = spawn(|| {}, thread_parameters);
    }

    #[test]
    fn spawn_stack_size_succeeds() {
        // Check that nothing fails - cannot check stack size from within.
        let stack_size = 1024 * 1024;
        let thread_parameters = ThreadParameters::new().stack_size(stack_size);
        let join_handle = spawn(
            || {
                // Do nothing.
            },
            thread_parameters,
        );
        let result = join_handle.join();
        assert!(result.is_ok());
    }
}
