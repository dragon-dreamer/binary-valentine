<img align="right" width="200" src="./.github/logo-light.svg">

# Binary valentine
Binary Valentine is the open-source and free to use executable files static analyzer. It is able to detect a range of various issues, which are usually not detected by other means (such as static code analysis).

Website: [binary-valentine.com](https://binary-valentine.com).

## Main features
* Finds bugs, misconfigurations, format and security issues related to the executable format and not the code it contains. Suggests hardening and optimization techniques.
* Supports flexible project configuration (file path, rule filters), which can be supplied as a separate file or in the command line.
* Supports several output formats (terminal output, plain text, SARIF).
* Supports both single executable rules and combined (cross-executable) rules.
* Can be built into the SSDLC (Secure Software Development Lifecycle).
* Multithreaded and fast. With the fast SSD drive, scans the whole Windows 10 `System32` folder in 10-20 seconds.

## What Binary Valentine currently supports

### Portable executable format
Binary Valentine currently only supports the Portable Executable format (which is used extensively on Windows, EFI, X-Box). For this format, it detects various problems:
* **Security issues**, such as absent hardening options (e.g. ASLR), insecure configurations (e.g. writable code sections), or usage of dangerous WinAPI functions.
* **Configuration issues**, for example, version information bugs, application icon and manifest issues.
* **Optimization issues**, which could be addressed to make the executable faster or smaller (e.g. profile-guided optimization usage).
* **System issues**, which are related to the operating system misuse, such as deprecated or internal WinAPI usage, system compatibility issues, and DPI awareness.
* **Format issues**, which are related to the PE format itself.

For Portable Executable, more than 160 rules are currently provided. Most rules are single-executable, but some of them do combined scans (e.g., version info cross-executable consistency check rules).

### Flexible project configuration
An XML project file can be supplied to Binary Valentine, which would scan the project according to selected rules and filters. A project can contain one or more files or directories to scan (with optional regular expressions to filter path names). Each path or file can be configured with a separate set of applied rules, warning levels and categories. Apart from that, XML projects can specify output format and execution options (e.g. thread count and max memory consumption limit).
In addition to project-based execution, Binary Valentine can be invoked by supplying all parameters to its command line.

### Industry standard output formats
Binary Valentine supports several output formats to ease integration with other systems. In addition to terminal and plain text output, it supports the [Static Analysis Results Interchange Format (SARIF) 2.1.0](https://docs.oasis-open.org/sarif/sarif/v2.1.0/sarif-v2.1.0.html), which is the industry standard format for the output of static analysis tools.

### Cross-platform
Binary Valentine can be built for a range of platforms with a set of C++20 compilers:
* Windows 10/11, MSVC 2022 (Visual Studio solution and CMake build are provided)
* Linux, GCC 13.0 (CMake build)
* Linux, Clang 16 (CMake build)

Windows 10/11 and modern Linux (kernel version 5.1 or newer with `io_uring` enabled) are supported as of today.
