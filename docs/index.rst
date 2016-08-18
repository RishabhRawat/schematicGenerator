.. schematik documentation master file, created by
   sphinx-quickstart on Fri Jul 29 13:03:05 2016.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Schematik
=========


Schematik is a tool to automatically generate schematic diagrams from data, it provides a C++ API, to integrate with native codebase, and a JS API for web applications.

It was initially targeted for integration with YosysJS and work as its graphics frontend, though the API is generic enough for any application.

It depends on Snapsvg.io for web svg rendering and thus needs you to import/include snapsvg.


.. toctree::
    :hidden:
    :maxdepth: 2

    started
    jsapi
    cppapi
    customize