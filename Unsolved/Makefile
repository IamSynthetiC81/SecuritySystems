flags = -ansi -pedantic-errors -Wall -Wextra -g
flags_secire = -ansi -fno-stack-protector -g 
Greeter: Greeter.c
	gcc $(flags) Greeter.c -o Greeter

secure_greeter: Greeter_secure.c
	gcc  $(flags_secure) Greeter_secure.c -o SecGreeter
clean:
	rm Greeter SecGreeter
run : 
	python generator.py > input.txt 
	@./Greeter < input.txt
	