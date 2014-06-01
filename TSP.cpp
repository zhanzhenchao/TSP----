/*Code provided by Rico(zhanzhenchao)*/
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <iomanip>
#include <math.h>
using namespace std;

//initialize City and TSP questions
#define num 10             //the number of city
#define m 11              //the amount of city about TSP m = num+1;
#define init_spe 100      // the population initialization
#define Cata 30           //Cataclysm

int hash[m*2] = {80,70,70,85,60,65,40,90,20,80,10,60,20,30,40,20,60,30,70,80,80,70};                       //initialize the location of Citys
//--------------------------------------------------------------------------------
typedef struct City
{
    int mark;                    //city mark
    int xi;
    int yi;
}City;

typedef struct Population
{
    City s[100];                     //city array
    double distance;
    double Fitness;              //adaptation
    double Fitness_pi;           //adaptation_rate
    double acc_pi;               //accumlation rate
    int cnt;                     //selection time
}Population;

class TSP
{
public:
    TSP();
    ~TSP();
    void Init_S_Population(int groups);
    void init_Population();
    void random_array(int n);
    void adaptation_rate();
    void accumlation_rate();
    void Roulette();
    void copy_population();
    void Intersect_all();
    void record();
    void Reset();
    bool Optimal();
    void Optimal_numeric();
    void RunOneMoreTime();
    void TSP_Answers();

private:
    double City_Distance(Population &a);
    double adaptation(Population &a);
    void Intersect(Population &a, Population &b);
    void Intersect_Remove(Population &a, int b[], int k);
    void Remove(int s[], int n);
    void copy(Population &a, Population &b);
    void swap(int &a, int &b);
    void RecordTheBest(int i);
    double DistanceC(int ax,int ay,int bx,int by);
    Population spe[init_spe];              //each population
    int randp[num];                       //random array from 1 to num-1
    int generation;
    int init_s[init_spe][m];              //initialize and random population
    Population best[Cata];                //record every best population before cataclysm
    int best_i;                           //for comparing all best[Cata] variables
};

TSP::TSP()
{
    for (int i = 0; i < init_spe; i++)
        {
            spe[i].Fitness = double(0);
            spe[i].Fitness_pi = double(0);
            spe[i].acc_pi = double(0);
            spe[i].cnt = 0;
        }
    generation = 0;
    best_i = 0;
}

TSP::~TSP(){}

double TSP::City_Distance(Population &a)                            //return Population S1 distance;
    {
        double d;
        a.distance = 0;
        for (int i = 0; i < num; i++)
            {
                d = DistanceC(hash[(a.s[i+1].mark)*2],hash[(a.s[i+1].mark)*2+1],
                    hash[(a.s[i].mark)*2],hash[(a.s[i].mark)*2+1]);
                a.distance += d;
            }
        return a.distance;
    }

double TSP::DistanceC(int ax,int ay,int bx,int by)
    {
        double hds;
        hds = (double)sqrt(pow((ax-bx),2)+pow((ay-by),2));
        return hds;
    }

void TSP::init_Population()
    {
        for (int i = 0; i < init_spe; i++)
            for (int j = 0; j < m; j++)
                spe[i].s[j].mark = init_s[i][j];
    }

double TSP::adaptation(Population &a)
    {
        double sum = City_Distance(a);
        a.Fitness = ((double)1/sum )*100;
        return a.Fitness;
    }

void TSP::adaptation_rate()
    {
        double d = 0;
        for (int i = 0; i < init_spe; i++)
        {
            adaptation(spe[i]);
            d += spe[i].Fitness;
        }
        for (int j = 0; j < init_spe; j++)
            spe[j].Fitness_pi = (double)spe[j].Fitness / (double)d;
    }

void TSP::accumlation_rate()
    {
        for (int i = 0; i < init_spe; i++)
            {
                for (int j = 0; j < i+1; j++)
                    spe[i].acc_pi += spe[j].Fitness_pi;
            }
    }

