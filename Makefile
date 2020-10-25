all: controller overseer

controller: controller.c
	gcc -o controller controller.c

overseer: overseer.c
	gcc -pthread overseer.c -o overseer

clean:
	rm -f controller overseer