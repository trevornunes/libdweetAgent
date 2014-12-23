Description:

 libdweetAgent.so
 C/C++ dweet.io wrapper library that uses cURL and cJSON

 dweet.io is a 'machine 2 machine' twitter like service. Small payloads can be pushed or streamed in JSON format to 
 the dweet.io service and any client can read back this data. This allows connectivity been different devices, operating systems
 for pushing/pulling data in realtime.

 -  Uses cURL, cJSON to connect and communicate with dweet.io
 -  Provides high level API for connect, disconnect, getItem, setItem etc.

 src/       C/C++ library
 examples/  simple push/pull apps.

 Compile:

	 mkdir mybuild
	 cd mybuild
	 cmake ..
	 make

 Install:

 	make install

 Test:

	 cd Examples
	 ./dweetWriter
	 ./dweetReader


 Bugs:
 	get("item")  throwing core's around like crazy, null ptr de-reference somewhere ...

 TBD:  
 			locks
 			multithread support ( it's safe but serialized with dumb locks currently )
	    'set' post API entities and JSON payload 'builders'
		   event wakeup upon change CB notify
