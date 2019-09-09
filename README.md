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

Which basically iterates through all the matrix and fills each position by calculating the distance between (X[i], Y[i]) and (X[j], Y[j]). Then, we need to find it's highest value, which can be done by iterating through the matrix:
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
(TODO)

## Part II - Paralellism Optimization
(TODO)
