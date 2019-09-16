# Code Optimization Project
During the Computer Architecture II course at UFPB, we're presented to the concepts of cache memory and processor parallelism, which can make computers faster. As an assigment we were instructed to optimize a given code using these concepts.

## Contents
[Introduction](https://github.com/LuanQBarbosa/code-optimization-project#introduction)<br>
[Part I - Cache Memory Optimization](https://github.com/LuanQBarbosa/code-optimization-project#part-i---cache-memory-optimization) <br>
[Part II - Parallelism Optimization](https://github.com/LuanQBarbosa/code-optimization-project#part-ii---parallelism-optimization) <br>

## Introduction
For this assignment we have implemented an algorithm that solves the following problem:
> Given a set of N points, each with a (X, Y) coordinate, generate the corresponding complete graph and it's adjacency matrix. Then, find in the matrix the highest value and normalize the matrix using the highest value previously found.

As a first approach to this problem we've implemented the following code (naive-algorithm.cpp):
```C++
int x[nVertices], y[nVertices], matrix[nVertices][nVertices];
for ( int i = 0; i < nVertices; i++ ) {
    for ( int j = 0; j < nVertices; j++ ) {
        matrix[j][i] = DISTANCE( x[i], y[i], x[j], y[j] );
    }
}
```

Which basically iterates through all the matrix and fills each position by calculating the distance between (X[ i ], Y[ i ]) and (X[ j ], Y[ j ]). Then, we need to find it's highest value, which can be done by iterating through the matrix:
```C++
double maxD = 0;
for ( int i = 0; i < nVertices; i++ ) {
    for ( int j = 0; j < nVertices; j++ ) {
        if ( matrix[j][i] > maxD )
            maxD = matrix[j][i];
    }
}
```

With the highest value found, we can then normalize the matrix:
```C++
for ( int i = 0; i < nVertices; i++ ) {
    for ( int j = 0; j < nVertices; j++ ) {
        matrix[j][i] /= maxD;
    }
}
```

Even though this approach solves the problem, it has a lot of flaws as we will show later in the following sections.

## Part I - Cache Memory Optimization
In this part, given the algorithm implemented [above](https://github.com/LuanQBarbosa/code-optimization-project#introduction), we have made some optimizations in order to allow it to have a better cache memory usage, and therefore a better performance.

### Loop Interchange
As we have learned on class, when a processor accesses something from memory for the first time, it will bring an entire block of data to cache, and then an access to something on that block can be done by on the cache, which is faster. When implementing the algorithm we have used the following syntax to access a matrix element:
```C++
matrix[j][i]
```

Which makes a poor use of cache memory. In C (and C++) array elements in the same row are stored consecutively in memory, and thus, an access to a row for the first time is likely to cause a cache miss, but on the other side we can access the other elements from that row without having a cache miss due to the block containing the element being transferred to cache. However, by incrementing the row number at every inner loop iteration, it is likely to cause a cache miss due to the next row not being on cache, and even worse, when finishing the inner loop the next element of the first row is likely to have been removed from cache due to its limited storage capacity which will cause a cache miss and therefore will decrease performance further. In order to fix this, we have changed the code to increment the column acessed in the inner loop, and the row in the outer loop in order to reduce the number of cache misses, this has been done in each loop of the algorithm:
```C++
for ( int i = 0; i < nVertices; i++ ) {
    for ( int j = 0; j < nVertices; j++ ) {
        matrix[i][j] = ...;
    }
}
```

### Loop Fusion
The code implemented as a näive approach had 3 loops, all iterating over the same range and data, however the third loop needs the result from the second in order to properly execute, because of this we may merge the first and second loops into one, which may decrease the number of cache misses due to using a previously referenced item consecutively which can be done on cache. So the code for those loops have become:
```C++
double maxD = 0;
for ( int i = 0; i < nVertices; i++ ) {
    for ( int j = 0; j < nVertices; j++ ) {
        matrix[i][j] = DISTANCE( points[i].x, points[i].y, points[j].x, points[j].y );
        if ( matrix[i][j] > maxD )
            maxD = matrix[i][j];
    }
}
```

### Array Merge
Another optimization idea was to merge the x and y arrays into one through the use of an array of struct called point. This will make a better use of spatial locality when computing the distance on the first loop. Since both informations will be together in the same array, it will generate only one cache miss, instead of two when accessing an index not on cache. Therefore, the code for the struct has become:
```C++
struct point {
    int x, y;
} points[nVertices];
```

And the fused loop has been changed to use the struct when computing the distance:
```C++
double maxD = 0;
for ( int i = 0; i < nVertices; i++ ) {
    for ( int j = 0; j < nVertices; j++ ) {
        matrix[i][j] = DISTANCE( points[i].x, points[i].y, points[j].x, points[j].y );
        if ( matrix[i][j] > maxD )
            maxD = matrix[i][j];
    }
}
```

### Comparison
After putting the optimizations made before together, we have made a comparison between the näive approach and the cache optimized code. Giving the following graph:
<p align = "center"><img src="https://raw.githubusercontent.com/LuanQBarbosa/code-optimization-project/master/images/comparison.png"></p>

As we can see on the graph we had an increase in performance of up to 45% in the optimized code in terms of execution time.

## Part II - Parallelism Optimization
As the second part, given the algorithm optimized for cache implemented [above](https://github.com/LuanQBarbosa/code-optimization-project#part-i---cache-memory-optimization), we have made some optimizations to make it take advantage of processor parallelism using the OpenMP library.

### Parallel Fill and Search
The first step of our optimized algorithm filled the matrix and found its highest value in a nested loop. In order to improve performance we can make the nested loop work in parallel, but it raises the critical section problem, since we have a shared variable containing the matrix highest value, which will be used to compare the current element and update if it's greater:
```C++
double maxD = 0;
...
if ( matrix[i][j] > maxD )
    maxD = matrix[i][j];
```

We cannot let more than one thread do the above code at same time, or we will have memory consistency issues. Thus, a first approach would be to use a critical section in the inner loop, however this would make the parallelized algorithm as bad as serialized one. In order to avoid it, we have used a private variable called localMaxD holding the highest distance found by each thread and a shared variable called globalMaxD which will hold the whole matrix highest value. And thus, we may only update it on the end of the inner loop, decreasing the number of critical regions and incresing the portion of parallelized work:
```C++
double globalMaxD = 0;
double localMaxD = 0;
#pragma omp parallel for private( localMaxD ) shared( globalMaxD )
for ( int i = 0; i < nVertices; i++ ) {
    for ( int j = 0; j < nVertices; j++ ) {
        matrix[i][j] = DISTANCE( points[i].x, points[j].x, points[i].y, points[j].y );
        if ( matrix[i][j] > localMaxD )
            localMaxD = matrix[i][j];
    }

    #pragma omp critical
    {
        if ( localMaxD > globalMaxD )
            globalMaxD = localMaxD;
    }
}
```

### Parallel Normalization
The second step of the algorithm did the matrix normalization, which is basically iterate over the matrix and divide each element by the highest value found previously. Since we do not have any critical sections, we don't need to declare private/shared variables, and thus the normalization code has become:
```C++
#pragma omp parallel for collapse( 2 )
for ( int i = 0; i < nVertices; i++ ) {
    for ( int j = 0; j < nVertices; j++ ) {
        matrix[i][j] /= globalMaxD;
    }
}
```

### Comparison
(TODO)

## References
Content seen on the Computer Architecture II class (Prof. Alisson Brito)<br>
[Loop Interchange](https://en.wikipedia.org/wiki/Loop_interchange)<br>
[Loop fusion and fission](https://en.wikipedia.org/wiki/Loop_fission_and_fusion)<br>
[Hands-on introduction to OpenMP by Tim Mattson](https://www.openmp.org/wp-content/uploads/Intro_To_OpenMP_Mattson.pdf)<br>
