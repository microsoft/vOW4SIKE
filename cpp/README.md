# vOW4SIKE++

This library aims to provide an extensible implementation of the van Oorschot-Wiener (vOW) parallel collision finding algorithm, aiming to match the efficiency of the C implementation, while being easier to extend to other target collision problems.
The core implementation is written in C++ and should compile on both Windows and Linux. Python2 wrappers are provided to make testing and statistics generation easier on Linux.

For instructions on dependencies, external source code, experiment generation, contributions and licensing, consult the C implementation's [/README.md](/README.md).

### C++ interface

We currently only re-implement the "`gen`" random step function based on AES-NI.

#### Linux

To compile and run the code on Linux, use the following commands from the `/cpp` directory in the repository.
The code assumes a 64-bit architecture is being targeted.

```bash
make bin/main
./bin/main -h
```

#### Windows

To compile and run the C++-only code on Windows:

1. Open `cpp/msvc/vOW++.sln` in Visual Studio 2015 (the Community Edition is sufficient).
2. Choose the `Release` configuration, and build the solution.
3. Execute `vOW++` to run experiments on a generic random function.

### Python 2 interface

To compile the Python 2 wrappers, and run experiments using our Python scripts, run the following code from `/cpp`.

A copy of `numpy.i` should be located in `/cpp/swig/numpy.i`. See [/README.md](/README.md) for instructions.

#### Linux

```bash
make swig
cd python
python2 gen.py -h
```

More details on the Python scripts can be found in [/cpp/python/README.md](/cpp/python/README.md).

## Reproducing results

A list of commands to reproduce our experiments can be found in [/cpp/data/EXPERIMENTS.md](/cpp/data/EXPERIMENTS.md).

Instructions for reproducing the tables and figures in [6] can be found in [/data/PAPER.md](/data/PAPER.md).

## Implementation details

### Directory structure

- `msvc` contains a Visual Studio 2017 solution file to compile the C++ library on Windows
- `python` provides example code to run experiments using the Python2 wrappers
- `src`
    - `types`, `utils`, `.` contain the C++ implementation of vOW
    - `memory` contains code implementing the large memory bank used to store distinguished points. Implementations need to adhere to the interface file provided.
    - `prng` contains code implementing the PRNG used by the attack. Implementations need to adhere to the interface file provided.
    - `dependencies` contains external library code needed. E.g. it may contain an implementation of a function we are trying to find collisions for.
    - `gen` contains the implementation of an example random function to attack.
    - `templating` contains code necessary to instantiate the templated `vOW` class to run the attack.
- `swig` contains the SWIG template files used to generate the Python2 wrappers

### Implementing f and S

The vOW algorithm finds collisions in a random function `f : S -> S` over some set of points `S`. A general random function is provided as an example in `/cpp/src/gen`.

To attack a different specific collision problem, for example a SIKE secret isogeny recovery problem, one can implement a module with the same interfaces as in `/cpp/src/gen`. "Point", "RandomFunction" and "Instances" classes have to be written, implementing the points in `S`, and `f`, and code to instantiate the problem from specific parameters.

Once the implementation is written, the files in `templating` should be updated to include appropriate templating of the vOW class over the new `f` and `S`.

### Implementing memory drivers

`/cpp/src/memory` contains an interface to store collisions in RAM. The idea of using explicitly writing this interface is to later allow implementing a similar "driver" for storing collisions on a (possibly distributed) remote data structure.




