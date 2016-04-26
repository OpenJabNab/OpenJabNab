[![Build Status](https://travis-ci.org/OpenJabNab/OpenJabNab.svg?branch=master)](https://travis-ci.org/OpenJabNab/OpenJabNab)

OpenJabNab
==========
An open PHP/C++-based proxy framework for the Nabaztag/Tag™ electronic pet with jabber protocol.

Nabaztag is a trade mark of Violet. OpenJabNab is not owned by or affiliated with Violet.


I) Terms of use
---------------

This software is provided under a GPL license.
Read the COPYING file for details.

OpenJabNab uses Qt Open Source Edition, see http://trolltech.com/products/qt/licenses/licensing/opensource for the license

II) Features
------------

This initial version has no true feature.
Its main use is understanding the dialog between the Nabaztag/tag and the Violet's servers : every call can be logged in a log files or printed on the screen.

OpenJabNab is composed of 2 distinct parts :
* An Http-Wrapper written in PHP to redirect all Http calls to the server
* A server written in C++ to handle all http calls & xmpp communications

 
III) Requirements
-----------------

* A Nabaztag/tag
* An Apache server (use of RewriteEngine in .htaccess file) or Lighttpd server (use of mod_rewrite, see README.lighty)
* PHP scripting enabled at prefered Webserver
* Qt 4 correctly installed (with dev libraries)

IV) Installation
----------------

1. Drop the PHP files into a web server folder. This folder must the root of a domain or subdomain (e.g. my.domain.com)
2. Generate Makefiles with qmake -r, then make
3. Edit the openjabnab.ini file and move it to the bin folder (you have to change all "my.domain.com" strings)
4. Execute openjabnab builded in the bin folder
5. Configure the Nabaztag/tag to use 'my.domain.com/vl' in advanced settings
6. Restart the Nabaztag/tag

V) History
----------

* 2008-01-24 v0.01 Initial revision
* 2010-06-09 v0.10 Added some formating
* 2011-08-16 v0.11 Removed Nabaztag.com-URL (Site suspended) Added Lighty as supported Webserver
