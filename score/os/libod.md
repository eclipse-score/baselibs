# Application os/lib OSAL

## Introduction
The purpose of OS lib is to give application developers abstraction layer towards Operating system.
OS functionalities are accessible as `cc_library` bazel targets.
> FFI tagged libraries can be used in ASIL-B safety components for detail see broken_link_g/swh/xpad_documentation/blob/2ba9ae612e204979c21ec86567e57459e20ea06b/enhancement_proposals/proposals/16_common_libraries.md
Following libraries are simple wrappers with no additional logic to OS function. For this libraries only basic description will be added, no static, dynamic architecture will be done.

By default return type can be  wrapped is into one of following

`score::cpp::expected` - score::cpp::expected - A container for an expected value or an error p0323r4:  https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0323r4.html .
Please refer: broken_link_g/swh/amp/blob/master/include/score/expected.hpp

`score::cpp::expected_blank` -  usage with functions that don't return a value.

see broken_link_g/swh/amp for details on amp


## acl
The acl library abstracts the access to the low-level OS functions of Access Control Lists
It implements an abstraction towards the IEEE 1003.1e draft 17 (“POSIX.1e”) part.
It is supported by:
* Linux: https://man7.org/linux/man-pages/man5/acl.5.html
* QNX: https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/acl.html

Be aware that upon usage, you might need to first check if your underlying file-system supports this.
For Linux ext3/4 support ACL extensions, for QNX QNX6FS and ramfs also support it.
Be ware, ACL is not the same as _normal_ mode handling (e.g. via chmod). It is an extension to this concept.
Notice, that a higher level abstraction is provided in score/os/utils/acl/acl.h

The acl library exports the following functions:
* `acl_get_fd` - This function get the access control list associated with a file descriptor. Reference:
* `acl_get_entry` - This function get an entry in an access control list . Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/acl_get_entry.html
* `acl_get_tag_type` - This function get the type of tag from an ACL entry. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/acl_get_tag_type.html
* `acl_get_qualifier` - This function get the qualifier from an ACL entry. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/acl_get_qualifier.html
* `acl_create_entry` - This function create an entry in an access control list . Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/acl_create_entry.html
* `acl_set_tag_type` - This function set the tag type of an ACL entry. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/acl_set_tag_type.html
* `acl_set_qualifier` - This function set the  qualifier for an ACL entry. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/acl_set_qualifier.html
* `acl_get_perm` - This function free the working storage area allocated for an access control list (ACL) data object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/acl_free.html
* `acl_get_permset` - This function checks whether a given permission is present in an ACL permissions set. Reference: https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/a/acl_get_perm_np.html
* `acl_clear_perms` - This function clear all permissions from an ACL permissions set. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/acl_clear_perms.html
* `acl_add_perm` - This function add a permission to an access control list (ACL) permissions set. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/acl_add_perm.html
* `acl_calc_mask` -  This function calculate the group class mask for an access control list (ACL). Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/acl_calc_mask.html
* `acl_valid` - This function validate an access control list (ACL). Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/acl_valid.html
* `acl_set_fd` - This function set the access ACL for the object associated with a file descriptor. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/acl_set_fd.html
* `acl_free` - This function free the working storage area allocated for an access control list (ACL) data object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/acl_free.html

The internal functions for the acl library are as follows:
* `ConvertToPlatformDependentTag` - This function is used to convert the internal used flags to the platform dependent ones for calling the acl_set_tag_type os function.
* `ConvertFromPlatformDependentTag` - This function is used to convert the platform flag received as the return value from the os function acl_get_tag_type to the internal flag
* `ConvertToPlatformDependentPermission` - This function is used to convert the internal used permission to the platform dependent ones for calling the acl_add_perm os function.
### External Dependencies
* errno
* object_seam

## utils/acl
Higher level abstraction for `platform/aas/lib/os:acl`.

The `utils/acl` library exports the following functions:
* `AllowUser` - This function assigns the given permission towards the given user. This can be invoked multiple times for any number of user and permission combinations.
* `VerifyMaskPermissions` - This function verifies whether current mask on the ACL matches given permissions. I.e. for every permission in the list the corresponding entry in the mask must not be "-".
* `FindUserIdsWithPermission` - This function retrieves the list of user IDs with the specified permission.

The internal functions for the acl library are as follows:
* `SetUser` -  This function is used to set the tag type of an ACL entry.
* `AddPermission` - his function is used to add a permission to an access control list permissions set.
* `FindFirstEntry` - This function is used to find the first entry that matches a given predicate callable.
* `CheckMaskPermissions` - This function is used to check if the mask in the acl text matches given permissions.
* `ArePermissionsValid` - This function is used to check if the given permissions are set in the acl text.
### External Dependencies
* platform/aas/lib/os:acl
* errno
* platform/aas/lib/result
* @amp

