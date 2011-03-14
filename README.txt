This is a managed wrapper around the native Judy array library. To find out more about Judy arrays, see http://judy.sourceforge.net.

See Downloads in GitHub for pre-built libraries for x86 and x64.

To build:
1. git submodule update
2. Open a command prompt and cd to JudyArray\judy\src
3. Execute vcvars.bat to setup the build environment
	* For x86: VS_HOME\VC\bin\vcvars32.bat
	* For x64: VS_HOME\VC\bin\amd64\vcvarsamd64.bat
4. Run JudyArray\judy\src\static_build.bat to build Judy.lib static link library
4. Build JudyArray.sln Win32 or x64 to build the JudyArray assembly

