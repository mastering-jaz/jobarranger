## Process this file with automake to produce Makefile.in

bin_PROGRAMS = jobarg_exec

jobarg_exec_SOURCES = jobarg_exec.c

jobarg_exec_CFLAGS = -I@top_srcdir@/jainclude/

jobarg_exec_LDADD = \
	$(top_srcdir)/src/libs/zbxcomms/libzbxcomms.a \
	$(top_srcdir)/src/libs/zbxcommon/libzbxcommon.a \
	$(top_srcdir)/src/libs/zbxlog/libzbxlog.a \
	$(top_srcdir)/src/libs/zbxsys/libzbxsys.a \
	$(top_srcdir)/src/libs/zbxcrypto/libzbxcrypto.a \
	$(top_srcdir)/src/libs/zbxconf/libzbxconf.a \
	$(top_srcdir)/src/libs/zbxjson/libzbxjson.a