## dirent
OS-independent abstraction of dirent (format of directory entries) see
https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/d/dirent.html
* `opendir closedir` - function opens/closes a directory stream corresponding to the directory name, and returns a pointer to the directory stream.  The stream is positioned at the first entry in the directory see https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/o/opendir.html.
* `scandir` - function scans the directory dirp, calling filter(), see https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/scandir.html
* `readdir` - The readdir() and readdir64() functions read the next directory entry from the directory specified by dirp, which is the value returned by a call to opendir(). The readdir64() function is a large-file support version of readdir see https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/r/readdir.html
### External Dependencies
* errno
* object_seam

## stat
Stat lib provides functions for retrieving information about files, such as their size, permissions, and modification times, essential for file management and manipulation.

This exports the following OS functions:
* `stat fstat` - these functions return information about a file see https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/stat.html
* `mkdir` - create a new directory see https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/stat.html
* `chmod, fchmod, umask, fchmodat` - change files permission  see https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.utilities/topic/c/chmod.html
This also maintains an unordered map `mapped_modes` which maps the mode bits to the enum in https://pubs.opengroup.org/onlinepubs/7908799/xsh/sysstat.h.html

This also provides the following internal function:
* `stat_to_statbuffer` - This function is used to convert the native buffer populated by the os functions like stat, lstat, fstat to the StatBuffer structure.
* `IntegerToMode` - This function converts the return value of the os function umask to the stat mode.
* `ModeToInteger` - This function converts the stat mode to value acceptable by the os functions like fchmodat, mkdir, chmod, fchmod and umask.
* `GetStatModesMap` - This function is used to access the map(mapped_modes) which maintains the stat mode.
### External Dependencies
* errno
* object_seam
* //platform/aas/lib/bitmanipulation:bitmask_operators

## fcntl
Fcntl lib provides the control over an open file. This function used for manipulating file descriptors. It provides a way to control various aspects of files and file descriptors beyond what can be done with basic file I/O system calls like open, read, write, and close.

The fcntl lib exports the following function:
* `fcntl` - This function provide control over an open file. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/f/fcntl.html
* `open` - This function opens a file. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/o/open.html
* `posix_fallocate`- This function allocate space for a file on the filesystem. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_fallocate.html

The fcntl lib helper functions are as follows:
* `CommandToInteger` - This function is used to convert the command received in the fcntl() to the desired expression.
* `IntegerToOpenFlag` -  This function is used to convert the return value of the os fcntl() to the corresponding flag.
* `OpenFlagToInteger` - This function is used to convert the native flags to corresponding flag for the os fcntl function call.
### External Dependencies
* errno
* object_seam
* stat
* //platform/aas/lib/bitmanipulation:bitmask_operators

## getopt
Parse options from a command line.

The getopt lib exports the following functionlities:
* `getopt` - This function parse options from a command line. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/g/getopt.html
* `getopt_long` -  This function in C for parsing command line arguments, allowing a program to accept both short and long options. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/g/getopt.html

This library provides the functions as follows:
* `getoptopt` - returns the optopt
* `getoptind` - returns the optind
* `getopterr` - returns the opterr
### External Dependencies
* errno
* object_seam

## inotify
The inotify lib provides a mechanism for monitoring filesystem events.

It exports the following functions:
* `inotify_init` - The function returns a file descriptor that can be read from to get event information. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/i/inotify_init.html
* `inotify_add_watch` - This function is used in conjuction with inotify_init. And it returns a watch descriptor, a non-negative integer that uniquely identifies the watch within the inotify instance. This descriptor is used in subsequent operations related to this specific watch. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/i/inotify_add_watch.html
* `inotify_rm_watch` - This function is used to remove an existing watch from an inotify instance. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/i/inotify_rm_watch.html

This library also provides following functions:
* `EventMaskToInteger` - The lib also extends the functionality for conversion to native event mask

### External Dependencies
* errno
* object_seam
* //platform/aas/lib/bitmanipulation:bitmask_operators

## inotify_instance_impl

The inotify_instance_impl library provides RAII-style wrapper around inotify subsystem.

It exports the following fucntions:
* `AddWatch` - The function adds a watch to the inotify instance.
* `RemoveWatch` - This function removes a watch from the inotify instance.
* `Read` - This function does blocking read operation on the inotify instance to gather events from the watches.
* `IsValid` - This function returns whether construction of the inotify instance was successful or errors that occurred.
* `Close` - This function closes the inotify instance and unblocks all pending read operations.

### External Dependencies
* errno
* fcntl
* inotify
* sys_poll
* unistd
* abortable_blocking_reader
* inotify_event
* inotify_instance
* inotify_watch_descriptor

## procmgr
The procmgr lib provides a mechanism for controlling process abilities and run it in the background.

It exports the following functions:
* `procmgr_ability` - The function takes a list of ability operations(and subrange) to control what the identified process is allowed to do. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/procmgr_ability.html
* `procmgr_daemon` - This function lets programs detach themselves from the controlling terminal and run in the background as system daemons. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/procmgr_daemon.html

### External Dependencies
* errno
* object_seam

## sys_poll
The procmgr lib provides a mechanism for controlling process abilities and run it in the background.

It exports the following functions:
* `poll` - This function provides applications with a mechanism for multiplexing input/output over a set of file descriptors. Reference: https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/p/poll.html

### External Dependencies
* errno
* object_seam

