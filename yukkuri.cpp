//
// Published Copyright(C) H.Itoh
//
//【yukkuri.cpp】
//
// 概要:  AquesTalk2 Linux を利用したゆっくり音声再生プログラム
//        引数に-oを指定することによりファイル名, -p を指定する
//        ことにより声色を設定することができる
//                
// usage: yukkuri -o <output.wav>
//                -p <phon>
//                 <  string 
// 声色リスト
// +---------------------------------------------------------+
// |       SofTalkの声 音声合成エンジン 声種 備考            |
// +---------------------------------------------------------+
// | 女性１ AquesTalk f1 女声１ ゆっくり霊夢でよく使われる   |
// | 女性２ AquesTalk f2 女声２ ゆっくり魔理沙でよく使われる |
// | 男性１ AquesTalk m1 男声１                              |
// | 男性２ AquesTalk m2 男声２                              |
// | ロボット AquesTalk r1 ロボット１                        |
// | 中性 AquesTalk IMD1                                     |
// | 機械  AquesTalk DVD                                     |
// | 特殊 AquesTalk JGR                                      |
// | 女性A AquesTalk2 defo1 別途公開                         |
// | 女性B AquesTalk2 f1b 公開終了                           |
// | 女性C AquesTalk2 f1c                                    |
// | 女性D AquesTalk2 huskey                                 |
// | 女性E AquesTalk2 momo1 別途公開                         |
// | 女性F AquesTalk2 rb2                                    |
// | 女性G AquesTalk2 rm                                     |
// | 女性H AquesTalk2 yukkuri                                |
// | 男性A AquesTalk2 m3 公開終了                            |
// | 男性B AquesTalk2 m4                                     |
// | ロボット2 AquesTalk2 robo                               |
// | 中性2 AquesTalk2 teto1 別途公開                         |
// +---------------------------------------------------------+
//
#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <string>
#include "AquesTalk2.h"
using namespace std;

// 声色読み込み関数
void* file_load(const char* file, int* psize);

int main(int argc, char *argv[]){
  int size;
  // 文字列長は1024文字
  char str[1024];
  // 出力ファイル名(.wav)
  string wav_file;
  // 音声ファイル名(.phont)
  string koe_file;
  int opt = 0;
  while((opt = getopt(argc, argv,"o:p:")) != -1){
    switch(opt){
    case 'o':
      wav_file = optarg;
      break;
    case 'p':
      koe_file = optarg;
      break;
    case ':': // no value applied
    case '?': // invalid option
      exit(1);
    }
  }
  // usage
  if(wav_file.empty() || koe_file.empty()){
    cerr << "[error] usage: yukkuri -o <wav> -p <phont> < string " << endl;
    return -1;
  }
  // 引数(-p)で指定された声色ファイルを指定
  void *pPhont = file_load(koe_file.c_str(), &size);
  if (pPhont == 0) return -2;
  
  // 音声記号列を標準入力から入れる
  if(fgets(str, 1024-1, stdin)==0) return 0;
  
  // メモリ上に音声データを音声合成
  unsigned char *wav = AquesTalk2_Synthe_Utf8(str, 100, &size, pPhont);
  if(wav == 0){
    fprintf(stderr, "ERR:%d\n",size);
    return -1;
  }
  // Phontデータの開放
  free(pPhont);
  
  // 音声データ(wavフォーマット)の出力
  FILE *fp = fopen(wav_file.c_str(), "wb");
  fwrite(wav, 1, size, fp);
  fclose(fp);
  
  // 音声データバッファの開放
  AquesTalk2_FreeWave(wav);
  
  return 0;
}

// 声色ファイルの読み込み
void* file_load(const char* file, int* psize){
  FILE *fp; 
  char *data;
  struct stat st; 
  *psize = 0; 
  if( stat(file, &st)!=0) return NULL; 
  if((data=(char *)malloc(st.st_size))==NULL){ 
    fprintf(stderr,"can not alloc memory(file_load)¥n");
    return NULL; 
  } 
  if((fp=fopen(file,"rb"))==NULL){
    free(data);
    perror(file);
    return NULL; 
  } 
  if(fread(data, 1, st.st_size, fp)<(unsigned)st.st_size){ 
    fprintf(stderr,"can not read data (file_load)¥n"); 
    free(data); 
    fclose(fp); 
    return NULL; 
  } 
  fclose(fp); 
  *psize = st.st_size; 
  return data; 
}
