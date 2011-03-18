
AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = testft testdf testsl testsync

testft_SOURCES = TestFileTree.cpp
testft_CPPFLAGS =-I$(srcdir)/../lib
testft_LDADD = ../lib/libgsync.la

testdf_SOURCES = TestFileDiff.cpp
testdf_CPPFLAGS =-I$(srcdir)/../lib
testdf_LDADD = ../lib/libgsync.la

testsl_SOURCES = TestSerilize.cpp
testsl_CPPFLAGS =-I$(srcdir)/../lib
testsl_LDADD = ../lib/libgsync.la

testsync_SOURCES = TestSync.cpp
testsync_CPPFLAGS =-I$(srcdir)/../lib
testsync_LDADD = ../lib/libgsync.la