var EventEmitter  = require('events').EventEmitter;
var MPR121Wrapper = require('bindings')('addon').MPR121Wrapper;
var util          = require('util');

function MPR121(address, opts) {
  // if address is not string, it's probably opts
  if (typeof address !== 'string') {
    opts    = address;
    address = undefined;
  }

  // update default opts settings
  opts = Object.assign({ interval: 10 }, opts);

  // turn this into EE
  EventEmitter.call(this);

  // start MPR121
  this.mpr121 = new MPR121Wrapper(address);

  // start emitting
  setInterval(function() {
    if (this.isRunning()) {
      var data = this.mpr121.step();
      this.emit('data', data);
    }
  }.bind(this), opts.interval);
}

// inherit EventEmitter prototype
util.inherits(MPR121, EventEmitter);

// add functions from C++ to wrapper prototype
[
  'setTouchThreshold',
  'setReleaseThreshold',
  'run',
  'stop',
  'reset',
  'isRunning',
  'isInited'
].forEach(function(key) {
  MPR121.prototype[key] = function(/* args */) {
    var args = Array.prototype.slice.call(arguments);
    return this.mpr121[key].apply(this.mpr121, args);
  };
});

// add sample period settings
MPR121.prototype.setSamplePeriod = function(targetSamplePeriod) {
  // find nearest power of two for given input (13 -> 16, 3 -> 4, ...)
  var samplePeriod = Math.pow(2, Math.ceil(Math.log(targetSamplePeriod) / Math.log(2)));

  // convert sample rate to power of two
  var samplePeriods = {
    1:   0,
    2:   1,
    4:   2,
    8:   3,
    16:  4,
    32:  5,
    64:  6,
    128: 7
  };

  // call to CPP function based on user input,
  // only if we can do this
  if (samplePeriods[samplePeriod]) {
    var selectedPeriod = samplePeriods[samplePeriod];
    this.mpr121.setSamplePeriod(selectedPeriod);
  }
};

// export wrapper
module.exports = MPR121;
