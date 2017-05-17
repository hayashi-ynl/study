// progress.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//2017-05-17 13:50
//利用上の注意：コピー元フォルダ内のコピー元ファイルは実行時１つであること
// ※２つ以上の場合はどのファイルがコピーされるか不明

#include "stdafx.h"
#include <time.h>
#include <shlwapi.h>
#include <string.h>
#include <windows.h>

unsigned int n_year; 
unsigned int n_month;
unsigned int n_day;

//char program[] = "hello.bat";
char program[] = "C:\\AMADA\\UIFSYS\\Bin\\UIFRSO.exe";

char masterPass[_MAX_PATH] = "M:\\Ｌ・進捗フォルダ\\";
char masterCpPass[_MAX_PATH] = "C:\\work\\JAMB-Line\\progress\\";

char base_filename[_MAX_PATH] = {};
char cp_filename[_MAX_PATH] = {};
char dl_filename[_MAX_PATH] = {};

void getSysTime(void);
int fileCopy(void);

int main(void) {
	printf("JAMB-ArchievementPasserProgram\n\n");

	/*psexecを叩いて進捗ファイルを作成する*/
    int result;
    result = system(program);
    if ( result == EXIT_SUCCESS ) {
        printf("正常に実行されました．\n");
    } else {
        printf("正常に実行されませんでした．\n");
    }

	/*現在時刻をシステムから取得*/
	getSysTime();

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
		fclose(date_fp);	//elseの場合fileOpenするのでclosing
	}

	HANDLE hFind, d_hFind, f_hFind;
	WIN32_FIND_DATA fd, d_fd, f_fd;
	FILETIME ft;
	SYSTEMTIME st;

	char findPass[_MAX_PATH] = {};
	char d_findPass[_MAX_PATH] = {};
	char f_findPass[_MAX_PATH] = {};

	/*保存先フォルダを作る*/
	sprintf(d_findPass, "%s%04d%02d%02d", masterCpPass, n_year, n_month, n_day);
	sprintf(masterCpPass, "%s%04d%02d%02d\\", masterCpPass, n_year, n_month, n_day);
	CreateDirectory(masterCpPass, NULL);

	/* 古いファイルの削除 */
	sprintf(d_findPass, "%s%04d%02d%02d*.scd", masterCpPass, n_year, n_month, n_day);
	d_hFind = FindFirstFile(d_findPass, &d_fd);
	if(d_hFind == INVALID_HANDLE_VALUE) {
		printf("削除該当ファイル無し\n\n");
		//何もせずに終了
	} else {
		//ファイルを削除
		do {
			sprintf(dl_filename, "%s%s", masterCpPass, d_fd.cFileName);
			remove(dl_filename);
		} while(FindNextFile(d_hFind, &d_fd)); //次のファイルを検索
	}

	/* 作成した進捗ファイル名を取得する */
	sprintf(findPass, "%s%04d%02d%02d*.scd", masterPass, n_year, n_month, n_day);
	hFind = FindFirstFile(findPass, &fd);
	if(hFind == INVALID_HANDLE_VALUE) {
		printf("exe実行失敗疑惑\n");
		return -2;
	}
	/* 検索結果の表示とファイルの移動(コピー) */
	unsigned char num = 1;	//ファイル数カウンタ
	printf("検索結果：\n");
	
	/* 更新時間取得 */
	FileTimeToLocalFileTime(&fd.ftLastWriteTime, &ft);
	FileTimeToSystemTime(&ft, &st);

	/* 結果の表示 */
	printf(" %d.ファイル名: %s\n", num, fd.cFileName);

	/* ディレクトリの場合は(ディレクトリ)と表示 */
	if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		printf("(ディレクトリ)\n");
	}

	/* ファイルの移動(コピー) */
	sprintf(base_filename, "%s%s", masterPass, fd.cFileName); //コピー元のパス
	sprintf(cp_filename, "%s%s", masterCpPass, fd.cFileName); //コピー先のパス
	fileCopy();

	num++;

	/* 検索終了 */
	FindClose(hFind);

	printf("\nEndTheProgram\n");
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

int fileCopy(void) {
	FILE* fpSrc;
	FILE* fpDest;
	char c;


	fpSrc = fopen( base_filename, "rb" );
	if( fpSrc == NULL ){
		printf(" コピー元のファイルオープン失敗。\n");
		exit( EXIT_FAILURE );
	}

	fpDest = fopen( cp_filename, "wb" );
	if( fpDest == NULL ){
		printf(" コピー先のファイルオープンに失敗。\n");
		exit( EXIT_FAILURE );
	}


	while( 1 ){
		/* バイナリデータとして 1Byteずつ読み込み、1Byteずつ書き込む */
		fread( &c, sizeof(c), 1, fpSrc );
		if( feof( fpSrc ) ){
			break;
		}
		if( ferror( fpSrc ) ){
			printf(" バイナリデータとしての読み込みに失敗。\n");
			exit( EXIT_FAILURE );
		}

		fwrite( &c, sizeof(c), 1, fpDest );
	}


	if( fclose( fpDest ) == EOF ){
		printf(" ファイルクローズに失敗しました。\n");
		exit( EXIT_FAILURE );
	}
	if( fclose( fpSrc ) == EOF ){
		printf(" ファイルクローズに失敗しました。\n");
		exit( EXIT_FAILURE );
	}

	return EXIT_SUCCESS;
}
