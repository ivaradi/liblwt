lib_LIBRARIES=liblwt.a

noinst_PROGRAMS=test

liblwt_a_SOURCES=\
	StackManager.cc		\
	Context.S		\
	Thread.cc		\
	EPoll.cc		\
	PolledFD.cc		\
	Socket.cc		\
	ThreadedSocket.cc	\
	Timer.cc		\
	Scheduler.cc		\
	IOServer.cc		\
	Dirent.cc		\
	Log.cc			\
	util.cc

test_SOURCES=test.cc

test_LDADD=liblwt.a

include_lwtdir=$(includedir)/lwt
include_lwt_HEADERS=\
	StackManager.h	\
	Context.h		\
	BlockedThread.h		\
	Thread.h		\
	EPoll.h			\
	PolledFD.h		\
	ThreadedFDMixin.h	\
	ThreadedFD.h		\
	Socket.h		\
	ThreadedSocket.h	\
	Timer.h			\
	Scheduler.h		\
	IOServer.h		\
	Dirent.h		\
	Log.h			\
	util.h
