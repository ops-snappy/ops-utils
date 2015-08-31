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
 * @defgroup hc_utils Halon Core Utilities
 * This library provides common utility functions used by various Halon
 * processes.
 * @{
 *
 * @defgroup hc_utils_public Public Interface
 * Public API for hc_utils library.
 *
 * - Math: A set of functions that do conversion from one format to another.
 * - PID: A set of functions used to record and read dameon process id.
 *
 * @{
 *
 * @file
 * Header for hc_utils library.
 ***************************************************************************/

#ifndef __HC_UTILS_H_
#define __HC_UTILS_H_

#include <netinet/ether.h>

/******************* MATH *************************/

#define HC_MAC_STR_SIZE	18    /*!< Number of bytes in a MAC ADDR string */
#define HC_WWN_STR_SIZE	24    /*!< Number of bytes in a WWN string */


/************************************************************************//**
 * Converts an Ethernet address pointed to by addr to a string
 * mac_a with leading zeros.
 *
 * @param[out] mac_a :MAC address string
 * @param[in]  addr  :Ethernet address
 *
 * @return MAC address string
 ***************************************************************************/
extern char *hc_ether_ntoa(char *mac_a, const struct ether_addr *addr);

/************************************************************************//**
 * This function is a World Wide Name version of hc_ether_ntoa and converts
 * a WWN address pointed to by wwn to a string wwn_a with leading zeros.
 *
 * @param[out] wwn_a :WWN address string
 * @param[in]  wwn   :World Wide Name
 *
 * @return WWN address string
 ***************************************************************************/
extern char *hc_wwn_ntoa(char *wwn_a, const char *wwn);

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
    hc_char_array_to_ulong_long(unsigned char *char_array, unsigned int length);

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
extern void hc_ulong_long_to_char_array(unsigned long long value,
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
extern char *hc_ether_array_to_string(char *mac_a, const unsigned char *addr);

/************************************************************************//**
 * Converts an Ethernet address stored as a long long
 * into a printable mac address with leading zeros. returns mac_a.
 *
 * @param[out] mac_a  :MAC address string
 * @param[in]  mac    :ull MAC address
 *
 * @return MAC address string
 ***************************************************************************/
extern char *hc_ether_ulong_long_to_string(char *mac_a,
        const unsigned long long mac);


/******************* PID Utility *******************/

/************************************************************************//**
 * Writes the process id to a file.
 *
 * @param[in]  filename  :Filename to write pid
 *
 * @return zero for success, else errno on failure
 ***************************************************************************/
extern int hc_record_pid(const char *filename);

/************************************************************************//**
 * Reads a process id stored in a file.
 *
 * @param[in]  filename  :Filename with the pid
 *
 * @return PID if sucessful, else errno on failure
 ***************************************************************************/
extern int hc_read_pid(const char *filename);

/************************************************************************//**
 * Reads the PID from a file based on process name.
 *
 * @param[in]  procname  :Process name to use to locate the PID file
 *
 * @return PID if sucessful, else errno on failure
 ***************************************************************************/
extern int hc_read_pid_by_procname(const char *procname);

#endif /* __HC_UTILS_H_ */
/** @} end of group hc_utils_public */
/** @} end of group hc_utils */
