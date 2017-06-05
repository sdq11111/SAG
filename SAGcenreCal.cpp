#include <iostream>
#include <map>
#include <cstdio>
#include <cmath>
#include <sstream>
#include <cstring>
#include <string>
#include <algorithm>
#include <queue>
#include <set>

using namespace std;

map <string, int> IDF;
const double threshold = 0.36, alpha = 0.12, beta = 0.4;
const int d = 200;

void getIDF(char idfName[])
{
    FILE * fp;
    fp = fopen("idf2.txt", "r");
    for (int i = 0; i < 16254; ++i)
    {
        int tmpnum;
        char tmpword[200];
        fscanf(fp, "%s %d\n", tmpword, &tmpnum);
        IDF[tmpword] = tmpnum;
        //cout << tmpword << ' ' << tmpnum <<endl;
    }
    fclose(fp);
}

struct Word
{
    string str;
    double weight;
};

class Info
{
private:
    FILE *fp1, *fp2;
    stringstream Num;
    string inputFile, inputClass;
    int ti;
    char s[5000], st[5000];

    void cut(char s[], char st[])
    {
        int pos;
        for (pos = 0; (s[pos] != ' ') & (s[pos] != '\n'); ++pos);
        for (int i = 0; i < pos; ++i)
        {
            st[i] = s[i];
        }
        st[pos] = 0;
        int l = strlen(s);
        for (int i = pos + 1; i < l; ++i)
        {
            s[i - (pos + 1)] = s[i];
        }
        s[l - (pos + 1)] = 0;
    }

    bool checkUse(char c)
    {
        if (c == 'n' || c == 'v' || c == 'a' || c == 'r' || c == 'z' || c == 'd' || c == 'b')
            return true;
        return false;
    }

    bool checkNoise(char s[])
    {
        char st[200];
        char word[200];
        bool noise = true;
        while(strlen(s))
        {
            memset(st, 0, sizeof(st));
            cut(s, st);
            int pos;
            for (pos = 0; st[pos] != '/'; ++pos);
            if (checkUse(st[pos + 1]))
            {
                noise = false;
                for (int i = 0; i < pos; ++i)
                {
                    word[i] = st[i];
                }
                word[pos] = 0;
                string tmp = word;
                if (!map1[tmp])
                {
                    map1[tmp] = ++totalWord;
                    word1[totalWord].str = tmp;
                    word1[totalWord].weight = 0;
                }
                sentence[totalSentence][sentenceNum[totalSentence]++] = map1[tmp];
            }
        }
        if (!noise)
            ++totalSentence;
        return noise;
    }

    void deal(char st[], int ti, int index)
    {
        char stmp[100];
        int d = 0;
        while(strlen(st))
        {
            cut(st, stmp);
            sscanf(stmp, "%lf", &vec[index - 1][d++]);
        }
        senti[index - 1] = ti;
    }

public:
    int totalSentence, totalWord;
    int sentenceNum[5000], senti[5000];
    int sentence[5000][200];
    double vec[5000][400];
    Word word1[100000];
    map <string, int> map1;
    string num;
    string fileName, type;
    Info(string fileName, string type, int ti)
    {
        Num << ti;
        Num >> num;
        inputFile.clear();
        inputClass.clear();
        this->fileName = fileName;
        this->type = type;
        totalSentence = 0;
        totalWord = 0;
        memset(sentenceNum, 0, sizeof(sentenceNum));
        for (int i = 0; i < 100000; ++i)
        {
            word1[i].weight = 0;
        }
        map1.clear();
    }

    ~Info()
    {

    }

    void getInput()
    {
        inputFile = "mean_vector_all_text_dir\\" + type + fileName + "_" + num + ".txt_vec";
        inputClass = type + fileName + "w_" + num + ".txt";
//        inputFile = "mean_vector_all_text_dir\\" + type + fileName + ".txt_vec";
//        inputClass = type + fileName + "w.txt";
       // inputClass = type + fileName + "_" + num + "w.txt";
        fp1 = fopen(inputFile.c_str(), "r");
        fp2 = fopen(inputClass.c_str(), "r");

        cout << inputFile << endl;
        cout <<inputClass <<endl;

        while(fgets(st, sizeof(st), fp1))
        {
            int i;
            sscanf(st, "%d", &i);
            fgets(st, sizeof(st), fp1);
            fgets(s, sizeof(s), fp2);
            bool noise = checkNoise(s);
            if (noise)
                continue;
            deal(st, i, totalSentence);
        }
        fclose(fp1);
        fclose(fp2);
    }

    void print()
    {
        cout << "Total sentence:" << totalSentence << endl;
        for (int i = 0; i < totalSentence; ++i)
        {
            for (int j = 0; j < sentenceNum[i]; ++j)
            {
                cout << word1[sentence[i][j]].str << ' ';
            }
            cout << endl;
        }
    }

};

