// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This software may be used and distributed according to the terms of the
// GNU General Public License version 2.

#ifndef __INTF_H
#define __INTF_H

#ifndef __KERNEL__
typedef unsigned int u32;
typedef unsigned long long u64;
#endif

struct soft_dirty_fault_event {
    u32 tid;
    u64 address;
};

#endif /* __INTF_H */
