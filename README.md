<img src="https://raw.githubusercontent.com/mahilab/Syntacts/master/logo/logo_text.png" width="500">

Syntacts is a haptic rendering framework for vibrotactile feedback. It eliminates the need for expensive haptic controllers or custom electronics by leveraging commercial-off-the-shelf audio interfaces. As a complete package, Syntacts provides the software and hardware needed to interface audio devices with low latency, synthesize complex waveforms, and amplify signals to appropriate levels. To learn more, please visit the official website:

## [www.syntacts.org](https://www.syntacts.org/)

# Quick Start

You can find in-depth tutorials on the main [wesbite](https://www.syntacts.org/), but if you're in a hurry, follow the quick start guides below:

## Syntacts GUI
- Get the latest [Release](https://github.com/mahilab/Syntacts/releases) and extract the files.
- Run the GUI executable `syntacts-gui` in the top level directory. On Windows, you may receive a "Windows protected your PC" screen. Click "More info", then "Run anyway".

## C/C++ API
- Get the source code by pulling the `master` branch on the GitHub repository or from the latest [Release](https://github.com/mahilab/Syntacts/releases). 
- Build and install **Syntacts** for your system my following the [tutorial](tutorials.md).
- Use the [template](https://github.com/mahilab/Syntacts/tree/master/template) to make a new Syntacts project with CMake.

## C# API
- Get the latest [Release](https://github.com/mahilab/Syntacts/releases) and extract the files.
- Navigate to the `csharp` directory.
- From the command line, build the Syntacts library and then run each example you want to try out:
  ```shell
  > cd csharp/Syntacts
  > dotnet build
  > cd ../examples/example_basic
  > dotnet run
  ```
    
## Unity 
- Get the latest [Release](https://github.com/mahilab/Syntacts/releases) and extract the files.
- Import `unity/syntacts.unitypackage` to your project.
- Add the `SyntactsHub` script to a scene object.

OR

- Navigate to the `unity/SyntactsDemo/Assets` directory.
- Open `Demo.unity` in Unity.

## Python API
- Get the latest [Release](https://github.com/mahilab/Syntacts/releases) and extract the files.
- Navigate to the `python` directory.
- Run any of the `example.py` files.

# Having issues?
- Check the [Tutorials](tutorials.md) and [FAQ](faq.md) pages.
- Head over to the [Issues](https://github.com/mahilab/Syntacts/issues) page and tell us your problem.
