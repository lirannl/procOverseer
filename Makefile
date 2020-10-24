all: controller overseer

controller: controller/controller.c
	gcc -o controller controller/controller.c

overseer: overseer/overseer.c
	gcc -o overseer overseer/overseer.c