cmake_minimum_required(VERSION 3.23)
project(linux_programming C)

set(CMAKE_C_STANDARD 11)

add_executable(linux_programming main.c file_io/lseek.c file_io/trunc.c file_io/stat.c file_io/unlink.c file_io/myls.c
        file_io/ls_recursive.c file_io/dup.c file_io/fcntl_dup.c process/fork.c process/loop_fork.c process/fork_shared.c
        process/fork_exec.c process/exec_ps.c process/zoom_test.c process/waitpid_test.c process/waitpid_while.c
        ipc/pipe/pipe.c ipc/pipe/ls_wc_l.c ipc/pipe/pipe_broth.c ipc/pipe/mkfifo.c ipc/mmap/mmap.c ipc/mmap/fork_mmap.c
        ipc/signal/kill.c ipc/signal/alarm.c ipc/signal/settimer_cycle.c ipc/signal/sigsfun.c ipc/signal/sigaction.c
        ipc/signal/catch_child.c thread/pthrd_creat.c thread/pthrd_loop.c thread/pthrd_join.c thread/pthrd_join2.c
        thread/pthrd_endof3.c thread/pthrd_detach.c sync/pthrd_shared.c sync/rwlock.c sync/condition_pc.c sync/semaphore_pc.c multiplex/select_test.c multiplex/poll_test.c)

#add_executable(fifo_w ipc/pipe/fifo_w.c)
#add_executable(fifo_r ipc/pipe/fifo_r.c)
#add_executable(mmap_w ipc/mmap/mmap_w.c)
#add_executable(mmap_r ipc/mmap/mmap_r.c)

include_directories(WRAP "wrap_socket")
add_executable(multiplex_select multiplex/select_test.c wrap_socket/wrap_socket.c wrap_socket/wrap_socket.h)
add_executable(multiplex_poll multiplex/poll_test.c wrap_socket/wrap_socket.c wrap_socket/wrap_socket.h)
add_executable(process_socket tcp_socket/process_socket.c wrap_socket/wrap_socket.c wrap_socket/wrap_socket.h)
add_executable(thread_socket tcp_socket/thread_socket.c wrap_socket/wrap_socket.c wrap_socket/wrap_socket.h)
add_executable(multiplex_epoll multiplex/epoll_test.c wrap_socket/wrap_socket.c wrap_socket/wrap_socket.h)

add_executable(server tcp_socket/server.c)
add_executable(epoll_reactor multiplex/epoll_reactor.c)
add_executable(thread_pool multiplex/thread_pool.c multiplex/thread_pool.h)