class SAG
{
public:
    struct GraphN
    {
        int totE;
        int to[20000000], nex[20000000], head[5000];
        double weight[20000000];
    }gra, graF;

private:
    double m[5000][5000];
    struct Graph
    {
        double val;
        int num;
    }subgraph[5000];
    Info * info;
    struct Edge
    {
        int x, y;
        double val;
    }edge[20000000];

    static bool cmp(Edge x, Edge y)
    {
        return x.val > y.val;
    }

    double dis(int x, int y)
    {
        double tmp1 = 0, tmp2 = 0, tmp3 = 0;
        for (int i = 0; i < d; ++i)
        {
            tmp1 += info->vec[x][i] * info->vec[y][i];
            tmp2 += info->vec[x][i] * info->vec[x][i];
            tmp3 += info->vec[y][i] * info->vec[y][i];
        }
        double tmp = tmp1 / (sqrt(tmp2) * sqrt(tmp3));
        tmp *= exp((-1.0) * abs((double)(info->senti[y] - info->senti[x])) * alpha);
        return tmp;
    }

    int getfather(int x)
    {
        if (st[x].fa == x) return st[x].fa;
        return st[x].fa = getfather(st[x].fa);
    }

    void graphUnion(int x, int y)
    {
        for (int i = 0; i < d; ++i)
        {
            st[x].centre[i] = (st[x].centre[i] * st[x].num + st[y].centre[i] * st[y].num) / (double)(st[x].num + st[y].num);
        }
        st[x].num += st[y].num;
        se.erase(st[y]);
        st[y].fa = x;
        st[x].cti = (st[x].cti * st[x].num + st[y].cti * st[y].num) / double(st[x].num + st[y].num);
    }

    void add(int u, int v, double val)
    {
        gra.to[gra.totE] = v;
        gra.nex[gra.totE] = gra.head[u];
        gra.head[u] = gra.totE;
        gra.weight[gra.totE++] = val;

        graF.to[graF.totE] = u;
        graF.nex[graF.totE] = graF.head[v];
        graF.head[v] = graF.totE;
        graF.weight[graF.totE++] = val;
    }

    void initGraph()
    {
        gra.totE = 0;
        graF.totE = 0;
        memset(gra.head, -1, sizeof(gra.head));
        memset(graF.head, -1, sizeof(graF.head));
        memset(gra.weight, 0, sizeof(gra.weight));
        memset(graF.weight, 0, sizeof(graF.weight));
    }

    struct Set
    {
        double centre[400];
        int num, fa;
        double ti, cti;
        friend bool operator <(Set a, Set b)
        {
            return a.fa < b.fa;
        }
//        bool operator ==(const Set& u)const
//        {
//            return this->num == u.num && this->fa == u.fa && this->ti == u.ti && this->cti == u.cti;
//        }
    }st[5000];
    int totalSet;
    void initSet()
    {
        for (int i = 0; i < info->totalSentence; ++i)
        {
            for (int j = 0; j < d; ++j)
            {
                st[i].centre[j] = info->vec[i][j];
            }
            st[i].ti = info->senti[i];
            st[i].cti = info->senti[i];
            st[i].num = 1;
            st[i].fa = i;
            totalSet++;
            se.insert(st[i]);
            //printf("st[%d]: %d\n", i, st[i].fa);
        }
        disq.clear();
    }

    struct Dist
    {
        double val;
        int x, y;
        friend bool operator < (Dist u, Dist v)
        {
            return u.val > v.val;
        }
    } ;
    set <Dist> disq;
    set <Set> se;
    int match[5000];
    double maval[5000];
   // set <Dist> lis[5000];

    void buildEdge()
    {
        initSet();
        printf("totalSet: %d\n", totalSet);
        for (int i = 0; i < totalSet; ++i)
        {
            maval[i] = 0;
            match[i] = st[i].fa;
          //  printf("%d %f\n", i, maval[i]);
            for (int j = 0; j < totalSet; ++j)
            {
                if (i == j)
                    continue;
                double md = dis(i, j);
              //  printf("%f\n", maval[i]);
                if (md < threshold)
                    m[i][j] = 0;
                else
                    m[i][j] = md;
               // printf("%d %d %f %f %f\n", i, j, md, maval[i], maval[j]);
                if (md > beta && md > maval[i] && md > maval[j])
                {
                    match[i] = j;
                    maval[i] = md;
                }
            }
            if (match[i] != i)
            {
                Dist tdist;
                tdist.x = i;
                tdist.y = match[i];
                tdist.val = maval[i];
//                lis[i].insert(tdist);
//                lis[match[i]].insert(tdist);
                disq.insert(tdist);
                //printf("%d %d %f\n", i, match[i], maval[i]);
            }
        }
    }
    double centreDis(int x, int y)
    {
        double tmp1 = 0, tmp2 = 0, tmp3 = 0;
        for (int i = 0; i < d; ++i)
        {
            tmp1 += st[x].centre[i] * st[y].centre[i];
            tmp2 += st[x].centre[i] * st[x].centre[i];
            tmp3 += st[y].centre[i] * st[y].centre[i];
        }
        double tmp = tmp1 / (sqrt(tmp2) * sqrt(tmp3));
        double t1, t2;
        if (st[x].ti < st[y].ti)
        {
            t1 = st[x].ti;
            t2 = st[y].cti;
        }
        else
        {
            t1 = st[y].ti;
            t2 = st[x].cti;
        }
        tmp *= exp((-1.0) * abs(t1 - t2) * alpha);
        return tmp;
    }

