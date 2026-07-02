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

/// The standard logging macro.
///
/// This macro will generically log with the specified `Level` and `format!` based argument list.
///
/// ```
/// use score_log::{log, Level};
///
/// let data = (42, "Forty-two");
/// let private_data = "private";
///
/// log!(Level::Error, "Received errors: {}, {}", data.0, data.1);
/// ```
///
/// Optionally, you can specify a `context` argument to attach a specific context to the log record.
/// By default, the context is provided by the logger.
///
/// ```
/// use score_log::{log, Level};
///
/// let data = (42, "Forty-two");
/// let private_data = "private";
///
/// log!(
///     context: "app_events",
///     Level::Error,
///     "Received errors: {}, {}",
///     data.0, data.1
/// );
/// ```
///
/// And optionally, you can specify a `logger` argument to use a specific logger instead of the default global logger.
///
/// ```
/// # struct MyLogger {}
/// # impl Log for MyLogger {
/// #     fn enabled(&self, _metadata: &score_log::Metadata) -> bool {
/// #         false
/// #     }
/// #     fn context(&self) -> &str {
/// #         "MyLogger"
/// #     }
/// #     fn log(&self, _record: &score_log::Record) {}
/// #     fn flush(&self) {}
/// # }
/// use score_log::{log, Level, Log};
///
/// let data = (42, "Forty-two");
/// let private_data = "private";
///
/// let my_logger = MyLogger {};
/// log!(
///     logger: my_logger,
///     Level::Error,
///     "Received errors: {}, {}",
///     data.0, data.1
/// );
/// ```
///
/// The `logger` argument accepts a value that implements the `Log` trait.
/// The value will be borrowed within the macro.
///
/// Note that the global level set via Cargo features, or through `set_max_level` will still apply, even when a custom logger is supplied with the `logger` argument.
#[macro_export]
#[clippy::format_args]
macro_rules! log {
    // log!(logger: my_logger, context: "my_context", Level::Info, "a {} event", "log");
    (logger: $logger:expr, context: $context:expr, $level:expr, $($arg:tt)+) => ({
        $crate::__log!(
            logger: $crate::__log_logger!($logger),
            context: $context,
            $level,
            $($arg)+
        )
    });

    // log!(logger: my_logger, Level::Info, "a log event")
    (logger: $logger:expr, $level:expr, $($arg:tt)+) => ({
        let logger = $crate::__log_logger!($logger);
        $crate::__log!(
            logger: logger,
            context: logger.context(),
            $level,
            $($arg)+
        )
    });

    // log!(context: "my_context", Level::Info, "a log event")
    (context: $context:expr, $level:expr, $($arg:tt)+) => ({
        $crate::__log!(
            logger: $crate::__log_logger!(__log_global_logger),
            context: $context,
            $level,
            $($arg)+
        )
    });

    // log!(Level::Info, "a log event")
    ($level:expr, $($arg:tt)+) => ({
        let logger = $crate::__log_logger!(__log_global_logger);
        $crate::__log!(
            logger: logger,
            context: logger.context(),
            $level,
            $($arg)+
        )
    });
}

#[doc(hidden)]
#[macro_export]
macro_rules! __log {
    // log!(logger: my_logger, context: "my_context", Level::Info, "a {} event", "log");
    (logger: $logger:expr, context: $context:expr, $level:expr, $($arg:tt)+) => ({
        let loc = core::panic::Location::caller();
        $logger.log(
            &$crate::Record::new(
                $crate::format_args!($($arg)+),
                $crate::Metadata::new($level, $context),
                core::module_path!(),
                loc.file(),
                loc.line()
            )
        );
    });
}

/// Logs a message at the fatal level.
///
/// # Examples
///
/// ```
/// use score_log::fatal;
///
/// # let my_logger = score_log::global_logger();
/// let (err_info, port) = ("No connection", 22);
///
/// fatal!("Fatal: {} on port {}", err_info, port);
/// fatal!(context: "app_events", "App Error: {}, Port: {}", err_info, port);
/// fatal!(logger: my_logger, "App Error: {}, Port: {}", err_info, port);
/// ```
#[macro_export]
#[clippy::format_args]
macro_rules! fatal {
    // fatal!(logger: my_logger, context: "my_context", "a {} event", "log")
    (logger: $logger:expr, context: $context:expr, $($arg:tt)+) => ({
        $crate::log!(logger: $logger, context: $context, $crate::Level::Fatal, $($arg)+)
    });

    // fatal!(logger: my_logger, "a {} event", "log")
    (logger: $logger:expr, $($arg:tt)+) => ({
        $crate::log!(logger: $logger, $crate::Level::Fatal, $($arg)+)
    });

    // fatal!(context: "my_context", "a {} event", "log")
    (context: $context:expr, $($arg:tt)+) => ({
        $crate::log!(context: $context, $crate::Level::Fatal, $($arg)+)
    });

    // fatal!("a {} event", "log")
    ($($arg:tt)+) => ($crate::log!($crate::Level::Fatal, $($arg)+))
}

