# LatticeBitFlipping
An implementation of the extended bit flipping algorithm.

This raw implementation of the extended bit flipping algorithm was used to estimate the decryption failure rate (DFR) of ["Ouroboros-E: An efficient lattice-based key-exchange protocol"](https://deneuville.github.io/publication/2018-05-08).

Details about both the key exchange protocol and the extended bit flipping algorithm can be found in the paper.

## Compilation

This piece of software should compile on Linux systems using the traditional `make` command. It will produce an executable file `lbf`.

## Usage

To run this piece of software, use:
```
./lbf n q D_thr number_of_lattices iterations_per_lattice
```

Where the parameters should be:
1. `n` the lattice dimension,
1. `q` the modulus,
1. `D_thr` a threshold for the lattice bit flipping algorithm,
1. `number_of_lattices` the number of lattices are tried,
1. `iterations_per_lattice` the number of randomness tried per lattice,

See Tab. 1 of [Ouroboros-E: An efficient lattice-based key-exchange protocol](https://deneuville.github.io/publication/2018-05-08) for more details on the parameters



