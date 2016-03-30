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

/************************************************************************//**
 * @defgroup ops_utils Core Utilities
 * This library provides common utility functions used by various OpenSwitch
 * processes.
 * @{
 *
 * @defgroup ops_utils_public Public Interface
 * Public API for ops_utils library.
 *
 * - Math: A set of functions that do conversion from one format to another.
 * - PID: A set of functions used to record and read dameon process id.
 *
 * @{
 *
 * @file
 * Header for ops_utils library.
 ***************************************************************************/

#ifndef __OPS_UTILS_H_
#define __OPS_UTILS_H_

#include <string.h>
#include <netinet/ether.h>
#include "shash.h"

/******************* MATH *************************/

#define OPS_MAC_STR_SIZE	18    /*!< Number of bytes in a MAC ADDR string */
#define OPS_WWN_STR_SIZE	24    /*!< Number of bytes in a WWN string */

/****** Root directory of well-known files ********/
char *g_install_rootdir;
char *g_data_rootdir;

#define ROOTDIR_GSTRS                           \
  char *g_install_rootdir = "";                 \
  char *g_data_rootdir = "";

#define ROOTDIR_OPTION_ENUMS                    \
        OPT_INSTALL_PATH,                       \
        OPT_DATA_PATH

#define ROOTDIR_LONG_OPTIONS                                            \
        {"install_path",  required_argument, NULL, OPT_INSTALL_PATH},   \
        {"data_path",     required_argument, NULL, OPT_DATA_PATH}

#define ROOTDIR_OPTION_HANDLERS                 \
        case OPT_INSTALL_PATH:                  \
            g_install_rootdir = strdup(optarg); \
            break;                              \
        case OPT_DATA_PATH:                     \
            g_data_rootdir = strdup(optarg);    \
            break;

void rootdir_usage(void);


/************************************************************************//**
 * Converts an Ethernet address pointed to by addr to a string
 * mac_a with leading zeros.
 *
 * @param[out] mac_a :MAC address string
 * @param[in]  addr  :Ethernet address
 *
 * @return MAC address string
 ***************************************************************************/
extern char *ops_ether_ntoa(char *mac_a, const struct ether_addr *addr);

/************************************************************************//**
 * This function is a World Wide Name version of ops_ether_ntoa and converts
 * a WWN address pointed to by wwn to a string wwn_a with leading zeros.
 *
 * @param[out] wwn_a :WWN address string
 * @param[in]  wwn   :World Wide Name
 *
 * @return WWN address string
 ***************************************************************************/
extern char *ops_wwn_ntoa(char *wwn_a, const char *wwn);

/************************************************************************//**
 * Converts a generic array of binary octets into
 * unsigned long long. This can be handy for incrementing MACs or WWNs.
 *
 * @param[in]  char_array :input array with most significant byte first
 * @param[in]  length     :length of input string
 *
 * @return ull binary value of input string
 ***************************************************************************/
extern unsigned long long
    ops_char_array_to_ulong_long(unsigned char *char_array, unsigned int length);

/************************************************************************//**
 * Converts an unsigned long long into generic array of
 * binary octets. Output is array with most significant byte first
 *
 * @param[in]  value      :ull binary value
 * @param[in]  length     :number of bytes to process
 * @param[out] char_array :char array with converted bytes
 *
 * @return void
 ***************************************************************************/
extern void ops_ulong_long_to_char_array(unsigned long long value,
        unsigned int length, unsigned char *char_array);

/************************************************************************//**
 * Converts an Ethernet address stored as a 6 byte binary array
 * into a printable mac address with leading zeros.
 *
 * @param[out] mac_a    :MAC address string
 * @param[in]  addr     :6 byte binary array with MAC address
 *
 * @return MAC address string
 ***************************************************************************/
extern char *ops_ether_array_to_string(char *mac_a, const unsigned char *addr);

/************************************************************************//**
 * Converts an Ethernet address stored as a long long
 * into a printable mac address with leading zeros. returns mac_a.
 *
 * @param[out] mac_a  :MAC address string
 * @param[in]  mac    :ull MAC address
 *
 * @return MAC address string
 ***************************************************************************/
extern char *ops_ether_ulong_long_to_string(char *mac_a,
        const unsigned long long mac);


/******************* PID Utility *******************/

/************************************************************************//**
 * Writes the process id to a file.
 *
 * @param[in]  filename  :Filename to write pid
 *
 * @return zero for success, else errno on failure
 ***************************************************************************/
extern int ops_record_pid(const char *filename);

/************************************************************************//**
 * Reads a process id stored in a file.
 *
 * @param[in]  filename  :Filename with the pid
 *
 * @return PID if sucessful, else errno on failure
 ***************************************************************************/
extern int ops_read_pid(const char *filename);

/************************************************************************//**
 * Reads the PID from a file based on process name.
 *
 * @param[in]  procname  :Process name to use to locate the PID file
 *
 * @return PID if sucessful, else errno on failure
 ***************************************************************************/
extern int ops_read_pid_by_procname(const char *procname);


/******************* Sort Utility *******************/

/************************************************************************//**
 * The function is a generic quick sort algorithm
 *
 * @param[out] sorted_list  : shash node containing sorted elements. The
 *                            caller must make sure to allocate/de-allocate
 *                            memory for sorted_list
 * @param[in]  sh           : shash node containing unsorted elements
 * @param[in]  ptr_fuc_sort : pointer to function which contains the
 *                            compartor logic for sorting algorithm
 *
 * @return zero for success, else non-zero on failure
 ***************************************************************************/
int
ops_sort(const struct shash *sh, void *ptr_func_sort,
         const struct shash_node ** sorted_list);

#endif /* __OPS_UTILS_H_ */
/** @} end of group ops_utils_public */
/** @} end of group ops_utils */
