immsocket
===============

socket library which can be used immediately (sock_stream) (unix_domain/inet_domain)


How to build
--------

Clone and make.

	$ git clone https://github.com/ysan/immsocket
	$ cd immsocket
	$ make

Then shared libraries is created.

	$ tree -P *.so
	.
	├── immsocket
	│   └── libimmsocket.so
	├── immsocketcommon
	│   └── libimmsocketcommon.so
	└── immsocketservice
	    └── libimmsocketservice.so

Example of use (sample codes)
--------

Please refer to the following code.

	$ ls -1 example/server/ example/client/
	example/client/:
	Android.mk
	ClMessageHandler.cpp
	ClMessageHandler.h
	Makefile
	main.cpp
	run.sh
	
	example/server/:
	Android.mk
	Makefile
	SvrClientHandler.cpp
	SvrClientHandler.h
	SvrMessageHandler.cpp
	SvrMessageHandler.h
	main.cpp
	run.sh

Component diagram
------------
![component diagram](https://github.com/ysan/immsocket/blob/master/etc/component_diagram.png)

Class diagram
------------
![class diagram](https://github.com/ysan/immsocket/blob/master/etc/class_diagram.png)

Packet format
------------
![packet format](https://github.com/ysan/immsocket/blob/master/etc/packet_format.png)


Platforms
------------
Generic Linux will be ok. (confirmed worked on Ubuntu, Fedora)

