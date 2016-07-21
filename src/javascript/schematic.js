var draw;
var cppfuncs = {};
var actives;

cppfuncs.createWire = function(x0,y0,x1,y1,width) {
	'use strict';
	draw.line(x0,y0,x1,y1).stroke({ width: width, color: 'black' });
};

cppfuncs.createRedActive = function(x0,y0,x1,y1,width) {
	'use strict';
	actives.line(x0,y0,x1,y1).stroke({ width: width, color: 'red' });
};
cppfuncs.createBlueActive = function(x0,y0,x1,y1,width) {
	'use strict';
	actives.line(x0,y0,x1,y1).stroke({ width: width, color: 'blue' });
};

cppfuncs.removeActives = function () {
	'use strict';
	actives.clear();
};

createDetailedSchematic = function(div, jsonString) {
	'use strict';
	var schematic = new Module.schematic();
	var string = JSON.stringify(jsonString);
	var data = JSON.parse(schematic.createDetailedJsonSchematicFromJson(string));

	var createRectChild = function(svgElem, x, y, size_x, size_y, strokeWidth) {
		svgElem.rect(size_x, size_y).attr({x: x, y: y, fill: '#fff'}).stroke({width: strokeWidth});
		return svgElem.nested().size(size_x, size_y).attr({x: x, y: y});
	};


	draw = SVG(div).size(data.size_x + 6, data.size_y + 6);
	var tRect = createRectChild(draw, 0, 0, data.size_x, data.size_y, 1);

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
createSchematic = function(div, jsonString) {
	'use strict';
	var schematic = new Module.schematic();
	var result = schematic.createJsonSchematicFromJson(JSON.stringify(jsonString))
	console.log(result);
	var data = JSON.parse(result);
	var createRectChild = function(svgElem, x, y, size_x, size_y, strokeWidth) {
		svgElem.rect(size_x, size_y).attr({x: x, y: y, fill: '#fff'}).stroke({width: strokeWidth});
		return svgElem.nested().size(size_x, size_y).attr({x: x, y: y});
	};

	draw = SVG(div).size(data.size_x + 6, data.size_y + 6);
	var tRect = createRectChild(draw, 0, 0, data.size_x, data.size_y, 1);
	actives = draw.nested();

	for (var i = 0; i < data.modules.length; i++) {
		var mod = data.modules[i];
		var mRect = createRectChild(tRect, mod.pos_x, mod.pos_y, mod.size_x, mod.size_y, 1);
		for(var j = 0; j < mod.terminals.length; j++)
			draw.circle(10).attr({cx: mod.terminals[j].pos_x, cy: mod.terminals[j].pos_y, color: 'red'});
	}

	for(var k = 0; k < data.systemTerminals.length; k++)
		draw.circle(10).attr({cx: data.systemTerminals[k].pos_x, cy: data.systemTerminals[k].pos_y, color: 'red'});
	schematic.doRouting();
};
