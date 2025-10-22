package lib
import "core:fmt"

IoringError :: enum {
	None,
	IoCanceled,
	ConnectionClosedByOtherParty,
}
when ODIN_OS == .Windows {
	Ioring :: IocpHandle
	IoringTimer :: TimerHandle
	IoringEvent :: struct {
		bytes:          u32,
		error:          IoringError,
		/* NOTE: `^OVERLAPPED` */
		user_data:      rawptr `fmt:"p"`,
		completion_key: uintptr,
	}
} else when ODIN_OS == .Linux {
	Ioring :: EpollHandle
	IoringTimer :: TimerHandle
	IoringEvent :: struct {
		bytes:     u32,
		error:     IoringError,
		user_data: rawptr,
	}
} else {
	#assert(false)
}

/* NOTE: different platforms have different io handling:
	name         | no syscalls | nonblocking | polling | wait on multiple objects | user_ptr
	linux ioring | If enabled  | Yes?        | Yes     | Yes                      | Yes
	windows IOCP | No          | No          | Yes     | Yes                      | Yes
	linux epoll  | No          | Yes?        | Yes     | Yes                      | Yes
*/
ioring_create :: proc() -> (ioring: Ioring) {
	when ODIN_OS == .Windows {
		/* NOTE: allow up to `logical_cores` threads */
		ioring = CreateIoCompletionPort(INVALID_HANDLE, 0, 0, 0)
		fmt.assertf(ioring != 0, "Failed to create ioring")
	} else when ODIN_OS == .Linux {
		ioring = epoll_create1({})
		fmt.assertf(ioring >= 0, "Failed to create ioring, err: %v", ioring)
	} else {
		assert(false)
	}
	return
}
ioring_set_timer_async :: proc(
	ioring: Ioring,
	timer: ^IoringTimer,
	ms: int,
	user_data: rawptr,
	on_timeout: proc "system" (user_data: rawptr, TimerOrWaitFired: b32) = nil,
) {
	ioring_cancel_timer(ioring, timer)
	when ODIN_OS == .Windows {
		ms_u32 := downcast_u32(ms)
		/* NOTE: this will set a timer on a system-created threadpool, but there's no easy way to just send a timer to an IOCP on windows... */
		ok := CreateTimerQueueTimer(timer, 0, on_timeout, user_data, ms_u32, 0, {.WT_EXECUTEONLYONCE})
		fmt.assertf(bool(ok), "Failed to create a timer")
	} else when ODIN_OS == .Linux {
		timer^ = timerfd_create(.CLOCK_MONOTONIC, {})
		timer_options := TimerOptions64 {
			it_value = {tv_sec = 1},
		}
		timerfd_settime_64b(timer^, {}, &timer_options)
		fmt.assertf(timer^ != TimerHandle(INVALID_HANDLE), "Failed to create a timer")
	} else {
		assert(false)
	}
}
ioring_cancel_timer :: proc "system" (ioring: Ioring, timer: ^IoringTimer) {
	timer_handle := timer^
	if timer_handle == TimerHandle(INVALID_HANDLE) {
		return /* NOTE: windows crashes your program if you don't do this... */
	}
	when ODIN_OS == .Windows {
		DeleteTimerQueueTimer(0, timer_handle)
	} else when ODIN_OS == .Linux {
		close(FileHandle(timer_handle))
	} else {
		assert(false)
	}
	timer^ = IoringTimer(INVALID_HANDLE)
}
ioring_wait_for_next_event :: proc(ioring: Ioring, event: ^IoringEvent) {
	when ODIN_OS == .Windows {
		ok := GetQueuedCompletionStatus(ioring, &event.bytes, &event.completion_key, (^^OVERLAPPED)(&event.user_data))
		event.error = .None
		if !ok {
			err := GetLastError()
			#partial switch err {
			case .ERROR_OPERATION_ABORTED:
				event.error = .IoCanceled
			case .ERROR_CONNECTION_ABORTED:
				event.error = .ConnectionClosedByOtherParty
			case:
				fmt.assertf(false, "Failed to wait for the next ioring event, err: %v", err)
			}
		}
	} else when ODIN_OS == .Linux {
		epoll_event: EpollEvent
		event_count := epoll_wait(ioring, &epoll_event, 1)
		fmt.assertf(event_count == 1, "Failed to wait for the next ioring event, err: %v", Errno(event_count))
		event.user_data = epoll_event.user_data.rawptr
		event.bytes = 0
	} else {
		assert(false)
	}
	return
}
