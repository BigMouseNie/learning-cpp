# 对应字符串范围的KMP算法

````C++
 void getNext(vector<int>& next,const string& p,int start,int end){
        int len = next.size();
        next[0] = -1;
        int k = start, j = start + 1;
        while (j < end) {
            if (p[j] == p[k]) {
                next[j + 1 - start] = k + 1 - start;
                ++k;++j;
            } else {
                if (k == start) {
                    next[j + 1 - start] = 0;
                    ++j;
                } else {
                    k = next[k - start] + start;
                }
            }
        }
        return;
    }

    int match(const string& s,const string& p,int sl,int sr,int pl,int pr){
        if(pr-pl > sr-sl){return -1;}
        vector<int> next(pr-pl+1,0);
        getNext(next,p,pl,pr);
        int i = sl;
        int j = pl;
        while(i<=sr&&j<=pr){
            if(j==pl-1 || s[i]==p[j]){
                ++i;++j;
            }else{
                j=pl+next[j-pl];
            }
        }
        return j==pr+1 ? i : -1;
    }

    bool isMatch(string s, string p) {
        int sLen = s.size(),pLen = p.size();
        int i=0,j=0;
        while(i<sLen && j<pLen){
            int start = j;
            while(i<sLen&&j<pLen&&(p[j]>='a'||p[j]=='?')){
                if(s[i]==p[j]||p[j]=='?'){++i;++j;}
                else{return false;}
            }
            if(j<pLen){
                while(j<pLen&&p[j]=='*'){++j;}
            }
        }
    }
````