void TSP::Roulette()
    {
        double r[init_spe];
        for (int i = 0; i < init_spe; i++)
            {
                r[i] = rand() / double(RAND_MAX);
            }
        for (int j = 0; j < init_spe; j++)
            {
                for (int i = 0; i < init_spe; i++)
                {
                    if (i == 0)
                    {
                        if (spe[i].acc_pi >= r[j])
                            spe[i].cnt++;
                    }
                    else
                    {
                        if (spe[i-1].acc_pi <= r[j] && spe[i].acc_pi >= r[j] )
                            spe[i].cnt++;
                    }
                }
            }
    }

void TSP::Intersect(Population &a, Population &b)       //intersect two population
{
    int k = 1 + rand() % (num - 1);               //intersecting  k digits
    int *a1 = new int[num+k], *b1 = new int[num+k];
    for (int i = 0; i < k; i++)               //choosing digits
        {
            a1[i] = a.s[i+1].mark;
            b1[i] = b.s[i+1].mark;
        }
    Intersect_Remove(a,b1,k);
    Intersect_Remove(b,a1,k);
    for (int i = 0; i < num; i++)
        {
            a.s[i+1].mark = b1[i];
            b.s[i+1].mark = a1[i];
        }
}

void TSP::Intersect_Remove(Population &a, int b[], int k)
    {
        int part[num];
        for (int i = 0; i < num; i++)
            {
                part[i] = a.s[i+1].mark;
            }
        for (int i = 0; i < num; i++)
            {
                b[k+i] = part[i];
            }
        Remove(b,num+k);
    }

void TSP::Remove(int s[],int n)
{
    bool a[num];
    memset(a, false, sizeof(bool)*num);
    int p = 0;
    for (int i = 0; i < n; i++)
        {
            int v = s[i];
            if (a[v] == false)
                {
                    s[p++] = s[i];
                    a[v] = true;
                }
        }
}

void TSP::Intersect_all()
    {
        for (int i = 0; i < init_spe/2; i++)
            Intersect(spe[i],spe[init_spe/2+i]);
    }

void TSP::copy_population()
    {
        int d = 0;
        for (int i = 0; i < init_spe; i++)
            {
                if (spe[i].cnt <= 1)continue;
                while (spe[i].cnt && spe[i].cnt != 1)
                    {
                        for (d; d < init_spe; d++)
                            {
                            if (!spe[d].cnt)
                                {
                                    copy(spe[i],spe[d++]);
                                    break;
                                }
                            }
                        spe[i].cnt--;
                    }
            }
        generation++;
    }

void TSP::copy(Population &a, Population &b)
    {
        for (int i = 0; i < m; i++)
            {
                b.s[i].mark = a.s[i].mark;
            }
    }

bool TSP::Optimal()
    {
        bool op[1000];
        memset(op, false, sizeof(bool)*1000);
        double v = spe[0].Fitness_pi;
        int idx = v * 1000;
        op[idx] = true;
        for (int i = 1; i < init_spe; i++)
            {
                v = spe[i].Fitness_pi;
                idx = v * 1000;
                if (!op[idx]) return false;
            }
        return true;
    }

void TSP::Optimal_numeric()
    {
        cout<<"the best sequence of TSP is :"<<endl;
        for (int i = 0; i < m; i++)
            cout<<spe[0].s[i].mark<<"  ";
        cout<<endl;
        cout<<"the shortest path of distance of TSP is:"<<endl;
        cout<<spe[0].distance<<endl;
        generation = 0;
    }