## abortable_blocking_reader
The abortable_blocking_reader library provides a helper class to use blocking reads on file descriptors.

It provides the following methods:
* `Read` - Performs a blocking read on the provided file_descriptor.
* `IsValid` - This function Returns the success of the internal setup at construction.
* `Stop` - This function Stops the reader and unblocks all pending read operations.

### External Dependencies
* fcntl
* sys_poll
* unistd
* @amp

## ioctl
The ioctl lib is used to control device-specific operations on file descriptors

The ioctl library exports the following function:
* `ioctl` - This function control a device. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/i/ioctl.html
### External Dependencies
* errno
* object_seam

## grp
Get information about the group with a given name.

The grp library exports the following functions:
* `getgrnam` - The grp lib is extened to call the internal function GroupToGroupBuffer which copies the name and gid to the memory and returns the address of it to the getgrnam() caller. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/g/getgrnam.html
### External Dependencies
* errno
* object_seam

## libgen
The libgen lib provides a set of functions commonly used for string manipulation, particularly in handling file paths

The libgen library exports the following functions:
* `base_name` - This function parses a file path and returns the base filename component. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/b/basename.html
* `dirname` - This function extracts the directory component of a given file path. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/d/dirname.html
### External Dependencies
* errno
* object_seam

## select
This function is used for monitoring multiple file descriptors to see if any of them has an incoming data or is ready for an operation like reading or writing

The select library exports the following functions:
* `select` - This function check for files that are ready for reading or writing or have an exceptional condition pending. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/select.html
### External Dependencies
* errno
* object_seam

## mqueue
The lib mqueue is used to allow the different processes to send and receive messages.

This lib exports the following OS functions:
* `mq_open` - This function after converting openflag to nativeflag calls the os function mq_open(). Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mq_open.html
* `mq_unlink` - This is a wrapper over the os mq_unlink(). This function removes a queue.Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mq_unlink.html
* `mq_send` - This is a wrapper over the os mq_send(). This fucntion sends a message to a queue. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mq_send.html
* `mq_timedsend` - This is a wrapper over the os mq_timedsend(). This fucntion sends a message to a message queue. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mq_timedsend.html
* `mq_receive` - This is a wrapper over the os mq_receive(). This function receives a message from a queue. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mq_receive.html
* `mq_timedreceive` - This is a wrapper over the os mq_timedreceive(). This function receive a message from a message queue Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mq_timedreceive.html
* `mq_close` - This is a wrapper over the os mq_close(). This function receives a message from a message queue. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mq_close.html
* `mq_getattr` - This is a wrapper over the os mq_getattr(). This function is used to get a message queue's attributes. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mq_getattr.html

This library provides the following internal function:
* `openflag_to_nativeflag` - This function converts the open flag of the mq_open function caller to the native flag for calling the mq_open function call.
* `modeflag_to_nativeflag` - This function converts the mode flag of the mq_open function caller to the native flag for calling the mq_open function call.
### External Dependencies
* errno
* object_seam
* static_destruction_guard
* //platform/aas/lib/bitmanipulation:bitmask_operators

## semaphore
This lib exports the following OS functions:

* `sem_init` - This is a wrapper over the os sem_init(). This function is to initialize an unnamed semaphore. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sem_init.html
* `sem_open` - This function after converting openflag to nativeflag and the modeflag to nativeflag(depending on the parameters) calls the os sem_open(). Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sem_open.html
* `sem_wait` - This is a wrapper over the os sem_wait(). This function wait on a named or unnamed semaphore. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sem_wait.html
* `sem_post` - This is a wrapper over the os sem_post(). This function increment a named or unnamed semaphore. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sem_post.html
* `sem_close` - This is a wrapper over the os sem_close(). This function is to close a named semaphore. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sem_close.html
* `sem_unlink` - This is a wrapper over the os sem_unlink(). This function is to destroy a named semaphore. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sem_unlink.html
* `sem_timedwait` - This is a wrapper over the os sem_timedwait(). This function waits on a named or unnamed semaphore, with a timeout. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sem_timedwait.html
* `sem_getvalue` - This is a wrapper over the os sem_getvalue(). This function is to get the value of a named or unnamed semaphore. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sem_getvalue.html
### External Dependencies
* errno
* object_seam
* //platform/aas/lib/bitmanipulation:bitmask_operators

## statvfs
This function provides the detailed information about a mounted file system. This exports the following function:

* `statvfs` - This is a wrapper over the os statvfs(). This function get filesystem information, given a pathReference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/statvfs.html
### External Dependencies
* errno
* object_seam
* //platform/aas/lib/bitmanipulation:bitmask_operators

## unistd
The unistd lib functions are used for handling various low-level, system-call-like functionalities such as file operations, process control, and communication.This exports the following function:

