/*
 Copyright (C) 2015 Hewlett-Packard Development Company, L.P.
 All Rights Reserved.

    Licensed under the Apache License, Version 2.0 (the "License"); you may
    not use this file except in compliance with the License. You may obtain
    a copy of the License at

         http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
    WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
    License for the specific language governing permissions and limitations
    under the License.
*/

*/
//-----
// slog.h - VC Enet syslog support. This file contains definitions for
//	VC Enet system log support.
//
//	This support will be based on the capabilities of the full Linux
//	syslogd. It will always generate the syslog calls so the burden is
//	on the daemon to selectively support a "verbose" mode when significant
//	debug/informational output is generated. If all messages are always
//	sent to syslogd we may encounter some performance problems.
//
//	The syslogd capabilities we're taking advantage of are:
//		- conditional direction of messages to specific log files
//		  (including /dev/null) based on priority and facility.
//		- ability to change the configuration dynamically (thus
//		  dis/enabling a class of messages from a specific service
//		  on an as needed basis assuming that a Linux shell is
//		  available).
//
//	default configuration (syslog.conf):
//		- direct each message level to a separate log messsages file,
//		  i.e.,
//			 log emerg, alert and crit messages to the local
//			 emerg.messages file and to the remote host "vcmhost"
//			  *.=emerg;*.=alert;*.=crit     /var/log/emerg.messages
//			  *.=emerg;*.=alert;*.=crit     @vcmhost
//
//			 log err, warning and notice messages to the local
//			 notice.messages file
//			  *.=err;*.=warning;*.=notice   /var/log/err.messages
//
//			 log info and debug messages to the local
//			 info.messages file
//			  *.=info;*.=debug         /var/log/info.messages
//			  lacpd.info;lacpd.debug   /var/log/lacpd.info.messages
//			  lldpd.info;lldpd.debug   /var/log/lldpd.info.messages
//			  med.info;med.debug       /var/log/med.info.messages
//			  portd.info;portd.debug   /var/log/portd.info.messages
//			  vchcpd.info;vchcpd.debug /var/log/vchcpd.info.messages
//
//	syslog options use request:
//		LOG_CONS - write directly to system console if error sending
//				to logger
//		LOG_NDELAY - open the logger connection immediately
//		LOG_PERROR - print to stderr as well
//
//	Use example:
//		SLOG_INIT(ident)		void openlog(const char *ident,
//							     int option,
//							     int facility);
//		SLOG(priority, format...)	void syslog(int priority,
//							    const char *format,
//							    ...);
//	where-
//		priority = syslog priority value
//		ident = daemon identification string
//		version = daemon version string
//		format... = remaining args, as in printf(3)
//
//
//	Expected output example:
//		(SLOG() output in signal handler)
//		Jan  1 00:01:16 localhost daemon.warn portd: sig 15 caught
//
//		(SLOG() output after error return from select())
//		Jan  1 00:01:16 localhost daemon.warn portd: select failed with
//				Interrupted syst
//
//		(SLOG() output after detection of "terminate" request after
//			catching a kill signal)
//		Jan  1 00:01:16 localhost daemon.notice portd: terminate
//				requested
//
//		(SLOG_EXIT() output with zero exit code)
//		Jan  1 00:01:16 localhost daemon.notice portd: exiting
//
//		(slog_atexit() output)
//		Jan  1 00:01:16 localhost daemon.notice portd: Exiting: logging
//				stopped, program terminating
//
//-----
#ifndef _SLOG_H
#define _SLOG_H

#include <stdlib.h>
#include <syslog.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <systemd/sd-journal.h>


#define SLOG_MAKEPRI(pri)		((0x00000001) << (pri))
#define SLOG_MAKE_SERVICEPRI(pri)	((0x00000100) << (pri))

//----------------------slog daemon/library log level MASK vals-
#define SLOG_EMERG	(SLOG_MAKEPRI(LOG_EMERG))	// 0=system is unusable
							//   VC Enet rendered
							//   unusable, urgent
							//   admin action req'd
#define SLOG_ALERT	(SLOG_MAKEPRI(LOG_ALERT))	// 1=take action now
							//   admin action req'd
#define SLOG_CRIT	(SLOG_MAKEPRI(LOG_CRIT))	// 2=critical conditions
							//   req'd FW component
							//   no longer available
