output: main.o
	gcc -g -O3 -DDONT_USE_VOL bin/*.o -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o bin/output
# gcc bin/*.o -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o bin/output
# gcc main.o -o main
# make clean

main.o: src/main.c
	mkdir bin && gcc -c src/main.c -o bin/main.o

start.o: src/game/start.c
	gcc -c src/game/start.c -o bin/start.o

components.o: src/globals/components.c
	gcc -c src/globals/components.c -o bin/components.o

human.o: src/entities/human.c
	gcc -c src/entities/human.c -o bin/human.o

skeleton.o: src/entities/skeleton.c
	gcc -c src/entities/skeleton.c -o bin/skeleton.o

#####################################
# components.o: src/components.c
	# gcc -c src/components.c -o bin/components.o

instantiate.o: src/instantiate.c
	gcc -c src/instantiate.c -o bin/instantiate.o

vars.o: src/vars.c
	gcc -c src/vars.c -o bin/vars.o

timer.o: src/timer.c
	gcc -c src/timer.c -o bin/timer.o

update.o: src/update.c
	gcc -c src/update.c -o bin/update.o

drawTiles.o: src/drawTiles.c
	gcc -c src/drawTiles.c -o bin/drawTiles.o

# __.o: src/__.c
#		gcc -c src/__.c -o bin/__.o
#

clean:
	rm -rf bin
	# rm bin/*.o bin/output

run:
	./bin/output

build:
	make clean && make && make run
	gcc -g -O3 -DDONT_USE_VOL bin/*.o -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o bin/output
