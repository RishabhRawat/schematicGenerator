var standardSymbols = {
    library: {
        adder: {
            width: 50,
            height: 50,
            inputs: [
                {x: 10, y: 20},
                {x: 10, y: 30}
            ],
            outputs: [
                {x: 10, y: 20},
                {x: 10, y: 30}
            ]
        },
        inverter: {
            width: 60,
            height: 70,
            inputs: [
                {x: 10, y: 25}
            ],
            outputs: [
                {x: 60, y: 25}
            ],
            pathString: 'm 10 15 l 0 40 l 40 -20 a 2.5 2.5 1 1 0 5 0 a 2.5 2.5 1 1 0 -5 0 Z'
        },
        and: {
            width: 75,
            height: 70,
            inputs: [
                {x: 10, y: 25}
            ],
            outputs: [
                {x: 60, y: 25}
            ],
            pathString: 'm 10 10 l 0 50 l 30 0 a 25 25 1 1 0 0 -50 Z'
        },
        multiplexer: {
            width: 70,
            height: 120,
            inputs: [
                {x: 10, y: 25}
            ],
            outputs: [
                {x: 60, y: 25}
            ],
            pathString: 'm 10 10 l 0 100 l 50 -20 l 0 -60 Z'
        },
        plain: {
            width: 70,
            height: 120,
            inputs: [],
            outputs: [],
            pathString: ' l 0 120 l 70 0 l 0 -120 Z'
        }
    },
    createSymbol: function (svg, type, pos_x, pos_y) {
        var m = svg.path('M ' + pos_x + ' ' + pos_y + ' ' + this.getLibraryElement(type).pathString);
        m.attr({fill: 'transparent', stroke: 'black'});
        return m;
    },
    getSize: function (type) {
        var m = this.getLibraryElement(type);
        return [m.width, m.height];
    },
    getLibraryElement: function (type) {
        if (type == 'not' || type == '$not' || type == '$logic_not') {
            return this.library.inverter;
        }
        else if (type == 'mux' || type == '$pmux' || type == '$mux') {
            return this.library.multiplexer;
        }
        else if (type == 'and' || type == '$and') {
            return this.library.and;
        }
        else {
            return this.library.plain;
        }
    }
};
var actives;
var cppfuncs = {

    createWire: function (x0, y0, x1, y1, width) {
        'use strict';
        draw.line(x0, y0, x1, y1).attr({'stroke-width': width, 'stroke': 'black'});
    },

    createRedActive: function (x0, y0, x1, y1, width) {
        'use strict';
        actives.line(x0, y0, x1, y1).attr({'stroke-width': width, 'stroke': 'red'});
    },
    createBlueActive: function (x0, y0, x1, y1, width) {
        'use strict';
        actives.line(x0, y0, x1, y1).attr({'stroke-width': width, 'stroke': 'blue'});
    },

    removeActives: function () {
        'use strict';
        actives.clear();
    }
};
schematik = function (div) {
    this.schematicInstance = new Module.schematic();
    this.diagramBlock = div;
};
schematik.prototype.processYosysJson = function (jsonData) {
    'use strict';
    var netObject = {};
    var topLevelModules = Object.keys(jsonData.modules);
    if (topLevelModules.length > 1) {
        console.error("Only one top level module is supported!!");
        return undefined;
    }
    this.tLM = jsonData.modules[topLevelModules[0]];
    var cell;

    for (cell in this.tLM.cells) {
        var m = this.schematicInstance.addModule(cell.toString());
        var s = standardSymbols.getSize(this.tLM.cells[cell].type);
        m.setSize(s[0], s[1]);
        var t;
        for (t in this.tLM.cells[cell].port_directions) {
            var netArray = this.tLM.cells[cell].connections[t];

            var newTerminal = m.addTerminal(t, this.getTerminalDirection(this.tLM.cells[cell].port_directions[t]), netArray.length);
            for (var i = 0; i < netArray.length; i++) {
                var netI = netArray[i];
                if (typeof netI == "string") {
                    newTerminal.partial(i, i).connect(new Module.terminal(netI));
                }
                else {
                    if (netObject[netI]) {
                        newTerminal.partial(i, i).connect(netObject[netI]);
                    }
                    else {
                        netObject[netI] = newTerminal.partial(i, i);
                    }
                }
            }
        }

    }

    var port;
    for (port in this.tLM.ports) {
        netArray = this.tLM.ports[port].bits;
        newTerminal = this.schematicInstance.addSystemTerminal(port, this.getTerminalDirection(this.tLM.ports[port].direction), netArray.length);
        for (i = 0; i < netArray.length; i++) {
            netI = netArray[i];
            if (typeof netI == "string") {
                newTerminal.partial(i, i).connect(new Module.terminal(netI));
            }
            else {
                if (netObject[netI]) {
                    newTerminal.partial(i, i).connect(netObject[netI]);
                }
                else {
                    netObject[netI] = newTerminal.partial(i, i);
                }
            }

        }

    }
};
schematik.prototype.getTerminalDirection = function (string) {
    if (string == "input")
        return Module.terminalType.in;
    else if (string == "output")
        return Module.terminalType.out;
    else if (string == "inout")
        return Module.terminalType.inout;
    else {
        console.error("Unknown terminal direction");
        return undefined;
    }
};
schematik.prototype.doPlacement = function () {
    'use strict';
    this.schematicInstance.doPlacement();
};
schematik.prototype.getPlacedModulesJson = function () {
    'use strict';
    return this.schematicInstance.getPlacedModulesJson();
};
schematik.prototype.doRouting = function () {
    'use strict';
    this.schematicInstance.doRouting();
};
schematik.prototype.getRoutedNetsJson = function () {
    'use strict';
    return this.schematicInstance.getRoutedNetsJson();
};
schematik.prototype.drawPlacement = function (placementData) {
    'use strict';
    if (placementData === undefined) {
        placementData = JSON.parse(this.getPlacedModulesJson());
    }
    var svg = Snap('#' + this.diagramBlock);
    svg.attr({
        viewBox: '0, 0, ' + (placementData.size_x + 6) + ',' + (placementData.size_y + 6)
    });
    // placementData.size_x + 6, placementData.size_y + 6
    var tRect = svg.rect(3, 3, placementData.size_x, placementData.size_y).appendTo(svg);
    actives = svg.svg(3, 3, placementData.size_x, placementData.size_y);
    tRect.attr({fill: 'white', stroke: 'black'});

    this.drawingArea = svg.svg(3, 3, placementData.size_x, placementData.size_y);
    svg.append(this.drawingArea);

    for (var i = 0; i < placementData.modules.length; i++) {
        var mod = placementData.modules[i];
        this.renderModule(mod, this.drawingArea);
    }

    for (var j = 0; j < placementData.systemTerminals.length; j++)
        this.renderTerminal(placementData.systemTerminals[j], this.drawingArea)
};
schematik.prototype.drawRouting = function (routingData) {
    if (routingData === undefined) {
        routingData = JSON.parse(this.getRoutedNetsJson());
    }
    for (var k = 0; k < routingData.length; k++)
        this.renderWires(routingData[k], this.drawingArea)
};
schematik.prototype.renderTerminal = function (term, parent) {
    'use strict';
    var c = this.drawingArea.circle(term.pos_x, term.pos_y, 3);
    var title = Snap.parse('<title>' + term.name + '</title>');
    c.append(title);
    parent.append(c);
    return c;
};
schematik.prototype.renderModule = function (mod, parent) {
    'use strict';
    var name = mod.name;
    var completeModule = this.tLM.cells[mod.name];
    if (completeModule.hide_name) {
        name = completeModule.type;
    }
    var text = '<foreignObject x="' + mod.pos_x + '" y="' + mod.pos_y + '" width="' + mod.size_x + '" height="' +
        mod.size_y + '">' +
        '<div class="thisDiv"  style="width: ' + mod.size_x + 'px; height: ' + mod.size_y + 'px" > <p' +
        ' class="centerStuffP">' +
        name + '</p></div></foreignObject>';
    var t = Snap.parse(text);
    parent.append(t);
    var m = standardSymbols.createSymbol(this.drawingArea, completeModule.type, mod.pos_x, mod.pos_y, mod.size_x, mod.size_y, 10);
    parent.append(m);
    var tooltipString = mod.name + '\n' + completeModule.type + '\n\n' +
        'PARAMETERS:';
    var paramKeys = Object.keys(completeModule.parameters);
    for (var i = 0; i < paramKeys.length; i++) {
        tooltipString += '\n' + paramKeys[i] + ': ' + completeModule.parameters[paramKeys[i]];
    }

    var title = Snap.parse('<title>' + tooltipString + '</title>');
    m.append(title);
    for (var j = 0; j < mod.terminals.length; j++)
        this.renderTerminal(mod.terminals[j], parent)
};
schematik.prototype.renderWires = function (cWire, group) {
    'use strict';
    var wireGroup = this.drawingArea.g();
    for (var i = 0; i < cWire.length; i++) {
        var svgPath = '';
        var linePoints = cWire[i]['points'];
        for (var j = 0; j < linePoints.length; j++) {
            if (j == 0) {
                svgPath += 'M '
            } else {
                svgPath += 'L '
            }
            svgPath += linePoints[j][0] + ' ' + linePoints[j][1] + ' ';
        }
        var path = this.drawingArea.path(svgPath).attr({'stroke-width': 1, 'stroke': 'black', 'fill': 'none'});
        wireGroup.append(path);
        wireGroup.addClass('netList');
    }
    group.append(wireGroup)
};