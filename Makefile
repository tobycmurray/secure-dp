default: main

.PHONY: default clean

main: main.c
	$(CC) $< -o $@ -lmastik -L.


clean:
	rm -f main
