This is a managed wrapper around the native Judy array library. To find out more about Judy arrays, see http://judy.sourceforge.net.

To build the project:
1. Download the Judy array source. The latest version as of this writing was 1.0.5.
2. Extract the Judy array source to the solution directory (e.g. $(SolutionDir)\judy-1.0.5).
3. Rename the Judy source directory to 'judy' (e.g. $(SolutionDir)\judy).
4. Modify the JUDY_SOURCE\src\build.bat to build a static library.
   * Add /DPOCO_STATIC to COPT
   * Comment out the DLL link step
5. Build JudyArray.sln. Since Judy was linked as a static lib, there is no separate native judy dll. You only need to distribute JudyArray.dll.
