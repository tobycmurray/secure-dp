# secure-dp

Playing around with security of differential privacy mechanisms.

The file `main.c` implements a trivial histogram in the obvious
way. As a proof of concept it runs a second thread that uses
a Flush+Reload attack to spy on the query and determine whether it
is being run over one of two datasets.

The dataset in question is the German Credit dataset from
https://archive.ics.uci.edu/ml/datasets/statlog+(german+credit+data)

We have two versions of the data set: one with the maximal record
and one without.

The idea is that the attacker can easily tell which data set we
are running on (and therefore the presence of the maximal record)
since it is only in that data set that a certain cache line is
touched in the histogram.

Some care is taken to ensure the two threads interleave, using
hand-crafted delays. It remains to demonstrate this on separate
processes and to see whether in that setting sufficient
parallelism exists to allow the spy to observe with sufficient
resolution.

# Building

Type `make`, this should produce two binaries:
* `main-present`: the version with the extra record present
* `main-absent`: the version with the extra record absent

# Running and output

The victim prints out `Here` when it is about to touch the extra
record. `.`s indicate another observation by the spy. The spy
prints out when it detects the presence of the extra record.

```
$ ./main-present
Creating query thread
Main thraed continuing
...................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................Here
.   1: max val touched
................................................................................................Query thread done
```

```
$ ./main-absent
Creating query thread
Main thraed continuing
.....................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................Query thread done
```