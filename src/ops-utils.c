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

/*************************************************************************//**
 * @ingroup ops_utils
 * This module contains the DEFINES and functions that comprise the ops-utils
 * library.
 *
 * @file
 * Source file for ops-utils library.
 *
 ****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <errno.h>

#include "ops-utils.h"

/*********************************************************
 *                      PID Utility                      *
 *********************************************************/

int
ops_record_pid(const char *filename)
{
	FILE *pid_file;

	pid_file = fopen(filename, "w");
	if ((FILE *)NULL == pid_file) {
		return errno;
	}

	fprintf(pid_file, "%d\n", getpid());
	fclose(pid_file);

	return 0;

} /* ops_record_pid */

int
ops_read_pid(const char *filename)
{
	FILE *pid_file;
	int pid;

	pid_file = fopen(filename, "r");
	if ((FILE *)NULL == pid_file) {
		return -errno;
	}

	if (1 != fscanf(pid_file,"%d", &pid)) {
		pid = -EINVAL;
	}
	fclose(pid_file);

	return pid;

} /* ops_read_pid */

int
ops_read_pid_by_procname(const char *procname)
{
	char filename[80];

	snprintf(filename,sizeof(filename),"/var/run/%s.pid", procname);

	return ops_read_pid(filename);

} /* ops_read_pid_by_procname */


/*********************************************************
 *                       MATH                            *
 *********************************************************/

/*
 * ops_char_array_to_ulong_long
 *
 * Converts a generic array of binary values into unsigned long long.
 * This can be handy for incrementing MACs or WWNs.
 * Input is array with most significant byte first
 */
unsigned long long
ops_char_array_to_ulong_long(unsigned char *char_array, unsigned int length)
{
	unsigned long long   value = (unsigned long long)0;
	unsigned int    i;

	for ( i = 0; i < length; i++ ) {
		value = ( value << 8 ) + char_array[i];
	}

	return value;
} /* ops_char_array_to_ulong_long */

/*
 * ops_ulong_long_to_char_array
 *
 * Converts an unsigned long long into generic array of binary values.
 * Output is array with most significant byte first
 */
void
ops_ulong_long_to_char_array(unsigned long long value, unsigned int length,
		unsigned char *char_array)
{
	unsigned long long   temp = value;
	int     i;

	for ( i = length - 1; i >= 0; i-- ) {
		char_array[i] = temp & 0xff;
		temp >>= 8;
	}
} /* ops_ulong_long_to_char_array */

/*
 * ops_ether_ntoa - the pretty version of ether_ntoa
 *
 * converts an Ethernet address pointed to by addr to a string mac_a
 * with leading zeros. returns mac_a.
 *
 */
char *
ops_ether_ntoa(char *mac_a, const struct ether_addr *addr)
{
	(void)snprintf(mac_a, OPS_MAC_STR_SIZE, "%02x:%02x:%02x:%02x:%02x:%02x",
			addr->ether_addr_octet[0], addr->ether_addr_octet[1],
			addr->ether_addr_octet[2], addr->ether_addr_octet[3],
			addr->ether_addr_octet[4], addr->ether_addr_octet[5]);
	return mac_a;
} /* ops_ether_ntoa */

/*
 * ops_wwn_ntoa - World Wide Name version of ops_ether_ntoa
 *
 * converts a WWN address pointed to by wwn to a string wwn_a
 * with leading zeros. returns wwn_a
 */
char *
ops_wwn_ntoa(char *wwn_a, const char *wwn)
{
	(void)snprintf(wwn_a, OPS_WWN_STR_SIZE, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
			wwn[0], wwn[1], wwn[2], wwn[3], wwn[4], wwn[5], wwn[6], wwn[7]);
	return wwn_a;
} /* ops_wwn_ntoa */

/*
 * ops_ether_array_to_string
 *
 * converts an Ethernet address stored as a 6 byte binary array
 * into a printable mac address with leading zeros. returns mac_a.
 *
 */
char *
ops_ether_array_to_string(char *mac_a, const unsigned char *addr)
{
	(void)snprintf(mac_a, OPS_MAC_STR_SIZE, "%02x:%02x:%02x:%02x:%02x:%02x",
			addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	return mac_a;
} /* ops_ether_array_to_string */

/*
 * ops_ether_ulong_long_to_string
 *
 * converts an Ethernet address stored as a long long
 * into a printable mac address with leading zeros. returns mac_a.
 *
 */
char *
ops_ether_ulong_long_to_string(char *mac_a, const unsigned long long mac)
{
	unsigned char addr[6];
	unsigned long long max_mac = 0xffffffffffff;

	if (mac > max_mac) {
		return ( (char *) NULL);
	}
	ops_ulong_long_to_char_array(mac, ETH_ALEN, addr);

	(void)snprintf(mac_a, OPS_MAC_STR_SIZE,
		       "%02x:%02x:%02x:%02x:%02x:%02x",
		       addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

	return mac_a;
} /* ops_ether_ulong_long_to_string */

/*
 * Sorting function for generic elemnts
 * on success, returns sorted elemnts list.
 */
int
ops_sort(const struct shash *sh, void *ptr_func_sort,
         const struct shash_node ** sorted_list)

{
    int ret_val = 0;

    if (ptr_func_sort == NULL || sorted_list == NULL || shash_is_empty(sh)) {
        ret_val = 1;
    } else {
        struct shash_node *node;

        size_t i, n;

        n = shash_count(sh);
        i = 0;
        SHASH_FOR_EACH (node, sh) {
            sorted_list[i++] = node;
        }
        ovs_assert(i == n);

        qsort(sorted_list, n, sizeof *sorted_list, ptr_func_sort);
    }
    return ret_val;
}

void
rootdir_usage(void)
{
    printf(
           "\nRoot Dir options:\n"
           "  -install_path=PATH  path to installed files root dir\n"
           "  -data_path=PATH     path to daemon data files root dir\n"
           );
}
