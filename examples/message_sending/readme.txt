======
README
======

This directory contains examples of use of the CppFCPLib. 

List of Files
=============

blocking_tcp_echo_server.cpp
----------------------------

Server listens on localhost on the specified port. Everything received
to that port is printed out to stdout. Used to test message creation
and message sending from the library.

Usage::

  ./blocking_tcp_echo_server <port>

send_message.cpp
----------------

Sending a data message to a server. Currently used only for testing.