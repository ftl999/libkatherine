Katherine Control Library
=========================

The Katherine control library contains a working implementation of the UDP-based communication protocol.
It may be used to control and receive data from the Katherine readout.


## Build Notes

The library is froked from the libkatherine library of petrmanek and uses the same dependencies:

 - C11 standard library,
 - POSIX threads,
 - BSD socket interface.
 
 Since this fork aimes to guarantee full windows compatibility, for windows the POSIX threads are replaced by standard windows API mutexes.

 The library is developed for *nix systems and Windows 7


## Documentation

The contents of the library use in-code and Javadoc-style documentation.
Pre-built documentation may be found in the `docs/` directory. Upon changes, the Doxygen tool can recreate its contents.

High-level overview may be found in the Chapter 3 of the thesis.


## Copyright

(c) Petr Mánek 2018, All rights reserved.

Contents of this library are provided for use under the conditions of the MIT License (see `LICENSE`).
