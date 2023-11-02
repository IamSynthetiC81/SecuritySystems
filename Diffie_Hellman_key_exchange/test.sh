#!/bin/bash

# Compile DH with flags
gcc -o dh_assign_1 Diffie_Hellman_Key_Exchange.c -lgmp -lpthread -g

# Run DH 200 times
for i in {1..200}
do

  echo -n "$i "   
  ./dh_assign_1 -o output.txt
  sleep 1
  if [ $? -ne 0 ]; then
    echo "Failure"
    exit 1
  fi
done

echo "Success"


