//----
// 使っているプログラミング環境の極限見極めチャレンジ
//
// 課題【アルゴリズムとして「エラトステネスの篩」を使え。】を尊重
// 時間が無いのでこれ以上踏み込めなかった…
//
// 2019/01/30 コンパイラに -O2 オプションつけ忘れてたのでつけた。あと変数の初期化忘れの修正。
//
// Intel(R) Xeon(R) CPU E5-2699 v3 @ 2.30GHz
// 標準入力:240000000
// 標準出力:13161544↵ 239999987↵
// 処理時間は大体2.99秒
//
// 2019/01/30 更新
// 標準入力:450000000
// 標準出力:23853038↵ 449999993↵
// 2.89秒経過↵
//
//----

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <x86intrin.h>
#define BYTE_OFFSET 64

#define TIMER_ON 0//提出時はここの「1」を「0」に書き換える
#if TIMER_ON
#include <time.h> 
clock_t start;
#endif

inline static void setflag(uintmax_t i, intptr_t *ptr) {
    //素数じゃない値に1を立てる用
    *(ptr + i / BYTE_OFFSET) |= (1ul << i % BYTE_OFFSET);
}

inline static uintmax_t getflag(uintmax_t i, intptr_t *ptr) {
    //素数じゃない時に1を返す
    return *(ptr + i / BYTE_OFFSET) & (1ul << i % BYTE_OFFSET);
}

//練習プログラムなのでバッファのチェックはしていない。
int main(int argc, char* argv[])
{
    #if TIMER_ON
    start = clock();
    #endif

    uintmax_t x;
    
    x = 0;
    
    //受け取れる最大値を受け取るように
    //おそらく 18446744073709551615
    scanf("%ju", &x);

    //動的メモリ確保ポインタ
    intptr_t *arr_ptr;
    
    //動的メモリ確保の為にとりあえずuintmax_tサイズ毎でcalloc　多分メモリアロケーションもその幅かもしれない
    //あまりがあったら1を足すようにしたい。
    uintmax_t r = x % BYTE_OFFSET;
    if( r > 0 ) r = 1;

    //intmax_tサイズで割って、余りがあったら+1の長さで確保
    arr_ptr = (intptr_t *)calloc( x / BYTE_OFFSET + r , sizeof(uintmax_t)); 
    
    if ( !arr_ptr ) {
        fprintf(stderr, "メモリが確保できなかった…\n");
        return -1;
    }
    
    //----
    // メイン処理
    //----
    
    //探す素数の最大値
    double sq_x = sqrt(x);
    
    uintmax_t prime_index,i;

    prime_index = 2; //素数探索は2から始める
    *(arr_ptr) |= (1ul << 0);//1は素数じゃない
    
    while(prime_index < sq_x)
    {
        i = prime_index;
        
        i *= i;
        
        //素数を篩で落とす処理
        while(i <= x)
        {
            setflag(i, arr_ptr);//素数じゃないフラグを立てる
            i += prime_index; //倍数にする
        }

        //次の素数を探す
        if(prime_index < 30) //30未満は普通に探す
        {
            prime_index++;
    
            //素数じゃないフラグ立ってる間インクリメント
            while(getflag(prime_index, arr_ptr))
            {
                prime_index++;
            }
        }
        else //30以上は篩い落としたものを飛ばしていくスタイル
        {
            do
            {
                i = prime_index % 30;
                switch(i)
                {
                    //7,11,13,17,19,23,29だけにしていくスタイル
                    //ちょっと書き方がかっこ悪い
                    case 1:
                            prime_index += 6;
                            break;
                    case 7: 
                    case 13:
                    case 19:
                            prime_index += 4;
                            break;
                    case 11: 
                    case 17: 
                    case 29:
                            prime_index += 2;
                            break;
                    default:
                            prime_index++;
                }
            }while(getflag(prime_index, arr_ptr));
        }
    }
    
    //表示部分
    if(x < 100)
    {
        for(i=2; i<x; i++)
        {
            if(!(getflag(i, arr_ptr)))
            {
                printf(" %jd", i);
            }
        }
        printf("\n");
    }
    else
    {
        uintmax_t j = 0, max;

        //64bit幅で数える
        for(i = 0; i < x / BYTE_OFFSET; i++)
        {
            j += _popcnt64(*(arr_ptr + i));
            
            //↓x86intrin.hをインクルードしてもコンパイラで有効化
            // されていない場合を考慮してアセンブラで書いたけど、
            // 特に高速化しなかったので使わなかった。
            /*
            uintmax_t count;
            __asm__ volatile (
                "popcntq %1, %0"
                : "=r"(count)
                : "r"(*(arr_ptr + i))
            );
            j += count;
            */
        }
        //余りを数える
        for(i=x - x % BYTE_OFFSET; i<=x; i++)
        {
            if((getflag(i, arr_ptr)))
            {
                j++;
            }
        }
        //最大値を逆から探索
        for(i = x; i > 0; i--)
        {
            if(getflag(i, arr_ptr) == 0)
            {
                max = i;
                break;
            }
        }
        printf("%jd\n%jd\n", x-j, max);
    }
    
    //freeにptrを投げると警告が出るのでvoid *へキャスト。これでいいのか不明。
    free((void *)arr_ptr);
 
    #if TIMER_ON
    printf("%.2f秒経過\n",(double)(clock()-start)/CLOCKS_PER_SEC);
    #endif
    
    return 0;
}
