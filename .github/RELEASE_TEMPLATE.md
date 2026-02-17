Module Name: baselibs
Release Tag: $RELEASE_TAG
Origin Release Tag: $PREVIOUS_RELEASE_TAG
Release Commit Hash: $COMMIT_SHA
Release Date: $RELEASE_DATE

Overview
--------
The baselibs module provides a selection of basic C++ utility libraries for common use in the S-CORE project.

Disclaimer
----------
This release is not intended for production use, as it does not include a safety argumentation or a completed safety assessment.
The work products compiled in the safety package are created with care according to the [S-CORE process](https://eclipse-score.github.io/process_description/main/index.html). However, as a non-profit, open-source organization, the project cannot assume any liability for its content.

For details on the features, see https://eclipse-score.github.io/score/main/features/baselibs/index.html

Improvements
------------

Bug Fixes
---------

Compatibility
-------------
- `x86_64-unknown-linux-gnu`, `x86_64-unknown-nto-qnx800` and  `aarch64-unknown-nto-qnx800` using [score_toolchains_gcc](https://github.com/eclipse-score/bazel_cpp_toolchains).

Performed Verification
----------------------
- Build for  `x86_64-unknown-linux-gnu` and `x86_64-unknown-nto-qnx800`.
- Unit tests executed on `x86_64-unknown-linux-gnu`.

Report: $ACTION_RUN_URL

Known Issues
------------

Upgrade Instructions
--------------------
Backward compatibility with the previous release is not guaranteed.

Contact Information
-------------------
For any questions or support, please contact the Base Libs Feature Team (https://github.com/orgs/eclipse-score/discussions/1223) or raise an issue/discussion.
