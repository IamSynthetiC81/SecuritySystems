#!/bin/bash

# Compile DH with flags
gcc ../DH_Key_exchange.c -o ../dh_assign_1 -lgmp -lpthread -g
sleep 1
# Wait for gcc to finish

# Run DH 500 times without printing anything on the terminal
for i in {1..500}
do
  ./../dh_assign_1 -o output.txt >/dev/null 2>&1
  if [ $? -ne 0 ]; then
  echo "non-threaded test"
  echo -ne '\n'
    echo -e "${RED}FAIL${ENDCOLOR}"
    exit 1
  fi
done


echo -e "${GREEN}Success${ENDCOLOR}"