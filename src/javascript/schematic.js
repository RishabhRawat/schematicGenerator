var standardSymbols = {
	library: {
		inverter: {
			width: 70,
			height: 70,
			inputs: [{x: 10, y: 25}],
			outputs: [{x: 60, y: 25}],
			pathString: 'm 10 15 l 0 40 l 40 -20 a 4 4 1 1 0 8 0 a 4 4 1 1 0 -8 0 Z',
			text: ' '
		},
		and: {
			width: 75,
			height: 70,
			inputs: [{x: 10, y: 25}],
			outputs: [{x: 60, y: 25}],
			pathString: 'm 10 10 l 0 50 l 30 0 a 25 25 1 1 0 0 -50 Z',
			text: ' '
		},
		nand: {
			width: 75,
			height: 70,
			inputs: [{x: 10, y: 25}],
			outputs: [{x: 60, y: 25}],
			pathString: 'm 10 10 l 0 50 l 30 0 a 25 25 1 0 0 25 -25 a 4 4 1 1 0 8 0 a 4 4 1 1 0 -8 0 a 25 25 1 0 0 -25 -25 Z',
			text: ' '
		},
		equality: {
			width: 70,
			height: 70,
			inputs: [{x: 10, y: 25}],
			outputs: [{x: 60, y: 25}],
			pathString: 'm 15 35 a 20 20 1 0 0 40 0 a 20 20 1 0 0 -40 0 Z m 15 2 l 10 0 m 0 -4 l -10 0',
			text: ' '
		},
		adder_circular: {
			width: 70,
			height: 70,
			inputs: [{x: 10, y: 25}],
			outputs: [{x: 60, y: 25}],
			pathString: 'm 15 35 a 20 20 1 0 0 40 0 a 20 20 1 0 0 -40 0 Z m 15 0 l 10 0 m -5 -5 l 0 10',
			text: ' '
		},
		subtracter_circular: {
			width: 70,
			height: 70,
			inputs: [{x: 10, y: 25}],
			outputs: [{x: 60, y: 25}],
			pathString: 'm 15 35 a 20 20 1 0 0 40 0 a 20 20 1 0 0 -40 0 Z m 15 0 l 10 0',
			text: ' '
		},
		multiplexer: {
			width: 70,
			height: 120,
			inputs: [{x: 10, y: 25}],
			outputs: [{x: 60, y: 25}],
			pathString: 'm 10 10 l 0 100 l 50 -20 l 0 -60 Z',
			text: ' '
		},
		plain: {width: 70, height: 120, inputs: [], outputs: [], pathString: ' l 0 120 l 70 0 l 0 -120 Z', text: ''}
	},
	createSymbol: function (svg, type, pos_x, pos_y) {
		var m = svg.path('M ' + pos_x + ' ' + pos_y + ' ' + this.getLibraryElement(type).pathString);
		m.attr({fill: '#0000ff', 'fill-opacity': 0.2, stroke: 'black'});
		return m;
	},
	getSize: function (type) {
		var m = this.getLibraryElement(type);
		return [m.width, m.height];
	},
	getLibraryElement: function (type) {
		var element = this.symbolMap[type];
		if (element)
			return element;
		else
			return this.library.plain;
	},
	getText: function (type) {
		return this.getLibraryElement(type).text;
	}
};
standardSymbols.symbolMap = {
	not: standardSymbols.library.inverter,
	$not: standardSymbols.library.inverter,
	$logic_not: standardSymbols.library.inverter,
	mux: standardSymbols.library.multiplexer,
	$pmux: standardSymbols.library.multiplexer,
	and: standardSymbols.library.and,
	$and: standardSymbols.library.and,
	equality: standardSymbols.library.equality,
	$eq: standardSymbols.library.equality,
	adder: standardSymbols.library.adder_circular,
	$add: standardSymbols.library.adder_circular,
	subtractor: standardSymbols.library.subtracter_circular,
	$sub: standardSymbols.library.subtracter_circular
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
	this.schematicInstance = new Module.topDesign();
	this.diagramBlock = div;
	this.yAxisUp = false;
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

			var newTerminal = m.addTerminal(
				t, this.getTerminalDirection(this.tLM.cells[cell].port_directions[t]), netArray.length);
			for (var i = 0; i < netArray.length; i++) {
				var netI = netArray[i];
				if (typeof netI == "string") {
					newTerminal.partial(i, i).connect(new Module.terminal(netI));
				} else {
					if (netObject[netI]) {
						newTerminal.partial(i, i).connect(netObject[netI]);
					} else {
						netObject[netI] = newTerminal.partial(i, i);
					}
				}
			}
		}
	}

	var port;
	for (port in this.tLM.ports) {
		netArray = this.tLM.ports[port].bits;
		newTerminal = this.schematicInstance.addSystemTerminal(
			port, this.getTerminalDirection(this.tLM.ports[port].direction), netArray.length);
		for (i = 0; i < netArray.length; i++) {
			netI = netArray[i];
			if (typeof netI == "string") {
				newTerminal.partial(i, i).connect(new Module.terminal(netI));
			} else {
				if (netObject[netI]) {
					newTerminal.partial(i, i).connect(netObject[netI]);
				} else {
					netObject[netI] = newTerminal.partial(i, i);
				}
			}
		}
	}
};
schematik.prototype.getTerminalDirection = function (direction) {
	if (direction == "input")
		return Module.terminalType.in;
	else if (direction == "output")
		return Module.terminalType.out;
	else if (direction == "inout")
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
	svg.attr({viewBox: '0, 0, ' + (placementData.size_x + 6) + ',' + (placementData.size_y + 6)});
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
	if (term.const_value) {
		var constantLine = [{"points": [[term.pos_x, term.pos_y]]}];
		var nextPoint = [];
		if (term.side == 0) {
			nextPoint = [term.pos_x - 17, term.pos_y];
		} else if (term.side == 1) {
			nextPoint = [term.pos_x, term.pos_y + 17];
		} else if (term.side == 2) {
			nextPoint = [term.pos_x + 17, term.pos_y];
		} else if (term.side == 3) {
			nextPoint = [term.pos_x, term.pos_y - 17];
		} else {
			throw "Incorrect Terminals!!";
		}
		constantLine[0].points.push(nextPoint);
		this.renderWires(constantLine, parent);
		var constText = this.drawingArea.text(nextPoint[0] + 2, nextPoint[1] - 2, term.const_value);
		constText.attr({'font-size': '10px'});
		parent.append(constText);
	}
	c.append(title);
	parent.append(c);
	return c;
};
schematik.prototype.renderModule = function (mod, parent) {
	'use strict';
	var name = mod.name;
	var completeModule = this.tLM.cells[mod.name];
	if (completeModule.hide_name) {
		name = standardSymbols.getText(completeModule.type);
		if (name == '')
			name = completeModule.type;
	}
	var text = '<foreignObject x="' + mod.pos_x + '" y="' + mod.pos_y + '" width="' + mod.size_x + '" height="' +
		mod.size_y + '">' +
		'<div class="schematikTextDiv"  style="width: ' + mod.size_x + 'px; height: ' + mod.size_y + 'px" > <p' +
		' class="schematikCenterP">' + name + '</p></div></foreignObject>';
	var t = Snap.parse(text);
	parent.append(t);
	var m = standardSymbols.createSymbol(
		this.drawingArea, completeModule.type, mod.pos_x, mod.pos_y, mod.size_x, mod.size_y, 10);
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
		wireGroup.addClass('schematikNetlist');
	}
	group.append(wireGroup)
};