#define SLOG_ERR	(SLOG_MAKEPRI(LOG_ERR))		// 3=error conditions
							//   req'd FW component
							//   op failed
#define SLOG_WARNING	(SLOG_MAKEPRI(LOG_WARNING))	// 4=warning conditions
							//   req'd FW component
							//   op encountered an
							//   unexpected but
							//   survivable event
#define SLOG_NOTICE	(SLOG_MAKEPRI(LOG_NOTICE))	// 5=normal-significant
#define SLOG_INFO	(SLOG_MAKEPRI(LOG_INFO))	// 6=infomational
							//   typically enabled
							//   with -v (verbose)
#define SLOG_DEBUG	(SLOG_MAKEPRI(LOG_DEBUG))	// 7=debug messages
							//   typically enabled
							//   with -d (debug)

//----------------------slog daemon/library log level masks------
unsigned int slog_level;		// default=log notices & above

// log msg format strings
#define SLOG_START_BANNER	"Logging started, logging level mask=0x%x"
#define SLOG_STOP_BANNER	"Logging stopped, %s"

#define SLOG_USAGE		\
"       Logging enable mask format is ...\n"  				\
"          0x00000001-0x00000020 reserved for LOG_EMERG-LOG_NOTICE,\n"	\
"                                which are always enabled.\n"		\
"          0x00000040 enable LOG_INFO level messages.\n"		\
"          0x00000080 enable LOG_DEBUG level messages.\n"		\
"          0x00000100-0x80000000 enable locally defined facility levels.\n" \
"          NOTE: messages with locally defined facility levels always\n"\
"                result in a LOG_DEBUG message.\n"

//----------------------slog local definitions--------------------------
//#define SLOG_OPTIONS	(LOG_CONS | LOG_NDELAY | LOG_PERROR)
#define SLOG_OPTIONS	(LOG_CONS | LOG_NDELAY | LOG_PID )
				// LOG_CONS
				//	Write  directly  to  system  console
				//	if there is an error while sending to
				//	system logger.
				// LOG_NDELAY
				//	Open the connection immediately
				//	(normally,  the  connection  is opened
				//	when the first message is logged).
				// LOG_PERROR
				//	(Not in SUSv3.) Print to stderr as well.

#define SLOG_FACILITY	(LOG_DAEMON)

//----------------------slog main SLOG macro definitions----------------
#define SLOG_MAP_PRI(priority,log_pri)					\
do {									\
	log_pri = priority;						\
	if ( priority <= SLOG_DEBUG ) {					\
		if ( priority & SLOG_EMERG ) {				\
			log_pri = LOG_EMERG;				\
		} else if ( priority & SLOG_ALERT ) {			\
			log_pri = LOG_ALERT;				\
		} else if ( priority & SLOG_CRIT ) {			\
			log_pri = LOG_CRIT;				\
		} else if ( priority & SLOG_ERR ) {			\
			log_pri = LOG_ERR;				\
		} else if ( priority & SLOG_WARNING ) {			\
			log_pri = LOG_WARNING;				\
		} else if ( priority & SLOG_NOTICE ) {			\
			log_pri = LOG_NOTICE;				\
		} else if ( priority & SLOG_INFO ) {			\
			log_pri = LOG_INFO;				\
		} else if ( priority & SLOG_DEBUG ) {			\
			log_pri = LOG_DEBUG;				\
		}							\
	}								\
	else {								\
		log_pri = LOG_DEBUG;					\
	}								\
} while (0)

#define SLOG_STOP(message)						\
do {									\
	syslog(LOG_NOTICE, SLOG_STOP_BANNER, message);			\
	closelog();							\
} while (0)

static void slog_atexit() __attribute__ ((__unused__));

static void slog_atexit()
{
	SLOG_STOP("program terminating");
}

#define SLOG_INIT(ident) 						\
do {									\
	atexit(slog_atexit);						\
	openlog(ident, SLOG_OPTIONS, SLOG_FACILITY);			\
	syslog(LOG_NOTICE, SLOG_START_BANNER, slog_level);		\
} while (0)

