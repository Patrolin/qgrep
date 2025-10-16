package lib
import "base:intrinsics"
import "core:fmt"
import "core:mem"

// constants
/* https://github.com/odin-lang/Odin/blob/master/src/build_settings.cpp#L1935 */
STACK_SIZE :: 1 << 20
/* NOTE: SSD block sizes are 512B or 4KiB */
SSD_BLOCK_SIZE :: 512
VIRTUAL_MEMORY_TO_RESERVE :: STACK_SIZE

PAGE_SIZE_EXPONENT :: 12
PAGE_SIZE :: 1 << PAGE_SIZE_EXPONENT
#assert(PAGE_SIZE == 4096)

HUGE_PAGE_SIZE_EXPONENT :: 21
HUGE_PAGE_SIZE :: 1 << HUGE_PAGE_SIZE_EXPONENT
#assert(HUGE_PAGE_SIZE == 2_097_152)

/* NOTE: multiple threads reading from the same cache line is fine, but writing can lead to false sharing */
CACHE_LINE_SIZE_EXPONENT :: 6
CACHE_LINE_SIZE :: 1 << CACHE_LINE_SIZE_EXPONENT
#assert(CACHE_LINE_SIZE == 64)

// virtual
@(private)
DEBUG_VIRTUAL :: false

init_page_fault_handler :: proc "contextless" () {
	when ODIN_OS == .Windows {
		_page_fault_exception_handler :: proc "system" (exception: ^_EXCEPTION_POINTERS) -> ExceptionResult {
			exception_code := exception.ExceptionRecord.ExceptionCode
			when DEBUG_VIRTUAL {
				context = runtime.default_context()
				exception_params := exception.ExceptionRecord.ExceptionInformation[:exception.ExceptionRecord.NumberParameters]
				fmt.printfln("exception %v: %v", exception_code, exception_params)
			}
			if exception_code == .EXCEPTION_ACCESS_VIOLATION {
				ptr := exception.ExceptionRecord.ExceptionInformation[1]
				page_ptr := rawptr(uintptr(ptr) & ~uintptr(PAGE_SIZE - 1))
				commited_ptr := VirtualAlloc(page_ptr, PAGE_SIZE, {.MEM_COMMIT}, {.PAGE_READWRITE})
				return page_ptr != nil && commited_ptr != nil ? .EXCEPTION_CONTINUE_EXECUTION : .EXCEPTION_EXECUTE_HANDLER
			}
			return .EXCEPTION_EXECUTE_HANDLER
		}
		SetUnhandledExceptionFilter(_page_fault_exception_handler)
	} else when ODIN_OS == .Linux {
		/* NOTE: linux has a default page fault handler */
	} else {
		assert_contextless(false)
	}
}
page_reserve :: proc(size: Size) -> []byte {
	when ODIN_OS == .Windows {
		ptr := VirtualAlloc(nil, size, {.MEM_RESERVE}, {.PAGE_READWRITE})
		assert(ptr != nil)
	} else when ODIN_OS == .Linux {
		ptr := mmap(nil, size, {.PROT_READ, .PROT_WRITE}, {.MAP_PRIVATE, .MAP_ANONYMOUS})
		assert(ptr != max(uintptr))
	} else {
		assert(false)
	}
	return ([^]byte)(ptr)[:size]
}
page_free :: proc(ptr: rawptr) {
	when ODIN_OS == .Windows {
		assert(bool(VirtualFree(ptr, 0, {.MEM_RELEASE})))
	} else when ODIN_OS == .Linux {
		assert(munmap(ptr, 0) == 0)
	} else {
		assert(false)
	}
}

