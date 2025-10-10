package lib
import "base:intrinsics"
import "base:runtime"
import "core:fmt"

// init
/* TODO: implement get_thread_count() */
get_thread_count :: proc() -> int {
	return 8
}
global_allocator: HalfFitAllocator

when ODIN_OS == .Windows {
	ThreadInfo :: struct {
		id:     ThreadId,
		handle: ThreadHandle,
	}
	OsBarrier :: SYNCHRONIZATION_BARRIER
} else {
	//#assert(false)
}
ThreadContext :: struct #align (64) {
	all:          []ThreadContext,
	value:        uintptr,
	is_first:     uint,
	was_first:    bool,
	barrier_ptr:  ^OsBarrier,
	main:         proc(),
	thread_index: int,
}

@(private = "file")
_get_odin_context :: proc(arena: ^ArenaAllocator, thread_context: ^ThreadContext) -> (ctx: runtime.Context) {
	ctx.user_index = thread_context.thread_index
	ctx.user_ptr = thread_context
	ctx.assertion_failure_proc = runtime.default_assertion_failure_proc
	ctx.allocator = runtime.Allocator{half_fit_allocator_proc, &global_allocator}
	ctx.temp_allocator = arena_allocator(arena, page_reserve(VIRTUAL_MEMORY_TO_RESERVE))
	return
}
run_multithreaded :: proc(main: proc(), thread_count: int = 0) {
	thread_count := thread_count
	if thread_count == 0 {thread_count = get_thread_count()}

	arena: ArenaAllocator = ---
	init_page_fault_handler()
	shared_allocator := half_fit_allocator(&global_allocator, page_reserve(VIRTUAL_MEMORY_TO_RESERVE))
	context.allocator = shared_allocator
	shared_barrier: OsBarrier
	_create_os_barrier(&shared_barrier, thread_count)
	thread_contexts := make([]ThreadContext, thread_count)

	// init contexts
	context = _get_odin_context(&arena, &thread_contexts[0])
	for thread_index in 0 ..< thread_count {
		thread_contexts[thread_index] = ThreadContext {
			all          = thread_contexts,
			thread_index = thread_index,
			barrier_ptr  = &shared_barrier,
			main         = main,
		}
	}
	// launch threads
	for thread_index in 1 ..< thread_count {
		param := &thread_contexts[thread_index]
		stack_size := Size(0)
		thread_proc :: proc "system" (param: rawptr) -> (exit_code: u32) {
			thread_context := (^ThreadContext)(param)
			arena: ArenaAllocator = ---
			context = _get_odin_context(&arena, thread_context)
			// run
			thread_context.main()
			return
		}
		when ODIN_OS == .Windows {
			thread_handle := CreateThread(nil, stack_size, thread_proc, param, 0, nil)
			fmt.assertf(thread_handle != 0, "Failed to launch a thread")
		} else {
			assert(false)
		}
	}
	// run
	main()
}

// synchronization
Lock :: distinct bool

mfence :: #force_inline proc "contextless" () {
	intrinsics.atomic_thread_fence(.Seq_Cst)
}
@(require_results)
get_lock :: #force_inline proc "contextless" (lock: ^Lock) -> (acquired: bool) {
	old_value := intrinsics.atomic_exchange(lock, true)
	return old_value == false
}
wait_for_lock :: #force_inline proc "contextless" (lock: ^Lock) {
	for {
		old_value := intrinsics.atomic_exchange(lock, true)
		if intrinsics.expect(old_value == false, true) {return}
		intrinsics.cpu_relax()
	}
}
release_lock :: #force_inline proc "contextless" (lock: ^Lock) {
	intrinsics.atomic_store(lock, false)
}

@(private = "file")
_create_os_barrier :: proc(barrier: ^OsBarrier, thread_count: int) {
	thread_count_i32 := i32(downcast_u32(thread_count))
	when ODIN_OS == .Windows {
		InitializeSynchronizationBarrier(barrier, thread_count_i32)
	} else {
		assert(false)
	}
}
barrier :: proc() {
	thread_context := (^ThreadContext)(context.user_ptr)
	when ODIN_OS == .Windows {
		/* NOTE: spinlock 2000 times or sleep and wait for all threads to enter the barrier */
		EnterSynchronizationBarrier(thread_context.barrier_ptr, {.SYNCHRONIZATION_BARRIER_FLAGS_NO_DELETE})
	} else {
		assert()
	}
}
barrier_sync :: proc(value: ^$T) {
	thread_context := (^ThreadContext)(context.user_ptr)
	shared_thread_context := &thread_context.all[0]
	if thread_context.was_first {
		shared_thread_context.value = uintptr(value^)
		barrier()
	} else {
		barrier()
		value^ = T(shared_thread_context.value)
	}
	barrier()
}
sync_is_first :: proc() -> (is_first: bool) {
	thread_context := (^ThreadContext)(context.user_ptr)
	shared_thread_context := &thread_context.all[0]
	old_value := intrinsics.atomic_add(&shared_thread_context.is_first, 1)
	is_first = old_value % uint(len(thread_context.all)) == 0
	thread_context.was_first = is_first
	return
}
