/*
 * schematiK - An automagic schematic generation tool
 *
 * Copyright (c) 2016, Rishabh Rawat <rishabhrawat1994@gmail.com>
 * Permission to use, copy, modify, and/or distribute this software for any purpose with or without
 * fee is hereby granted, provided that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 */

#include <schematic.h>
#include "simple_svg_1.0.0.hpp"

void drawSchematicObject(schematic schematic);

int main(int argc, char** argv) {
	std::ifstream t("test.json");
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	schematic schematicObject;
	schematicObject.createJsonSchematicFromJson(str);
	drawSchematicObject(schematicObject);
	schematicObject.doRouting();
	std::cout<<schematicObject.getRoutedNetsJson()<<std::endl;
	return 0;
}

void drawSchematicObject(schematic schematicObject){
	std::cout<< schematic
}
