# Design Decisions

## Dynamic backend loading for mw::log

### Appoach Comparison for the intended use case

| Criterion | *dlopen() | **DT_NEEDED | Weak Symbols | Global Backend Table (static) - existing implemenation |
|-----------|----------------------|--------------------------|---------------------------|--------------------------------------|
| Graceful degradation | Yes - fallback chain (console to stub) | No - process fails if .so missing | Yes - weak defaults | Yes - console fallback for unavailable backends |
| Runtime dependency | Plugin .so must be on trusted filesystem | .so must be on loader search path | None beyond normal linking | None - all resolved at link time |
| Attack surface | Lower than single bundle - each process loads only the backends it needs; a file-only process never loads DataRouter deps | Medium - loader search paths | Lowest - no runtime loading | Lowest - no runtime loading, no filesystem deps |
| QNX OS compatibility | Supported (RTLD_NOSHARE available) | Supported | Supported but toolchain-dependent | Full - standard C++ |
| Deployment complexity | per-backend .so files deployed to well-known directory; directory-based scanning | Similar to dl_open | Lowest | Lowest - single binary |
| Customer plugin support | Full - customer ships .so without rebuilding | No | No | No - requires static linking |
| Implementation complexity | Requires explicit loader code (dynamic backend loader: dlopen, dlsym, version check, fallback chain) | Zero - runtime linker resolves everything automatically before main(); no application code needed | Requires weak symbol declarations and fallback definitions | Requires registration table and factory map |

*dlopen() - Is an imperative, runtime API. The application explicitly calls dlopen() in code, controls when and whether to load a library, inspects the return value, and can fall back gracefully on failure.

**DT_NEEDED - Is a declarative, link-time mechanism. The dependency is recorded in the ELF .dynamic section when you link with -lfoo. The runtime linker (ld.so / ldqnx.so) resolves all DT_NEEDED entries automatically before main() runs. If any listed .so is missing, the process aborts at startup - the application never gets a chance to handle the failure.

dlopen() satisfies all the criteria especially the graceful degradation and extensibility for plugin based backend selection. Hence dl_open solution is preferred for the usecase in ./dynamic_backend_loading_design.md

References:
- [POSIX dlopen](https://man7.org/linux/man-pages/man3/dlopen.3.html)
- [QNX dlopen](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/d/dlopen.html)
- [DT_NEEDED elf](https://refspecs.linuxfoundation.org/elf/elf.pdf)
- [QNX8 lazy loading](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.prog/topic/devel_Lazy_loading.html)
