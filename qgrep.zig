const std = @import("std");
const fs = std.fs;

pub fn main() !void {
    // setup a million things
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    const arenaAllocator = arena.allocator();
    const argv = try std.process.argsAlloc(arenaAllocator);
    const stdout = std.io.getStdOut().writer();
    
    // parse arguments
    if (argv.len < 2) return error.NotEnoughArguments;
    const word = argv[1];
    
    // get cwd
    var buffer = [_]u8{undefined} ** 98302; // what the fuck?
    var cwdPath = try fs.realpath(".", &buffer);
    var cwdWithIterationFlag = try fs.openDirAbsolute(cwdPath, .{ .iterate = true, .access_sub_paths = true });
    
    // print lines matching word
    var walker = try cwdWithIterationFlag.walk(arenaAllocator);
    while (try walker.next()) |walkerEntry| {
        try stdout.print("{s}: {s}\n", .{walkerEntry.path, walkerEntry.kind});
        if (walkerEntry.kind != .File) continue;
        const dir = walkerEntry.dir;
        const path = walkerEntry.path;
        var absolutePath = try fs.realpath(".", &buffer);
        const file = dir.openFile(absolutePath, .{ .mode = .read_only }) catch |err| {
            try stdout.print("{s}\n", .{err});
            continue;
        };
        const data = file.readToEndAlloc(arenaAllocator, std.math.pow(u64, 2, 20)) catch continue;
        var lineIterator = std.mem.tokenize(u8, data, "\n");
        while (lineIterator.next()) |line| {
            var includesWord = std.mem.indexOf(u8, line, word) != null;
            if (includesWord) {
                try stdout.print("{s}: {s}\n", .{path, line});
            }
        }
    }
}
