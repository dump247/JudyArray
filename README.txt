This is a managed wrapper around the native Judy array library. To find out more about Judy arrays, see http://judy.sourceforge.net.

See Downloads in GitHub for pre-built libraries for x86 and x64.

To build:
1. Checkout the native Judy code at https://github.com/dump247/judy to $(SolutionDir)\judy and switch to the x64 branch
2. Open a command prompt and cd to $(SolutionDir)\judy
3. Execute vcvars.bat to setup the build environment
	* For x86: VS_HOME\VC\bin\vcvars32.bat
	* For x64: VS_HOME\VC\bin\amd64\vcvarsamd64.bat
3. Run $(SolutionDir)\judy\src\static_build.bat to build Judy.lib
4. Build JudyArray.sln Win32 or x64 to build the JudyArray assembly

