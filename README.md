# P-ray Engine (OpenCL+OpenGL)

P-ray Engine is an experimental ray tracing engine for rendering traditional polygon scenes. OpenCL is used to compute the frame rendering which is then displayed in an OpenGL window. Interoperability between OpenCL and OpenGL is used so that OpenCL can share memory with OpenGL and write directly to the framebuffer.

The source code currently available is still in the very early stages of development, it is incomplete and has several bugs, but feel free to mess around with it. The method it currently uses to render objects isn't very efficient, it needs to be recoded to apply the 2D acceleration structure discussed in this paper: 

http://j-d-b.net/files/Real-Time_Ray-Tracing_Methodology.pdf

Dependencies:

* OpenCL 1.2
* GLFW 3.2
* GLEW 2.0

You will also need the OpenCL C++ Wrapper from Khronos and you'll need a video card which supports the OpenGL sharing extension for OpenCL interoperability. The AMD APP SKD for OpenCL 1.2 includes many of the decencies but some are outdated and will need to be replaced with newer versions.

The ultimate goal is to create a fast cross-platform ray tracing engine which uses OpenCL to do all the rendering calculations. However it's not yet very fast nor is the code cross-platform compatible but it shouldn't be difficult to remove all the Windows specific code when the time comes.
