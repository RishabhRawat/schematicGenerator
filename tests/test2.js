jsonData = {
	"creator": "Yosys 0.6+135 (git sha1 ca91bcc, clang 3.4-1ubuntu3 -fPIC -Os)",
	"modules": {
		"up3down5": {
			"ports": {
				"clock": {
					"direction": "input",
					"bits": [
						2
					]
				},
				"data_in": {
					"direction": "input",
					"bits": [
						3,
						4,
						5,
						6,
						7,
						8,
						9,
						10,
						11
					]
				},
				"up": {
					"direction": "input",
					"bits": [
						12
					]
				},
				"down": {
					"direction": "input",
					"bits": [
						13
					]
				},
				"carry_out": {
					"direction": "output",
					"bits": [
						14
					]
				},
				"borrow_out": {
					"direction": "output",
					"bits": [
						15
					]
				},
				"count_out": {
					"direction": "output",
					"bits": [
						16,
						17,
						18,
						19,
						20,
						21,
						22,
						23,
						24
					]
				},
				"parity_out": {
					"direction": "output",
					"bits": [
						25
					]
				}
			},
			"cells": {
				"$abc$464$auto$blifparse.cc:276:parse_blif$465": {
					"hide_name": 1,
					"type": "$_NOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							12
						],
						"B": [
							13
						],
						"Y": [
							26
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$466": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							13
						],
						"Y": [
							27
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$467": {
					"hide_name": 1,
					"type": "$_NOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							12
						],
						"B": [
							27
						],
						"Y": [
							28
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$468": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							28
						],
						"Y": [
							29
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$469": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							12
						],
						"B": [
							27
						],
						"Y": [
							30
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$470": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							30
						],
						"Y": [
							31
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$471": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							12
						],
						"B": [
							13
						],
						"Y": [
							32
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$472": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							32
						],
						"Y": [
							33
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$473": {
					"hide_name": 1,
					"type": "$_MUX_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"S": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							31
						],
						"B": [
							33
						],
						"S": [
							16
						],
						"Y": [
							34
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$474": {
					"hide_name": 1,
					"type": "$_OAI3_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							29
						],
						"B": [
							16
						],
						"C": [
							34
						],
						"Y": [
							35
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$475": {
					"hide_name": 1,
					"type": "$_MUX_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"S": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							35
						],
						"B": [
							3
						],
						"S": [
							26
						],
						"Y": [
							36
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$476": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							4
						],
						"Y": [
							37
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$477": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							17
						],
						"B": [
							16
						],
						"Y": [
							38
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$478": {
					"hide_name": 1,
					"type": "$_NOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							38
						],
						"B": [
							29
						],
						"Y": [
							39
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$479": {
					"hide_name": 1,
					"type": "$_NAND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							32
						],
						"B": [
							17
						],
						"Y": [
							40
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$480": {
					"hide_name": 1,
					"type": "$_OAI3_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							38
						],
						"B": [
							31
						],
						"C": [
							40
						],
						"Y": [
							41
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$481": {
					"hide_name": 1,
					"type": "$_NOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							41
						],
						"B": [
							39
						],
						"Y": [
							42
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$482": {
					"hide_name": 1,
					"type": "$_MUX_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"S": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							42
						],
						"B": [
							37
						],
						"S": [
							26
						],
						"Y": [
							43
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$483": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							43
						],
						"Y": [
							44
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$484": {
					"hide_name": 1,
					"type": "$_NOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							17
						],
						"B": [
							16
						],
						"Y": [
							45
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$485": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							45
						],
						"B": [
							18
						],
						"Y": [
							46
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$486": {
					"hide_name": 1,
					"type": "$_NOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							46
						],
						"B": [
							31
						],
						"Y": [
							47
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$487": {
					"hide_name": 1,
					"type": "$_AOI3_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							32
						],
						"B": [
							18
						],
						"C": [
							47
						],
						"Y": [
							48
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$488": {
					"hide_name": 1,
					"type": "$_OAI3_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							46
						],
						"B": [
							29
						],
						"C": [
							48
						],
						"Y": [
							49
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$489": {
					"hide_name": 1,
					"type": "$_MUX_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"S": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							49
						],
						"B": [
							5
						],
						"S": [
							26
						],
						"Y": [
							50
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$490": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							6
						],
						"Y": [
							51
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$491": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							18
						],
						"Y": [
							52
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$492": {
					"hide_name": 1,
					"type": "$_OR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							45
						],
						"B": [
							52
						],
						"Y": [
							53
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$493": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							53
						],
						"B": [
							19
						],
						"Y": [
							54
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$494": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							19
						],
						"Y": [
							55
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$496": {
					"hide_name": 1,
					"type": "$_OAI4_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"D": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							54
						],
						"B": [
							31
						],
						"C": [
							33
						],
						"D": [
							55
						],
						"Y": [
							56
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$497": {
					"hide_name": 1,
					"type": "$_AOI3_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							54
						],
						"B": [
							28
						],
						"C": [
							56
						],
						"Y": [
							57
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$498": {
					"hide_name": 1,
					"type": "$_MUX_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"S": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							57
						],
						"B": [
							51
						],
						"S": [
							26
						],
						"Y": [
							58
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$499": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							58
						],
						"Y": [
							59
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$500": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							7
						],
						"Y": [
							60
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$501": {
					"hide_name": 1,
					"type": "$_OR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							17
						],
						"B": [
							16
						],
						"Y": [
							61
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$502": {
					"hide_name": 1,
					"type": "$_AOI3_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							61
						],
						"B": [
							18
						],
						"C": [
							19
						],
						"Y": [
							62
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$503": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							62
						],
						"B": [
							20
						],
						"Y": [
							63
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$504": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							20
						],
						"Y": [
							64
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$505": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							18
						],
						"B": [
							19
						],
						"Y": [
							65
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$506": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							65
						],
						"B": [
							61
						],
						"Y": [
							66
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$507": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							66
						],
						"B": [
							64
						],
						"Y": [
							67
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$508": {
					"hide_name": 1,
					"type": "$_OAI4_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"D": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							67
						],
						"B": [
							31
						],
						"C": [
							33
						],
						"D": [
							64
						],
						"Y": [
							68
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$509": {
					"hide_name": 1,
					"type": "$_AOI3_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							63
						],
						"B": [
							28
						],
						"C": [
							68
						],
						"Y": [
							69
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$510": {
					"hide_name": 1,
					"type": "$_MUX_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"S": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							69
						],
						"B": [
							60
						],
						"S": [
							26
						],
						"Y": [
							70
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$511": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							70
						],
						"Y": [
							71
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$512": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							8
						],
						"Y": [
							72
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$513": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							62
						],
						"B": [
							64
						],
						"Y": [
							73
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$514": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							73
						],
						"B": [
							21
						],
						"Y": [
							74
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$515": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							21
						],
						"Y": [
							75
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$516": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							66
						],
						"B": [
							20
						],
						"Y": [
							76
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$517": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							76
						],
						"B": [
							75
						],
						"Y": [
							77
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$518": {
					"hide_name": 1,
					"type": "$_OAI4_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"D": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							77
						],
						"B": [
							31
						],
						"C": [
							33
						],
						"D": [
							75
						],
						"Y": [
							78
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$519": {
					"hide_name": 1,
					"type": "$_AOI3_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							74
						],
						"B": [
							28
						],
						"C": [
							78
						],
						"Y": [
							79
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$520": {
					"hide_name": 1,
					"type": "$_MUX_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"S": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							79
						],
						"B": [
							72
						],
						"S": [
							26
						],
						"Y": [
							80
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$521": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							80
						],
						"Y": [
							81
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$522": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							22
						],
						"Y": [
							82
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$523": {
					"hide_name": 1,
					"type": "$_OAI3_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							45
						],
						"B": [
							52
						],
						"C": [
							55
						],
						"Y": [
							83
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$524": {
					"hide_name": 1,
					"type": "$_OR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							21
						],
						"B": [
							20
						],
						"Y": [
							84
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$525": {
					"hide_name": 1,
					"type": "$_NOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							21
						],
						"B": [
							20
						],
						"Y": [
							85
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$526": {
					"hide_name": 1,
					"type": "$_AOI3_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							85
						],
						"B": [
							83
						],
						"C": [
							84
						],
						"Y": [
							86
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$527": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							86
						],
						"B": [
							82
						],
						"Y": [
							87
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$528": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							21
						],
						"B": [
							20
						],
						"Y": [
							88
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$529": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							88
						],
						"B": [
							66
						],
						"Y": [
							89
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$530": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							89
						],
						"B": [
							22
						],
						"Y": [
							90
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$531": {
					"hide_name": 1,
					"type": "$_AOI4_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"D": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							90
						],
						"B": [
							30
						],
						"C": [
							32
						],
						"D": [
							22
						],
						"Y": [
							91
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$532": {
					"hide_name": 1,
					"type": "$_OAI3_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							87
						],
						"B": [
							29
						],
						"C": [
							91
						],
						"Y": [
							92
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$533": {
					"hide_name": 1,
					"type": "$_MUX_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"S": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							92
						],
						"B": [
							9
						],
						"S": [
							26
						],
						"Y": [
							93
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$534": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							86
						],
						"B": [
							82
						],
						"Y": [
							94
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$535": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							94
						],
						"B": [
							23
						],
						"Y": [
							95
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$536": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							95
						],
						"B": [
							28
						],
						"Y": [
							96
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$537": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							23
						],
						"Y": [
							97
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$538": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							89
						],
						"B": [
							22
						],
						"Y": [
							98
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$539": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							98
						],
						"B": [
							97
						],
						"Y": [
							99
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$540": {
					"hide_name": 1,
					"type": "$_OAI4_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"D": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							99
						],
						"B": [
							31
						],
						"C": [
							33
						],
						"D": [
							97
						],
						"Y": [
							100
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$541": {
					"hide_name": 1,
					"type": "$_OR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							100
						],
						"B": [
							96
						],
						"Y": [
							101
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$542": {
					"hide_name": 1,
					"type": "$_MUX_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"S": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							101
						],
						"B": [
							10
						],
						"S": [
							26
						],
						"Y": [
							102
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$543": {
					"hide_name": 1,
					"type": "$_NOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							23
						],
						"B": [
							22
						],
						"Y": [
							103
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$544": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							103
						],
						"B": [
							85
						],
						"Y": [
							104
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$545": {
					"hide_name": 1,
					"type": "$_NAND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							103
						],
						"B": [
							84
						],
						"Y": [
							105
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$547": {
					"hide_name": 1,
					"type": "$_NAND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							103
						],
						"B": [
							105
						],
						"Y": [
							106
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$548": {
					"hide_name": 1,
					"type": "$_AOI3_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							104
						],
						"B": [
							83
						],
						"C": [
							106
						],
						"Y": [
							107
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$549": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							107
						],
						"B": [
							24
						],
						"Y": [
							108
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$550": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							108
						],
						"B": [
							28
						],
						"Y": [
							109
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$551": {
					"hide_name": 1,
					"type": "$_NOT_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							24
						],
						"Y": [
							110
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$552": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							23
						],
						"B": [
							22
						],
						"Y": [
							111
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$553": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							111
						],
						"B": [
							88
						],
						"Y": [
							112
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$554": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							112
						],
						"B": [
							66
						],
						"Y": [
							113
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$555": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							113
						],
						"B": [
							110
						],
						"Y": [
							114
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$556": {
					"hide_name": 1,
					"type": "$_OAI4_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"C": "input",
						"D": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							114
						],
						"B": [
							31
						],
						"C": [
							33
						],
						"D": [
							110
						],
						"Y": [
							115
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$557": {
					"hide_name": 1,
					"type": "$_OR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							115
						],
						"B": [
							109
						],
						"Y": [
							116
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$558": {
					"hide_name": 1,
					"type": "$_MUX_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"S": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							116
						],
						"B": [
							11
						],
						"S": [
							26
						],
						"Y": [
							117
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$559": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							43
						],
						"B": [
							36
						],
						"Y": [
							118
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$560": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							58
						],
						"B": [
							50
						],
						"Y": [
							119
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$561": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							119
						],
						"B": [
							118
						],
						"Y": [
							120
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$562": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							80
						],
						"B": [
							70
						],
						"Y": [
							121
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$563": {
					"hide_name": 1,
					"type": "$_XNOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							102
						],
						"B": [
							93
						],
						"Y": [
							122
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$564": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							122
						],
						"B": [
							121
						],
						"Y": [
							123
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$565": {
					"hide_name": 1,
					"type": "$_XNOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							123
						],
						"B": [
							120
						],
						"Y": [
							124
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$566": {
					"hide_name": 1,
					"type": "$_XOR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							124
						],
						"B": [
							117
						],
						"Y": [
							125
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$567": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							24
						],
						"B": [
							12
						],
						"Y": [
							126
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$568": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							126
						],
						"B": [
							113
						],
						"Y": [
							127
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$569": {
					"hide_name": 1,
					"type": "$_OR_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							107
						],
						"B": [
							24
						],
						"Y": [
							128
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$570": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							110
						],
						"B": [
							13
						],
						"Y": [
							129
						]
					}
				},
				"$abc$464$auto$blifparse.cc:276:parse_blif$571": {
					"hide_name": 1,
					"type": "$_AND_",
					"parameters": {
					},
					"attributes": {
					},
					"port_directions": {
						"A": "input",
						"B": "input",
						"Y": "output"
					},
					"connections": {
						"A": [
							129
						],
						"B": [
							128
						],
						"Y": [
							130
						]
					}
				},
				"$auto$simplemap.cc:403:simplemap_dff$137": {
					"hide_name": 1,
					"type": "$_DFF_P_",
					"parameters": {
					},
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					},
					"port_directions": {
						"C": "input",
						"D": "input",
						"Q": "output"
					},
					"connections": {
						"C": [
							2
						],
						"D": [
							36
						],
						"Q": [
							16
						]
					}
				},
				"$auto$simplemap.cc:403:simplemap_dff$138": {
					"hide_name": 1,
					"type": "$_DFF_P_",
					"parameters": {
					},
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					},
					"port_directions": {
						"C": "input",
						"D": "input",
						"Q": "output"
					},
					"connections": {
						"C": [
							2
						],
						"D": [
							44
						],
						"Q": [
							17
						]
					}
				},
				"$auto$simplemap.cc:403:simplemap_dff$139": {
					"hide_name": 1,
					"type": "$_DFF_P_",
					"parameters": {
					},
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					},
					"port_directions": {
						"C": "input",
						"D": "input",
						"Q": "output"
					},
					"connections": {
						"C": [
							2
						],
						"D": [
							50
						],
						"Q": [
							18
						]
					}
				},
				"$auto$simplemap.cc:403:simplemap_dff$140": {
					"hide_name": 1,
					"type": "$_DFF_P_",
					"parameters": {
					},
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					},
					"port_directions": {
						"C": "input",
						"D": "input",
						"Q": "output"
					},
					"connections": {
						"C": [
							2
						],
						"D": [
							59
						],
						"Q": [
							19
						]
					}
				},
				"$auto$simplemap.cc:403:simplemap_dff$141": {
					"hide_name": 1,
					"type": "$_DFF_P_",
					"parameters": {
					},
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					},
					"port_directions": {
						"C": "input",
						"D": "input",
						"Q": "output"
					},
					"connections": {
						"C": [
							2
						],
						"D": [
							71
						],
						"Q": [
							20
						]
					}
				},
				"$auto$simplemap.cc:403:simplemap_dff$142": {
					"hide_name": 1,
					"type": "$_DFF_P_",
					"parameters": {
					},
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					},
					"port_directions": {
						"C": "input",
						"D": "input",
						"Q": "output"
					},
					"connections": {
						"C": [
							2
						],
						"D": [
							81
						],
						"Q": [
							21
						]
					}
				},
				"$auto$simplemap.cc:403:simplemap_dff$143": {
					"hide_name": 1,
					"type": "$_DFF_P_",
					"parameters": {
					},
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					},
					"port_directions": {
						"C": "input",
						"D": "input",
						"Q": "output"
					},
					"connections": {
						"C": [
							2
						],
						"D": [
							93
						],
						"Q": [
							22
						]
					}
				},
				"$auto$simplemap.cc:403:simplemap_dff$144": {
					"hide_name": 1,
					"type": "$_DFF_P_",
					"parameters": {
					},
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					},
					"port_directions": {
						"C": "input",
						"D": "input",
						"Q": "output"
					},
					"connections": {
						"C": [
							2
						],
						"D": [
							102
						],
						"Q": [
							23
						]
					}
				},
				"$auto$simplemap.cc:403:simplemap_dff$145": {
					"hide_name": 1,
					"type": "$_DFF_P_",
					"parameters": {
					},
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					},
					"port_directions": {
						"C": "input",
						"D": "input",
						"Q": "output"
					},
					"connections": {
						"C": [
							2
						],
						"D": [
							117
						],
						"Q": [
							24
						]
					}
				},
				"$auto$simplemap.cc:403:simplemap_dff$146": {
					"hide_name": 1,
					"type": "$_DFF_P_",
					"parameters": {
					},
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					},
					"port_directions": {
						"C": "input",
						"D": "input",
						"Q": "output"
					},
					"connections": {
						"C": [
							2
						],
						"D": [
							127
						],
						"Q": [
							14
						]
					}
				},
				"$auto$simplemap.cc:403:simplemap_dff$147": {
					"hide_name": 1,
					"type": "$_DFF_P_",
					"parameters": {
					},
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					},
					"port_directions": {
						"C": "input",
						"D": "input",
						"Q": "output"
					},
					"connections": {
						"C": [
							2
						],
						"D": [
							130
						],
						"Q": [
							15
						]
					}
				},
				"$auto$simplemap.cc:403:simplemap_dff$148": {
					"hide_name": 1,
					"type": "$_DFF_P_",
					"parameters": {
					},
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					},
					"port_directions": {
						"C": "input",
						"D": "input",
						"Q": "output"
					},
					"connections": {
						"C": [
							2
						],
						"D": [
							125
						],
						"Q": [
							25
						]
					}
				}
			},
			"netnames": {
				"$0\\borrow_out[0:0]": {
					"hide_name": 1,
					"bits": [
						130
					],
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					}
				},
				"$0\\carry_out[0:0]": {
					"hide_name": 1,
					"bits": [
						127
					],
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					}
				},
				"$0\\count_nxt[8:0]": {
					"hide_name": 1,
					"bits": [
						36,
						44,
						50,
						59,
						71,
						81,
						93,
						102,
						117
					],
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					}
				},
				"$0\\parity_out[0:0]": {
					"hide_name": 1,
					"bits": [
						125
					],
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:14"
					}
				},
				"$abc$464$n100": {
					"hide_name": 1,
					"bits": [
						92
					],
					"attributes": {
					}
				},
				"$abc$464$n102": {
					"hide_name": 1,
					"bits": [
						94
					],
					"attributes": {
					}
				},
				"$abc$464$n103": {
					"hide_name": 1,
					"bits": [
						95
					],
					"attributes": {
					}
				},
				"$abc$464$n104": {
					"hide_name": 1,
					"bits": [
						96
					],
					"attributes": {
					}
				},
				"$abc$464$n105": {
					"hide_name": 1,
					"bits": [
						97
					],
					"attributes": {
					}
				},
				"$abc$464$n106": {
					"hide_name": 1,
					"bits": [
						98
					],
					"attributes": {
					}
				},
				"$abc$464$n107": {
					"hide_name": 1,
					"bits": [
						99
					],
					"attributes": {
					}
				},
				"$abc$464$n108": {
					"hide_name": 1,
					"bits": [
						100
					],
					"attributes": {
					}
				},
				"$abc$464$n109": {
					"hide_name": 1,
					"bits": [
						101
					],
					"attributes": {
					}
				},
				"$abc$464$n111": {
					"hide_name": 1,
					"bits": [
						103
					],
					"attributes": {
					}
				},
				"$abc$464$n112_1": {
					"hide_name": 1,
					"bits": [
						104
					],
					"attributes": {
					}
				},
				"$abc$464$n113": {
					"hide_name": 1,
					"bits": [
						105
					],
					"attributes": {
					}
				},
				"$abc$464$n115_1": {
					"hide_name": 1,
					"bits": [
						106
					],
					"attributes": {
					}
				},
				"$abc$464$n116": {
					"hide_name": 1,
					"bits": [
						107
					],
					"attributes": {
					}
				},
				"$abc$464$n117": {
					"hide_name": 1,
					"bits": [
						108
					],
					"attributes": {
					}
				},
				"$abc$464$n118": {
					"hide_name": 1,
					"bits": [
						109
					],
					"attributes": {
					}
				},
				"$abc$464$n119": {
					"hide_name": 1,
					"bits": [
						110
					],
					"attributes": {
					}
				},
				"$abc$464$n120": {
					"hide_name": 1,
					"bits": [
						111
					],
					"attributes": {
					}
				},
				"$abc$464$n121": {
					"hide_name": 1,
					"bits": [
						112
					],
					"attributes": {
					}
				},
				"$abc$464$n122": {
					"hide_name": 1,
					"bits": [
						113
					],
					"attributes": {
					}
				},
				"$abc$464$n123": {
					"hide_name": 1,
					"bits": [
						114
					],
					"attributes": {
					}
				},
				"$abc$464$n124": {
					"hide_name": 1,
					"bits": [
						115
					],
					"attributes": {
					}
				},
				"$abc$464$n125": {
					"hide_name": 1,
					"bits": [
						116
					],
					"attributes": {
					}
				},
				"$abc$464$n127": {
					"hide_name": 1,
					"bits": [
						118
					],
					"attributes": {
					}
				},
				"$abc$464$n128": {
					"hide_name": 1,
					"bits": [
						119
					],
					"attributes": {
					}
				},
				"$abc$464$n129": {
					"hide_name": 1,
					"bits": [
						120
					],
					"attributes": {
					}
				},
				"$abc$464$n130": {
					"hide_name": 1,
					"bits": [
						121
					],
					"attributes": {
					}
				},
				"$abc$464$n131": {
					"hide_name": 1,
					"bits": [
						122
					],
					"attributes": {
					}
				},
				"$abc$464$n132": {
					"hide_name": 1,
					"bits": [
						123
					],
					"attributes": {
					}
				},
				"$abc$464$n133": {
					"hide_name": 1,
					"bits": [
						124
					],
					"attributes": {
					}
				},
				"$abc$464$n135": {
					"hide_name": 1,
					"bits": [
						126
					],
					"attributes": {
					}
				},
				"$abc$464$n137": {
					"hide_name": 1,
					"bits": [
						128
					],
					"attributes": {
					}
				},
				"$abc$464$n138": {
					"hide_name": 1,
					"bits": [
						129
					],
					"attributes": {
					}
				},
				"$abc$464$n33": {
					"hide_name": 1,
					"bits": [
						26
					],
					"attributes": {
					}
				},
				"$abc$464$n34": {
					"hide_name": 1,
					"bits": [
						27
					],
					"attributes": {
					}
				},
				"$abc$464$n35": {
					"hide_name": 1,
					"bits": [
						28
					],
					"attributes": {
					}
				},
				"$abc$464$n36": {
					"hide_name": 1,
					"bits": [
						29
					],
					"attributes": {
					}
				},
				"$abc$464$n37": {
					"hide_name": 1,
					"bits": [
						30
					],
					"attributes": {
					}
				},
				"$abc$464$n38": {
					"hide_name": 1,
					"bits": [
						31
					],
					"attributes": {
					}
				},
				"$abc$464$n39": {
					"hide_name": 1,
					"bits": [
						32
					],
					"attributes": {
					}
				},
				"$abc$464$n40": {
					"hide_name": 1,
					"bits": [
						33
					],
					"attributes": {
					}
				},
				"$abc$464$n41": {
					"hide_name": 1,
					"bits": [
						34
					],
					"attributes": {
					}
				},
				"$abc$464$n42": {
					"hide_name": 1,
					"bits": [
						35
					],
					"attributes": {
					}
				},
				"$abc$464$n44": {
					"hide_name": 1,
					"bits": [
						37
					],
					"attributes": {
					}
				},
				"$abc$464$n45": {
					"hide_name": 1,
					"bits": [
						38
					],
					"attributes": {
					}
				},
				"$abc$464$n46": {
					"hide_name": 1,
					"bits": [
						39
					],
					"attributes": {
					}
				},
				"$abc$464$n47": {
					"hide_name": 1,
					"bits": [
						40
					],
					"attributes": {
					}
				},
				"$abc$464$n48": {
					"hide_name": 1,
					"bits": [
						41
					],
					"attributes": {
					}
				},
				"$abc$464$n49": {
					"hide_name": 1,
					"bits": [
						42
					],
					"attributes": {
					}
				},
				"$abc$464$n50": {
					"hide_name": 1,
					"bits": [
						43
					],
					"attributes": {
					}
				},
				"$abc$464$n52": {
					"hide_name": 1,
					"bits": [
						45
					],
					"attributes": {
					}
				},
				"$abc$464$n53": {
					"hide_name": 1,
					"bits": [
						46
					],
					"attributes": {
					}
				},
				"$abc$464$n54": {
					"hide_name": 1,
					"bits": [
						47
					],
					"attributes": {
					}
				},
				"$abc$464$n55": {
					"hide_name": 1,
					"bits": [
						48
					],
					"attributes": {
					}
				},
				"$abc$464$n56_1": {
					"hide_name": 1,
					"bits": [
						49
					],
					"attributes": {
					}
				},
				"$abc$464$n58": {
					"hide_name": 1,
					"bits": [
						51
					],
					"attributes": {
					}
				},
				"$abc$464$n59_1": {
					"hide_name": 1,
					"bits": [
						52
					],
					"attributes": {
					}
				},
				"$abc$464$n60_1": {
					"hide_name": 1,
					"bits": [
						53
					],
					"attributes": {
					}
				},
				"$abc$464$n61": {
					"hide_name": 1,
					"bits": [
						54
					],
					"attributes": {
					}
				},
				"$abc$464$n62_1": {
					"hide_name": 1,
					"bits": [
						55
					],
					"attributes": {
					}
				},
				"$abc$464$n64": {
					"hide_name": 1,
					"bits": [
						56
					],
					"attributes": {
					}
				},
				"$abc$464$n65_1": {
					"hide_name": 1,
					"bits": [
						57
					],
					"attributes": {
					}
				},
				"$abc$464$n66_1": {
					"hide_name": 1,
					"bits": [
						58
					],
					"attributes": {
					}
				},
				"$abc$464$n68": {
					"hide_name": 1,
					"bits": [
						60
					],
					"attributes": {
					}
				},
				"$abc$464$n69": {
					"hide_name": 1,
					"bits": [
						61
					],
					"attributes": {
					}
				},
				"$abc$464$n70": {
					"hide_name": 1,
					"bits": [
						62
					],
					"attributes": {
					}
				},
				"$abc$464$n71_1": {
					"hide_name": 1,
					"bits": [
						63
					],
					"attributes": {
					}
				},
				"$abc$464$n72_1": {
					"hide_name": 1,
					"bits": [
						64
					],
					"attributes": {
					}
				},
				"$abc$464$n73": {
					"hide_name": 1,
					"bits": [
						65
					],
					"attributes": {
					}
				},
				"$abc$464$n74_1": {
					"hide_name": 1,
					"bits": [
						66
					],
					"attributes": {
					}
				},
				"$abc$464$n75_1": {
					"hide_name": 1,
					"bits": [
						67
					],
					"attributes": {
					}
				},
				"$abc$464$n76_1": {
					"hide_name": 1,
					"bits": [
						68
					],
					"attributes": {
					}
				},
				"$abc$464$n77_1": {
					"hide_name": 1,
					"bits": [
						69
					],
					"attributes": {
					}
				},
				"$abc$464$n78_1": {
					"hide_name": 1,
					"bits": [
						70
					],
					"attributes": {
					}
				},
				"$abc$464$n80_1": {
					"hide_name": 1,
					"bits": [
						72
					],
					"attributes": {
					}
				},
				"$abc$464$n81_1": {
					"hide_name": 1,
					"bits": [
						73
					],
					"attributes": {
					}
				},
				"$abc$464$n82_1": {
					"hide_name": 1,
					"bits": [
						74
					],
					"attributes": {
					}
				},
				"$abc$464$n83": {
					"hide_name": 1,
					"bits": [
						75
					],
					"attributes": {
					}
				},
				"$abc$464$n84_1": {
					"hide_name": 1,
					"bits": [
						76
					],
					"attributes": {
					}
				},
				"$abc$464$n85": {
					"hide_name": 1,
					"bits": [
						77
					],
					"attributes": {
					}
				},
				"$abc$464$n86_1": {
					"hide_name": 1,
					"bits": [
						78
					],
					"attributes": {
					}
				},
				"$abc$464$n87": {
					"hide_name": 1,
					"bits": [
						79
					],
					"attributes": {
					}
				},
				"$abc$464$n88_1": {
					"hide_name": 1,
					"bits": [
						80
					],
					"attributes": {
					}
				},
				"$abc$464$n90_1": {
					"hide_name": 1,
					"bits": [
						82
					],
					"attributes": {
					}
				},
				"$abc$464$n91": {
					"hide_name": 1,
					"bits": [
						83
					],
					"attributes": {
					}
				},
				"$abc$464$n92_1": {
					"hide_name": 1,
					"bits": [
						84
					],
					"attributes": {
					}
				},
				"$abc$464$n93": {
					"hide_name": 1,
					"bits": [
						85
					],
					"attributes": {
					}
				},
				"$abc$464$n94_1": {
					"hide_name": 1,
					"bits": [
						86
					],
					"attributes": {
					}
				},
				"$abc$464$n95": {
					"hide_name": 1,
					"bits": [
						87
					],
					"attributes": {
					}
				},
				"$abc$464$n96": {
					"hide_name": 1,
					"bits": [
						88
					],
					"attributes": {
					}
				},
				"$abc$464$n97": {
					"hide_name": 1,
					"bits": [
						89
					],
					"attributes": {
					}
				},
				"$abc$464$n98": {
					"hide_name": 1,
					"bits": [
						90
					],
					"attributes": {
					}
				},
				"$abc$464$n99": {
					"hide_name": 1,
					"bits": [
						91
					],
					"attributes": {
					}
				},
				"borrow_out": {
					"hide_name": 0,
					"bits": [
						15
					],
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:9"
					}
				},
				"carry_out": {
					"hide_name": 0,
					"bits": [
						14
					],
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:9"
					}
				},
				"clock": {
					"hide_name": 0,
					"bits": [
						2
					],
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:6"
					}
				},
				"count_out": {
					"hide_name": 0,
					"bits": [
						16,
						17,
						18,
						19,
						20,
						21,
						22,
						23,
						24
					],
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:8"
					}
				},
				"data_in": {
					"hide_name": 0,
					"bits": [
						3,
						4,
						5,
						6,
						7,
						8,
						9,
						10,
						11
					],
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:5"
					}
				},
				"down": {
					"hide_name": 0,
					"bits": [
						13
					],
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:6"
					}
				},
				"parity_out": {
					"hide_name": 0,
					"bits": [
						25
					],
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:9"
					}
				},
				"up": {
					"hide_name": 0,
					"bits": [
						12
					],
					"attributes": {
						"src": "tests/simple/fiedler-cooley.v:6"
					}
				}
			}
		}
	}
}
createSchematic('drawing',jsonData);