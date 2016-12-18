# P-ray Engine (OpenCL+OpenGL)

P-ray Engine is an experimental ray tracing engine for rendering traditional polygon scenes. OpenCL is used to compute the frame rendering which is then displayed in an OpenGL window. Interoperability between OpenCL and OpenGL is used so that OpenCL can share memory with OpenGL and write directly to the framebuffer.

The source code currently available is still in the very early stages of development, it is incomplete and has several bugs, and lacks many features present in the [prototype engine](https://github.com/JacobBruce/Ray-Tracer-CPU-Prototype). The method it currently uses to render objects isn't very efficient, it needs to be recoded to apply the 2D acceleration structure discussed in this paper: 

http://j-d-b.net/files/Real-Time_Ray-Tracing_Methodology.pdf

The ultimate goal is to create a fast cross-platform ray tracing engine which uses OpenCL to do all the rendering calculations. I'm sharing the code now now since development is slow and it could be a useful resource for OpenCL/OpenGL interoperability. Since many features are lacking it shouldn't be very hard to understand and mess around with it.

Dependencies:

* OpenCL 1.2
* GLFW 3.2
* GLEW 2.0

You will also need the OpenCL C++ Wrapper from Khronos and you'll need a video card which supports the OpenGL sharing extension for OpenCL interoperability. The AMD APP SKD for OpenCL 1.2 includes many of the dependencies but some are outdated and will need to be replaced with newer versions.
