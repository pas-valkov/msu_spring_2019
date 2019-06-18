#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iterator>

#include <thread>

using namespace std;

const char* infilename = "data_init.bin";
const char* outfilename = "data.bin";
const char* restfilename = "rest.bin";
const char* tempfilename = "temp.bin";

const char* fname1 = "main_1.bin";
const char* fname2 = "main_2.bin";

size_t batch_size = 50;
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
        ofstream frest(restfilename, ios::out | std::ios::binary);
        //~ for (size_t i = 0; i < rest; ++i) frest << v[i] << ' ';
        frest.write(reinterpret_cast<const char*>(&v[0]), rest*sizeof(v[0]));
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

    uint64_t v1, v2;

    file1.read(reinterpret_cast<char*>(&v1), sizeof(uint64_t));
    file2.read(reinterpret_cast<char*>(&v2), sizeof(uint64_t));

    while(true) {
        if (file1.eof()) {
            fout.write(reinterpret_cast<char*>(&v2), sizeof(uint64_t));
            //~ while(!file2.eof()) {
            while(true) {
                file2.read(reinterpret_cast<char*>(&v2), sizeof(uint64_t));
                if (file2.eof())
                    break;
                fout.write(reinterpret_cast<char*>(&v2), sizeof(uint64_t));
            }
            break;
        }
        if (file2.eof()) {
            fout.write(reinterpret_cast<char*>(&v1), sizeof(uint64_t));
            //~ while(!file1.eof()) {
            while(true) {
                file1.read(reinterpret_cast<char*>(&v1), sizeof(uint64_t));
                if (file1.eof())
                    break;
                fout.write(reinterpret_cast<char*>(&v1), sizeof(uint64_t));
            }
            break;
        }
        if (v1 < v2) {
            fout.write(reinterpret_cast<char*>(&v1),  sizeof(uint64_t));
            file1.read(reinterpret_cast<char*>(&v1), sizeof(uint64_t));
        }
        else {
            fout.write(reinterpret_cast<char*>(&v2),  sizeof(uint64_t));
            file2.read(reinterpret_cast<char*>(&v2), sizeof(uint64_t));
        }
    }
    
    file1.close();
    file2.close();
    fout.close();
}

void merge_files(char proc_number, size_t size_in_batch) {
    string name3 = string(1, proc_number) + "__" + outfilename;
    
    if (size_in_batch-1 == 0) {
        string s = string(1, proc_number) + "_0.bin";
        rename(s.c_str(), name3.c_str());
        remove(s.c_str());
    }
    else {
        for (size_t i = 0; i < size_in_batch-1; ++i) {
            string name1 = string(1, proc_number) + "_" + to_string(i) + ".bin";
            string name2 = string(1, proc_number) + "_" + to_string(i+1) + ".bin";
            base_merge(name1.c_str(), name2.c_str(), name3.c_str());
            //~ exit(0);
            remove(name1.c_str());
            remove(name2.c_str());
            if (i != size_in_batch-2)
                rename(name3.c_str(), name2.c_str());
        }
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
        string name = string(1, proc_number) + "_" + to_string(i) + ".bin";
        ofstream tmp(name, ios::out | ios::binary);
        //~ for (auto x : v) cout << x << ' ';
        tmp.write(reinterpret_cast<char*>(&v[0]), sizeof(uint64_t)*batch_size);
        tmp.close();
        i++;
    }
    f.close();
    
    merge_files(proc_number, i);
}

void remove_final_files() {
    remove("0__data.bin");
    remove("1__data.bin");
    remove(restfilename);
    remove(tempfilename);
    remove(fname1);
    remove(fname2);
}

int main() {
    file_size_in_nums = filesize(infilename);
    divide_data_for_procs();
    //~ separation(fname1, '0'); //0__data.bin
    //~ separation(fname2, '1'); //1__data.bin
    thread t1(separation, fname1, '0');
    thread t2(separation, fname2, '1');
    t1.join();
    t2.join();
    base_merge("0__data.bin", "1__data.bin", tempfilename);
    base_merge(tempfilename, restfilename, outfilename);
    remove_final_files();
    return 0;
}
