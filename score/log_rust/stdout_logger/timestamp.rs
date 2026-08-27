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

//! Generate human-readable timestamp from Unix epoch time.
//! Without allocations and external dependencies.
//!
//! WARNING: range checks are provided only for debug builds!

use core::time::Duration;

/// Determine if provided year is a leap year.
fn is_leap_year(year: u64) -> bool {
    (year.is_multiple_of(4) && !year.is_multiple_of(100)) || year.is_multiple_of(400)
}

/// Calculate time - hour, minute, second.
/// `secs_in_day` must be less than number of seconds in a day.
fn get_time(secs_in_day: u64) -> (u64, u64, u64) {
    debug_assert!(secs_in_day < 24 * 60 * 60);

    const SECS_PER_HOUR: u64 = 60 * 60;
    let hour = secs_in_day / SECS_PER_HOUR;
    let secs_in_day = secs_in_day % SECS_PER_HOUR;
    let minute = secs_in_day / 60;
    let second = secs_in_day % 60;

    // Validate output.
    debug_assert!(hour < 24);
    debug_assert!(minute < 60);
    debug_assert!(second < 60);

    (hour, minute, second)
}

/// Calculate date - year, month, day.
/// `days_in_epoch` must be less than number of days between:
/// - 1970/01/01
/// - 10000/01/01
fn get_date(days_in_epoch: u64) -> (u64, u64, u64) {
    debug_assert!(days_in_epoch <= 2932896);

    // Local and mutable copy of `days_in_epoch`.
    let mut days = days_in_epoch;

    // Calculate year.
    let mut year = 1970;
    loop {
        let days_in_year = if is_leap_year(year) { 366 } else { 365 };
        if days < days_in_year {
            break;
        }
        days -= days_in_year;
        year += 1;
    }

    // Calculate month.
    let days_in_feb = if is_leap_year(year) { 29 } else { 28 };
    let days_in_month = [31, days_in_feb, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
    let mut month_index = 0;
    while days >= days_in_month[month_index] {
        days -= days_in_month[month_index];
        month_index += 1;
    }
    let month = month_index as u64 + 1;

    // Calculate day.
    let day = days + 1;

    // Validate output.
    debug_assert!((1970..=9999).contains(&year));
    debug_assert!((1..=12).contains(&month));
    debug_assert!(day >= 1 && day <= days_in_month[month_index]);

    (year, month, day)
}

/// Write integer value to `u8` slice as ASCII characters.
fn write<const N: usize>(buf: &mut [u8], value: u64) {
    for (i, v) in buf.iter_mut().enumerate() {
        let digit = 10u64.pow((N - i - 1) as u32);
        *v = b'0' + (value / digit % 10) as u8;
    }
}

/// Get timestamp in following format:
/// `[year]/[month]/[day] [hour]:[minute]:[second].[subsecond digits:7]`
pub fn timestamp(duration_since_epoch_start: Duration) -> [u8; 27] {
    debug_assert!(duration_since_epoch_start.as_secs() <= 253402300799);

    let secs = duration_since_epoch_start.as_secs();
    let subsec_nanos = duration_since_epoch_start.subsec_nanos() as u64;

    const SECS_PER_DAY: u64 = 24 * 60 * 60;

    // Calculate number of days since epoch and seconds in a day (remainder).
    let days_in_epoch = secs / SECS_PER_DAY;
    let secs_in_day = secs % SECS_PER_DAY;

    // Calculate time.
    let (hour, minute, second) = get_time(secs_in_day);

    // Calculate date.
    let (year, month, day) = get_date(days_in_epoch);

    // Format output as `u8` array of known size.
    let mut output = [0; _];

    // Write date in `YYYY/MM/dd ` format.
    write::<4>(&mut output[0..4], year);
    output[4] = b'/';
    write::<2>(&mut output[5..7], month);
    output[7] = b'/';
    write::<2>(&mut output[8..10], day);
    output[10] = b' ';

    // Write time in `HH:mm:ss.nnnnnnn` format.
    write::<2>(&mut output[11..13], hour);
    output[13] = b':';
    write::<2>(&mut output[14..16], minute);
    output[16] = b':';
    write::<2>(&mut output[17..19], second);
    output[19] = b'.';
    write::<7>(&mut output[20..27], subsec_nanos / 100);

    output
}

#[cfg(test)]
mod tests {
    use super::{get_date, get_time, timestamp};
    use core::time::Duration;

    #[test]
    fn test_get_time_zero() {
        let (hour, minute, second) = get_time(0);
        assert_eq!(hour, 0);
        assert_eq!(minute, 0);
        assert_eq!(second, 0);
    }

    #[test]
    fn test_get_time_in_range() {
        let (hour, minute, second) = get_time(45296);
        assert_eq!(hour, 12);
        assert_eq!(minute, 34);
        assert_eq!(second, 56);
    }

    #[test]
    fn test_get_time_max_allowed() {
        let (hour, minute, second) = get_time(24 * 60 * 60 - 1);
        assert_eq!(hour, 23);
        assert_eq!(minute, 59);
        assert_eq!(second, 59);
    }

    #[test]
    #[should_panic]
    fn test_get_time_out_of_range() {
        let (_hour, _minute, _second) = get_time(24 * 60 * 60);
    }

    #[test]
    fn test_get_date_zero() {
        let (year, month, day) = get_date(0);
        assert_eq!(year, 1970);
        assert_eq!(month, 1);
        assert_eq!(day, 1);
    }

    #[test]
    fn test_get_date_in_range() {
        let (year, month, day) = get_date(20481);
        assert_eq!(year, 2026);
        assert_eq!(month, 1);
        assert_eq!(day, 28);
    }

    #[test]
    fn test_get_date_max_allowed() {
        let (year, month, day) = get_date(2932896);
        assert_eq!(year, 9999);
        assert_eq!(month, 12);
        assert_eq!(day, 31);
    }

    #[test]
    #[should_panic]
    fn test_get_date_out_of_range() {
        let (_hour, _minute, _second) = get_date(u64::MAX);
    }

    #[test]
    fn test_timestamp_zero() {
        let duration = Duration::from_secs(0);
        let ts_u8 = timestamp(duration);
        let ts_str = str::from_utf8(ts_u8.as_slice()).unwrap();
        assert_eq!(ts_str, "1970/01/01 00:00:00.0000000");
    }

    #[test]
    fn test_timestamp_in_range() {
        let duration = Duration::from_secs(1769604017) + Duration::from_nanos(123456789);
        let ts_u8 = timestamp(duration);
        let ts_str = str::from_utf8(ts_u8.as_slice()).unwrap();
        assert_eq!(ts_str, "2026/01/28 12:40:17.1234567");
    }

    #[test]
    fn test_timestamp_max_allowed() {
        let duration = Duration::from_secs(253402300799) + Duration::from_nanos(999999999);
        let ts_u8 = timestamp(duration);
        let ts_str = str::from_utf8(ts_u8.as_slice()).unwrap();
        assert_eq!(ts_str, "9999/12/31 23:59:59.9999999");
    }

    #[test]
    #[should_panic]
    fn test_timestamp_out_of_range() {
        let duration = Duration::from_secs(253402300800);
        let _ = timestamp(duration);
    }
}
