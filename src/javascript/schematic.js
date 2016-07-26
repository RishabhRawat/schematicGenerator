var draw;
var cppfuncs = {};
var actives;

cppfuncs.createWire = function (x0, y0, x1, y1, width) {
    'use strict';
    draw.line(x0, y0, x1, y1).attr({
        'stroke-width': width,
        'stroke': 'black'
    });
};

cppfuncs.createRedActive = function (x0, y0, x1, y1, width) {
    'use strict';
    actives.line(x0, y0, x1, y1).attr({
        'stroke-width': width,
        'stroke': 'red'
    });
};
cppfuncs.createBlueActive = function (x0, y0, x1, y1, width) {
    'use strict';
    actives.line(x0, y0, x1, y1).attr({
        'stroke-width': width,
        'stroke': 'blue'
    });
};

cppfuncs.removeActives = function () {
    'use strict';
    actives.clear();
};

var createRectChild = function (svgElem, x, y, size_x, size_y, strokeWidth) {
    svgElem.rect(size_x, size_y).attr({x: x, y: y, fill: '#fff'}).stroke({width: strokeWidth});
    return svgElem.nested().size(size_x, size_y).attr({x: x, y: y});
};

var renderTerminal = function (term, parent) {
    'use strict';
    var c = draw.circle(term.pos_x, term.pos_y, 5);
    var title = Snap.parse('<title>' + term.name + '</title>');
    c.append(title);
    return c;
};

var renderModule = function (mod, parent) {
    'use strict';
    var text = '<svg><foreignObject x="' + mod.pos_x + '" y="' + mod.pos_y + '" width="' + mod.size_x + '" height="' + mod.size_y + '"><style> .thisDiv { display: table; font-size: 0.7rem; width: ' + mod.size_x + 'px; height: ' + mod.size_y + 'px; }</style> <div class="thisDiv"> <p class="centerStuffP">' + mod.name + '</p>  </div> </foreignObject></svg>';
    var t = Snap.parse(text)
    parent.append(t)
    var m = draw.rect(mod.pos_x, mod.pos_y, mod.size_x, mod.size_y);
    m.attr({
        fill: 'transparent',
        stroke: 'black'
    });
    var title = Snap.parse('<title>' + mod.name + '</title>');
    m.append(title);
    for (var j = 0; j < mod.terminals.length; j++)
        renderTerminal(mod.terminals[j], parent)
};

var renderWires = function (cWire,group) {
    'use strict';
    var wireGroup = draw.g();
    for(var i = 0; i < cWire.length; i++) {
        var svgPath = '';
        var linePoints = cWire[i]['points'];
        for(var j = 0; j < linePoints.length; j++) {
        if(j == 0){
            svgPath += 'M'
        }
        else {
            svgPath +='L'
        }
        svgPath += linePoints[j][0] + ' '+ linePoints[j][1];

        }
        var path = draw.path(svgPath).attr({
            'stroke-width': 1,
            'stroke': 'black',
            'fill': 'none'
        });
        wireGroup.append(path);
        wireGroup.addClass('netList');
    }
    group.append(wireGroup)
};


createDetailedSchematic = function (div, jsonString) {
    'use strict';
    var schematic = new Module.schematic();
    var string = JSON.stringify(jsonString);
    var data = JSON.parse(schematic.createDetailedJsonSchematicFromJson(string));

    draw = Snap(data.size_x + 6, data.size_y + 6);
    var tRect = paper.rect(3, 3, data.size_x, data.size_y);
    draw.append(tRect);

    for (var i = 0; i < data.partitions.length; i++) {
        var partition = data.partitions[i];
        var pRect = createRectChild(tRect, partition.pos_x - data.offset_x, partition.pos_y - data.offset_y,
            partition.size_x, partition.size_y, 1);
        for (var j = 0; j < partition.boxes.length; j++) {
            var box = partition.boxes[j];
            var bRect = createRectChild(
                pRect, box.pos_x - partition.offset_x, box.pos_y - partition.offset_y, box.size_x, box.size_y, 1);
            for (var k = 0; k < box.modules.length; k++) {
                var mod = box.modules[k];
                var mRect = createRectChild(
                    bRect, mod.pos_x - box.offset_x, mod.pos_y - box.offset_y, mod.size_x, mod.size_y, 1);
            }
        }
    }
};
createSchematic = function (div, jsonString) {
    'use strict';
    var schematic = new Module.schematic();
    var data = JSON.parse(schematic.createJsonSchematicFromJson(JSON.stringify(jsonString)))
    console.log(JSON.stringify(data));
    var svg = Snap(data.size_x + 6, data.size_y + 6);
    var tRect = svg.rect(3, 3, data.size_x, data.size_y);
    actives = svg.svg(3, 3, data.size_x, data.size_y);
    tRect.attr({
        fill: 'white',
        stroke: 'black'
    });

    draw = svg.svg(3, 3, data.size_x, data.size_y);
    svg.append(draw);

    for (var i = 0; i < data.modules.length; i++) {
        var mod = data.modules[i];
        renderModule(mod, draw);
    }

    for (var j = 0; j < data.systemTerminals.length; j++)
        renderTerminal(data.systemTerminals[j], draw)
    schematic.doRouting()
    var nets = JSON.parse(schematic.getRoutedNetsJson());
    console.log(JSON.stringify(nets));
    for (var k = 0; k < nets.length; k++)
        renderWires(nets[k], draw)
};
