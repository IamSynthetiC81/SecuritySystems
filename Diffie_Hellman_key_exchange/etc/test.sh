#!/bin/bash

# Compile DH with flags
gcc ../Diffie_Hellman_Key_Exchange.c -o ../dh_assign_1 -lgmp -lpthread -g
sleep 1
# Wait for gcc to finish

# Run DH 200 times
for i in {1..1000}
do

  echo -n "$i "   
  ./../dh_assign_1 -o output.txt
  sleep 0.001
  if [ $? -ne 0 ]; then
    echo "Failure"
    exit 1
  fi
done

echo "Success"


