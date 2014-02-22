/*
 * main.c - Main program routines
 *
 * Copyright (c) 2013   A. Dilly
 *
 * AirCat is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * AirCat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AirCat.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>

#include "config_file.h"
#include "httpd.h"
#include "airtunes.h"
#include "avahi.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef CONFIG_PATH
	#define CONFIG_PATH "/etc/aircat/"
#endif

#ifndef VERSION
	#define VERSION "1.0.0"
#endif

static char *config_file = NULL;	/* Alternative configuration file */
static int verbose = 0;			/* Verbosity */
static int stop_signal = 0;		/* Stop signal */

static void print_usage(const char *name)
{
	printf("Usage: %s [OPTIONS]\n"
		"\n"
		"Options:\n"
		"-c      --config=FILE        Use FILE as configuration file\n"
		"-h      --help               Print this usage and exit\n"
		"-v      --verbose            Active verbose output\n"
		"        --version            Print version and exit\n",
		 name);
}

static void print_version(void)
{
	printf("AirCat " VERSION "\n");
}

static void parse_opt(int argc, char * const argv[])
{
	int c;

	/* Get options */
	while(1)
	{
		int option_index = 0;
		static const char *short_options = "c:hv";
		static struct option long_options[] =
		{
			{"version",      no_argument,        0, 0},
			{"config",       required_argument,  0, 'c'},
			{"help",         no_argument,        0, 'h'},
			{"verbose",      no_argument,        0, 'v'},
			{0, 0, 0, 0}
		};

		/* Get next option */
		c = getopt_long (argc, argv, short_options, long_options, &option_index);
		if(c == EOF)
			break;

		/* Parse option */
		switch(c)
		{
			case 0:
				switch(option_index)
				{
					case 0:
						/* Version */
						print_version();
						exit(EXIT_SUCCESS);
						break;
				}
				break;
			case 'c':
				/* Config file */
				config_file = strdup(optarg);
				break;
			case 'v':
				/* Verbose */
				verbose = 1;
				break;
			case 'h':
				/* Help */
				print_usage(argv[0]);
				exit(EXIT_SUCCESS);
				break;
			default:
				print_usage(argv[0]);
				exit(EXIT_FAILURE);
		}
	}
}

void signal_handler(int signum)
{
	if(signum == SIGINT || signum == SIGTERM)
	{
		printf("Received Stop signal...\n");
		stop_signal = 1;
	}
}

int main(int argc, char* argv[])
{
	struct avahi_handle *avahi;
	struct airtunes_handle *airtunes;
	struct httpd_attr httpd_attr;
	struct httpd_handle *httpd;
	struct timeval timeout;
	fd_set fds;

	/* Default AirCat configuration: overwritten by config_load() */
	config_default();

	/* Parse options */
	parse_opt(argc, argv);

	/* Load configuration file */
	if(config_file == NULL)
		config_file = strdup(CONFIG_PATH "/aircat.conf");
	config_load(config_file);

	/* Setup signal handler */
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	/* Open Avahi Client */
	avahi_open(&avahi);

	/* Open Airtunes Server */
	airtunes_open(&airtunes, avahi);

	/* Start Airtunes Server */
	if(config.raop_enabled)
		airtunes_start(airtunes);

	/* Prepare attributes for HTTP Server */
	httpd_attr.config_filename = config_file;
	httpd_attr.airtunes = airtunes;

	/* Open HTTP Server */
	httpd_open(&httpd, &httpd_attr);

	/* Start HTTP Server */
	httpd_start(httpd);

	/* Wait an input on stdin (only for test purpose) */
	while(!stop_signal)
	{
		FD_ZERO(&fds);
		FD_SET(0, &fds); 
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		if(select(1, &fds, NULL, NULL, &timeout) < 0)
			break;

		if(FD_ISSET(0, &fds))
			break;

		/* Iterate Avahi client */
		avahi_loop(avahi, 10);
	}

	/* Stop HTTP Server */
	httpd_stop(httpd);

	/* Stop Airtunes Server */
	airtunes_stop(airtunes);

	/* Close HTTP Server */
	httpd_close(httpd);

	/* Close Airtunes Server */
	airtunes_close(airtunes);

	/* Close Avahi Client */
	avahi_close(avahi);

	/* Free config */
	if(config_file != NULL)
		free(config_file);
	config_free();

	return EXIT_SUCCESS;
}
