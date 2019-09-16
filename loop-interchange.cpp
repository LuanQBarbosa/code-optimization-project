#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <chrono>

#define MAX_N 1000
#define DISTANCE( x1, x2, y1, y2 ) sqrt( (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) )

int *x, *y;
double **matrix;

void generateVertices( int nVertices )
{
    x = new int[nVertices];
    y = new int[nVertices];
    int i = 0, j = 0, k = 0;
    while ( i < nVertices ) {
        while ( rand( ) % 4 ) {
            j++;
            k = 0;
            while ( rand( ) % 10 ) {
                k++;
            }
            x[i] = j;
            y[i] = k;
            i++;
            
            if ( i >= nVertices ) {
                break;
            }
        }
    }
    
#ifdef DEBUG
    for ( i = 0; i < num_vertices; i++ ) {
        std::cout << x[i] << " " << y[i] << std::endl;
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
    delete [] x;
    delete [] y;
    
    for(int i = 0; i < n; i++) {
        delete [] matrix[i];
    }
}

int main( int argc, char** argv )
{
    std::ofstream file( "interchange.txt" );
    srand( time( nullptr ) );
    
    for ( int nVertices = 1; nVertices < MAX_N; nVertices++ ) {
        // Generating X and Y values of each point in separated arrays
        generateVertices( nVertices );
        allocateMatrix( nVertices );
        
        // Saving execution starting time
        auto start = std::chrono::steady_clock::now( );
        
        // Filling matrix with the distance between each vertex ( Loop interchange optimization: matrix[j][i] to matrix[i][j]
        for ( int i = 0; i < nVertices; i++ ) {
            for ( int j = 0; j < nVertices; j++ ) {
                matrix[j][i] = DISTANCE( x[i], x[j], y[i], y[j] );
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
        
        // Finding the longest distance ( similar loop, interchange optimization: matrix[j][i] to matrix[i][j] )
        double maxD = 0;
        for ( int i = 0; i < nVertices; i++ ) {
            for ( int j = 0; j < nVertices; j++ ) {
                if ( matrix[i][j] > maxD )
                    maxD = matrix[i][j];
            }
        }
        
        // Normalizing all elements of the matrix ( Loop interchange optimization: matrix[j][i] to matrix[i][j] )
        for ( int i = 0; i < nVertices; i++ ) {
            for ( int j = 0; j < nVertices; j++ ) {
                matrix[i][j] /= maxD;
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
        
        // Saving execution ending time and calculating total execution time
        auto end = std::chrono::steady_clock::now( );
        auto duration = end - start;
        
        // Writing into file
        // file << nVertices << " ";
        file << std::chrono::duration<double, std::milli> (duration).count( ) << std::endl;
        
        deallocate(nVertices);
    }

    file.close( );
    return 0;
}

