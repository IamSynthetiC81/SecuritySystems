
#!/bin/bash
echo "Cleaning up testfiles"
rm -f test/.testfiles/*

# Create test/.testfilesfiles folder if it doesn't exist
mkdir -p test/.testfiles


echo "Creating test files " 

# Create user_read.c file and set permissions
touch test/.testfiles/user_read.c
echo "echo user_read sallutes you !!!" > test/.testfiles/user_read.c
chmod 400 test/.testfiles/user_read.c

# Create user_write.c file and set permissions
touch test/.testfiles/user_write.c
echo "echo user_write sallutes you !!!" > test/.testfiles/user_write.c
chmod 200 test/.testfiles/user_write.c

# Create user_execute.c file and set permissions
gcc -o test/.testfiles/user_execute etc/template.c
chmod 100 test/.testfiles/user_execute

echo -n "."

# Create user_read_write.c file and set permissions
touch test/.testfiles/user_read_write.c
echo "echo user_read_write sallutes you !!!" > test/.testfiles/user_read_write.c
chmod 600 test/.testfiles/user_read_write.c

# Create user_read_execute.c file and set permissions
gcc -o test/.testfiles/user_read_execute etc/template.c
chmod 500 test/.testfiles/user_read_execute

# Create user_write_execute.c file and set permissions
gcc -o test/.testfiles/user_write_execute etc/template.c
chmod 300 test/.testfiles/user_write_execute

echo -n "."

# Create group_read.c file and set permissions
touch test/.testfiles/group_read.c
echo "echo group_read sallutes you !!!" > test/.testfiles/group_read.c
chmod 040 test/.testfiles/group_read.c

# Create group_write.c file and set permissions
touch test/.testfiles/group_write.c
chmod 020 test/.testfiles/group_write.c

# Create group_execute.c file and set permissions
gcc -o test/.testfiles/group_execute etc/template.c
chmod 010 test/.testfiles/group_execute

echo -n "."

# Create group_read_write.c file and set permissions
touch test/.testfiles/group_read_write.c
chmod 060 test/.testfiles/group_read_write.c

# Create group_read_execute.c file and set permissions
gcc -o test/.testfiles/group_read_execute etc/template.c
chmod 050 test/.testfiles/group_read_execute

# Create group_write_execute.c file and set permissions
gcc -o test/.testfiles/group_write_execute etc/template.c
chmod 030 test/.testfiles/group_write_execute

echo -n "."

# Create other_read.c file and set permissions
touch test/.testfiles/other_read.c
chmod 004 test/.testfiles/other_read.c

# Create other_write.c file and set permissions
touch test/.testfiles/other_write.c
chmod 002 test/.testfiles/other_write.c

# Create other_execute.c file and set permissions
gcc -o test/.testfiles/other_execute etc/template.c
chmod 001 test/.testfiles/other_execute

echo -n "."

# Create other_read_write.c file and set permissions
touch test/.testfiles/other_read_write.c
chmod 006 test/.testfiles/other_read_write.c

# Create other_read_execute.c file and set permissions
gcc -o test/.testfiles/other_read_execute etc/template.c
chmod 005 test/.testfiles/other_read_execute

# Create other_write_execute.c file and set permissions
gcc -o test/.testfiles/other_write_execute etc/template.c
chmod 003 test/.testfiles/other_write_execute

echo "."

echo "PRELOADING libmylib.so"
export LD_PRELOAD=./out/libmylib.so 