# node-picap

Node module for interfacing with the MPR121 on the [Bare Conductive Pi Cap](https://www.bareconductive.com/shop/pi-cap/).

[![Bare Conductive](http://bareconductive.com/assets/images/LOGO_256x106.png)](http://www.bareconductive.com/)

## Installation
This library requires Node.js v6.7.0 or higher and also requires that the [Bare Conductive MPR121 Wiring Pi Library](https://github.com/BareConductive/wiring-pi-mpr121) be installed.

If you're using a Raspberry Pi, this is most easily achieved by running

```sh
sudo apt-get install picap
```
which will install this module along with lots of example code and setup utilities that will help you get the most out of your Pi Cap.


If you're a masochist, start with


```sh
npm install node-picap
```

## Usage

### Simple Touch example

```js
var MPR121 = require('node-picap');
var mpr121;

// correct address for the Pi Cap - other boards may vary
mpr121 = new MPR121('0x5C');

mpr121.on('data', function(data) {
  data.forEach(function(electrode, i) {
    if (electrode.isNewTouch) {
      console.log('electrode ' + i + ' was just touched');
    }
    else if (electrode.isNewRelease) {
      console.log('electrode ' + i + ' was just released');
    }
  });
});
```

### Datastream example

```js
var MPR121 = require('node-picap');
var mpr121;

// note that interval sets the interval (in ms) at which 
// the module attempts to emit data events - the MPR121 
// hardware sets a lower limit on this - default is 10ms, 
// can set down to 1ms
mpr121 = new MPR121('0x5C', { interval: 10 });

mpr121.on('data', function(data) {
  // split out each of the various data streams...
  var touch = data.map(function(electrode) { return electrode.isTouched ? 1 : 0; });
  var tths  = data.map(function(electrode) { return electrode.touchThreshold; });
  var rths  = data.map(function(electrode) { return electrode.releaseThreshold; });
  var fdat  = data.map(function(electrode) { return electrode.filtered; });
  var bval  = data.map(function(electrode) { return electrode.baseline; });
  var diff  = data.map(function(electrode) { return electrode.baseline - electrode.filtered; });

  // ...and send them out via stdout - simples!
  console.log('TOUCH: ' + touch.join(' '));
  console.log('TTHS: ' + tths.join(' '));
  console.log('RTHS: ' + rths.join(' '));
  console.log('FDAT: ' + fdat.join(' '));
  console.log('BVAL: ' + bval.join(' '));
  console.log('DIFF: ' + diff.join(' '));
});
```

## License

  This work is licensed under a [MIT license](https://opensource.org/licenses/MIT)

  Copyright (c) 2016-2017, Bare Conductive