    int totalColor, color[5000], colornum[5000], colormap[5000];
    int lis[5000][5000];
    void initCol()
    {
        totalColor = 0;
        memset(color, 0, sizeof(color));
        memset(colornum, 0, sizeof(colornum));
        memset(colormap, 0, sizeof(colormap));
        for (int i = 0; i < info->totalSentence; ++i)
        {
            int fa = getfather(i);
            if (!colormap[fa])
            {
                colormap[fa] = totalColor++;
            }
            color[i] = colormap[fa];
            lis[color[i]][colornum[color[i]]++] = i;
            //colornum[color[i]]++;
        }
    }

    double intraDis()
    {
        int K = 0;
        double totalIntra = 0;
        for (int k = 0; k < totalColor; ++k)
        {
            if (colornum[k] <= 1)
                continue;
            K++;
            double totalDis = 0;
            for (int i = 0; i < colornum[k]; ++i)
            {
                for (int j = i + 1; j < colornum[k]; ++j)
                {
                    totalDis += 2.0 * dis(lis[k][i], lis[k][j]);
                }
            }
            totalDis /= (double)(colornum[k] * (colornum[k] - 1));
            totalIntra += totalDis;
        }
        return totalIntra /= K;
    }
    double interDis()
    {
        int K = 0;
        double totalInter = 0;
        for (int k1 = 0; k1 < totalColor; ++k1)
        {
            if (colornum[k1] == 1)
                continue;
            K++;
            for (int k2 = k1 + 1; k2 < totalColor; ++k2)
            {
                if (colornum[k2] == 1)
                    continue;
                double totalDis = 0;
                for (int i = 0; i < colornum[k1]; ++i)
                {
                    for (int j = 0; j < colornum[k2]; ++j)
                    {
                        totalDis += dis(lis[k1][i], lis[k2][j]);
                    }
                }
                totalDis /= double(colornum[k1] *colornum[k2]);
                totalInter += totalDis;
            }
        }
        return totalInter /= (double)(0.5 * K * (K - 1));
    }

public:
    SAG(Info * info)
    {
        this->info = info;
    }
    double p[5000];

    void cluster()
    {
        buildEdge();
        while (!disq.empty())
        {
            //printf("DISNUM: %d ", disq.size());
            Dist distmp  = *disq.begin();
            disq.erase(distmp);

            //printf("DISNUM: %d\n", disq.size());
           // printf("Dis %d %d %f\n", distmp.x, distmp.y, distmp.val);

            set <int> ulist;
            set <Dist>::iterator iterd;

            for (iterd = disq.begin(); iterd != disq.end(); ++iterd)
            {
                bool delt = false;
                if (iterd->x == distmp.x || iterd->x == distmp.y)
                {
                    ulist.insert(iterd->y);
                    delt = true;
                }
                if (iterd->y == distmp.x || iterd->y == distmp.y)
                {
                    ulist.insert(iterd->x);
                    delt = true;
                }
                if (delt)
                    disq.erase(*iterd);
            }

            int fax = distmp.x;
            int fay = distmp.y;

            graphUnion(fax, fay);
            ulist.insert(fax);
            ulist.erase(fay);

            set <int>::iterator liter;
            if (ulist.empty()) continue;
            for (liter = ulist.begin(); liter != ulist.end(); ++liter)
            {
                if (getfather(*liter) != (*liter))
                    continue;
                //***********************
                match[*liter] = *liter;
                maval[*liter] = 0;
                set <Set>::iterator iter;
                for (iter = se.begin(); iter != se.end(); ++iter)
                {
                    Set setmp = *iter;
                    if (setmp.fa == *liter)
                        continue;
                    double tmp = centreDis(*liter, setmp.fa);
                    if (tmp > beta && tmp > maval[*liter])
                    {
                        match[*liter] = setmp.fa;
                        maval[*liter] = tmp;
                    }
                }
                if (match[*liter] != *liter)
                {
                    Dist tdist;
                    tdist.x = *liter;
                    tdist.y = match[*liter];
                    tdist.val = maval[*liter];
                    disq.insert(tdist);

                }

            }
        }
    }
    void buildGraph()
    {
        //cout << "startbuildgraph" << endl;
        initGraph();
        initCol();
        //cout << "endinit" << endl;
       // printf("%d\n", info->totalSentence);
        for (int i = 0; i < info->totalSentence; ++i)
       {
           for (int j = i + 1; j < info->totalSentence; ++j)
           {
               //printf("%d %d : ", i, j);
               if (color[i] == color[j])
               {
                   add(i, j, m[i][j]);
                   //printf("%d %d\n", i, j);
               }
           }
       }
    }

