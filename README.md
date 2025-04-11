# Spline Tool

An interactive OpenGL-based application for creating and editing **cubic Bézier splines** in real time. Built using GLFW, GLEW, and GLM, this tool allows users to intuitively place nodes, manipulate control points, and visualize dynamic spline curves with smooth rendering.

## Preview

![Spline Tool Demo](splineTool.gif)

## Features

- Click to add spline nodes on the canvas
- Adjustable control points for curvature editing
- Interior nodes use symmetric dual handles
- Real-time curve updates while dragging
- Rendered using 200 segments per Bézier segment
- Press `E` to reset the canvas

## Environment Setup

Follow these steps to configure your environment on Windows using MSYS2:

1. **Install MSYS2**  
   https://www.msys2.org/  
   Make sure to install it in `C:\msys64`

2. **Open the MSYS2 MINGW64 terminal**, then run:

   ```bash
   pacman -Syu
   ```

   _(The terminal will close after updating. Reopen the MINGW64 terminal to continue.)_

3. **Install required packages**:

   ```bash
   pacman -S mingw-w64-x86_64-toolchain \
               mingw-w64-x86_64-freeglut \
               mingw-w64-x86_64-glew \
               mingw-w64-x86_64-glfw \
               mingw-w64-x86_64-glm
   ```

4. **Update your PATH**  
   Add the following to your user environment variables:
   ```
   C:\msys64\mingw64\bin
   ```

## Build and Run

1. Clone the repository
   ```bash
   git clone https://github.com/NicoM-7/spline-tool.git
   cd spline-tool
   ```
2. Open `splineTool.cpp` in [Visual Studio Code](https://code.visualstudio.com/)
3. From the top menu, go to: `Terminal` → `Run Build Task`
4. Locate the built executable (`splineTool.exe`)
5. Run it via:
   ```bash
   .\splineTool.exe <width> <height>
   ```
   Example:
   ```bash
   .\splineTool.exe 1280 720
   ```
