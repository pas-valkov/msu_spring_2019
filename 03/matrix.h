#ifndef MATRIX
#define MATRIX

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>

using namespace std;

class Matrix {
    
    class Row {
        friend class Matrix;
        size_t cols;
        int* row = nullptr;
    public:
    
        Row() : cols(0) {}
        
        Row(size_t _cols) : cols(_cols) {
            row = new int [cols];
        }
        
        int& operator[](size_t a) {
            if (a >= cols)
                throw out_of_range("");
            return row[a];
        }
        
        int operator[](size_t a) const {
            if (a >= cols)
                throw out_of_range("");
            return row[a];
        }
        
        //~ ~Row () { delete[] row; }
    };
    
    size_t rows, cols;
    Row* matrix = nullptr;
            
public:
    
    Matrix(size_t _rows, size_t _cols) : rows(_rows), cols(_cols) {
        matrix = new Row [rows];
        for (size_t i = 0; i < rows; ++i)
            matrix[i] = Row(cols);
    }

    Row& operator[](size_t a) {
        if (a >= rows)
            throw out_of_range("");
        return matrix[a];
    }
    
    Row& operator[](size_t a) const {
        if (a >= rows)
            throw out_of_range("");
        return matrix[a];
    }
    
    Matrix &operator*=(size_t n) {
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                matrix[i][j] *= n;
        return *this;
    }
    
    bool operator==(const Matrix& m) const {
        if (this == &m)
            return true;
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                if (matrix[i][j] != m.matrix[i][j])
                    return false;
        return true;
    }
    
    bool operator!=(const Matrix& m) const{
        return !(*this == m);
    }
    
    ~Matrix() {
        for (size_t i = 0; i < rows; ++i)
            delete[] matrix[i].row;
        delete[] matrix;
    }

    size_t getColumns() const {
        return cols;
    }
    
    size_t getRows() const {
        return rows;
    }    
};

#endif
