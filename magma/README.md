# vOW4SIKE - Magma code

This Magma code supports the C code for testing purposes. We emphasize that this Magma code is OUTDATED and does not align with the current version of the C code, but still contains a working version of the vOW algorithm. We have included it since it might prove useful to others.

The algorithm can be executed by running the vOW.m script in Magma. Before doing so, one should uncomment of the lines

* 13+14: Run the SIKE version
* 16+17: Run the SIDH version
* 19+20: Run the Generic version

Moreover, this script is used to print parameters sets that can be copied directly into the C header files. This is done at the bottom of the vOW.m scripts, which uses print functions defined in print.m
