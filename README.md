# CBitmapCompetition

## Description

We start from lists of bitmaps, each representing a dataset.
For example, we might have 200 bitmaps.

-  We report the memory usage as best we can. Of course, data can always be compressed more or less aggressively to disk, but an important metric is how much RAM the bitmaps are using.
-  Intersections between successive bitmaps, each time generating a new bitmap. We then query and check the cardinality of the result.
-  Union between successive bitmaps, each time generating a new bitmap. We then query and check the cardinality of the result.
-  Finally, we compute the union of the whole set. This can be done in various ways. A basic way involves doing successive in-place unions.

## Usage

To pull all the submodules:
```bash
git pull && git submodule init && git submodule update && git submodule status
```

```bash
make
make test
```


## Usage for developers of the project

If one of the submodules changes and you need to update it:
```bash
git submodule foreach git pull origin master
make 
make test
git commit -a
git push
```


