# Assignment 2
## Access Controll Logging Tool

### Description

The access controll logging system monitors and keeps track of every file access and modification that occurs in the system.

> Warning :warning:
> 
> Systemwide logging can throw errors (notably in the case of 'gcc') that are not related to the program itself, but to the nature of the temp files these commands use. 
>
> Solution ❕
> 
> It is best to disable the preloading before running these commands.

### Compilation

to compile the program, run the following command in the root directory of the project:

```bash
make clean
make
```

### Execution

the program runs in the backround. The systemwide can be enabled and disabled using the following commands:

```bash
LD_PRELOAD=./out/libmylib.so    // Enables the logging
LD_PRELOAD=                     // Disables the logging
```

alternatively, logging can be enabled for a single program by running the following command:

```bash
LD_PRELOAD=./out/libmylib.so <program>
```

### Testing

The program can be tested with the following commands:

```bash
make test
```

or 
```bash
./out/aclog   // this runs make test
```

### Documentation

For further documentatin a doxygen generated report is located in the doc directory


