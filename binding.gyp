{
  "targets": [
    {
      "target_name": "hello",
      "sources": [ "hello.cc",
                    "anObject.cc",
                    "async_fcb.cc",
                    
                    "async_fcb.h",
                    "anObject.h",
       ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "./",
        "C:/XFSManager SDK/INCLUDE/"
      ],
      "conditions":[
        [
          'OS=="win"',{
            #'cflags':['/WX', '/m']
          }
        ],
      ],
      "link_settings":{
        "libraries": [
          "msxfs.lib",
          "xfs_supp.lib"
        ],
        "library_dirs": [
          "C:/XFSManager SDK/LIB/"
        ],
      },
      "defines": [

      ]
    }
  ]
}
