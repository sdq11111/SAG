#include <iostream>
#include <map>
#include <cstdio>
#include <cmath>
#include <sstream>
#include <cstring>
#include <string>
#include <algorithm>

using namespace std;

map <string, int> IDF;
const double threshold = 0.0, alpha = 0.12, beta = 0.34;
const int d = 200;
const string vecFile = "sentence_vector\\", segFile = "word_segmentation\\", tagFile = "result\\";

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
/* load the IDF score*/
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
    /* check the part of speech of each word in TSCs */
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
    /* If none of the words in a TSC is a meaningful word (according to the part of speech), then the TSC is noise.*/ 
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
    Info(string fileName)
    {
        inputFile.clear();
        inputClass.clear();
        this->fileName = fileName;
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
        inputFile = vecFile + fileName + ".txt";
        inputClass = segFile + fileName + ".txt";
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

    int fa[5000];
    int lis[5000][5000];
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
    int totalEdge;

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
    /* calculate the semantic distance of two sentences*/
    void initCluster()
    {
        for (int i = 0; i < info->totalSentence; ++i)
        {
            fa[i] = i;
            subgraph[i].num = 1;
            subgraph[i].val = 0;
            lis[i][0] = i;
        }
    }
    int getfather(int x)
    {
        if (fa[x] == x) return fa[x];
        return fa[x] = getfather(fa[x]);
    }
    double calUnion(int x, int y)
    {
        double tmp = 0;
        for (int i = 0; i < subgraph[x].num; ++i)
        {
            for (int j = 0; j < subgraph[y].num; ++j)
            {
                tmp += (m[lis[x][i]][lis[y][j]] + m[lis[y][j]][lis[x][i]]);
            }
        }
        return (subgraph[x].val * ((subgraph[x].num - 1)* subgraph[x].num / 2.0) + subgraph[y].val * ((subgraph[y].num - 1)* subgraph[y].num / 2) + tmp) / ((double)(subgraph[x].num + subgraph[y].num) * (subgraph[x].num + subgraph[y].num - 1) / 2.0);
    }
    void graphUnion(int x, int y, double val)
    {
        for (int i = subgraph[x].num; i < subgraph[x].num + subgraph[y].num; ++i)
        {
            lis[x][i] = lis[y][i - subgraph[x].num];
        }
        subgraph[x].num += subgraph[y].num;
        fa[y] = x;
        subgraph[x].val = val;
        subgraph[y].num = 0;
        subgraph[y].val = 0;
    }
	/* basic algorithm of dis-joint set*/
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
	/* add an edge into the graph*/
    void initGraph()
    {
        gra.totE = 0;
        graF.totE = 0;
        memset(gra.head, -1, sizeof(gra.head));
        memset(graF.head, -1, sizeof(graF.head));
        memset(gra.weight, 0, sizeof(gra.weight));
        memset(graF.weight, 0, sizeof(graF.weight));
    }

    int totalColor, thisColor, color[5000], colornum[5000];
    bool v[5000];
    void initCol()
    {
        totalColor = 0;
        memset(color, 0, sizeof(color));
        memset(colornum, 0, sizeof(colornum));
        memset(v, false, sizeof(v));
    }

    void dfs(int u)
    {
        thisColor++;
        v[u] = true;
        color[u] = totalColor;
        lisC[totalColor][colornum[totalColor]++] = u;
        for (int i = gra.head[u]; ~i; i = gra.nex[i])
        {
            int vp = gra.to[i];
            if (!v[vp])
            {
                dfs(vp);
            }
        }
    }
	/* visit the Connected graph */
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
                    totalDis += 2.0 * dis(lisC[k][i], lisC[k][j]);
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
                        double tmp = dis(lisC[k1][i], lisC[k2][j]);
                        totalDis += tmp;
//                        if (tmp > 1e-6)
//                        {
//                            printf("%d %d %f", i, j, tmp);
//                        }
                    }
                }
                totalDis /= double(colornum[k1] * colornum[k2]);
                totalInter += totalDis;
            }
        }
        return totalInter /= (double)(0.5 * K * (K - 1));
    }

public:
    SAG(Info * info)
    {
        this->info = info;
        totalEdge = 0;
    }
    double p[5000];

    void buildEdge()
    {
        for (int i = 0; i < info->totalSentence; ++i)
        {
            for (int j = i + 1; j < info->totalSentence; ++j)
            {
                m[i][j] = dis(i,j);
                if (m[i][j] <threshold)
                    m[i][j] = 0;
                if (m[i][j])
                {
                    edge[totalEdge].x = i;
                    edge[totalEdge].y = j;
                    edge[totalEdge++].val = m[i][j];
                }
                //printf("%d %d %d %d\n", info->totalSentence, i, j, totalEdge);
            }
        }
    }

    void cluster()
    {
        std::sort(edge, edge + totalEdge, cmp);
        initCluster();
        for (int i = 0; i < totalEdge; ++i)
        {
            int fax = getfather(edge[i].x);
            int fay = getfather(edge[i].y);
            if (fax == fay)
                continue;
            else
            {
                double g = calUnion(fax, fay);
               if (g > beta)
                   graphUnion(fax, fay, g);
               else
                   m[edge[i].x][edge[i].y] = 0;
            }
        }
    }
	/* Algorithm 1*/
    void buildGraph()
    {
        //cout << "startbuildgraph" << endl;
        initGraph();
        //cout << "endinit" << endl;
       // printf("%d\n", info->totalSentence);
        for (int i = 0; i < info->totalSentence; ++i)
       {
           for (int j = i + 1; j < info->totalSentence; ++j)
           {
               //printf("%d %d : ", i, j);
               if (m[i][j] > 0)
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
       initCol();
       double mulnum = 1;
       for (int i = 0; i < info->totalSentence; ++i)
       {
           if (!v[i])
           {
              thisColor = 0;
              dfs(i);
              totalColor++;
              mulnum *= thisColor;
           }
       }
       mulnum = pow(mulnum, 1.0 / (double)(totalColor));
       for (int i = 0; i < info->totalSentence; ++i)
       {
           p[i] = p[i] * (double)colornum[color[i]] / mulnum;
       }
    }
	/* calculate the weight of TSCs by Eq.(13)-(16) */
	
    int lisC[5000][5000];
    //int Tcol[5000], totC, C[5000];
    double calH()
    {

        double itrd = intraDis();
        double ited = interDis();
        printf("%f %f %f\n", itrd, ited, itrd / ited);
        return itrd / ited;
    }
    /* calculate the H value to check the clustering effect*/
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
    file3 = tagFile + info->fileName + ".txt";
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
 //   freopen("test.out", "w", stdout);
    string filename = "sample";
    getIDF("idf2.txt");
    Info *info = new Info(filename);
    info->getInput();
    //info->print();
    //cout <<  "done!" << endl;
    SAG *sag = new SAG(info);
    sag->buildEdge();
   // cout <<  "done!" << endl;
    sag->cluster();
 //   cout <<  "done!" << endl;
    sag->buildGraph();
  //   cout <<  "done!" << endl;
    sag->getWeight();
 //    cout <<  "done!" << endl;
    getTag(info, sag);

    sag->calH();
    delete(sag);
    delete(info);
 //   cout <<  "done!" << endl;
    return 0;
}
