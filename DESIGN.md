High-level design of ops-utils
==============================
ops-utils repo containts the source to create the opsutils library. opsutils library is a small library of functions that are commonly used, such as converting MAC addresses from strings to ulong long.

Reponsibilities
---------------
Any function that might be a commonly used function that is not already provided in another library (Linux provided, OVS provided, etc), may be included in this library.

Design choices
--------------
This library was created to collect together commonly used functions that are not provided in other libraries.

Relationships to external OpenSwitch entities
---------------------------------------------

OVSDB-Schema
------------

Internal structure
------------------

References
----------
