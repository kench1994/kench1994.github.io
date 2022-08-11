---
layout: post
title: "systemd service paths"
date: 2022-08-11 09:12:34
categories: linux
tags: systemd
---

* content
{:toc}

From: https://www.freedesktop.org/software/systemd/man/systemd.exec.html#


**ExecSearchPath=**
Takes a colon separated list of absolute paths relative to which the executable used by the Exec*= (e.g. ExecStart=, ExecStop=, etc.) properties can be found. ExecSearchPath= overrides $PATH if $PATH is not supplied by the user through Environment=, EnvironmentFile= or PassEnvironment=. Assigning an empty string removes previous assignments and setting ExecSearchPath= to a value multiple times will append to the previous setting.

**WorkingDirectory=**
Takes a directory path relative to the service's root directory specified by RootDirectory=, or the special value "~". Sets the working directory for executed processes. If set to "~", the home directory of the user specified in User= is used. If not set, defaults to the root directory when systemd is running as a system instance and the respective user's home directory if run as user. If the setting is prefixed with the "-" character, a missing working directory is not considered fatal. If RootDirectory=/RootImage= is not set, then WorkingDirectory= is relative to the root of the system running the service manager. Note that setting this parameter might result in additional dependencies to be added to the unit (see above).

**RootDirectory=**
Takes a directory path relative to the host's root directory (i.e. the root of the system running the service manager). Sets the root directory for executed processes, with the chroot(2) system call. If this is used, it must be ensured that the process binary and all its auxiliary files are available in the chroot() jail. Note that setting this parameter might result in additional dependencies to be added to the unit (see above).

The MountAPIVFS= and PrivateUsers= settings are particularly useful in conjunction with RootDirectory=. For details, see below.

If RootDirectory=/RootImage= are used together with NotifyAccess= the notification socket is automatically mounted from the host into the root environment, to ensure the notification interface can work correctly.

Note that services using RootDirectory=/RootImage= will not be able to log via the syslog or journal protocols to the host logging infrastructure, unless the relevant sockets are mounted from the host, specifically:

> Example 1. Mounting logging sockets into root environment
BindReadOnlyPaths=/dev/log /run/systemd/journal/socket /run/systemd/journal/stdout

This option is only available for system services, or for services running in per-user instances of the service manager when PrivateUsers= is enabled.