    void getWeight()
    {
        for (int i = 0; i < info->totalSentence; ++i)
        {
            p[i] = 1;
        }

        for (int k = 0; k < 200; ++k)
       {
           for (int i = info->totalSentence - 1; i >= 0; --i)
           {
               double tmp = p[i];
               for (int j = gra.head[i]; ~j; j = gra.nex[j])
               {
                   tmp += gra.weight[j] * p[gra.to[j]];
               }
               p[i] = tmp;
           }
           for (int i = 0; i < info->totalSentence; ++i)
           {
               double tmp = p[i];
               for (int j = graF.head[i]; ~j; j = graF.nex[j])
               {
                   tmp += graF.weight[j] * p[graF.to[j]];
               }
               p[i] /= tmp;
           }
       }

       double mulnum = 1;

       for (int i = 0; i < totalColor; ++i)
       {
           mulnum *= colornum[i];
       }
       mulnum = pow(mulnum, 1.0 / (double)(totalColor));
       for (int i = 0; i < info->totalSentence; ++i)
       {
           p[i] = p[i] * (double)colornum[color[i]] / mulnum;
       }
    }

    double calH()
    {
        double itrd = intraDis();
        double ited = interDis();
        printf("%f %f %f\n", itrd, ited, itrd / ited);
        return itrd / ited;
    }

    void print()
    {
        printf("TotalColor: %d\n", totalColor);
        printf("TotalEdge: %d\n", gra.totE);
        for (int i = 0; i < totalColor; ++i)
        {
            printf("Color %d : %d\n", i, colornum[i]);
        }
//        for (int i = 0; i < info->totalSentence; ++i)
//        {
//            for (int j = i + 1; j < info->totalSentence; ++j)
//            {
//                printf("(%d, %d):%f ",i, j, m[i][j]);
//            }
//            printf("\n");
//        }
    }
    ~SAG()
    {

    }
};
bool cmp2 (Word x, Word y)
{
    return x.weight > y.weight;
}
void getTag(Info *info, SAG *sag)
{
    FILE * fp3;
    string file3;
    file3.clear();
    file3 = "SAGcenterResult\\" + info->type + info->fileName + "_" + info->num + ".txt_vec";
    cout << file3 << endl;
    fp3 = fopen(file3.c_str(), "w");
    for (int i = 0; i < info->totalSentence; ++i)
    {
        for (int j = 0; j < info->sentenceNum[i]; ++j)
        {
            info->word1[info->sentence[i][j]].weight += sag->p[i];
        }
    }
    for (int i = 0; i < info->totalWord; ++i)
    {
        info->word1[i].weight *=  log((20000000.0 / (double)(IDF[info->word1[i].str] + 1e-9)));
    }
    sort(info->word1, info->word1 + info->totalWord, cmp2);

    int totaltag = 0;
    for (int i = 0; i < info->totalWord; ++i)
    {
        if (info->word1[i].str.length() > 3)
        {
            totaltag++;
            fprintf(fp3, "%s %f\n", info->word1[i].str.c_str(), info->word1[i].weight);
            if (totaltag >= 20)
                break;
        }
    }
    fclose(fp3);
}
int main()
{
    freopen("test.out", "w", stdout);
    string filename = "ac2695616", type = "movie\\";
    getIDF("idf2.txt");
    for (int i = 1; i <= 1; ++i)
    {

        Info *info = new Info(filename, type, i);
        info->getInput();
        //info->print();
        //cout <<  "done!" << endl;
        SAG *sag = new SAG(info);

        sag->cluster();
     //   cout <<  "done!" << endl;
        sag->buildGraph();
      //   cout <<  "done!" << endl;
        sag->getWeight();
     //    cout <<  "done!" << endl;
        getTag(info, sag);
        printf("H: %f\n", sag->calH());
        delete(info);
        delete(sag);
        //sag->print();
     //   cout <<  "done!" << endl;
    }
    return 0;
}
