# schematiK
An automagic schematic generation tool which creates stunning svg images out of json Files. It primarily serves as a graphical end to [Yosys Tools](http://www.clifford.at/yosys/) 

## Getting Started

It takes input a json file as an input in the following format:

    {
      "modules": {
        <module_name>: {
          "ports": {
            <port_name>: <port_details>,
            ...
          },
          "cells": {
            <cell_name>: <cell_details>,
            ...
          },
          "netnames": {
            <net_name>: <net_details>,
            ...
          }
        }
      },
      "models": {
        ...
      },
    }

Where <port_details> is:

    {
      "direction": <"input" | "output" | "inout">,
      "bits": <bit_vector>
    }

And <cell_details> is:

    {
      "hide_name": <1 | 0>,
      "type": <cell_type>,
      "parameters": {
        <parameter_name>: <parameter_value>,
        ...
      },
      "attributes": {
        <attribute_name>: <attribute_value>,
        ...
      },
      "port_directions": {
        <port_name>: <"input" | "output" | "inout">,
        ...
      },
      "connections": {
        <port_name>: <bit_vector>,
        ...
      },
    }

And <net_details> is:

    {
      "hide_name": <1 | 0>,
      "bits": <bit_vector>
    }

To create a svg, all you have to do is 

    createSchematic('drawInThisDiv', jsonData);

Currently configuration options and API for non-Yosys schematics are being worked on.

