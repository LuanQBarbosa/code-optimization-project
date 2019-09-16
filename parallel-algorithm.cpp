#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <chrono>
#include <omp.h>

#define MAX_N 1000
#define numThreads 8
#define DISTANCE( x1, x2, y1, y2 ) sqrt( (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) )

// Struct that merge both arrays representing the coordinates of a point
struct point {
    int x, y;
} *points;

double **matrix;

void generateVertices( int nVertices )
{
    points = new point[nVertices];
    int i = 0, j = 0, k = 0;
    while ( i < nVertices ) {
        while ( rand( ) % 4 ) {
            j++;
            k = 0;
            while ( rand( ) % 10 ) {
                k++;
            }
            points[i].x = j;
            points[i].y = k;
            i++;

            if ( i >= nVertices ) {
                break;
            }
        }
    }

    #ifdef DEBUG
        for ( i = 0; i < nVertices; i++ ) {
            std::cout << points[i].x << " " << points[i].y << std::endl;
        }
        std::cout << std::endl;
    #endif
}

// Function to allocate adjacency matrix
void allocateMatrix(int dimension) {
    matrix = new double*[dimension];
    for ( int i = 0; i < dimension; i++ ) {
        matrix[i] = new double[dimension];
    }
}

void deallocate(int n) {
    delete [] points;
    
    for(int i = 0; i < n; i++) {
        delete [] matrix[i];
    }
}

int main( int argc, char** argv )
{
    std::ofstream file( "parallel.txt" );
    srand( time( nullptr ) );

    for ( int nVertices = 1; nVertices < MAX_N; nVertices++ ) {
        // Generating X and Y values of each point in separated arrays
        generateVertices( nVertices );
        allocateMatrix( nVertices );
        
        // Saving execution starting time
        auto start = std::chrono::steady_clock::now( );
        
        // Filling matrix with the distance between each vertex ( Loop interchange optimization: matrix[j][i] to matrix[i][j] and loop fusion optimization )
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


        #ifdef DEBUG
            for ( int i = 0; i < nVertices; i++ ) {
                for ( int j = 0; j < nVertices; j++ ) {
                    std::cout << matrix[i][j] << "  ";
                }

                std::cout << std::endl;
            }
            std::cout << std::endl;
        #endif

        // Normalizing all elements of the matrix ( Loop interchange optimization: matrix[j][i] to matrix[i][j] )
        #pragma omp parallel for collapse( 2 )
        for ( int i = 0; i < nVertices; i++ ) {
            for ( int j = 0; j < nVertices; j++ ) {
                matrix[i][j] /= globalMaxD;
            }
        }

        #ifdef DEBUG
            for ( int i = 0; i < nVertices; i++ ) {
                for ( int j = 0; j < nVertices; j++ ) {
                    std::cout << matrix[i][j] << "  ";
                }

                std::cout << std::endl;
            }
            std::cout << std::endl;
        #endif

        // Saving execution ending time and calculating total execution time
        auto end = std::chrono::steady_clock::now( );
        auto duration = end - start;

        // Writing into file
        //file << nVertices << " ";
        file << std::chrono::duration<double, std::milli> (duration).count( ) << std::endl;
        
        deallocate(nVertices);
    }
    file.close( );
    return 0;
}