* `close` -  This is a wrapper over the os close(). This function is to close a file. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/c/close.html
* `unlink` - This is a wrapper over the os unlink(). This function is removes a link to a file. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/u/unlink.html
* `access` - This is a wrapper over the os access(). This function is used to check to see if a file or directory can be accessed. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/access.html
* `pipe` - This is a wrapper over the os pipe(). This fucntion creates a pipe. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pipe.html
* `dup` - This is a wrapper over the os dup().This fucntion duplicates a file descriptor. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/d/dup.html
* `dup2` - This is a wrapper over the os dup2(). This function duplicates a file descriptor, specifying the new descriptor. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/d/dup2.html
* `read` - This is a wrapper over the os read(). This function allows to read bytes from a file. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/r/read.html
* `pread` - This is a wrapper over the os pread(). This function reads from a file without moving the file pointer. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pread.html
* `write` - This is a wrapper over the os write(). Reference: This function writes bytes to a file. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/w/write.html
* `pwrite` - This is a wrapper over the os pwrite(). This function write into a file without changing the file pointer. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pwrite.html
* `lseek` - This is a wrapper over the os lseek(). This function set the current read/write file offset. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/l/lseek.html
* `ftruncate` - This is a wrapper over the os ftruncate(). This function truncate a file. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/f/ftruncate.html
* `getpid` - This is a wrapper over the os getpid(). This function gets the process ID. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/g/getpid.html
* `gettid` - This is a wrapper over the os gettid() or syscall()(depending on QNX and Linux). This function gets the thread ID. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/g/gettid.html
* `getuid` - This is a wrapper over the os getuid(). This function gets the user ID. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/g/getuid.html
* `getgid` - This is a wrapper over the os getgid(). This function get the group ID. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/g/getgid.html
* `getppid` - This is a wrapper over the os getppid(). This function gets the parent process ID. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/g/getppid.html
* `setuid` - This is a wrapper over the os setuid(). This function set the real, effective and saved user IDs. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/setuid.html
* `setgid` - This is a wrapper over the os setgid(). This function set the real, effective and saved set-group IDs. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/setgid.html
* `readlink` - This is a wrapper over the os readlink(). This function place the contents of a symbolic link into a buffer. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/r/readlink.html
* `fsync` - This is a wrapper over the os fsync(). This function synchronize the file state. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/f/fsync.html
* `fdatasync` - This is a wrapper over the os fdatasync(). This function synchronize file data. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/f/fdatasync.html
* `nanosleep` - This is a wrapper over the os nanosleep(). This function suspend a thread until a timeout or signal occurs. Reference:
* `sysconf` - This is a wrapper over the os sysconf(). This function return the value of a configurable system limit or option. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sysconf.html
* `link` - This is a wrapper over the os link(). This function create a link to an existing file. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/l/link.html
* `symlink` - This is a wrapper over the os symlink(). This function create a symbolic link to a path. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/symlink.html
* `chdir` - This is a wrapper over the os chdir(). This function change the current working directory. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/c/chdir.html
* `chown` - This is a wrapper over the os chown(). This function change the user ID and group ID of a file. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/c/chown.html
* `getcwd` - This is a wrapper over the os getcwd(). This function is to get the name of the current working directory. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/g/getcwd.html
* `alarm` - This is a wrapper over the os alarm(). This function is to schedule an alarm. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/a/alarm.html
### External Dependencies
* errno
* object_seam
* static_destruction_guard
* //platform/aas/lib/bitmanipulation:bitmask_operators

## mount
mount lib provides the functionality for mounting/unmounting a filesystem. It extends the functional;ity to convert the mount flag to read only mode.

The mount library exports the following functions:
* `mount` - This function is available to mount a filesystem. For detailed reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mount.html
* `umount` - This function is available to unmount a filesystem. For detailed reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/u/umount.html
### External Dependencies
* errno
* object_seam
* //platform/aas/lib/bitmanipulation:bitmask_operators

## stdio
The stdio lib provides a set of functions for input and output operations.

The stdio library exports the following function:
* `fopen` - This function is used to open a file stream. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/f/fopen.html
* `fclose` - This function is to close a stream. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/f/fclose.html
* `remove` - This function removes a link to a file. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/r/remove.html
* `rename` - This function renames a file. Refernce: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/r/rename.html
* `popen` - This function executes a command, creating a pipe to it. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/popen.html
* `pclose` - This function closes a pipe. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pclose.html
* `fileno` - This function returns the file descriptor for a stream. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/f/fileno.html
### External Dependencies
* errno
* object_seam
* //platform/aas/lib/bitmanipulation:bitmask_operators

## stdlib
The stdlib library offers functionalities for system-level operations, process termination, environment manipulation, and file management, essential for robust and efficient system programming.

The stdlib exports the following function:
* `system_call` - This function calls the system() function of the os which executes a system command. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/system.html
* `exit` - This function is used to exit the calling program. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/e/exit.html
* `quick_exit` - This function is used to exit the calling program without completely cleaning up resources. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/q/quick_exit.html
* `getenv` - This function gets the value of an environment variable. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/g/getenv.html
* `realpath` - This function resolves a pathname. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/r/realpath.html
* `mkstemp` - This function makes a unique temporary filename and open the file. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mkstemp.html
* `mkstemps` - This function makes a unique temporary filename and open the file. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mkstemp.html
### External Dependencies
* errno
* object_seam

