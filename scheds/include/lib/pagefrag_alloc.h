/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/* Copyright (c) 2024-2025 Meta Platforms, Inc. and affiliates. */
#pragma once
#include <scx/bpf_arena_common.bpf.h>

#ifndef __round_mask
#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#endif
#ifndef round_up
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#endif

static void __arena * __arena page_frag_cur_page[NR_CPUS];
static int __arena page_frag_cur_offset[NR_CPUS];

struct {
	__uint(type, BPF_MAP_TYPE_ARENA);
	__uint(map_flags, BPF_F_MMAPABLE);
#if defined(__TARGET_ARCH_arm64) || defined(__aarch64__)
	__uint(max_entries, 1 << 16); /* number of pages */
        __ulong(map_extra, (1ull << 32)); /* start of mmap() region */
#else
	__uint(max_entries, 1 << 20); /* number of pages */
        __ulong(map_extra, (1ull << 44)); /* start of mmap() region */
#endif
} arena __weak SEC(".maps");

/* Simple page_frag allocator */
static inline void __arena* pagefrag_alloc(unsigned int size)
{
	__u64 __arena *obj_cnt;
	__u32 cpu = bpf_get_smp_processor_id();
	void __arena *page = page_frag_cur_page[cpu];
	int __arena *cur_offset = &page_frag_cur_offset[cpu];
	int offset;

	size = round_up(size, 8);
	if (size >= PAGE_SIZE - 8)
		return NULL;
	if (!page) {
refill:
		page = bpf_arena_alloc_pages(&arena, NULL, 1, NUMA_NO_NODE, 0);
		if (!page)
			return NULL;
		cast_kern(page);
		page_frag_cur_page[cpu] = page;
		*cur_offset = PAGE_SIZE - 8;
		obj_cnt = page + PAGE_SIZE - 8;
		*obj_cnt = 0;
	} else {
		cast_kern(page);
		obj_cnt = page + PAGE_SIZE - 8;
	}

	offset = *cur_offset - size;
	if (offset < 0)
		goto refill;

	(*obj_cnt)++;
	*cur_offset = offset;
	return page + offset;
}

static inline void pagefrag_free(void __arena *addr)
{
	__u64 __arena *obj_cnt;

	addr = (void __arena *)(((long)addr) & ~(PAGE_SIZE - 1));
	obj_cnt = addr + PAGE_SIZE - 8;
	if (--(*obj_cnt) == 0)
		bpf_arena_free_pages(&arena, addr, 1);
}
