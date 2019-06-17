#ifndef MATRIX
#define MATRIX

#include <iostream>

using namespace std;

class Matrix {
    
    class Row {
        friend class Matrix;
        size_t cols = 0;
        int* row = nullptr;
    public:
    
        //Row() {}
        
        Row(size_t cols) : cols(cols) {
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
        void destructor() { delete[] row; }
    };
    
    size_t rows, cols;
    Row* matrix = nullptr;
            
public:
    
    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
        matrix = static_cast<Row*>(operator new[] (rows*sizeof(Row)));
        //~ matrix = new Row [rows];
        for (size_t i = 0; i < rows; ++i)
            new (&matrix[i]) Row(cols);
            //~ matrix[i] = Row(cols);
    }

    Row& operator[](size_t a) {
        if (a >= rows)
            throw out_of_range("");
        return matrix[a];
    }
    
    const Row& operator[](size_t a) const {
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
        if (m.rows != rows || m.cols != cols)
            return false;
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
            matrix[i].destructor();
            //~ delete[] matrix[i].row;
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