void TSP::record()
    {
        cout<<setw(40)<<generation<<endl;
        cout<<setw(22)<<"Population"<<" "<<"|"<<setw(10)<<"Fitness"<<" "<<"|"<<setw(10)<<"Fitness_pi"<<" "<<"|"
                    <<setw(10)<<"Accumulation"<<" "<<"|"<<setw(5)<<"count"<<" "<<"|"<<endl;
         cout<<"---------------------------------------------------------------------"<<endl;
        for (int i = 0; i < init_spe; i++)
            {
                for (int j = 0; j < m; j++)
                    {
                        cout<<spe[i].s[j].mark<<" ";
                    }
                cout<<" "<<"|"<<setw(10)<<spe[i].Fitness<<" "<<"|"<<setw(10)<<spe[i].Fitness_pi<<" "<<"|"
                    <<setw(12)<<spe[i].acc_pi<<" "<<"|"<<setw(5)<<spe[i].cnt<<" "<<"|"<<endl;
                cout<<"---------------------------------------------------------------------"<<endl;
            }
    }

void TSP::Reset()
    {
    for (int i = 0; i < init_spe; i++)
        {
            spe[i].Fitness = double(0);
            spe[i].Fitness_pi = double(0);
            spe[i].acc_pi = double(0);
            spe[i].cnt = 0;
        }

    }

void TSP::swap(int &a, int &b)
    {
        int temp = a;
        a = b;
        b = temp;
    }

void TSP::random_array(int n)  //num;
    {
        int j = 0;
        for (int i = 1; i < n; i++)
            randp[i] = i;
        for (int i = 1; i < n - 1; i++)
            {
                j = i + rand() % (n-1-i);
                swap(randp[i],randp[j]);
            }
    }

void TSP::Init_S_Population(int groups)  //m, groups
{
    for (int i = 0; i < groups; i++)
        {
            init_s[i][0] = 0;
            init_s[i][m-1] = 0;
            random_array(num);
            for (int j = 1; j < num; j++)
                {
                    init_s[i][j] = randp[j];
                }
        }
}

void TSP::RecordTheBest(int i)
    {
        for (int j = 0; j < m; j++)
            best[i].s[j].mark = spe[0].s[j].mark;
        best[i].distance = spe[0].distance;
    }

void TSP::TSP_Answers()
    {
        int temp = 0;
        for (int i = 0; i < Cata-1; i++)
            {
                if ( best[temp].distance > best[i].distance)
                    temp = i;
            }
        cout<<endl<<endl;
        cout<<"the ultimate answers of TSP sequence is:";
        for (int i = 0; i < m; i++)
            cout<<best[temp].s[i].mark<<"  ";
        cout<<endl;
        cout<<"the shortest path of distance of TSP is:"<<endl;
        cout<<best[temp].distance<<endl;
    }

void TSP::RunOneMoreTime()
    {
        RecordTheBest(best_i++);
        Init_S_Population(init_spe);
        init_Population();
        adaptation_rate();
        accumlation_rate();
        Roulette();
        //record();
        for ( int i = 0; i < 100; i++)        //ten generation
            {
                copy_population();
                Reset();
                Intersect_all();
                adaptation_rate();
                accumlation_rate();
                Roulette();
                //record();
                if (Optimal())break;
            }
        Optimal_numeric();
    }

int main()
    {
        srand((unsigned)time(NULL));
        TSP GA;
        GA.Init_S_Population(init_spe);
        GA.init_Population();                //initialize four groups of population
        //-----------------------
        GA.adaptation_rate();                //count each group of adaptation rate;
        GA.accumlation_rate();               //count each group of accumlation_rate;
        GA.Roulette();                       //random to choose population
        //GA.record();
        for ( int i = 0; i < 100; i++)        //ten generation
            {
                GA.copy_population();         //copy population and cover previous population
                GA.Reset();                   //resect all data;

                GA.Intersect_all();           //intersect and changed every two parts array
                GA.adaptation_rate();
                GA.accumlation_rate();
                GA.Roulette();
                //GA.record();
                if (GA.Optimal())break;
            }
        GA.Optimal_numeric();
        for (int i = 0; i < Cata; i++)
           GA.RunOneMoreTime();
        GA.TSP_Answers();
        //---------------------
    }
