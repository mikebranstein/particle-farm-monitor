# particle-farm-monitor

A Particle project to monitor my farm. Components used:
- Temperature probe: DS18B20 (https://www.sparkfun.com/products/11050?_ga=2.245278917.1908945936.1500347143-1779639555.1497142554)
- Soil moisture probe: (https://www.sparkfun.com/products/13322?_ga=2.185709932.413155604.1500572398-1779639555.1497142554_)

## Pre-requisites

Install the Particle.io CLI:

```
> npm install -g particle-cli
```

## To use

Run `npm update`:

```
> npm update
```

Compile with `npm run build`:

```
> npm run build
```

This places a binary (.bin) file in the root of the project. Flash this to the Electron via USB. Before you flash, place the photon/electron in upload mode by holding the *Mode* button down while *Reset* is pressed. Release the *Mode* button when the particle D7 LED flashes yellow.

```
> particle flash --usb {filename}.bin
```

## To debug

Initialize the weather service with a *true* value to place it in debug mode. Debug mode will pause for serial port input before running code. 

Interact with the particle over serial:

```
> sudo cu -s 9600 -l /dev/tty.usbmodem1411
```
 
or

```
> npm run monitor
```

## Welcome to your project!

Every new Particle project is composed of 3 important elements that you'll see have been created in your project directory for particle-farm-monitor.

#### ```/src``` folder:  
This is the source folder that contains the firmware files for your project. It should *not* be renamed. 
Anything that is in this folder when you compile your project will be sent to our compile service and compiled into a firmware binary for the Particle device that you have targeted.

If your application contains multiple files, they should all be included in the `src` folder. If your firmware depends on Particle libraries, those dependencies are specified in the `project.properties` file referenced below.

#### ```.ino``` file:
This file is the firmware that will run as the primary application on your Particle device. It contains a `setup()` and `loop()` function, and can be written in Wiring or C/C++. For more information about using the Particle firmware API to create firmware for your Particle device, refer to the [Firmware Reference](https://docs.particle.io/reference/firmware/) section of the Particle documentation.

#### ```project.properties``` file:  
This is the file that specifies the name and version number of the libraries that your project depends on. Dependencies are added automatically to your `project.properties` file when you add a library to a project using the `particle library add` command in the CLI or add a library in the Desktop IDE.

## Adding additional files to your project

#### Projects with multiple sources
If you would like add additional files to your application, they should be added to the `/src` folder. All files in the `/src` folder will be sent to the Particle Cloud to produce a compiled binary.

#### Projects with external libraries
If your project includes a library that has not been registered in the Particle libraries system, you should create a new folder named `/lib/<libraryname>/src` under `/<project dir>` and add the `.h` and `.cpp` files for your library there. All contents of the `/lib` folder and subfolders will also be sent to the Cloud for compilation.

## Compiling your project

When you're ready to compile your project, make sure you have the correct Particle device target selected and run `particle compile <platform>` in the CLI or click the Compile button in the Desktop IDE. The following files in your project folder will be sent to the compile service:

- Everything in the `/src` folder, including your `.ino` application file
- The `project.properties` file for your project
- Any libraries stored under `lib/<libraryname>/src`