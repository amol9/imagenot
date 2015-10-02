imagenot
========

An image deletion utility.

It is suitable for rapidly going through a folder with a large number of images one by one and deleting as needed. The deleted images are collected in a trash folder specified at the command line.



Code
====

The code is written in 3 different languages: python, java and c. The functionality and interface provided by code in each of these languages is exactly the same. 

I did this as an experiment to observe two things:

#. How to interface with gtk in different languages.

#. How long it'll take to develop program in each of the languages.

Conclusions:

#. Interfacing with gtk is fairly straight forward in all the languages.

#. As anybody can guess, the c program took most time. While java and python versions were programmed in apprx. 3-4 hours each, the c code took approximately double that time. The additional complexity was mostly due to the complicated memory management API for GObjects.


Build
=====

Build all language versions::

        make all

Build individual versions::

        make java
        make c


Usage
=====

Launch the application with command line arguments: source dir path, trash dir path

e.g. `> imagenot ~/Pictures/2015 ./trash`

Once the window shows, use left and right arrow keys to move back and forth through the images. Press "d" to delete the current image.

You can launch the different language versions using make::
        make run_py
        make run_java
        make run_c


Dependencies
============

* Python

  package: python-gi

* Java

  package: libjava-gnome-java

* C 

  package: libgtk

