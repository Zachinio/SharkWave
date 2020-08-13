
Requirements:
1.Download protobuf compiler. (protoc) add it to $PATH - VERSION 3.11.4 ( lib and compiler must match).


Compile libprotobuf-lite.a:
1.Download the specific version of protobuf runtime from Github for C++.
2.Download the NDK toolchain you want to compile with or use the one in the ../Sdk/ndk-bundle, Follow the Guide in /Protobuf/Protobuf-NDK tutorial.html
3.Install the requirements in the Github tutorial (cmake ,autoconf..)
4.Set these variables in your terminal: https://developer.android.com/ndk/guides/other_build_systems#autoconf
5.Run the configure with "--disabled-shared" to make static lib in addition to the shared libs.
6.Run make - you can find the libs in /usr/local/lib ( unless you defined prefix which defined where to build).

The final ELF executable is in build dir /Main/ , the file's name is "Cheser".