/// Logs a message at the error level.
///
/// # Examples
///
/// ```
/// use score_log::error;
///
/// # let my_logger = score_log::global_logger();
/// let (err_info, port) = ("No connection", 22);
///
/// error!("Error: {} on port {}", err_info, port);
/// error!(context: "app_events", "App Error: {}, Port: {}", err_info, port);
/// error!(logger: my_logger, "App Error: {}, Port: {}", err_info, port);
/// ```
#[macro_export]
#[clippy::format_args]
macro_rules! error {
    // error!(logger: my_logger, context: "my_context", "a {} event", "log")
    (logger: $logger:expr, context: $context:expr, $($arg:tt)+) => ({
        $crate::log!(logger: $logger, context: $context, $crate::Level::Error, $($arg)+)
    });

    // error!(logger: my_logger, "a {} event", "log")
    (logger: $logger:expr, $($arg:tt)+) => ({
        $crate::log!(logger: $logger, $crate::Level::Error, $($arg)+)
    });

    // error!(context: "my_context", "a {} event", "log")
    (context: $context:expr, $($arg:tt)+) => ({
        $crate::log!(context: $context, $crate::Level::Error, $($arg)+)
    });

    // error!("a {} event", "log")
    ($($arg:tt)+) => ($crate::log!($crate::Level::Error, $($arg)+))
}

/// Logs a message at the warn level.
///
/// # Examples
///
/// ```
/// use score_log::warn;
///
/// # let my_logger = score_log::global_logger();
/// let warn_description = "Invalid Input";
///
/// warn!("Warning! {}!", warn_description);
/// warn!(context: "input_events", "App received warning: {}", warn_description);
/// warn!(logger: my_logger, "App received warning: {}", warn_description);
/// ```
#[macro_export]
#[clippy::format_args]
macro_rules! warn {
    // warn!(logger: my_logger, context: "my_context", "a {} event", "log")
    (logger: $logger:expr, context: $context:expr, $($arg:tt)+) => ({
        $crate::log!(logger: $logger, context: $context, $crate::Level::Warn, $($arg)+)
    });

    // warn!(logger: my_logger, "a {} event", "log")
    (logger: $logger:expr, $($arg:tt)+) => ({
        $crate::log!(logger: $logger, $crate::Level::Warn, $($arg)+)
    });

    // warn!(context: "my_context", "a {} event", "log")
    (context: $context:expr, $($arg:tt)+) => ({
        $crate::log!(context: $context, $crate::Level::Warn, $($arg)+)
    });

    // warn!("a {} event", "log")
    ($($arg:tt)+) => ($crate::log!($crate::Level::Warn, $($arg)+))
}

/// Logs a message at the info level.
///
/// # Examples
///
/// ```
/// use score_log::info;
///
/// # let my_logger = score_log::global_logger();
/// # struct Connection { port: u32, speed: f32 }
/// let conn_info = Connection { port: 40, speed: 3.20 };
///
/// info!("Connected to port {} at {} Mb/s", conn_info.port, conn_info.speed);
/// info!(
///     context: "connection_events",
///     "Successful connection, port: {}, speed: {}",
///     conn_info.port, conn_info.speed
/// );
/// info!(
///     logger: my_logger,
///     "Successful connection, port: {}, speed: {}",
///     conn_info.port, conn_info.speed
/// );
/// ```
#[macro_export]
#[clippy::format_args]
macro_rules! info {
    // info!(logger: my_logger, context: "my_context", "a {} event", "log")
    (logger: $logger:expr, context: $context:expr, $($arg:tt)+) => ({
        $crate::log!(logger: $logger, context: $context, $crate::Level::Info, $($arg)+)
    });

    // info!(logger: my_logger, "a {} event", "log")
    (logger: $logger:expr, $($arg:tt)+) => ({
        $crate::log!(logger: $logger, $crate::Level::Info, $($arg)+)
    });

    // info!(context: "my_context", "a {} event", "log")
    (context: $context:expr, $($arg:tt)+) => ({
        $crate::log!(context: $context, $crate::Level::Info, $($arg)+)
    });

    // info!("a {} event", "log")
    ($($arg:tt)+) => ($crate::log!($crate::Level::Info, $($arg)+))
}

/// Logs a message at the debug level.
///
/// # Examples
///
/// ```
/// use score_log::debug;
///
/// # let my_logger = score_log::global_logger();
/// # struct Position { x: f32, y: f32 }
/// let pos = Position { x: 3.234, y: -1.223 };
///
/// debug!("New position: x: {}, y: {}", pos.x, pos.y);
/// debug!(context: "app_events", "New position: x: {}, y: {}", pos.x, pos.y);
/// debug!(logger: my_logger, "New position: x: {}, y: {}", pos.x, pos.y);
/// ```
#[macro_export]
#[clippy::format_args]
macro_rules! debug {
    // debug!(logger: my_logger, context: "my_context", "a {} event", "log")
    (logger: $logger:expr, context: $context:expr, $($arg:tt)+) => ({
        $crate::log!(logger: $logger, context: $context, $crate::Level::Debug, $($arg)+)
    });

    // debug!(logger: my_logger, "a {} event", "log")
    (logger: $logger:expr, $($arg:tt)+) => ({
        $crate::log!(logger: $logger, $crate::Level::Debug, $($arg)+)
    });

    // debug!(context: "my_context", "a {} event", "log")
    (context: $context:expr, $($arg:tt)+) => ({
        $crate::log!(context: $context, $crate::Level::Debug, $($arg)+)
    });

    // debug!("a {} event", "log")
    ($($arg:tt)+) => ($crate::log!($crate::Level::Debug, $($arg)+))
}

