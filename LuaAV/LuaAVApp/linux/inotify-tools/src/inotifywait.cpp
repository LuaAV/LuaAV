// kate: replace-tabs off; space-indent off;

#include "../config.h"
#include "common.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <regex.h>

#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>

#include <getopt.h>

extern char *optarg;
extern int optind, opterr, optopt;

#define EXIT_OK 0
#define EXIT_ERROR 1
#define EXIT_TIMEOUT 2

#define MAX_STRLEN 4096
#define EXCLUDE_CHUNK 1024

#define nasprintf(...) niceassert( -1 != asprintf(__VA_ARGS__), "out of memory")

// METHODS
bool parse_opts(
  int * argc,
  char *** argv,
  int * events,
  bool * monitor,
  int * quiet,
  unsigned long int * timeout,
  int * recursive,
  bool * csv,
  char ** format,
  char ** timefmt,
  char ** fromfile,
  char ** regex,
  char ** iregex
);

void print_help();



char * csv_escape( char * string ) {
	static char csv[MAX_STRLEN+1];
	static unsigned int i, ind;

	if ( strlen(string) > MAX_STRLEN ) {
		return NULL;
	}

	if ( strlen(string) == 0 ) {
		return NULL;
	}

	// May not need escaping
	if ( !strchr(string, '"') && !strchr(string, ',') && !strchr(string, '\n')
	     && string[0] != ' ' && string[strlen(string)-1] != ' ' ) {
		strcpy( csv, string );
		return csv;
	}

	// OK, so now we _do_ need escaping.
	csv[0] = '"';
	ind = 1;
	for ( i = 0; i < strlen(string); ++i ) {
		if ( string[i] == '"' ) {
			csv[ind++] = '"';
		}
		csv[ind++] = string[i];
	}
	csv[ind++] = '"';
	csv[ind] = '\0';

	return csv;
}



void validate_format( char * fmt ) {
	// Make a fake event
	struct inotify_event * event =
	   (struct inotify_event *)malloc(sizeof(struct inotify_event) + 4);
	if ( !event ) {
		fprintf( stderr, "Seem to be out of memory... yikes!\n" );
		exit(EXIT_ERROR);
	}
	event->wd = 0;
	event->mask = IN_ALL_EVENTS;
	event->len = 3;
	strcpy( event->name, "foo" );
	FILE * devnull = fopen( "/dev/null", "a" );
	if ( !devnull ) {
		fprintf( stderr, "Couldn't open /dev/null: %s\n", strerror(errno) );
		free( event );
		return;
	}
	if ( -1 == inotifytools_fprintf( devnull, event, fmt ) ) {
		fprintf( stderr, "Something is wrong with your format string.\n" );
		exit(EXIT_ERROR);
	}
	free( event );
	fclose(devnull);
}


void output_event_csv( struct inotify_event * event ) {
	printf("%s,", csv_escape( inotifytools_filename_from_wd( event->wd )));
	printf("%s,", csv_escape( inotifytools_event_to_str( event->mask ) ) );
	if ( event->len > 0 )
		printf("%s", csv_escape( event->name ) );
	printf("\n");
}

