var draw;
var cppfuncs = {};
cppfuncs.createWire = function(x0,y0,x1,y1,width) {
	'use strict';
	console.log("tureaaaaaaa");
	draw.line(x0,y0,x1,y1).stroke({ width: width, color: 'black' });
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
	var string = JSON.stringify(jsonString);
	var data = JSON.parse(schematic.createJsonSchematicFromJson(string));
	console.log(data.modules[0]);
	var createRectChild = function(svgElem, x, y, size_x, size_y, strokeWidth) {
		svgElem.rect(size_x, size_y).attr({x: x, y: y, fill: '#fff'}).stroke({width: strokeWidth});
		return svgElem.nested().size(size_x, size_y).attr({x: x, y: y});
	};

	draw = SVG(div).size(data.size_x + 6, data.size_y + 6);
	var tRect = createRectChild(draw, 0, 0, data.size_x, data.size_y, 1);

	for (var i = 0; i < data.modules.length; i++) {
		var mod = data.modules[i];
		var mRect = createRectChild(tRect, mod.pos_x, mod.pos_y, mod.size_x, mod.size_y, 1);
	}
	schematic.doRouting();
};
