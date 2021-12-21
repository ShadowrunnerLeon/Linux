cmake_minimum_required(VERSION 3.19.1)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)

add_executable(pthread_cancel pthread_cancel.c)
target_link_libraries(pthread_cancel ${CMAKE_THREAD_LIBS_INIT})

add_executable(pthread_cancel_asynchronous pthread_cancel_asynchronous.c)
target_link_libraries(pthread_cancel_asynchronous ${CMAKE_THREAD_LIBS_INIT})

add_executable(pthread_cleanup pthread_cleanup.c)
target_link_libraries(pthread_cleanup ${CMAKE_THREAD_LIBS_INIT})

add_executable(pthread_cond pthread_cond.c)
target_link_libraries(pthread_cond ${CMAKE_THREAD_LIBS_INIT})

add_executable(pthread_join pthread_join.c)
target_link_libraries(pthread_join ${CMAKE_THREAD_LIBS_INIT})

add_executable(pthread_main pthread_main.c)
target_link_libraries(pthread_main ${CMAKE_THREAD_LIBS_INIT})

add_executable(pthread_mutex pthread_mutex.c)
target_link_libraries(pthread_mutex ${CMAKE_THREAD_LIBS_INIT})

add_executable(pthread_once pthread_once.c)
target_link_libraries(pthread_once ${CMAKE_THREAD_LIBS_INIT})

add_executable(pthread_self pthread_self.c)
target_link_libraries(pthread_self ${CMAKE_THREAD_LIBS_INIT})

add_executable(pthread_sigchld pthread_sigchld.c)
target_link_libraries(pthread_sigchld ${CMAKE_THREAD_LIBS_INIT})

add_executable(pthread_signal pthread_signal.c)
target_link_libraries(pthread_signal ${CMAKE_THREAD_LIBS_INIT})

add_executable(pthread_strerror_dlt pthread_strerror_dlt.c)
target_link_libraries(pthread_strerror_dlt ${CMAKE_THREAD_LIBS_INIT})

add_executable(process_semaphore process_semaphore.c)
target_link_libraries(process_semaphore ${CMAKE_THREAD_LIBS_INIT})

add_executable(semaphore semaphore.c)
target_link_libraries(semaphore ${CMAKE_THREAD_LIBS_INIT})

add_executable(spinlock spinlock.c)
target_link_libraries(spinlock ${CMAKE_THREAD_LIBS_INIT})

add_executable(rwlock rwlock.c)
target_link_libraries(rwlock ${CMAKE_THREAD_LIBS_INIT})