/*
int main(int argc, char ** argv)
{
	int events = 0;
	bool monitor = false;
	int quiet = 0;
	unsigned long int timeout = 0;
	int recursive = 0;
	bool csv = false;
	char * format = NULL;
	char * timefmt = NULL;
	char * fromfile = NULL;
	char * regex = NULL;
	char * iregex = NULL;

	// Parse commandline options, aborting if something goes wrong
	if ( !parse_opts(&argc, &argv, &events, &monitor, &quiet, &timeout,
	                 &recursive, &csv, &format, &timefmt, &fromfile,
	                 &regex, &iregex) ) {
		return EXIT_ERROR;
	}

	if ( !inotifytools_initialize() ) {
		fprintf(stderr, "Couldn't initialize inotify.  Are you running Linux "
		                "2.6.13 or later, and was the\n"
		                "CONFIG_INOTIFY option enabled when your kernel was "
		                "compiled?  If so, \n"
		                "something mysterious has gone wrong.  Please e-mail "
		                PACKAGE_BUGREPORT "\n"
		                " and mention that you saw this message.\n");
		return EXIT_ERROR;
	}

	if ( timefmt ) inotifytools_set_printf_timefmt( timefmt );
	if (
		(regex && !inotifytools_ignore_events_by_regex(regex, REG_EXTENDED) ) ||
		(iregex && !inotifytools_ignore_events_by_regex(iregex, REG_EXTENDED|
		                                                        REG_ICASE))
	) {
		fprintf(stderr, "Error in `exclude' regular expression.\n");
		return EXIT_ERROR;
	}


	if ( format ) validate_format(format);

	// Attempt to watch file
	// If events is still 0, make it all events.
	if (events == 0)
		events = IN_ALL_EVENTS;

	FileList list = construct_path_list( argc, argv, fromfile );

	if (0 == list.watch_files[0]) {
		fprintf(stderr, "No files specified to watch!\n");
		return EXIT_ERROR;
	}

	if ( !quiet ) {
		fprintf( stderr, "Setting up watches.  " );
		if ( recursive ) {
			fprintf( stderr, "Beware: since -r was given, this may take a "
			         "while!" );
		}
		fprintf( stderr, "\n" );
	}

	// now watch files
	for ( int i = 0; list.watch_files[i]; ++i ) {
		char const *this_file = list.watch_files[i];
		if ( (recursive && !inotifytools_watch_recursively_with_exclude(
		                        this_file,
		                        events,
		                        list.exclude_files ))
		     || (!recursive && !inotifytools_watch_file( this_file, events )) ){
			if ( inotifytools_error() == ENOSPC ) {
				fprintf(stderr, "Failed to watch %s; upper limit on inotify "
				                "watches reached!\n", this_file );
				fprintf(stderr, "Please increase the amount of inotify watches "
				        "allowed per user via `/proc/sys/fs/inotify/"
				        "max_user_watches'.\n");
			}
			else {
				fprintf(stderr, "Couldn't watch %s: %s\n", this_file,
				        strerror( inotifytools_error() ) );
			}
			return EXIT_ERROR;
		}
	}

	if ( !quiet ) {
		fprintf( stderr, "Watches established.\n" );
	}

	// Now wait till we get event
	struct inotify_event * event;
	char * moved_from = 0;

	do {
		event = inotifytools_next_event( timeout );
		if ( !event ) {
			if ( !inotifytools_error() ) {
				return EXIT_TIMEOUT;
			}
			else {
				fprintf(stderr, "%s\n", strerror( inotifytools_error() ) );
				return EXIT_ERROR;
			}
		}

		if ( quiet < 2 ) {
			if ( csv ) {
				output_event_csv( event );
			}
			else if ( format ) {
				inotifytools_printf( event, format );
			}
			else {
				inotifytools_printf( event, "%w %,e %f\n" );
			}
		}

		// if we last had MOVED_FROM and don't currently have MOVED_TO,
		// moved_from file must have been moved outside of tree - so unwatch it.
		if ( moved_from && !(event->mask & IN_MOVED_TO) ) {
			if ( !inotifytools_remove_watch_by_filename( moved_from ) ) {
				fprintf( stderr, "Error removing watch on %s: %s\n",
				         moved_from, strerror(inotifytools_error()) );
			}
			free( moved_from );
			moved_from = 0;
		}

		if ( monitor && recursive ) {
			if ((event->mask & IN_CREATE) ||
			    (!moved_from && (event->mask & IN_MOVED_TO))) {
				// New file - if it is a directory, watch it
				static char * new_file;

				nasprintf( &new_file, "%s%s",
				           inotifytools_filename_from_wd( event->wd ),
				           event->name );

				if ( isdir(new_file) &&
				    !inotifytools_watch_recursively( new_file, events ) ) {
					fprintf( stderr, "Couldn't watch new directory %s: %s\n",
					         new_file, strerror( inotifytools_error() ) );
				}
				free( new_file );
			} // IN_CREATE
			else if (event->mask & IN_MOVED_FROM) {
				nasprintf( &moved_from, "%s%s/",
				           inotifytools_filename_from_wd( event->wd ),
				           event->name );
				// if not watched...
				if ( inotifytools_wd_from_filename(moved_from) == -1 ) {
					free( moved_from );
					moved_from = 0;
				}
			} // IN_MOVED_FROM
			else if (event->mask & IN_MOVED_TO) {
				if ( moved_from ) {
					static char * new_name;
					nasprintf( &new_name, "%s%s/",
					           inotifytools_filename_from_wd( event->wd ),
					           event->name );
					inotifytools_replace_filename( moved_from, new_name );
					free( moved_from );
					moved_from = 0;
				} // moved_from
			}
		}

		fflush( NULL );

	} while ( monitor );

	// If we weren't trying to listen for this event...
	if ( (events & event->mask) == 0 ) {
		// ...then most likely something bad happened, like IGNORE etc.
		return EXIT_ERROR;
	}

	return EXIT_OK;
}




bool parse_opts(
  int * argc,
  char *** argv,
  int * events,
  bool * monitor,
  int * quiet,
  unsigned long int * timeout,
  int * recursive,
  bool * csv,
  char ** format,
  char ** timefmt,
  char ** fromfile,
  char ** regex,
  char ** iregex
) {
	assert( argc ); assert( argv ); assert( events ); assert( monitor );
	assert( quiet ); assert( timeout ); assert( csv ); assert( format );
	assert( timefmt ); assert( fromfile ); assert( regex ); assert( iregex );

	// Short options
	char * opt_string = "mrhcqt:fe:";

	// Construct array
	struct option long_opts[14];

	// --help
	long_opts[0].name = "help";
	long_opts[0].has_arg = 0;
	long_opts[0].flag = NULL;
	long_opts[0].val = (int)'h';
	// --event
	long_opts[1].name = "event";
	long_opts[1].has_arg = 1;
	long_opts[1].flag = NULL;
	long_opts[1].val = (int)'e';
	int new_event;
	// --monitor
	long_opts[2].name = "monitor";
	long_opts[2].has_arg = 0;
	long_opts[2].flag = NULL;
	long_opts[2].val = (int)'m';
	// --quiet
	long_opts[3].name = "quiet";
	long_opts[3].has_arg = 0;
	long_opts[3].flag = NULL;
	long_opts[3].val = (int)'q';
	// --timeout
	long_opts[4].name = "timeout";
	long_opts[4].has_arg = 1;
	long_opts[4].flag = NULL;
	long_opts[4].val = (int)'t';
	char * timeout_end = NULL;
	// --filename
	long_opts[5].name = "filename";
	long_opts[5].has_arg = 0;
	long_opts[5].flag = NULL;
	long_opts[5].val = (int)'f';
	// --recursive
	long_opts[6].name = "recursive";
	long_opts[6].has_arg = 0;
	long_opts[6].flag = NULL;
	long_opts[6].val = (int)'r';
	// --csv
	long_opts[7].name = "csv";
	long_opts[7].has_arg = 0;
	long_opts[7].flag = NULL;
	long_opts[7].val = (int)'c';
	// --format
	long_opts[8].name = "format";
	long_opts[8].has_arg = 1;
	long_opts[8].flag = NULL;
	long_opts[8].val = (int)'n';
	// format with trailing newline
	static char * newlineformat;
	// --timefmt
	long_opts[9].name = "timefmt";
	long_opts[9].has_arg = 1;
	long_opts[9].flag = NULL;
	long_opts[9].val = (int)'i';
	// --fromfile
	long_opts[10].name = "fromfile";
	long_opts[10].has_arg = 1;
	long_opts[10].flag = NULL;
	long_opts[10].val = (int)'z';
	// --exclude
	long_opts[11].name = "exclude";
	long_opts[11].has_arg = 1;
	long_opts[11].flag = NULL;
	long_opts[11].val = (int)'a';
	// --excludei
	long_opts[12].name = "excludei";
	long_opts[12].has_arg = 1;
	long_opts[12].flag = NULL;
	long_opts[12].val = (int)'b';
	// Empty last element
	long_opts[13].name = 0;
	long_opts[13].has_arg = 0;
	long_opts[13].flag = 0;
	long_opts[13].val = 0;

	// Get first option
	char curr_opt = getopt_long(*argc, *argv, opt_string, long_opts, NULL);

	// While more options exist...
	while ( (curr_opt != '?') && (curr_opt != (char)-1) )
	{
		switch ( curr_opt )
		{
			// --help or -h
			case 'h':
				print_help();
				// Shouldn't process any further...
				return false;
				break;

			// --monitor or -m
			case 'm':
				*monitor = true;
				break;

			// --quiet or -q
			case 'q':
				(*quiet)++;
				break;

			// --recursive or -r
			case 'r':
				(*recursive)++;
				break;

			// --csv or -c
			case 'c':
				(*csv) = true;
				break;

			// --filename or -f
			case 'f':
				fprintf(stderr, "The '--filename' option no longer exists.  "
				                "The option it enabled in earlier\nversions of "
				                "inotifywait is now turned on by default.\n");
				return false;
				break;

			// --format
			case 'n':
				newlineformat = (char *)malloc(strlen(optarg)+2);
				strcpy( newlineformat, optarg );
				strcat( newlineformat, "\n" );
				(*format) = newlineformat;
				break;

			// --timefmt
			case 'i':
				(*timefmt) = optarg;
				break;

			// --exclude
			case 'a':
				(*regex) = optarg;
				break;

			// --excludei
			case 'b':
				(*iregex) = optarg;
				break;

			// --fromfile
			case 'z':
				if (*fromfile) {
					fprintf(stderr, "Multiple --fromfile options given.\n");
					return false;
				}
				(*fromfile) = optarg;
				break;

			// --timeout or -t
			case 't':
				*timeout = strtoul(optarg, &timeout_end, 10);
				if ( *timeout_end != '\0' )
				{
					fprintf(stderr, "'%s' is not a valid timeout value.\n"
					        "Please specify an integer of value 0 or "
					        "greater.\n",
					        optarg);
					return false;
				}
				break;

			// --event or -e
			case 'e':
				// Get event mask from event string
				new_event = inotifytools_str_to_event(optarg);

				// If optarg was invalid, abort
				if ( new_event == -1 )
				{
					fprintf(stderr, "'%s' is not a valid event!  Run with the "
					                "'--help' option to see a list of "
					                "events.\n", optarg);
					return false;
				}

				// Add the new event to the event mask
				(*events) = ( (*events) | new_event );

				break;


		}

		curr_opt = getopt_long(*argc, *argv, opt_string, long_opts, NULL);

	}

	if ( *monitor && *timeout != 0 ) {
		fprintf(stderr, "-m and -t cannot both be specified.\n");
		return false;
	}

	if ( *regex && *iregex ) {
		fprintf(stderr, "--exclude and --excludei cannot both be specified.\n");
		return false;
	}

	if ( *format && *csv ) {
		fprintf(stderr, "-c and --format cannot both be specified.\n");
		return false;
	}

	if ( !*format && *timefmt ) {
		fprintf(stderr, "--timefmt cannot be specified without --format.\n");
		return false;
	}

	if ( *format && strstr( *format, "%T" ) && !*timefmt ) {
		fprintf(stderr, "%%T is in --format string, but --timefmt was not "
		                "specified.\n");
		return false;
	}

	(*argc) -= optind;
	*argv = &(*argv)[optind];

	// If ? returned, invalid option
	return (curr_opt != '?');
}


void print_help()
{
	printf("inotifywait %s\n", PACKAGE_VERSION);
	printf("Wait for a particular event on a file or set of files.\n");
	printf("Usage: inotifywait [ options ] file1 [ file2 ] [ file3 ] "
	       "[ ... ]\n");
	printf("Options:\n");
	printf("\t-h|--help     \tShow this help text.\n");
	printf("\t@<file>       \tExclude the specified file from being "
	       "watched.\n");
	printf("\t--exclude <pattern>\n"
	       "\t              \tExclude all events on files matching the\n"
	       "\t              \textended regular expression <pattern>.\n");
	printf("\t--excludei <pattern>\n"
	       "\t              \tLike --exclude but case insensitive.\n");
	printf("\t-m|--monitor  \tKeep listening for events forever.  Without\n"
	       "\t              \tthis option, inotifywait will exit after one\n"
	       "\t              \tevent is received.\n");
	printf("\t-r|--recursive\tWatch directories recursively.\n");
	printf("\t--fromfile <file>\n"
	       "\t              \tRead files to watch from <file> or `-' for "
	       "stdin.\n");
	printf("\t-q|--quiet    \tPrint less (only print events).\n");
	printf("\t-qq           \tPrint nothing (not even events).\n");
	printf("\t--format <fmt>\tPrint using a specified printf-like format\n"
	       "\t              \tstring; read the man page for more details.\n");
	printf("\t--timefmt <fmt>\tstrftime-compatible format string for use with\n"
	       "\t              \t%%T in --format string.\n");
	printf("\t-c|--csv      \tPrint events in CSV format.\n");
	printf("\t-t|--timeout <seconds>\n"
	       "\t              \tWhen listening for a single event, time out "
	       "after\n"
	       "\t              \twaiting for an event for <seconds> seconds.\n"
	       "\t              \tIf <seconds> is 0, inotifywait will never time "
	       "out.\n");
	printf("\t-e|--event <event1> [ -e|--event <event2> ... ]\n"
	       "\t\tListen for specific event(s).  If omitted, all events are \n"
	       "\t\tlistened for.\n\n");
	printf("Exit status:\n");
	printf("\t%d  -  An event you asked to watch for was received.\n",
	       EXIT_OK );
	printf("\t%d  -  An event you did not ask to watch for was received\n",
	       EXIT_ERROR);
	printf("\t      (usually delete_self or unmount), or some error "
	       "occurred.\n");
	printf("\t%d  -  The --timeout option was given and no events occurred\n",
	       EXIT_TIMEOUT);
	printf("\t      in the specified interval of time.\n\n");
	printf("Events:\n");
	print_event_descriptions();
}
*/

