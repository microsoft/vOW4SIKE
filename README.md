# vOW4SIKE - Parallel Collision Search for the Cryptanalysis of SIKE

The vOW4SIKE project provides C code that implements the parallel collision search algorithm by van Oorschot and Wiener (vOW) [1].
The algorithm can be instantiated for generic collision finding and for solving the supersingular isogeny problem in the context of
the Supersingular Isogeny Diffie-Hellman key exchange protocol SIDH [2,3], as well as the Supersingular Isogeny Key Encapsulation protocol SIKE [4].
The goal of the project is to concretely assess the classical security of SIKE parameter sets. The code was used for the experiments to obtain the results described in more detail in the research paper [6], to reproduce the results for SIDH presented in [5], and was developed for experimentation purposes only.

An extensible C++ version is also provided (in `cpp/`) that can be used to extend the attack more easily to other target collision problems.

## Installation instructions

### Dependencies

These are the main dependencies required to either run the experiments or generate the plots in [6]:

- SIDH v3.2 (C Edition) (included)
- OpenMP 4.5
- Sagemath 8.9

Linux only:
- SWIG 3.0
- python-numpy
- python-psutil
- libpython2.7-dev

Windows only:
- Visual Studio 2015

### External source dependencies

Some dependencies are in the form of open source software source code.
Instructions on how to fetch them are provided below.

#### Numpy SWIG interface file

This is a single source file that can be obtained [here](https://github.com/numpy/numpy/blob/maintenance/1.18.x/tools/swig/numpy.i) and should be saved as `/swig/numpy.i`.
An archived copy can be found [here](http://web.archive.org/web/20200402001801/https://github.com/numpy/numpy/blob/maintenance/1.18.x/tools/swig/numpy.i).

#### xxHash

The xxHash non-cryptographic hash function can be optionally used for the XOF for improved performance.

To enable this option uncomment the define for USE_XXHASH_XOF in `\src\prng.h`, comment out the define for USE_AES_XOF,
and then download the source code by running the following command from `/src/`:

```
git clone https://github.com/Cyan4973/xxHash.git xxhash
cd xxhash
git checkout 0f2dd4a1cb103e3fc8c55c855b821eb24c6d82c3
```

### C interface

vOW4SIKE includes a C implementation of the parallel collision search algorithm vOW using OpenMP, together with Python 2 wrappers (written using SWIG).
This repository also provides the Python scripts used to generate the results in [6], together with output data, and scripts to generate the tables and figures in [6].
The parallel collision algorithm can be run with different instantiations of the random step function in vOW:

- `gen`: a generic random function based on either AES or xxHash,
- `sidh`: a SIDH-related random function computing fixed-length walks in the 2-isogeny graph,
- `sike`: a SIKE-related random function computing fixed-length walks in the 2-isogeny graph.

To change the instantiation of the random functions attacked in the C code, edit `tests/test_vOW_gen.c`, `tests/test_vOW_SIDH.c` or `tests/test_vOW_SIKE.c`. In the case of SIDH and SIKE, to change the field characteristic in the C experiments, edit the `P128_PRIME` variable in the Makefile.

#### Linux

To compile and run the assembly-optimized code on Linux, use the following commands from the root directory in the repository.
The code assumes a 64-bit architecture is being targeted.

```bash
make tests_vow
./test_vOW_gen -h

make tests_vow_sidh
./test_vOW_SIDH_128 -h
./test_vOW_SIDH_434 -h

make tests_vow_sike
./test_vOW_SIKE_128 -h
./test_vOW_SIKE_434 -h
```

#### Windows

To compile and run the C-only code on Windows:

1. Open `Visual Studio/SIDH/SIDH.sln` in Visual Studio 2015 (the Community Edition is sufficient).
2. Choose the `Fast` configuration, and build the solution.
3. Execute `test-vOW_gen` (resp. `test-vOW_SIDH`, `test-vOW-SIKE`) to run experiments on a generic random function (resp. SIDH, SIKE).

### Python 2 interface

To compile the Python 2 wrappers, and run experiments using our Python scripts, run the following code from `/python`.

**NOTE**: when building the interface, sometimes `ld` struggles finding the OpenMP shared objects. To address this, the `Makefile` contains the following line towards the beginning, that tries to point directly to the library. Yours may be located at a different path, requiring a change to be made to `LDOMP`:

```bash
LDOMP=-L/usr/lib/x86_64-linux-gnu/ -l:libgomp.so.1
```

#### Linux

```bash
make gen
python2 gen.py -h

make sidh128
python2 sidh.py -h

make sidh434
python2 sidh.py -h

make sike128
python2 sike.py -h

make sike434
python2 sike.py -h
```

More details on the Python scripts can be found in [python/README.md](python/README.md).

## Reproducing results

A list of commands to reproduce our experiments can be found in [data/EXPERIMENTS.md](data/EXPERIMENTS.md).

Instructions for reproducing the tables and figures in [6] can be found in [data/PAPER.md](data/PAPER.md).

## C++ implementation

Information about the C++ implementation of the attack against a generic AES-NI random function can be found in [cpp/README.md](cpp/README.md).

## Contributors

- Fernando Virdia
- Joost Renes

## License

vOW4SIKE is licensed under the MIT License; see [`License`](LICENSE) for details.

## References

[1] Paul C. van Oorschot, Michael J. Wiener: Parallel Collision Search with Cryptanalytic Applications. Journal of Cryptology, 12(1):1–28, 1999.

[2] David Jao, Luca De Feo: Towards quantum-resistant cryptosystems from supersingular elliptic curve isogenies, PQCrypto 2011, pp. 19-34, [`https://dx.doi.org/10.1007/978-3-642-25405-5_2`](doi:10.1007/978-3-642-25405-5_2).

[3] David Jao, Luca De Feo, Jérôme Plût: Towards quantum-resistant cryptosystems from supersingular elliptic curve isogenies, J. Math. Cryptol. 8(3):209-247, 2014, [`https://eprint.iacr.org/2011/506`](https://eprint.iacr.org/2011/506).

[4] David Jao, Reza Azarderakhsh, Matthew Campagna, Craig Costello, Luca De Feo, Basil Hess, Amir Jalali, Brian Koziel, Brian LaMacchia, Patrick Longa, Michael Naehrig, Geovandro Pereira, Joost Renes, Vladimir Soukharev, David Urbanik: SIKE: Supersingular Isogeny Key Encapsulation,
[`https://sike.org`](https://sike.org).

[5] Gora Adj, Daniel Cervantes-Vázquez, Jesús-Javier Chi-Domínguez, Alfred Menezes, Francisco Rodríguez-Henríquez: On the cost of computing isogenies between supersingular elliptic curves, SAC 2018, pp. 322-343,
[`https://eprint.iacr.org/2018/313`](https://eprint.iacr.org/2018/313).

[6] Craig Costello, Patrick Longa, Michael Naehrig, Joost Renes, Fernando Virdia: Improved Classical Cryptanalysis of SIKE in Practice, PKC 2020,
[`https://eprint.iacr.org/2019/298`](https://eprint.iacr.org/2019/298).

## Contributing

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
