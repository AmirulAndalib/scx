/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2025 Meta Platforms, Inc. and affiliates.
 * Copyright (c) 2025 Daniel Hodges <hodgesd@meta.com>
 */
#ifndef BPF_PERCPU_H
#define BPF_PERCPU_H

#ifdef LSP
#define __bpf__
#include "../vmlinux.h"
#else
#include "vmlinux.h"
#endif

#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>


extern int sd_llc_size __ksym __weak;
extern int sd_llc_id __ksym __weak;
extern int sched_core_priority __ksym __weak;
extern struct sugov_cpu sugov_cpu __ksym __weak;
extern struct psi_group_cpu psi_group_cpu __ksym __weak;
extern struct kernel_stat kernel_stat __ksym __weak;
extern struct kernel_cpustat kernel_cpustat __ksym __weak;

#define DEFINE_PER_CPU_PTR_FUNC(func_name, type, var_name)        \
type *func_name(s32 cpu) {                                        \
    type *ptr;                                                    \
    if (!&var_name)                                               \
        return NULL;                                              \
    ptr = bpf_per_cpu_ptr(&var_name, cpu);                        \
    if (!ptr)                                                     \
        return NULL;                                              \
    return ptr;                                                   \
}

#define DEFINE_PER_CPU_VAL_FUNC(func_name, type, var_name)        \
type func_name(s32 cpu) {                                         \
    type *ptr;                                                    \
    ptr = bpf_per_cpu_ptr(&var_name, cpu);                        \
    if (!ptr)                                                     \
        return -EINVAL;                                           \
    return *ptr;                                                  \
}

#define DEFINE_THIS_CPU_VAL_FUNC(orig_func_name)			\
static inline typeof(orig_func_name(0)) this_##orig_func_name(void) {	\
    return orig_func_name(bpf_get_smp_processor_id());			\
}

#define DEFINE_THIS_CPU_PTR_FUNC(orig_func_name)			\
static inline typeof(orig_func_name(0)) this_##orig_func_name(void) {	\
    return orig_func_name(bpf_get_smp_processor_id());			\
}

DEFINE_PER_CPU_VAL_FUNC(cpu_llc_size, int, sd_llc_size)
DEFINE_PER_CPU_VAL_FUNC(cpu_llc_id, int, sd_llc_id)
DEFINE_PER_CPU_VAL_FUNC(cpu_priority, int, sched_core_priority)
DEFINE_PER_CPU_PTR_FUNC(cpu_sugov, struct sugov_cpu, sugov_cpu)
DEFINE_PER_CPU_PTR_FUNC(cpu_psi_group, struct psi_group_cpu, psi_group_cpu)
DEFINE_PER_CPU_PTR_FUNC(cpu_kernel_stat, struct kernel_stat, kernel_stat)
DEFINE_PER_CPU_PTR_FUNC(cpu_kernel_cpustat, struct kernel_cpustat, kernel_cpustat)

DEFINE_THIS_CPU_VAL_FUNC(cpu_llc_size)
DEFINE_THIS_CPU_VAL_FUNC(cpu_llc_id)
DEFINE_THIS_CPU_VAL_FUNC(cpu_priority)
DEFINE_THIS_CPU_PTR_FUNC(cpu_sugov)
DEFINE_THIS_CPU_PTR_FUNC(cpu_psi_group)
DEFINE_THIS_CPU_PTR_FUNC(cpu_kernel_stat)
DEFINE_THIS_CPU_PTR_FUNC(cpu_kernel_cpustat)

#endif /* BPF_PERCPU_H */
