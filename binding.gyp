{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "addon.cc", "mpr121wrapper.cc" ],
      "include_dirs" : [
        "/usr/local/include"
      ],
      "link_settings": {
        "libraries": [
          "-lMPR121", "-lwiringPi", "-lwiringPiDev", "-lpthread", "-lm"
        ],
        "ldflags": [
          "-L/usr/local/lib"
        ]
      }
    }
  ]
}
