#!/bin/bash

# Compile DH with flags
gcc ../DH_Key_exchange.c -o ../dh_assign_1 -lgmp -lpthread -g
gcc ../DH_Key_exchange_threaded.c -o ../dh_assign_1_threaded -lgmp -lpthread -g
sleep 1
# Wait for gcc to finish

# Run DH 500 times without printing anything on the terminal
echo -n "Non-Threaded : "
for i in {1..500}
do
  ./../dh_assign_1 -o output.txt >/dev/null 2>&1
  if [ $? -ne 0 ]; then
  
  echo -ne '\n'
    echo -e "${RED}FAIL${ENDCOLOR}"
    exit 1
  fi
done

echo -e "${GREEN}Success${ENDCOLOR}"

# Run DH 500 times without printing anything on the terminal
echo -n "Threaded : "
for i in {1..500}
do
  ./../dh_assign_1_threaded -o output.txt >/dev/null 2>&1
  if [ $? -ne 0 ]; then
  
  echo -ne '\n'
    echo -e "${RED}FAIL${ENDCOLOR}"
    exit 1
  fi
done

echo -e "${GREEN}Success${ENDCOLOR}"