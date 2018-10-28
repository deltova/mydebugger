# ToyDebugger

This project is a debugger that target compiled language like C/C++.

It targets ELF binary and in the future Dwarf format for the debug info.

## Getting Started

To begin with this project you just could clone it from github.

### Prerequisites

To make it work you need libbfd.

### Installing

To compile the project:

```
mkdir build
cd build
cmake ..
make
```

It will create a binary called mygdb.

### How to use this debugger

Launch it:

```
./mygdb programm_to_debug
```

Access the helper:

```
mygdb> h
Available command:
        b $addr: set a breakpoint at $addr
        c: Continue to the next breakpoint
        h: print the helper of commands
        n: Go to next instruction
        p $register: print the value of the $register
        p 0xaddr: print the content at $addr
```

## Authors

* **Clement Magnard** - [Deltova](https://github.com/deltova)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
