// JAMB.cpp : コンソール アプリケーションのエントリ ポイントを定義します。

#include "stdafx.h"
#include <time.h>
#include <shlwapi.h>

unsigned int n_year; 
unsigned int n_month;
unsigned int n_day;

int main(void) {
	void getSysTime(void);

	printf("JAMB-ファイル収集処理を始めます。\n");

	/*現在時刻をシステムから取得*/
	getSysTime();

	/*パスを用意*/
	char pass1[_MAX_PATH] = "\"C:\\Users\\70001502\\Desktop\\JAMB-prj\\test_data\\JAMB-Line\\Patlite\\patlog_auto";
	char yearPath[5];
	char pass2[] = "_";
	char monthPath[3];
	char pass3[] = "_";
	char dayPath[3];
	char pass4[] = ".csv\"";

	/*年月日を文字列に変換*/
	sprintf(yearPath, "%04d", n_year);
	sprintf(monthPath, "%02d", n_month);
	sprintf(dayPath, "%02d", n_day);

	/*完全パスになるよう結合*/
	strcat(pass1, yearPath);  /* 文字列連結(pass$ = pass$ + pass2$) */
	strcat(pass1, pass2);  /* 文字列連結(pass$ = pass$ + pass2$) */
	strcat(pass1, monthPath);  /* 文字列連結(pass$ = pass$ + pass2$) */
	strcat(pass1, pass3);  /* 文字列連結(pass$ = pass$ + pass2$) */
	strcat(pass1, dayPath);  /* 文字列連結(pass$ = pass$ + pass2$) */
	strcat(pass1, pass4);  /* 文字列連結(pass$ = pass$ + pass2$) */
						   //	system(pass1);

						   /*ファイルオープン*/
	FILE *fp;
	char *fname = "test.txt";
	char s[1000];

	fp = fopen(fname, "r");
	if (fp == NULL) {
		printf("%sファイルが開けません ¥n", fname);
		return -1;
	}
	printf("\n");
	printf("-- fgets() --");
	printf("\n");
	while (fgets(s, 100, fp) != NULL) {
		printf("%s", s);
	}

	fclose(fp);
	/*ファイル内容nullまで一行ずつ読み出し*/
	/*２カラム目の値で条件分岐*/
	/*例外処理　即終了*/
	/*ファイル作成*/



	printf("終了します。\n");
	return EXIT_SUCCESS;
}

void getSysTime(void) {
	time_t timer; struct tm *local; struct tm *utc;

	/* 現在時刻を取得 */
	timer = time(NULL);

	local = localtime(&timer); /* 地方時に変換 */

							   /* 地方時 変換後表示 */
	printf("処理時刻: ");

	n_year = local->tm_year + 1900;
	n_month = local->tm_mon + 1;
	n_day = local->tm_mday;

	printf("%04d/", n_year);
	printf("%02d/", n_month);
	printf("%02d ", n_day);
	printf("%02d:", local->tm_hour);
	printf("%02d:", local->tm_min);
	printf("%02d", local->tm_sec);
	printf("\n");
}