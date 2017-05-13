// JAMB.cpp : コンソール アプリケーションのエントリ ポイントを定義します。

#include "stdafx.h"
#include <time.h>
#include <shlwapi.h>
#include <string.h>

unsigned int n_year; 
unsigned int n_month;
unsigned int n_day;

char str1[] = "58C232FFFE57673C";	/*ﾐﾆFA-ﾀﾚﾊﾟﾝIN*/

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
	/*元データ*/
	FILE *data_fp;
	char filename[] = "\F:test.txt";
	char s[1000];
	char tok_s[1000];
	data_fp = fopen(filename, "r");
	if (data_fp == NULL) {
		printf("%sファイルが開けません ¥n", filename);
		return EXIT_FAILURE;
	}
	/*ファイル内容nullまで一行ずつ読み出し*/
	printf("\n");
	printf("-- fgets() --");
	printf("\n");
	unsigned int row, col;
	char *tok;
	unsigned int cmp_row, cmp_col;
	char *cmp_tok;
	char split[] = ",";
	for (row = 1; fgets(s, sizeof(s), data_fp) != NULL; row++) {
		strcpy(tok_s, s);
//	while (fgets(s, 1000, data_fp) != NULL) {
		/*最初の行*/
		if (row == 1) {
			printf("%s", s);
//			continue;
		}
		else
		{
		/*二行目以降*/
			tok = strtok(tok_s, split); /* 最初の列を読み出し */
			for (col = 1; tok != NULL; col++) {
//			while (tok != NULL) {
				printf("%s\n", tok);
				/*２カラム目の値で条件分岐*/
				if (col == 2) {
					/*比較用iniデータ*/
					FILE *cmp_fp;
					char cmp_filename[] = "PatMACInit.ini";
					char cmp_s[100];
					cmp_fp = fopen(cmp_filename, "r");
					if (cmp_fp == NULL) {
						printf("%sファイルが開けません ¥n", cmp_filename);
						return EXIT_FAILURE;
					}

					unsigned char flg1 = 0; /*例外処理用　ヒットしなかった場合*/
					for (cmp_row = 1; fgets(cmp_s, sizeof(cmp_s), cmp_fp) != NULL; cmp_row++) {
						cmp_tok = strtok(cmp_s, split); /* 最初の列を読み出し */
						if (strcmp(tok, cmp_tok) == 0) {
							/*ファイル作成*/
							cmp_tok = strtok(NULL, split); /* 次の列を読み出し = 作成するファイルパス */
							FILE *put_fp;
							char put_s[100];
							put_fp = fopen(cmp_tok, "a");	/*追記で開く（なければつくる）*/
							if (put_fp == NULL) {
								printf("%sファイルが開けません ¥n", cmp_tok);
								return EXIT_FAILURE;
							}
							fprintf(put_fp, s);	/*書き込み*/
							fclose(put_fp);
							flg1 = 1;
							break;
						}
						
					}
					if(flg1 == 0)
					{
						/*例外処理　即終了*/
						printf("登録されていないデータがあります。\n");
						return -2;
					}
					flg1 = 0;
					fclose(cmp_fp);
				}
				tok = strtok(NULL, split); /* 次の列を読み出し */
			}
//			printf("\n");
		}
//		row += 1;
	}

	fclose(data_fp);

	printf("\n終了します。\n");
	return EXIT_SUCCESS;
}

void getSysTime(void) {
	time_t timer; struct tm *local;

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