/*
 Copyright (C) 2016 Hewlett-Packard Development Company, L.P.
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
 * @defgroup vrf_utils Core Utilities
 * This library provides common utility functions used by various OpenSwitch
 * processes.
 * @{
 *
 * @defgroup vrf_utils_public Public Interface
 * Public API for vrf_utils library.
 *
 * - Math: A set of functions that do conversion from one format to another.
 * - PID: A set of functions used to record and read dameon process id.
 *
 * @{
 *
 * @file
 * Header for vrf_utils library.
 ***************************************************************************/

#ifndef __VRF_UTILS_H_
#define __VRF_UTILS_H_

#include "vswitch-idl.h"

/************************************************************************//**
 * Reads the vrf row from a ovsdb based on vrf name.
 *
 * @param[in]  vrf_name  :VRF name to use to locate the record
 * @param[in]  idl       : idl reference to OVSDB
 *
 * @return row if sucessful, else NULL on failure
 ***************************************************************************/
extern const struct ovsrec_vrf *
vrf_lookup(const struct ovsdb_idl *idl, const char *vrf_name);

/************************************************************************//**
 * Returns the default vrf row from a ovsdb.
 *
 * @param[in]  idl       : idl reference to OVSDB
 *
 * @return row if sucessful, else NULL on failure
 ***************************************************************************/
extern const struct ovsrec_vrf *
get_default_vrf(const struct ovsdb_idl *idl);

#endif /* __VRF_UTILS_H_ */
/** @} end of group vrf_utils_public */
/** @} end of group vrf_utils */
