PROJECT(ppr C)

SET(CMAKE_C_FLAGS "-D_REENTRANT")

ADD_EXECUTABLE(ppr_mypipe ppr_mypipe.c)
TARGET_LINK_LIBRARIES(ppr_mypipe pthread)
ADD_EXECUTABLE(ppr_pipe ppr_pipe.c)
ADD_EXECUTABLE(ppr_mq ppr_mq.c)
ADD_EXECUTABLE(ppr_shmem ppr_shmem.c)