## string
The string library is used for manipulating strings and arrays, providing essential functions.

The string library exports the following functions:
* `memcpy` - This function copy bytes from one buffer to another. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/memcpy.html
* `strerror` - This function converts an error number into an error message. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/strerror.html
* `memset` - This function sets memory to a given value. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/memset.html
### External Dependencies
* errno
* object_seam

## time
The time library accurately retrieving and setting system time, essential for precise time synchronization and measurement.

The time library exports the following function:
* `clock_settime` - This function sets a clock. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/c/clock_settime.html
* `clock_gettime` - This function gets the current time of a clock. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/c/clock_gettime.html
### External Dependencies
* errno
* object_seam

## static_destruction_guard
An abstraction of the Nifty Counter Idiom which ensures that variables with static storage duration are only destructed after _all_ their users are destructed. This shall only be used if a static variable is used in a destructor.

The static_destruction_guard library exports the following function:
* `GetStorage` - Enables access to underlying local storage and returns the StorageType reference.

## mman
The mman library provides powerful memory management functions like mmap and munmap for mapping and unmapping files to memory, and shm_open and shm_unlink for creating and managing shared memory objects, essential for efficient file handling.

The mman library exports the following functions:
* `mmap` - This function maps a memory region into a process's address space. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mmap.html
* `mmap64` - This function maps a memory region into a process's address space, large-file support version of mmap. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mmap.html
* `munmap` - This function unmaps the previously mapped addresses. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/munmap.html
* `shm_open` - This function opens a shared memory object based on a pathname. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/shm_open.html
* `shm_unlink` - This function removes the name of a shared memory object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/shm_unlink.html
* `mmap_device_io` - This function maps device I/O memory into a process's address space, allowing access via in*() and out*() functions. The mmap_device_io() function is a wrapper function for mmap() and is only provided for backwards compatibilityReference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mmap_device_io.html
* `munmap_device_io` - This function unmaps the previously mapped with mmap_device_io. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/munmap_device_io.html
* `shm_open_handle` - This function opens a shared memory object based on a shared memory handle. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/shm_open_handle.html
* `shm_create_handle` - This function creates a handle so a specific process can access a shared memory object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/shm_create_handle.html
* `shm_ctl` - This function gives special attributes to a shared memory object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/shm_ctl.html
* `mem_offset` - This function gets the physical address of a virtually mapped memory block. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mem_offset.html
* `mem_offset64` - This function gets the physical address of a virtually mapped memory block, large-file support version of mem_offset. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mem_offset.html

### External Dependencies
* errno
* fcntl
* object_seam
* rt
* static_destruction_guard
* //platform/aas/lib/bitmanipulation:bitmask_operators

## errno
This abstraction is an operating system independent version of error codes. This will be used as part of any functions return code within the os abstraction which consists of an `score::cpp::expected`.

The errno provides the following functions:
* `createFromErrno` - This function creates a new instance from the error indicated by errno
* `createUnspecifiedError` - This function creates a new instance from the error for Unspecified errors to support non OS agnostic sys calls
* `ToString` - This function turns the error into a string
* `ToStringContainer` - This function wraps error into an array of 32 characters
### External Dependencies
* @amp

## errno_logging
This is Global overload of a LogStream operator to enable translation from error to human readable representation
### External Dependencies
* errno
* //platform/aas/mw/log:log_stream

## pthread
The pthread library allows for creating and managing threads, which are a way of achieving concurrency.

The pthread library exports the following functions:
* `getcpuclockid` - Reference: https://man7.org/linux/man-pages/man3/pthread_getcpuclockid.3.html
* `setname_np` - Reference: https://man7.org/linux/man-pages/man3/pthread_setname_np.3.html
* `getname_np` - Reference: https://man7.org/linux/man-pages/man3/pthread_getname_np.3.html
* `condattr_init` - This function initialize the attribute object for creating condition variables. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_condattr_init.html
* `condattr_setpshared` - This function set the process-shared attribute in a condition variable attribute object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_condattr_setpshared.html
* `condattr_destroy` - This function Destroy a condition-variable attribute object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_condattr_destroy.html
* `cond_init` - This function initialize a condition variable. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_cond_init.html
* `cond_destroy` - This function destroy a condition variable. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_cond_destroy.html
* `mutexattr_init` - This function initialize a mutex attribute object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_mutexattr_init.html
* `mutexattr_setpshared` - This function set the process-shared attribute in mutex attribute object.Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_mutexattr_setpshared.html
* `mutexattr_destroy` - This function destroy mutex attribute object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_mutexattr_destroy.html
* `mutex_init` - This function initialize a mutex. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_mutex_init.html
* `mutex_destroy` - This function destroy a mutex. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_mutex_destroy.html
* `pthread_setschedparam` - This function set thread scheduling parameters. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_setschedparam.html
* `pthread_getschedparam` - This function get a thread's scheduling parameters. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_getschedparam.html
### External Dependencies
* errno
* object_seam

## object_seam
This encapsulates helper methods for the object-seam approach to reduce code duplication. This also contains class that will automatically register an object as the test object for the specified interface.

