var sch = new Module.schematicGenerator();
jsonData = {
  "creator": "Yosys 0.6+86 (git sha1 958fb29, clang 3.8.0-2ubuntu3 -fPIC -Os)",
  "modules": {
    "uut_always01": {
      "ports": {
        "clock": {
          "direction": "input",
          "bits": [ 2 ]
        },
        "reset": {
          "direction": "input",
          "bits": [ 3 ]
        },
        "count": {
          "direction": "output",
          "bits": [ 4, 5, 6, 7 ]
        }
      },
      "cells": {
        "$ternary$tests/simple/always01.v:8$3": {
          "hide_name": 1,
          "type": "$mux",
          "parameters": {
            "WIDTH": 32
          },
          "attributes": {
            "src": "tests/simple/always01.v:8"
          },
          "port_directions": {
            "Y": "output",
            "S": "input",
            "B": "input",
            "A": "input"
          },
          "connections": {
            "Y": [ 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 ],
            "S": [ 3 ],
            "B": [ "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" ],
            "A": [ 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71 ]
          }
        },
        "$add$tests/simple/always01.v:8$2": {
          "hide_name": 1,
          "type": "$add",
          "parameters": {
            "Y_WIDTH": 32,
            "B_WIDTH": 32,
            "A_WIDTH": 4,
            "B_SIGNED": 0,
            "A_SIGNED": 0
          },
          "attributes": {
            "src": "tests/simple/always01.v:8"
          },
          "port_directions": {
            "Y": "output",
            "B": "input",
            "A": "input"
          },
          "connections": {
            "Y": [ 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71 ],
            "B": [ "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" ],
            "A": [ 4, 5, 6, 7 ]
          }
        }
      },
      "netnames": {
        "$ternary$tests/simple/always01.v:8$3_Y": {
          "hide_name": 1,
          "bits": [ 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 ],
          "attributes": {
            "src": "tests/simple/always01.v:8"
          }
        },
        "$add$tests/simple/always01.v:8$2_Y": {
          "hide_name": 1,
          "bits": [ 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71 ],
          "attributes": {
            "src": "tests/simple/always01.v:8"
          }
        },
        "$0\\count[3:0]": {
          "hide_name": 1,
          "bits": [ 72, 73, 74, 75 ],
          "attributes": {
            "src": "tests/simple/always01.v:7"
          }
        },
        "count": {
          "hide_name": 0,
          "bits": [ 4, 5, 6, 7 ],
          "attributes": {
            "src": "tests/simple/always01.v:4"
          }
        },
        "reset": {
          "hide_name": 0,
          "bits": [ 3 ],
          "attributes": {
            "src": "tests/simple/always01.v:3"
          }
        },
        "clock": {
          "hide_name": 0,
          "bits": [ 2 ],
          "attributes": {
            "src": "tests/simple/always01.v:3"
          }
        }
      }
    }
  }
}

string = JSON.stringify(jsonData)
// console.log(string);
var structure = sch.createJsonSchematicFromJson(string);
var data = JSON.parse(structure);
console.log(structure);

var createRectChild = function (svgElem, x, y, size_x, size_y, strokeWidth) {
  svgElem.rect(size_x,size_y).attr({x: x, y: y, fill: '#fff'}).stroke({ width: 1 });
  return svgElem.nested().size(size_x,size_y).attr({x: x, y: y});
}


var draw = SVG('drawing').size(data.size_x+6,data.size_y+6);
var tRect = createRectChild(draw, 0, 0, data.size_x, data.size_y, 1); 

for( i = 0; i < data.partitions.length; i++ ) {
  var partition = data.partitions[i];
  var pRect = createRectChild(tRect, data.offset_x + partition.pos_x, data.offset_y + partition.pos_y, partition.size_x, partition.size_y, 1);
  for( j = 0; j < partition.boxes.length; j++ ) {
    var box = partition.boxes[j];
    var bRect = createRectChild(pRect, partition.offset_x + box.pos_x, partition.offset_y + box.pos_y, box.size_x, box.size_y, 1);
    for( k = 0; k < box.modules.length; k++ ) {
      var mod = box.modules[k];
      var mRect = createRectChild(bRect, box.offset_x + mod.pos_x, box.offset_y + mod.pos_y, mod.size_x, mod.size_y, 1);
      console.log(box.offset_x + mod.pos_x, box.offset_y + mod.pos_y, mod.size_x, mod.size_y);
      console.log(mod.pos_x, mod.pos_y, mod.size_x, mod.size_y);
    }
  }
}