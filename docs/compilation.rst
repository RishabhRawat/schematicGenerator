Building Schematik
==================  

SchematiK currently supports C++ for native applications and JS for web applications.
 
#. `Building for C++`_
#. `Building for JS`_


Building for C++
################

Building for C++ is fairly straightforward. It needs a C++11 compliant compiler (I use g++) and CMake 2.8 or greater

First clone the source 

.. code-block:: bash

    git clone https://github.com/RishabhRawat/schematicGenerator.git

I strongly advise a out of source build though it should not matter either ways.

.. code-block:: bash

    cd schematicGenerator
    mkdir build && cd build
    cmake ..
    make

In this folder a libSchematiK-core.so will be generated, You can link against this library to use SchematiK in you native applications.

.. note:: you will need to include **"schematic.h"** present in the include directory to use the library.    



Building for JS
###############

.. important:: SchematiK uses emscripten to compile the C++ to Javascript, It is essential that you have emscripten installed and all the necessary variables exported. Visit `Emscripten Installation <https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html#installation-instructions>`_ for more details

After you have Emscripten installed you can build SchematiK using the following commands - 

.. code-block:: bash

    git clone https://github.com/RishabhRawat/schematicGenerator.git
    cd schematicGenerator
    mkdir build && cd build
    cmake -D EMCC=yes ..
    make schema.js

The file schema.js has to included on your page before you can use the JS API

.. important:: SchematiK uses `Snap.svg <http://snapsvg.io/>`_ for rendering svg on webpages, so before you can use any rendering API you need to include *snapsvg.js*, or you can use some other svg rendering library to render it yourself.


    