The object_seam exports the following functions:
* `set_testing_instance` - This function enables the injection of an user-owned testing instance. Without transferring ownership
* `restore_instance` - This function removes any instance that was set by set_testing_instance()
* `select_instance` - This function is thread safe stand-alone to select the instance

## sched
The sched library provides an interface for real-time scheduling. It includes functions that allow you to control the scheduling policy and priority of threads and processes.

The sched library exports the following functions:
* `sched_getparam` - This function get the current scheduling parameters of a process. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sched_getparam.html
* `sched_getscheduler` - This function get the current scheduling policy for a process. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sched_getscheduler.html
* `sched_setparam` - This function change the priority of a process. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sched_setparam.html
* `sched_setscheduler` - This function change the priority and scheduling policy of a process. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sched_setscheduler.html
* `sched_yield` - This function yield to other ready threads at the same priority. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sched_yield.html
* `sched_rr_get_interval` - This function get the execution time limit of a process. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sched_rr_get_interval.html
* `sched_get_priority_min` - This function get the minimum priority for the scheduling policy. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sched_get_priority_min.html
* `sched_get_priority_max` - This function get the maximum priority for the scheduling policy. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sched_get_priority_max.html
* `sched_get_priority_adjust` - This function for QNXNTO calculate the allowable priority for the scheduling policy. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sched_get_priority_adjust.html
### External Dependencies
* errno
* object_seam

## spawn
The spawn library is used for the creation of a new process by a parent process for enabling multitasking.

The spawn library exports the following functions:
* `posix_spawnattr_init` - This function initialize a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_init.html
* `posix_spawnattr_destroy` - This function destroy a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_destroy.html
* `posix_spawnattr_getflags` - This function get the POSIX flags from a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_getflags.html
* `posix_spawnattr_setflags` - This function set the POSIX flags in a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setflags.html
* `posix_spawnattr_getsigdefault` - This function get the “default signals” attribute of a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_getsigdefault.html
* `posix_spawnattr_setsigdefault` - This function set the “default signals” attribute of a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setsigdefault.html
* `posix_spawnattr_getsigmask` - This function get the signal mask attribute of a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_getsigmask.html
* `posix_spawnattr_setsigmask` - This function set the signal mask attribute of a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setsigmask.html
* `posix_spawnattr_getpgroup` - This function get the process group attribute of a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_getpgroup.html
* `posix_spawnattr_setpgroup` - This function set the process group attribute in a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setpgroup.html
* `posix_spawnattr_getschedparam` - This function get the scheduling parameters attribute of a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_getschedparam.html
* `posix_spawnattr_setschedparam` - This function set the scheduling parameters attribute of a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setschedparam.html
* `posix_spawnattr_getschedpolicy` - This function get the scheduling policy attribute of a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_getschedpolicy.html
* `posix_spawnattr_setschedpolicy` - This function set the scheduling policy attribute of a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setschedpolicy.html
* `posix_spawn_file_actions_init` - This function initialize a spawn file actions object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawn_file_actions_init.html
* `posix_spawn_file_actions_destroy` - This function destroy a spawn file actions object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawn_file_actions_destroy.html
* `posix_spawn_file_actions_addclose` - This function add a “close a file descriptor” action to a spawn file actions object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawn_file_actions_addclose.html
* `posix_spawn_file_actions_adddup2` - This function add a “duplicate a file descriptor” action to a spawn file actions object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawn_file_actions_adddup2.html
* `posix_spawn_file_actions_addopen` - This function dd an “open a file” action to a spawn file action object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawn_file_actions_addopen.html
* `posix_spawn` - This function spawn a process. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawn.html
* `posix_spawnp` - This function spawn a process. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawn.html
The following functions are for QNXNTO
* `posix_spawnattr_getxflags` - This function get the POSIX flags and the QNX Neutrino extended flags from a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_getxflags.html
* `posix_spawnattr_setxflags` - This function set the POSIX flags and the QNX Neutrino extended flags in a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setxflags.html
* `posix_spawnattr_getrunmask` - This function get the runmask attribute from a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_getrunmask.html
* `posix_spawnattr_setrunmask` - This function set the runmask attribute in a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setrunmask.html
* `posix_spawnattr_getsigignore` - This function get the “ignored signals” attribute from a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_getsigignore.html
* `posix_spawnattr_setsigignore` - This function set the “ignored signals” attribute of a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setsigignore.html
* `posix_spawnattr_getstackmax` - This function get the maximum stack size attribute from a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_getstackmax.html
* `posix_spawnattr_setstackmax` - This function set the maximum stack size attribute in a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setstackmax.html
* `posix_spawnattr_getnode` - This function get the remote node attribute from a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_getnode.html
* `posix_spawnattr_setnode` - This function set the remote node attribute in a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setnode.html
* `posix_spawnattr_getcred` - This function get the user and group ID attributes from a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_getcred.html
* `posix_spawnattr_setcred` - This function set the user and group ID attributes in a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setcred.html
* `posix_spawnattr_gettypeid` - This function get the type identifier of a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_gettypeid.html
* `posix_spawnattr_settypeid` - This function set the type identifier of a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_settypeid.html
* `posix_spawnattr_setasid` - This function set the address space ID attribute in a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setasid.html
* `posix_spawnattr_getaslr` - This function return the Address Space Layout Randomization (ASLR) setting from a posix_spawn() attribute structure. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_getaslr.html
* `posix_spawnattr_setaslr` - This function control how the Address Space Layout Randomization (ASLR) setting of a child is determined. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setaslr.html
* `posix_spawnattr_setcwd_np` - This function set the current working directory attribute in a spawn attributes object. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/posix_spawnattr_setcwd_np.html
### External Dependencies
* errno
* object_seam

