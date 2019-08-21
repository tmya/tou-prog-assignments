#include <stdio.h>
#include <math.h>

//練習プログラムなのでバッファのチェックはしていない。
int main(int argc, char* argv[])
{
    int i,p;
    double sq_x;
    int x;
    
    p = 2; //エラトステネスの篩なので2からスタート
    scanf("%d", &x);
    
    char ar[x];
    for(i=0; i<x; i++) ar[i] = 0;
    
    sq_x = sqrt((double)x);
    
    //xの平方根まで探索
    while(p < sq_x)
    {
        i = p; //計算用を一時保存
        
        //xまで繰り返す
        while(i <= x)
        {
            if(i != p)
            {
                ar[i-1] = 1; //篩い落とすフィルタ
            }
            i += p; //倍数にする
        }
        //次のpを探す
        p++;
        while(ar[p-1] == 1)
        {
            p++;
        }
    }
    
    //出力制御
    if(x < 100)
    {
        for(i=0; i<x; i++)
        {
            if(ar[i] == 0 && i > 0)
            {
                printf(" %d", i+1);
            }
        }
        printf("\n");
    }
    else
    {
        int j = 0, max;
        for(i=0; i<x; i++)
        {
            if(ar[i] == 0 && i > 0)
            {
                j++;
                max = i+1;
            }
        }
        printf("%d\n%d\n", j, max);
    }
    //printf("%f", sq_x);
    
    return 0;
}
