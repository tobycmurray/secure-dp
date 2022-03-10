default: main-present main-absent

.PHONY: default clean

main-present: main.c
	$(CC) -DPRESENT $< -o $@ -lmastik -L.

main-absent: main.c
	$(CC) $< -o $@ -lmastik -L.


clean:
	rm -f main-present main-absent