## sys_wait
The sys_wait library is used for process synchronization, allowing a parent process to pause execution and wait for the completion or status change of its child processes.

The sys_wait library exports the following functions:
* `wait` - This function wait for the status of a terminated child process. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/w/wait.html
* `waitpid` - This function wait for a child process to stop or terminate. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/w/waitpid.html
### External Dependencies
* errno
* object_seam

## Utilities
The following are the utility libararies:

## mqueue_headers, mqueue
The MQueue library are deplyoed in two classes MQueuePrivate and MQueue. The  MQueuePrivate provides the following functions:

* `send` - This function calls the MqueueImpl::mq_send(). The necessary parameters are added for the call.
* `timed_send` - This function calls the MqueueImpl::mq_timedsend(). The necessary parameters are added for the call.
* `receive` - This function calls the MqueueImpl::mq_receive(). The necessary parameters are added for the call. Depending on the parameter list the return type would be eithier the message data as string or score::cpp::expected<size_t, Error>.
* `get_msg_size` - This accessor function is to retrive the message size from the m_attr structure.
* `timed_receive` - This function calls the MqueueImpl::mq_timedreceive() with all the necessary params including the current time. The return value would be pair of the return value of the Mqueue::mq_timedreceive() along with the flag stating if t has the value or not.
* `unlink` - This function calls the MqueueImpl::mq_unlink() with the corresponding name.
* `get_id` - This function returns the id.
* `get_mq_st_mode` - This accessor function calls the StatImpl::fstat() and returns the st_mode. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/stat_struct.html

This library provides the following internal function:
* `open_create` - This function initiates by calling the unlink function prior to the creation of a new message queue. This is accomplished by invoking MqueueImpl::mq_open(). Subsequently, it employs StatImpl::fchmod() to modify the file permissions, setting them to a combination of {score::os::Stat::Mode::kReadUser | score::os::Stat::Mode::kWriteUser | score::os::Stat::Mode::kReadGroup | score::os::Stat::Mode::kWriteGroup | score::os::Stat::Mode::kReadOthers}.
* `create_name` - This function serves to either append a '/' character to an existing string name, or, in the case of an identifier id, it first converts this id to a string and then appends a '/' to it.

The MQueue class provides the following functions:
* `get_msg_size` - This function calls the MQueuePrivate::get_msg_size
* `send` - This function calls the MQueuePrivate::mq_send()
* `timed_send` - This function calls the MQueuePrivate::timed_send()
* `receive` - This function calls the MQueuePrivate::receive()
* `unlink` - This function calls the MQueuePrivate::unlink()
* `get_id` - This function calls the MQueuePrivate::get_id()
* `timed_receive` - This function calls the MQueuePrivate::timed_receive()
* `get_mq_st_mode` - This function calls the MQueuePrivate::get_mq_st_mode()
### External Dependencies
* errno
* @amp
* //platform/aas/lib/os:mqueue
* //platform/aas/lib/os:stat
* //platform/aas/lib/utils

## signal_headers, signal
The signal library is used for variour signal functionalities.

The signal library provides the following functions:
* `AddTerminationSignal` - This function calls os sigaddset(). It add a signal to a set. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sigaddset.html
* `PthreadSigMask` - This function calls os pthread_sigmask() and returns the value. It modify or examine a thread's signal-blocked mask. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_sigmask.html
* `SendSelfSigterm` - This function calls os kill() with itsown pid.
* `GetCurrentBlockedSignals` - This function calls the os sigemptyset() to initialize a set to contain no signals and calls Signal::PthreadSigMask() populates the signal set.
* `IsSignalBlocked` - This function calls the os sigemptyset() to initialize a set to contain no signals, then calls GetCurrentBlockedSignals() to get the set of blocked siganls and checks if a specified signal is blocked
* `SigIsMember` - This function calls os sigismember() which see if a given signal is in a given set. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sigismember.html
* `SigFillSet` - This function calls os sigfillset(). It initialize a set to contain all signals. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sigfillset.html
* `SigEmptySet` - This function calls os sigemptyset().
* `SigWait` - This function calls os sigwait()
* `SigAddSet` - This function calls os sigaddset().
* `SigAction` - This function calls os sigaction().
* `Kill` - This function calls os kill(). It send a signal SIGTERM to a process or a group of processes. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/k/kill.html

### External Dependencies
* //platform/aas/lib/os:errno
* @amp

## thread_headers, thread
The thread library is used to set or get the thread name.