/// Logs a message at the trace level.
///
/// # Examples
///
/// ```
/// use score_log::trace;
///
/// # let my_logger = score_log::global_logger();
/// # struct Position { x: f32, y: f32 }
/// let pos = Position { x: 3.234, y: -1.223 };
///
/// trace!("Position is: x: {}, y: {}", pos.x, pos.y);
/// trace!(context: "app_events", "x is {} and y is {}",
///        if pos.x >= 0.0 { "positive" } else { "negative" },
///        if pos.y >= 0.0 { "positive" } else { "negative" });
/// trace!(logger: my_logger, "x is {} and y is {}",
///        if pos.x >= 0.0 { "positive" } else { "negative" },
///        if pos.y >= 0.0 { "positive" } else { "negative" });
/// ```
#[macro_export]
#[clippy::format_args]
macro_rules! trace {
    // trace!(logger: my_logger, context: "my_context", "a {} event", "log")
    (logger: $logger:expr, context: $context:expr, $($arg:tt)+) => ({
        $crate::log!(logger: $logger, context: $context, $crate::Level::Trace, $($arg)+)
    });

    // trace!(logger: my_logger, "a {} event", "log")
    (logger: $logger:expr, $($arg:tt)+) => ({
        $crate::log!(logger: $logger, $crate::Level::Trace, $($arg)+)
    });

    // trace!(context: "my_context", "a {} event", "log")
    (context: $context:expr, $($arg:tt)+) => ({
        $crate::log!(context: $context, $crate::Level::Trace, $($arg)+)
    });

    // trace!("a {} event", "log")
    ($($arg:tt)+) => ($crate::log!($crate::Level::Trace, $($arg)+))
}

/// Determines if a message logged at the specified level in that module will be logged.
///
/// This can be used to avoid expensive computation of data provided as a log message argument.
///
/// # Examples
///
/// ```
/// use score_log::{debug, log_enabled, Level};
///
/// # struct Data { x: u32, y: u32 }
/// # fn expensive_call() -> Data { Data { x: 0, y: 0 } }
/// # let my_logger = score_log::global_logger();
/// if log_enabled!(Level::Debug) {
///     let data = expensive_call();
///     debug!("expensive debug data: {} {}", data.x, data.y);
/// }
///
/// if log_enabled!(context: "Global", Level::Debug) {
///    let data = expensive_call();
///    debug!(context: "Global", "expensive debug data: {} {}", data.x, data.y);
/// }
///
/// if log_enabled!(logger: my_logger, Level::Debug) {
///    let data = expensive_call();
///    debug!(context: "Global", "expensive debug data: {} {}", data.x, data.y);
/// }
/// ```
///
/// This macro accepts the same `context` and `logger` arguments as [`macro@log`].
#[macro_export]
macro_rules! log_enabled {
    // log_enabled!(logger: my_logger, context: "my_context", Level::Info)
    (logger: $logger:expr, context: $context:expr, $level:expr) => ({
        $crate::__log_enabled!(logger: $crate::__log_logger!($logger), context: $context, $level)
    });

    // log_enabled!(logger: my_logger, Level::Info)
    (logger: $logger:expr, $level:expr) => ({
        let logger = $crate::__log_logger!($logger);
        $crate::__log_enabled!(logger: logger, context: logger.context(), $level)
    });

    // log_enabled!(context: "my_context", Level::Info)
    (context: $context:expr, $level:expr) => ({
        $crate::__log_enabled!(logger: $crate::__log_logger!(__log_global_logger), context: $context, $level)
    });

    // log_enabled!(Level::Info)
    ($level:expr) => ({
        let logger = $crate::__log_logger!(__log_global_logger);
        $crate::__log_enabled!(logger: logger, context: logger.context(), $level)
    });
}

#[doc(hidden)]
#[macro_export]
macro_rules! __log_enabled {
    // log_enabled!(logger: my_logger, context: "my_context", Level::Info)
    (logger: $logger:expr, context: $context:expr, $level:expr) => {{
        let level = $level;
        level <= $crate::max_level() && $logger.enabled(&$crate::Metadata::new(level, $context))
    }};
}

// Determine the logger to use.
#[doc(hidden)]
#[macro_export]
macro_rules! __log_logger {
    (__log_global_logger) => {{
        $crate::global_logger()
    }};

    ($logger:expr) => {{
        &($logger)
    }};
}
