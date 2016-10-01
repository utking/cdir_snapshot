# cdir_snapshot

C-program traversing a directory and writing its content into a listing file

## Prerequisites

- CMake

## Installation

```sh
$ cmake && make
```

## Usage

```sh
// show usage info
$ ./cdir_snapshot -h

// create a snapshot for the current directory
$ ./cdir_snapshot .

// create a snapshot for the current directory in verbose mode
$ ./cdir_snapshot . -v
```

## License

This code uses the [ISC License](https://opensource.org/licenses/ISC)