// copy
zero :: proc(buffer: []byte) {
	dest := uintptr(raw_data(buffer))
	dest_end := dest + uintptr(len(buffer))
	dest_end_64B := dest_end & 63
	zero_src := (#simd[64]byte)(0)
	for dest < dest_end_64B {
		(^#simd[64]byte)(dest)^ = zero_src
		dest += 64
	}
	for dest < dest_end {
		(^byte)(dest)^ = 0
		dest += 1
	}
}
copy :: proc(from, to: []byte) {
	src := uintptr(raw_data(from))
	dest := uintptr(raw_data(to))
	dest_end := dest + uintptr(min(len(from), len(to)))
	dest_end_64B := dest_end & 63
	for dest < dest_end_64B {
		(^#simd[64]byte)(dest)^ = (^#simd[64]byte)(src)^
		dest += 64
		src += 64
	}
	for dest < dest_end {
		(^byte)(dest)^ = (^byte)(src)^
		dest += 1
		src += 1
	}
}

// arena
ArenaAllocator :: struct {
	buffer_start: uintptr,
	buffer_end:   uintptr,
	next_ptr:     uintptr,
	/* NOTE: for asserting single-threaded */
	lock:         Lock,
}

arena_allocator :: proc(arena_allocator: ^ArenaAllocator, buffer: []byte) -> mem.Allocator {
	buffer_start := uintptr(raw_data(buffer))
	buffer_end := buffer_start + uintptr(len(buffer))
	arena_allocator^ = ArenaAllocator{buffer_start, buffer_end, buffer_start, false}
	return mem.Allocator{arena_allocator_proc, arena_allocator}
}
arena_allocator_proc :: proc(
	allocator: rawptr,
	mode: mem.Allocator_Mode,
	size, _alignment: int,
	old_ptr: rawptr,
	old_size: int,
	loc := #caller_location,
) -> (
	data: []byte,
	err: mem.Allocator_Error,
) {
	arena_allocator := (^ArenaAllocator)(allocator)
	// assert single threaded
	ok := get_lock(&arena_allocator.lock)
	assert(ok, loc = loc)
	defer release_lock(&arena_allocator.lock)

	#partial switch mode {
	case .Alloc, .Alloc_Non_Zeroed:
		// alloc
		data = _arena_alloc(arena_allocator, size)
		if intrinsics.expect(arena_allocator.next_ptr > arena_allocator.buffer_end, false) {
			err = .Out_Of_Memory
			break
		}
		// zero
		if mode == .Alloc {zero(data)}
	case .Resize, .Resize_Non_Zeroed:
		new_section_ptr := uintptr(old_ptr) + uintptr(old_size)
		if new_section_ptr == arena_allocator.next_ptr {
			// resize in place
			data = ([^]byte)(old_ptr)[:size]
			arena_allocator.next_ptr = uintptr(old_ptr) + uintptr(size)
			if intrinsics.expect(arena_allocator.next_ptr > arena_allocator.buffer_end, false) {
				err = .Out_Of_Memory
				break
			}
			if intrinsics.expect(size > old_size, true) {
				new_section := ([^]byte)(new_section_ptr)[:size - old_size]
				if mode == .Resize {zero(new_section)}
			}
		} else {
			// alloc
			data = _arena_alloc(arena_allocator, size)
			if intrinsics.expect(arena_allocator.next_ptr > arena_allocator.buffer_end, false) {
				err = .Out_Of_Memory
				break
			}
			// copy
			old_data := ([^]byte)(old_ptr)[:old_size]
			if mode == .Resize {zero(data)}
			copy(old_data, data)
		}
	case .Free_All:
		arena_allocator.next_ptr = arena_allocator.buffer_start
	}
	return
}
@(private)
_arena_alloc :: proc(arena_allocator: ^ArenaAllocator, size: int) -> []byte {
	ptr := arena_allocator.next_ptr
	// NOTE: align forward to 64B, so we can do faster simd ops
	ARENA_ALIGN :: 64
	remainder := ptr & (ARENA_ALIGN - 1)
	align_offset: uintptr
	if remainder != 0 {align_offset = ARENA_ALIGN - remainder}
	ptr += align_offset
	// update allocator
	arena_allocator.next_ptr = ptr + uintptr(size)
	return ([^]byte)(ptr)[:size]
}
