// JAMB.cpp : コンソール アプリケーションのエントリ ポイントを定義します。

#include "stdafx.h"
#include <time.h>
#include <shlwapi.h>
#include <string.h>

unsigned int n_year; 
unsigned int n_month;
unsigned int n_day;

int main(void) {
	void getSysTime(void);

	printf("JAMB-FilePasserProgramStart\n");

	/*現在時刻をシステムから取得*/
	getSysTime();

	/*パスを用意*/
	char pass1[_MAX_PATH] = "C:\\work\\test\\patlite\\patlog_auto";
	char yearPath[5];
	char pass2[] = "_";
	char monthPath[3];
	char pass3[] = "_";
	char dayPath[3];
	char pass4[] = ".csv";

	/*年月日を指定（date.iniファイルが存在する場合）*/
	FILE *date_fp;
	char date_filename[] = "date.ini";
	char date_s[10];
	unsigned char date_row;
	date_fp = fopen(date_filename, "r");
	if (date_fp == NULL) {
		/*ファイルが存在しない場合はなにもしない*/
	}
	else {
		/*ファイルが存在する場合は指定の年月日に書き換える*/
		for (date_row = 1; fgets(date_s, sizeof(date_s), date_fp) != NULL; date_row++) {
			switch (date_row) {
			case 1:
				n_year = atoi(date_s);
				break;
			case 2:
				n_month = atoi(date_s);
				break;
			case 3:
				n_day = atoi(date_s);
				break;
			default:
				break;
			}
		}
	}
	fclose(date_fp);

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

	/*ファイルオープン*/
	/*元データ*/
	FILE *data_fp;
	char s[500];
	char first_row[500];	/*最初の行格納用*/
	char tok_s[500];
	data_fp = fopen(pass1, "r");
	if (data_fp == NULL) {
		printf("%s Couldn't Open¥n", pass1);
		return -1;
	}
	/*ファイル内容nullまで一行ずつ読み出し*/
	unsigned int row, col;
	char *tok;
	unsigned int cmp_row;
	char *cmp_tok;
	char split[] = ",";
	for (row = 1; fgets(s, sizeof(s), data_fp) != NULL; row++) {
		strcpy(tok_s, s);
		/*最初の行*/
		if (row == 1) {
			strcpy(first_row, s);
		}
		else {
		/*二行目以降*/
			tok = strtok(tok_s, split); /* 最初の列を読み出し */
			for (col = 1; tok != NULL; col++) {
				/*２カラム目の値で条件分岐*/
				if (col == 2) {
					/*比較用iniデータ*/
					FILE *cmp_fp;
					char cmp_filename[] = "PatMACInit.ini";
					char cmp_s[100];
					cmp_fp = fopen(cmp_filename, "r");
					if (cmp_fp == NULL) {
						printf("%s Couldn't Open¥n", cmp_filename);
						return -2;
					}
					unsigned char flg1 = 0; /*例外処理用　ヒットしなかった場合*/
					for (cmp_row = 1; fgets(cmp_s, sizeof(cmp_s), cmp_fp) != NULL; cmp_row++) {
						cmp_tok = strtok(cmp_s, split); /* 最初の列を読み出し */
						if (strcmp(tok, cmp_tok) == 0) {
							/*ファイル作成*/
							cmp_tok = strtok(NULL, split); /* 次の列を読み出し = 作成するファイルパス */
							FILE *put_fp;
							char outfilename[100];
							/*ファイルパスにファイル名(年月日)をマージ*/
							sprintf(outfilename, "%s%s%s%s", cmp_tok, yearPath, monthPath, dayPath);
							cmp_tok = strtok(NULL, split); /* 次の列を読み出し = 作成するファイル拡張子 */
							strcat(outfilename, cmp_tok);  /* 文字列連結(pass$ = pass$ + pass2$) */
							/*追記で開く（なければつくる）*/
							put_fp = fopen(outfilename, "a+");	
							if (put_fp == NULL) {
								printf("%s Couldn't Open¥n", outfilename);
								return -3;
							}
							/*書き込む前にファイルを確認（既に記載済みならスキップ）*/
							unsigned int put_row;
							unsigned char flg2 = 0; /*比較結果判断用　既に記載済みの場合*/
							char chk_s[100];
							for (put_row = 1; fgets(chk_s, sizeof(chk_s), put_fp) != NULL; put_row++) {
								if (strcmp(chk_s, s) == 0) {
									flg2 = 1;
									break;/*記載済み発見次第ループ抜け*/
								}
							}
							/*書き込み*/
							if (flg2 != 1) {
								long put_sz;
								/*ファイルサイズを確認し、0であればfirst_rowを記入*/
								fseek(put_fp, 0, SEEK_END);
								put_sz = ftell(put_fp);
								if (put_sz == 0) {
									fprintf(put_fp, first_row);
								}
								/*生データ書き込み*/
								fprintf(put_fp, s);
							}
							flg1 = 1;
							flg2 = 0;
							fclose(put_fp);
							break;
						}
					}
					if (flg1 == 0) {
						/*例外処理　即終了*/
						printf("There are some unregistered data\n");
						return -4;
					}
					flg1 = 0;
					fclose(cmp_fp);
				}
				tok = strtok(NULL, split); /* 次の列を読み出し */
			}
		}
	}

	fclose(data_fp);

	printf("EndTheProgram\n");
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

