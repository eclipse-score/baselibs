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

use pal::CpuSet;
use score_log::ScoreDebug;

/// Scheduler policy.
#[repr(i32)]
#[derive(Clone, Copy, Debug, ScoreDebug, PartialEq, Eq)]
pub enum SchedulerPolicy {
    Other = pal::SCHED_OTHER,
    Fifo = pal::SCHED_FIFO,
    RoundRobin = pal::SCHED_RR,
}

impl SchedulerPolicy {
    /// Get min thread priority for this policy.
    pub fn priority_min(&self) -> i32 {
        let policy_native = *self as i32;
        // SAFETY:
        // Native policy value is ensured.
        // Operation is non-fallible.
        unsafe { pal::sched_get_priority_min(policy_native) }
    }

    /// Get max thread priority for this policy.
    pub fn priority_max(&self) -> i32 {
        let policy_native = *self as i32;
        // SAFETY:
        // Native policy value is ensured.
        // Operation is non-fallible.
        unsafe { pal::sched_get_priority_max(policy_native) }
    }
}

/// Indicates that provided scheduler policy is unknown or unsupported.
#[derive(Clone, Copy, Default, Debug)]
pub struct UnknownSchedulerPolicy;

impl core::fmt::Display for UnknownSchedulerPolicy {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        write!(f, "unknown or unsupported scheduler policy")
    }
}

impl score_log::fmt::ScoreDebug for UnknownSchedulerPolicy {
    fn fmt(&self, f: score_log::fmt::Writer, _spec: &score_log::fmt::FormatSpec) -> score_log::fmt::Result {
        score_log::fmt::score_write!(f, "unknown or unsupported scheduler policy")
    }
}

impl core::error::Error for UnknownSchedulerPolicy {}

impl TryFrom<i32> for SchedulerPolicy {
    type Error = UnknownSchedulerPolicy;

    fn try_from(value: i32) -> Result<Self, Self::Error> {
        match value {
            pal::SCHED_OTHER => Ok(SchedulerPolicy::Other),
            pal::SCHED_FIFO => Ok(SchedulerPolicy::Fifo),
            pal::SCHED_RR => Ok(SchedulerPolicy::RoundRobin),
            _ => Err(UnknownSchedulerPolicy),
        }
    }
}

/// Scheduler parameters.
#[derive(Clone, Copy, Debug, ScoreDebug, PartialEq, Eq)]
pub struct SchedulerParameters {
    policy: SchedulerPolicy,
    priority: i32,
}

impl SchedulerParameters {
    /// Create a new [`SchedulerParameters`].
    ///
    /// # Panics
    ///
    /// Priority must be in allowed range for the scheduler policy.
    pub fn new(policy: SchedulerPolicy, priority: i32) -> Self {
        let allowed_priority_range = policy.priority_min()..=policy.priority_max();
        if !allowed_priority_range.contains(&priority) {
            panic!("priority is not in allowed range for the scheduler policy")
        }

        Self { policy, priority }
    }

    /// Scheduler policy.
    pub fn policy(&self) -> SchedulerPolicy {
        self.policy
    }

    /// Thread priority.
    pub fn priority(&self) -> i32 {
        self.priority
    }
}

/// Thread parameters.
#[derive(Clone, Default, Debug, ScoreDebug, PartialEq, Eq)]
pub struct ThreadParameters {
    pub(crate) scheduler_parameters: Option<SchedulerParameters>,
    pub(crate) affinity: Option<CpuSet>,
    pub(crate) stack_size: Option<usize>,
}

impl ThreadParameters {
    /// Create a new [`ThreadParameters`] containing default values.
    pub fn new() -> Self {
        Self::default()
    }

    /// Scheduler parameters, including scheduler policy and thread priority.
    pub fn scheduler_parameters(mut self, scheduler_parameters: SchedulerParameters) -> Self {
        self.scheduler_parameters = Some(scheduler_parameters);
        self
    }

    /// Set thread affinity - array of CPU core IDs that the thread can run on.
    pub fn affinity(mut self, affinity: &[usize]) -> Self {
        self.affinity = Some(CpuSet::new(affinity));
        self
    }

    /// Set stack size.
    pub fn stack_size(mut self, stack_size: usize) -> Self {
        self.stack_size = Some(stack_size);
        self
    }
}

#[cfg(all(test, not(miri)))]
mod tests {
    use crate::parameters::{SchedulerParameters, SchedulerPolicy};
    use crate::ThreadParameters;
    use pal::CpuSet;

    #[test]
    fn scheduler_policy_min_max_priority() {
        let policy = SchedulerPolicy::Fifo;
        assert_eq!(policy.priority_min(), 1);
        assert_eq!(policy.priority_max(), 99);
    }

    #[test]
    fn scheduler_policy_from_i32_succeeds() {
        let policy_as_int = SchedulerPolicy::Other as i32;
        let policy = SchedulerPolicy::try_from(policy_as_int);
        assert!(policy.is_ok_and(|p| p == SchedulerPolicy::Other));
    }

    #[test]
    fn scheduler_policy_from_i32_unknown() {
        let result = SchedulerPolicy::try_from(123);
        assert!(result.is_err())
    }

    #[test]
    fn scheduler_parameters_new_succeeds() {
        let policy = SchedulerPolicy::Fifo;
        let priority = 40;
        let params = SchedulerParameters::new(policy, priority);
        assert_eq!(params.policy(), policy);
        assert_eq!(params.priority(), priority);
    }

    #[test]
    #[should_panic(expected = "priority is not in allowed range for the scheduler policy")]
    fn scheduler_parameters_new_priority_out_of_range() {
        let policy = SchedulerPolicy::Other;
        let priority = 1;
        let _ = SchedulerParameters::new(policy, priority);
    }

    #[test]
    fn thread_parameters_new_succeeds() {
        let new_tp = ThreadParameters::new();
        let def_tp = ThreadParameters::default();

        assert_eq!(new_tp.scheduler_parameters, None);
        assert_eq!(new_tp.affinity, None);
        assert_eq!(new_tp.stack_size, None);
        assert_eq!(new_tp, def_tp);
    }

    #[test]
    fn thread_parameters_parameters_succeeds() {
        let exp_scheduler_parameters = SchedulerParameters::new(SchedulerPolicy::Fifo, 50);
        let exp_affinity = vec![1, 2, 3];
        let exp_stack_size = 1024 * 1024;
        let thread_parameters = ThreadParameters::new()
            .scheduler_parameters(exp_scheduler_parameters)
            .affinity(&exp_affinity)
            .stack_size(exp_stack_size);

        assert_eq!(thread_parameters.scheduler_parameters, Some(exp_scheduler_parameters));
        assert_eq!(thread_parameters.affinity, Some(CpuSet::new(&exp_affinity)));
        assert_eq!(thread_parameters.stack_size, Some(exp_stack_size));
    }
}
