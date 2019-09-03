#include <iostream>
#include <fstream>
#include <cmath>

double computeDistance( double x1, double y1, double x2, double y2 )
{
    return sqrt( (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) );
}

int main( int argc, char** argv )
{
    std::ifstream file( argv[1] );

    // Storing X and Y values of each point in separated arrays
    int nVertices, *x, *y, i = 0, j = 0;

    file >> nVertices;
    x = new int[nVertices];
    y = new int[nVertices];
    while ( file >> x[i] >> y[i] ) i++;

    #ifdef DEBUG
        for ( i = 0; i < nVertices; i++ ) {
            std::cout << x[i] << " " << y[i] << std::endl;
        }
        std::cout << std::endl;
    #endif

    // Allocating adjacency matrix
    double **matrix;
    matrix = new double*[nVertices];
    for ( i = 0; i < nVertices; i++ )
        matrix[i] = new double[nVertices];
    
    // Filling matrix with the distance between each vertex ( inefficient access of matrix positions )
    for ( i = 0; i < nVertices; i++ ) {
        for ( j = 0; j < nVertices; j++ ) {
            matrix[j][i] = computeDistance( x[i], y[i], x[j], y[j] );
        }
    }

    #ifdef DEBUG
        for ( i = 0; i < nVertices; i++ ) {
            for ( j = 0; j < nVertices; j++ ) {
                std::cout << matrix[i][j] << "  ";
            }

            std::cout << std::endl;
        }
        std::cout << std::endl;
    #endif

    // Finding the longest distance ( inefficient access and similar loop )
    double maxD = 0;
    for ( i = 0; i < nVertices; i++ ) {
        for ( j = 0; j < nVertices; j++ ) {
            if ( matrix[j][i] > maxD )
                maxD = matrix[j][i];
        }
    }

    // Normalizing all elements of the matrix ( inefficient access of matrix positions )
    for ( i = 0; i < nVertices; i++ ) {
        for ( j = 0; j < nVertices; j++ ) {
            matrix[j][i] /= maxD;
        }
    }

    #ifdef DEBUG
        for ( i = 0; i < nVertices; i++ ) {
            for ( j = 0; j < nVertices; j++ ) {
                std::cout << matrix[i][j] << "  ";
            }

            std::cout << std::endl;
        }
        std::cout << std::endl;
    #endif

    return 0;
}