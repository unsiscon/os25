// single_sort.cpp
#include <bits/stdc++.h>
using namespace std;
using u32 = uint32_t;

int main(int argc, char** argv){
    if(argc<3){ cerr<<"Usage: "<<argv[0]<<" <input.bin> <output.txt>\n"; return 1; }
    const char* infile = argv[1];
    const char* outfile = argv[2];

    FILE* f = fopen(infile,"rb");
    if(!f){ perror("fopen"); return 1; }
    fseek(f,0,SEEK_END);
    long sz = ftell(f);
    rewind(f);
    size_t count = sz / sizeof(u32);
    vector<u32> a(count);
    size_t r = fread(a.data(), sizeof(u32), count, f);
    fclose(f);
    a.resize(r);

    auto t0 = chrono::high_resolution_clock::now();
    sort(a.begin(), a.end());
    auto t1 = chrono::high_resolution_clock::now();

    // write value:count runs to output (compact)
    FILE* out = fopen(outfile,"w");
    if(!out){ perror("fopen out"); return 1; }
    if(!a.empty()){
        u32 cur = a[0];
        uint64_t cnt = 1;
        for(size_t i=1;i<a.size();++i){
            if(a[i]==cur) ++cnt;
            else {
                fprintf(out, "%u:%llu\n", cur, (unsigned long long)cnt);
                cur = a[i]; cnt = 1;
            }
        }
        fprintf(out, "%u:%llu\n", cur, (unsigned long long)cnt);
    }
    fclose(out);
    auto t2 = chrono::high_resolution_clock::now();
    double sort_s = chrono::duration<double>(t1-t0).count();
    double out_s = chrono::duration<double>(t2-t1).count();
    printf("Read: %zu items, sort %.3f s, output %.3f s\n", a.size(), sort_s, out_s);
    return 0;
}
