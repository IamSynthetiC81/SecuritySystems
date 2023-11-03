# Diffie & HELLMAN Key Exchange Algorithm and Implementation

## Introduction
This algorithm implements the Diffie & Hellman algorithm in C. 
1. DH_Key_exchange.c : This is the simpler implementation of the algorithm.
2. DH_Key_exchange_threaded.c : This is a more complex implementation of the algorithm using threads to simulate true client to client communication. (This was done mostly for fun !!! yaeh i'm fun at parties)

## Theory

The Diffie & Hellman algorithm is a key exchange algorithm that allows two parties to exchange a secret key over an insecure channel. The algorithm is based on the discrete logarithm problem. The algorithm is as follows:

1. Alice and Bob agree on a prime number $p$ and a base $g$.
2. Alice selects a random number $a$ and calculates $A = g^a mod p$.
3. Bob selects a random number $b$ and calculates $B = g^b mod p$.
4. Alice and Bob exchange $A$ and $B$.
5. Alice calculates $s = g^{ab} mod p$.
6. Bob calculates $s = g^{ab} mod p$.
7. Alice and Bob now have a shared secret key **s**.

## Compilations Parameters
When compiled the program can take the following flags
- -**DDEBUG** : Prints statements about each thread and its 
    state to the console. (*Only for the threaded implementation*) 
- -**DVERBOSE**   : Prints the various variables used in the
        algorithm to the console.

The program is compiled with the following command:

```gcc -o dh_assign_1 DH_Key_exchange.c -lgmp -g -DVERBOSE```

or

```gcc -o dh_assign_1_threaded DH_Key_exchange_threaded.c -lgmp -lpthread -g -DVERBOSE -DDEBUG```

Alternatively, both programs can also be compiled through the makefile with the following command:

```make```

## Runtime Parameters

The program is run with the following command:

```./dh_assign_1 [-o filename] [-p prime] [-g base] [-a host_key] [-b client_key]```
```./dh_assign_1_threaded [-o filename] [-p prime] [-g base] [-a host_key] [-b client_key]```

Options:
- **-o**      :   Write the output to the specified file.
- **-p**      :   Use the specified prime number
- **-g**      :   Use the specified base number
- **-a**      :   Use the specified private host key
- **-b**      :   Use the specified private client key

>if any of {**-o**,**-p**,**-g**,**-a**,**-b**} is ommited, the program will 
select a random number for that option.
>
>!!! Each implementation can take the same compilation flags and the same runtime parameters, all of which are explained with ```./dh_assign_1 -h``` or ```./dh_assign_1_threaded -h```.

## How to run
Compile the code using the following command:

```make```

this will create two executables:

```dh_assign_1``` and ```dh_assign_1_threaded```

Run the executables using the following command:

```./dh_assign_1``` or ```./dh_assign_1_threaded```
## Misc

Alternatively, in the etc/ directive there is a bash script that can be used to test the program. The script can be run with the following commands:

``` cd/etc```

```chmod +x test.sh```

```./test.sh```
 
 this will run the program 500 times and catch any mismatches between the two secret keys. Also the generated numbers are saved in the etc/output.txt file.