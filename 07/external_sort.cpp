#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iterator>

#include <thread>

using namespace std;

const char* infilename = "data_init.dat";
const char* outfilename = "data.txt";


const char* fname1 = "main_1.dat";
const char* fname2 = "main_2.dat";

size_t batch_size = 1000;
//~ size_t proc_number = 2;
size_t file_size_in_nums;

std::ifstream::pos_type filesize(const char* filename) {
    ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg() / sizeof(uint64_t); 
}

void divide_data_for_procs() {
    vector<uint64_t> v(batch_size);
    ifstream f(infilename, ios::in | std::ios::binary);
    ofstream f1(fname1, ios::out | std::ios::binary);
    ofstream f2(fname2, ios::out | std::ios::binary);
    
    for (size_t i = 0; i < file_size_in_nums/batch_size/2; ++i) {
        f.read(reinterpret_cast<char*>(&v[0]), sizeof(uint64_t)*batch_size);
        f1.write(reinterpret_cast<const char*>(&v[0]), v.size()*sizeof(v[0]));
    }
    for (size_t i = 0; i < file_size_in_nums/batch_size - file_size_in_nums/batch_size/2; ++i) {
        f.read(reinterpret_cast<char*>(&v[0]), sizeof(uint64_t)*batch_size);
        f2.write(reinterpret_cast<const char*>(&v[0]), v.size()*sizeof(v[0]));
    }
    if (file_size_in_nums%batch_size) {
        size_t rest = file_size_in_nums%batch_size;
        f.read(reinterpret_cast<char*>(&v[0]), sizeof(uint64_t)*rest);
        ofstream frest("rest.txt", ios::out | std::ios::binary);
        for (size_t i = 0; i < rest; ++i) frest << v[i] << ' ';
        //~ frest.write(reinterpret_cast<const char*>(&v[0]), rest*sizeof(v[0]));
        frest.close();
    }
    
    f1.close();
    f2.close();
    f.close();
}

void base_merge (const char* f1, const char* f2, const char* f3) {
    ifstream file1(f1, ios::in | ios::binary);
    ifstream file2(f2, ios::in | ios::binary);
    ofstream fout(f3, ios::out | ios::binary);

    using istream_iterator = std::istream_iterator<uint64_t> ;

    std::merge( istream_iterator(file1), istream_iterator(),
                istream_iterator(file2), istream_iterator(),
                ostream_iterator<uint64_t>(fout, " ") );    
    file1.close();
    file2.close();
    fout.close();
}

void merge_files(char proc_number, size_t size_in_batch) {
    string name3 = string(1, proc_number) + "__" + outfilename;
    for (size_t i = 0; i < size_in_batch-1; ++i) {
        //~ string name1 = string(1, proc_number) + "_" + to_string(i) + ".dat"; //c .dat merge не хочет работать
        //~ string name2 = string(1, proc_number) + "_" + to_string(i+1) + ".dat";
        string name1 = string(1, proc_number) + "_" + to_string(i) + ".txt";
        string name2 = string(1, proc_number) + "_" + to_string(i+1) + ".txt";
        base_merge(name1.c_str(), name2.c_str(), name3.c_str());
        //~ return;
        remove(name1.c_str());
        remove(name2.c_str());
        if (i != size_in_batch-2)
            rename(name3.c_str(), name2.c_str());
    }
}

void separation(const char* fname1, char proc_number) {
    vector<uint64_t> v(batch_size);
    ifstream f(fname1, ios::in | ios::binary);
    
    int i = 0;
    while(true) { //!f.eof() не работает почему-то правильно
        f.read(reinterpret_cast<char*>(&v[0]), sizeof(uint64_t)*batch_size);
        if (f.eof())
            break;
        sort(v.begin(), v.end());
        string name = string(1, proc_number) + "_" + to_string(i) + ".txt";
        ofstream tmp(name, ios::out | ios::binary);
        for (auto x : v) tmp << x << ' ';
        //~ tmp.write(reinterpret_cast<char*>(&v[0]), sizeof(uint64_t)*batch_size);
        tmp.close();
        i++;
    }
    f.close();
    
    merge_files(proc_number, i);
    
}

void remove_final_files() {
    remove("0__data.txt");
    remove("1__data.txt");
    remove("rest.txt");
    remove("temp.txt");
    remove(fname1);
    remove(fname2);
}

int main() {
    file_size_in_nums = filesize(infilename);
    divide_data_for_procs();
    //~ separation(fname1, '0'); //0__data.txt
    //~ separation(fname2, '1'); //1__data.txt
    thread t1(separation, fname1, '0');
    thread t2(separation, fname2, '1');
    t1.join();
    t2.join();
    base_merge("0__data.txt", "1__data.txt", "temp.txt");
    base_merge("temp.txt", "rest.txt", outfilename);
    remove_final_files();
    return 0;
}
