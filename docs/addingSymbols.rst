Adding New Symbols Schematic
============================

Symbols are svg images of modules which can replace the default box symbol used for unrecognised modules. Using a relevant symbol can go long way in simplyfying schematics. Schematik allows you to easily modify existing symbols and add new symbols.

Creating a new symbol
#####################

The library of inbuilt symbols are present in the variable called ``standardSymbols.library``, an example for symbol of inverter is - 

.. code-block:: javascript

    inverter: {
        width: 70,
        height: 70,
        inputs: [
            {x: 10, y: 25}
        ],
        outputs: [
            {x: 60, y: 25}
        ],
        pathString: 'm 10 15 l 0 40 l 40 -20 a 4 4 1 1 0 8 0 a 4 4 1 1 0 -8 0 Z',
        text: ' '
    }, 

The description looks very intuitive, the ``width`` and ``height`` represent the bounding box of the symbol. This is important because it is used to set collision bounds during placement and routing, thus no net/module will overlap this region.

The ``inputs`` and ``outputs`` field represent where exactly the connection ports can join. It is part of a functionality which will be implemented in the coming versions but you can safely ignore it for the moment.

The ``pathString`` is the svg path of the symbol. It is important to notive that the path is in relative form, this is beneficial as it allows us to render the svg at any position by prepending it with ``"M x y"`` string. It also allows better performance as we do not need to use transforms to move the symbol around (which can slow down the rendering process)

The ``text`` field represents the text that is to be rendered inside the module. If no text is present (or it is an empty string), the renderer will use the ``type`` of the module as the text. To prevent this you can use a single space as text.

.. warning:: text will be rendered inside the module bounding box and not inside the shape created by the ``pathString``, so if the difference between them is too big or the text is large, you might notice part of the text rendered outside the symbol. This is something I will fix in the later versions

Now that you know how to create a symbol, all you need to put it in the library which can be as simple as 

.. code-block:: javascript

    standardSymbols.library.mysymbol = {...}
 
Creating a new symbol
#####################
All that is left now registering the new symbols. 

.. note:: The symbols are indexed by the module types

The "type to symbol" mapping is stored in ``standardSymbols.symbolMap``, so you need to add entries from the required type to your symbol. Ex- 

.. code-block:: javascript
    
    standardSymbols.symbolMap.mytype = standardSymbols.library.mysymbol

You should now be able to render your own symbols using Schematik.

