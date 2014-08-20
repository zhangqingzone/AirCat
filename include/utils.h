/*
 * utils.h - Some common function
 *
 * Copyright (c) 2014   A. Dilly
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

#ifndef _UTILS_H
#define _UTILS_H

/* Base64 functions */
char *base64_encode(const char *buffer, int length);
int base64_decode(char *buffer);

/* URL parser */
enum { URL_HTTP, URL_HTTPS };
int parse_url(const char *url, int *protocol, char **hostname,
	      unsigned int *port, char **username, char **password,
	      char **resource);

/* Random string generator */
char *random_string(int size);

/* Custom dirent structure */
struct _dirent {
	ino_t inode;
	mode_t mode;
	off_t size;
	time_t atime;
	time_t mtime; 
	time_t ctime;
	char name[256];
};

/* Custom alphasort function */
int _alphasort(const struct _dirent **a, const struct _dirent **b);

/* Custom alphasort function: folder are first in list */
int _alphasort_first(const struct _dirent **a, const struct _dirent **b);

/* Custom scandir function */
int _scandir(const char *path, struct _dirent ***list,
	     int (*selector) (const struct _dirent *),
	     int (*compar)(const struct _dirent **, const struct _dirent **));

#endif

