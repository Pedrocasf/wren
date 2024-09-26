const z = @import("std").zig;
const std = @import("std");
const builtin = std.builtin;
const LazyPath = std.Build.LazyPath;
pub fn build(b: *std.Build) !void {
    var feature_set: std.Target.Cpu.Feature.Set = std.Target.Cpu.Feature.Set.empty;
    feature_set.addFeature(@intFromEnum(std.Target.mips.Feature.single_float));
    feature_set.addFeature(@intFromEnum(std.Target.mips.Feature.vfpu));
    //PSP-Specific Build Options
    // const target = z.CrossTarget{};
    // const target = b.standardTargetOptions(.{ .whitelist = &.{.{
    // }} });
    const target = b.resolveTargetQuery(.{
        .cpu_arch = .mipsel,
        .os_tag = .freestanding,
        .cpu_model = .{ .explicit = &std.Target.mips.cpu.mips2 },
        .cpu_features_add = feature_set,
    });

    //All of the release modes work
    //Debug Mode can cause issues with trap instructions - use ReleaseSafe for "Debug" builds
    const optimize = builtin.Mode.ReleaseFast;

    //Build from your main file!
    const wren = b.addStaticLibrary(.{
        .name = "wren",
        .target = target,
        .optimize = optimize,
        .strip = true, // disable as cannot be used with "link_emit_relocs = true"
        .link_libc = true,
        .single_threaded = true,
        .pic = true,
    });

    wren.link_eh_frame_hdr = true;
    wren.link_emit_relocs = false;
    wren.addCSourceFiles(.{
        .files = &.{
            "src/vm/wren_compiler.c",
            "src/vm/wren_core.c",
            "src/vm/wren_debug.c",
            "src/vm/wren_primitive.c",
            "src/vm/wren_utils.c",
            "src/vm/wren_value.c",
            "src/vm/wren_vm.c",
            "src/optional/wren_opt_meta.c",
            "src/optional/wren_opt_random.c",
        },
        .flags = &.{
            "-std=c99",
            "-DWREN_OPT_META=1",
            "-DWREN_OPT_RANDOM=1",
            "-DWREN_NAN_TAGGING=0",
            "-DWREN_COMPUTED_GOTO=1"
        }});
    wren.addIncludePath(.{.cwd_relative = "/usr/local/pspdev/psp/include/"});
    wren.addIncludePath(b.path("src/include/"));
    wren.addIncludePath(b.path("src/vm/"));
    wren.addIncludePath(b.path("src/optional/"));
    wren.linkLibC();
    b.installArtifact(wren);
}
