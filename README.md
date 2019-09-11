# Code Optimization Project
During the Computer Architecture II course at UFPB, we're presented to the concepts of cache memory and processor paralellism, which can make computers faster. As an assigment we were instructed to optimize a given code using these concepts.

## Contents
[Introduction](https://github.com/LuanQBarbosa/code-optimization-project#introduction)<br>
[Part I - Cache Memory Optimization](https://github.com/LuanQBarbosa/code-optimization-project#part-i---cache-memory-optimization) <br>
[Part II - Paralellism Optimization](https://github.com/LuanQBarbosa/code-optimization-project#part-ii---paralellism-optimization) <br>

## Introduction
Given a set of N points, each with a (X, Y) coordinate, generate the corresponding complete graph and it's adjacency matrix. Then, find in the matrix the highest value and normalize the matrix using the highest value previously found.

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
In this part, given the code implemented [above](https://github.com/LuanQBarbosa/code-optimization-project#introduction), we have made some optimizations in order to allow it to have a better cache memory usage, and therefore a better performance.

### Loop Interchange
As we have learned on class, when a processor accesses something from memory for the first time, it will bring an entire block of data to cache, and then an access to something on that block can be done by on the cache, which is faster. When implementing the algorithm we have used the following syntax to access a matrix element:
```C++
matrix[j][i]
```

Which makes a poor use of cache memory. Because at each iteration of the inner loop we are incrementing J, and since in C++ array elements in the same row are stored consecutively in memory, an access to a row for the first time is likely to cause a cache miss, but on the other side we can access the other elements from that row without having a cache miss. However, by incrementing the row number at every inner loop iteration, it is likely to cause a cache miss due to the next row not being on cache, and even worse, when finishing the inner loop the next element of the first row is likely to have been removed from cache due to limited storage capacity which will cause a cache miss and therefore decreasing performance. In order to fix this, we have changed the code to increment the column acessed in the inner loop, and the row in the outer loop in order to reduce the number of cache misses, this has been done in each loop of the algorithm:
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
(TODO)

### Comparison
(TODO)

## Part II - Paralellism Optimization
(TODO)

## References
Content seen on the Computer Architecture II class (Prof. Alisson Brito)<br>
[Loop Interchange](https://en.wikipedia.org/wiki/Loop_interchange)<br>
(TODO)
