// Build script to compile and generate BPF skeleton and interface for scxcash.
fn main() {
    scx_utils::BpfBuilder::new()
        .unwrap()
        .enable_intf("src/bpf/intf.h", "bpf_intf.rs")
        .enable_skel("src/bpf/soft_dirty.bpf.c", "bpf")
        .enable_skel("src/bpf/perf_sample.bpf.c", "perf_bpf")
        .compile_link_gen()
        .unwrap();
}
