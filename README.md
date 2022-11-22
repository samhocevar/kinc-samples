# Kinc samples

This repository contains some simple programs that use the [Kinc](https://github.com/Kode/Kinc) library.

They have all been tested on the following platforms:
 - Linux (x64)
 - Windows (x64)
 - PlayStation 5
 - Xbox One
 - Xbox Series S|X
 - Nintendo Switch

## `plasma`

<img src="https://github.com/samhocevar/kinc-samples/raw/main/plasma.png" height="320"/>

A simple plasma effect. Uses old-school palette rotation.

Tests the following features:
 - referencing two texture units in the same shader
 - point texture filtering
 - linear texture filtering
 - grey8 texture data
 - rgba32 texture data

## `gamepad`

<img src="https://github.com/samhocevar/kinc-samples/raw/main/gamepad.png" height="320"/>

A visualiser for gamepad state. Uses SDF 2D vector rendering.

Tests the following features:
 - up to 16 gamepad buttons
 - up to 4 gamepad axes
