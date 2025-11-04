// proc_sort.cpp
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;
using u32 = uint32_t;

void child_work(const string &infile, size_t offset, size_t cnt, const string &tmp){
    FILE* f = fopen(infile.c_str(),"rb");
    if(!f){ perror("fopen"); exit(1); }
    fseek(f, offset * sizeof(u32), SEEK_SET);
    vector<u32> buf(cnt);
    size_t r = fread(buf.data(), sizeof(u32), cnt, f);
    fclose(f);
    buf.resize(r);
    sort(buf.begin(), buf.end());
    FILE* out = fopen(tmp.c_str(),"wb");
    fwrite(buf.data(), sizeof(u32), buf.size(), out);
    fclose(out);
    exit(0);
}

int main(int argc,char**argv){
    if(argc<4){ cerr<<"Usage: "<<argv[0]<<" <input> <procs> <total_items>\n"; return 1; }
    string infile = argv[1];
    int procs = atoi(argv[2]);
    size_t total = stoull(argv[3]);
    size_t per = total / procs;
    size_t rem = total % procs;
    size_t offset = 0;
    vector<string> tmpfiles;
    for(int i=0;i<procs;++i){
        size_t cnt = per + (i < (int)rem ? 1 : 0);
        string tmp = infile + ".pblk." + to_string(i) + ".bin";
        tmpfiles.push_back(tmp);
        pid_t pid = fork();
        if(pid==0){
            child_work(infile, offset, cnt, tmp);
        } else if(pid < 0){
            perror("fork");
            return 1;
        }
        offset += cnt;
    }
    for(int i=0;i<procs;++i) wait(NULL);

    // 合并：重用同样的 k-way merge 思路，输出 runs 文本
    string outname = infile + ".proc.result.txt";
    FILE* out = fopen(outname.c_str(),"w");
    struct R{ FILE* f; u32 cur; bool has; };
    vector<R> readers(tmpfiles.size());
    for(size_t i=0;i<tmpfiles.size();++i){
        readers[i].f = fopen(tmpfiles[i].c_str(),"rb");
        readers[i].has = false;
        if(readers[i].f){
            if(fread(&readers[i].cur, sizeof(u32), 1, readers[i].f)==1) readers[i].has = true;
        }
    }
    using PQ = pair<u32,int>;
    auto cmp = [](const PQ &a, const PQ &b){ return a.first > b.first; };
    priority_queue<PQ, vector<PQ>, decltype(cmp)> pq(cmp);
    for(size_t i=0;i<readers.size();++i) if(readers[i].has) pq.push({readers[i].cur, (int)i});
    bool first=true; u32 curv=0; unsigned long long cnt=0;
    while(!pq.empty()){
        auto [v, idx] = pq.top(); pq.pop();
        if(first){ curv=v; cnt=1; first=false; }
        else{
            if(v==curv) ++cnt;
            else { fprintf(out, "%u:%llu\n", curv, cnt); curv=v; cnt=1; }
        }
        if(fread(&readers[idx].cur, sizeof(u32), 1, readers[idx].f)==1) pq.push({readers[idx].cur, idx});
        else fclose(readers[idx].f);
    }
    if(!first) fprintf(out, "%u:%llu\n", curv, cnt);
    fclose(out);
    printf("Merged result in %s\n", outname.c_str());
    return 0;
}