#define SLOG(priority, format...)					\
do {									\
	unsigned int	__log_pri;					\
	int		__enabled=slog_level & priority;		\
	/* translate the SLOG priority to the LOG priority */		\
	SLOG_MAP_PRI(priority,__log_pri);				\
	/* always enable SLOG_NOTICE and more important messages */	\
	if ( priority <= SLOG_NOTICE ) {				\
		__enabled = 1;						\
	}								\
	if ( __enabled ) {						\
		sd_journal_print(__log_pri, format);			\
	}								\
} while (0)

#define VLOG(priority, format, ...)					\
do {									\
	unsigned int	__log_pri;					\
	int		__enabled=slog_level & priority;		\
	/* translate the SLOG priority to the LOG priority */		\
	SLOG_MAP_PRI(priority,__log_pri);				\
	/* always enable SLOG_NOTICE and more important messages */	\
	if ( priority <= SLOG_NOTICE ) {				\
		__enabled = 1;						\
	}								\
	if ( __enabled ) {						\
		sd_journal_send("PRIORITY=%i",__log_pri,		\
				"MESSAGE=" format, ##__VA_ARGS__);		\
	}								\
} while (0)

#define SLOG_EXIT(exit_code, format...)					\
do {									\
	if (exit_code) {						\
		SLOG(SLOG_CRIT, format);				\
	}								\
	exit(exit_code);						\
} while (0)

//-------------- Basic slog suppression support -------------------
// The following macros provide a primitive mechanism to define
// multiple logging contexts and the ability to enable/disable
// logging for individual contexts.
// The logging context data is maintained by the application
// using context data type slog_context_info_t.
// Each application can define its own set of logging contexts by
// defining bit fields in slog_context_info_t. The application will
// then use the SLOG_ONCE* macros to provide the context for SLOG
// macros.
//

// Handle for slog limit context information. Currently it is defined
// as a bitmap of SLOG contexts.
// Each bit, when set indicates to the SLOG_ONCE macro not to log
// for that particular context.
//
typedef unsigned int slog_context_info_t;

// Macro to explicitly enable SLOG suppression for a given context.
#define SLOG_ONCE_SET(ctx, ctx_info)					\
do {									\
 	slog_context_info_t	*_p_ctx_info = (ctx_info);		\
	if (_p_ctx_info) {						\
		*_p_ctx_info |= ctx;					\
	}								\
} while (0)

// Macro to explicitly clear SLOG suppression status for a given context.
#define SLOG_ONCE_CLEAR(ctx, ctx_info)					\
do {									\
 	slog_context_info_t	*_p_ctx_info = (ctx_info);		\
	if (_p_ctx_info) {						\
		*_p_ctx_info &= ~ctx;					\
	}								\
} while (0)

// Variation of SLOG macro with a wrapper to check whether logging
// is enabled for a given context.
// If logging is allowed, the SLOG macro is invoked and the context
// is marked to not allow any more SLOG_ONCE logs until the context's
// bit is cleared.
#define SLOG_ONCE(ctx, ctx_info, priority, format...)			\
do {									\
 	slog_context_info_t	*_p_ctx_info = (ctx_info);		\
	if (_p_ctx_info && !(ctx & *_p_ctx_info)) {			\
		SLOG(priority, format);					\
		*_p_ctx_info |= ctx;					\
	}								\
} while (0)


///////////// Support Dump Macros ///////////////////////

FILE *slog_sdump_fd;

#define SDUMP_PATH_LEN	256
#define SDUMP_DIR	"/run/sdump/"

#define SDUMP_OPEN(rc, name)						\
do {								  	\
	char path[SDUMP_PATH_LEN] = SDUMP_DIR;				\
									\
	mkdir(SDUMP_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);	\
	strncat(path, name, SDUMP_PATH_LEN-strlen(path)-1);		\
	path[SDUMP_PATH_LEN-1] = '\0';					\
	slog_sdump_fd = fopen(path, "w+"); 				\
	if ((FILE *) NULL == slog_sdump_fd) { 				\
		SLOG(SLOG_ERR,						\
		"fopen failed for %s, errno=%d", path, errno);		\
		rc = false;						\
	} else {							\
		setbuf(slog_sdump_fd, (char *) NULL);			\
		rc = true;						\
	}								\
} while (0)

#define SDUMP(format...) fprintf(slog_sdump_fd, format);		\

#define SDUMP_CLOSE() fclose(slog_sdump_fd);				\

#endif  // _SLOG_H
