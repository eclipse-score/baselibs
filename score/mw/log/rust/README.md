log
===

A Rust library providing a lightweight logging *facade*.
Keep in mind that this is currently `Enabler` solution means it directly
uses open source `log` crate. There is ongoing work to decide whether S-CORE
will implement own logging *facade* on improve some existing one.


A logging facade provides a single logging API that abstracts over the actual
logging implementation. Libraries can use the logging API provided by this
crate, and the consumer of those libraries can choose the logging implementation
that is most suitable for its use case.

## Usage

### In libraries

Libraries should link only to the `log` crate, and use the provided macros to
log whatever information will be useful to downstream consumers:

```rust
use log::{info, trace, warn};

pub fn shave_the_yak(yak: &mut Yak) {
    trace!("Commencing yak shaving");

    loop {
        match find_a_razor() {
            Ok(razor) => {
                info!("Razor located: {razor}");
                yak.shave(razor);
                break;
            }
            Err(err) => {
                warn!("Unable to locate a razor: {err}, retrying");
            }
        }
    }
}
```

### In executables

In order to produce log output, executables have to use a logger implementation
compatible with the facade. The currently supported implementation is located in
[logging repo](https://github.com/eclipse-score/logging) as `mw_log_subscriber`
target.