This library exposes the following functions:
* `set_thread_affinity` - For Linux the os pthread_setaffinity_np() is called. Reference: https://man7.org/linux/man-pages/man3/pthread_setaffinity_np.3.html. For QNX ThreadCtl() is called here. Reference: https://www.qnx.com/developers/docs/7.1/index.html#com.qnx.doc.neutrino.lib_ref/topic/t/threadctl.html#threadctl___NTO_TCTL_RUNMASK_GET_AND_SET
* `set_thread_name` - This function calls Pthread::setname_np() and sets the provided name to the thread.
* `get_thread_name` - This function calls Pthread::getname_np() and returns the string
### External Dependencies
* //platform/aas/lib/logging:headers
* //platform/aas/lib/os:pthread

## high_resolution_steady_clock
In QNX, `std::chrono::steady_clock` only offers millisecond resolution. Hence, another clock must be used under QNX to obtain a higher resolution. From QNX SDP 8.39.10 on, nanosecond resolution can be acheived by using `std::chrono::high_resolution_clock` instead since it utilizes QNX's `ClockCycles()`. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/c/clockcycles.html)

This library exposes the following function:
* `now()` - The high_resolution_steady_clock library provides  for the higher resolution time.

## interprocess_mutex
The interprocess_mutex library is used to lock or unlock a mutex.

This library exports the following functions for mutex:
* `lock` - This function calls os pthread_mutex_lock(). Blocks until a lock can be obtained for the current execution agent (thread, process, task). If an exception is thrown, no lock is obtained.
* `unlock` - This function calls os pthread_mutex_unlock(). Releases the lock held by the execution agent. Throws no exceptions.
* `try_lock` - This function calls os pthread_mutex_trylock()Attempts to acquire the lock for the current execution agent (thread, process, task) without blocking. If an exception is thrown, no lock is obtained. It true if the lock was acquired, false otherwise
### External Dependencies
* @amp

## interprocess_conditional_variable
The InterprocessConditionalVariable would be used to coordinate actions between different processes. Upon careting the InterprocessConditionalVariable the os pthread_condattr_init(), pthread_condattr_setpshared(), pthread_cond_init() would be called sequentially. Upon destrotying the InterprocessConditionalVariable, the os pthread_cond_destroy() would be called.

This library provides the following functions:
* `notify_one` - This function calls os pthread_cond_signal(). It unblock a thread that's waiting on a condition variable. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_cond_signal.html
* `notify_all` - This function uses os pthread_cond_broadcast() to broadcast about the convar.  It unblock threads waiting on a condition. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_cond_broadcast.html
* `wait` - This function calls os pthread_cond_wait(). It wait on a condition variable. Reference: https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_cond_wait.html
### External Dependencies
* interprocess_mutex
* @amp

## stdout_pipe
The stdout_pipe library is used to create or stop/close/clean the pipe for std out.

This library provides the following functions:
* `pipe` - This function makes sure if the piping has already started and if not the Unistd::pipe() used to create the pipe. The Unistd::dup2() is called to duplicate file descriptor to pipe
* `stop` - This function calls stop source

The library provides the following internal functions:
* `isPiped` - This function validated if piped or not.
* `restoreOriginalFD` - This function restores the original file descriptor.
* `readPipe` - This function calls Unistd::read() to read from the pipe and close once the reading is done.
* `stopCallback` - This function close pipe and restores the original file descriptor.
* `cleanup` - This function call the close()and then restoreOriginalFD() for cleanup.
* `closePipe` -  This function calls Unistd::close to close the pipe.
### External Dependencies
* //platform/aas/lib/logging:headers
* //platform/aas/lib/os:asil_qm
* @amp

## path_header, path
The path library is used to retrive the path such as base name, parent directory name, execution program name.

This library provides the following functions:
* `get_base_name` - This function is used to retrive the base name. It uses Libgen::base_name() for this purpose.
* `get_parent_dir` - This function is used to retrive the parent directory name. It uses Libgen::dirname() for this purpose.
* `get_exec_path` - This function is used retrive the the pathname of the executing program and NULL if the pathname of the executing program cannot be determined.
### External Dependencies
* //platform/aas/lib/os:libgen
* //platform/aas/lib/os:unistd
* @amp

## machine_header, machine
The machine library is used to check if it is QEMU or SCTF

This library provides the following functions:
* `is_qemu` - This function is to check if it is on QEMU
* `is_sctf` - This fucntion is to checck if it is SCTF
### External Dependencies
//platform/aas/lib/os:stdlib

## detect_os
The detect_os library is to check for the os type. It checks is the os is QNX or Linux

This library provides the following functions:
* `IsLinux` - This function checks if the os is Linux. It uses the IsOs() function for this purpose.
* `IsQnx` - This function checks if the os is QNX. It uses the IsOs() function for this purpose.
* `IsOs` - This function returns true if the input string is same as that of the system name.
### External Dependencies
//platform/aas/lib/os:asil_qm

## Design rationales
EPIC for OSAL implementation broken_link_j/Ticket-12624

for design rationale see README for lib/os README.md
