# vOW4SIKE
## Parallel collision search for the cryptanalysis of SIKE

The vOW4SIKE project provides C code that implements the parallel collision search algorithm by van Oorschot and Wiener (vOW) [1]. The algorithm can be instantiated for generic collision finding and for solving the supersingular isogeny problem in the context of the Supersingular Isogeny Key Encapsulation protocol SIKE [2]. The goal of the project is to concretely assess the classical security of SIKE parameter sets. The code was used for the experiments to obtain the results described in more detail in the research paper [3].

The code was developed by [Microsoft Research](http://research.microsoft.com/) for experimentation purposes.

# Dependencies
- SIDH v3.0 (C Edition) (included)


## Installation instructions

## Contributors
- Fernando Virdia
- Joost Renes

## License
vOW4SIKE is licensed under the MIT License; see [`License`](LICENSE) for details.

# References
[1] Paul C. van Oorschot, Michael J. Wiener: Parallel Collision Search with Cryptanalytic Applications. Journal of Cryptology, 12(1):1–28, 1999.
[2] David Jao, Reza Azarderakhsh, Matthew Campagna, Craig Costello, Luca De Feo, Basil Hess, Amir Jalali, Brian Koziel, Brian LaMacchia, Patrick Longa, Michael Naehrig, Geovandro Pereira, Joost Renes, Vladimir Soukharev, and David Urbanik: SIKE: Supersingular Isogeny Key Encapsulation.
[`https://sike.org`](https://sike.org).
[3] Craig Costello, Patrick Longa, Michael Naehrig, Joost Renes, Fernando Virdia: Improved Classical Cryptanalysis of SIKE in Practice.
[`https://eprint.iacr.org/2019/298`](https://eprint.iacr.org/2019/298).


# Contributing

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.