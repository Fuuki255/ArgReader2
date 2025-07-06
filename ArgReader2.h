#ifndef ARG_READER_H
#define ARG_READER_H

#define ARG_READER_VERSION {2, 0, 0}

/* Library */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>



/* Pre-Processer */

#define MAX(a, b) ((a) > (b) ? (a) : (b))



/* Enumeration */

typedef enum ARG_OptionType {
	ARG_OPTION_STRING,
	ARG_OPTION_BOOLEAN,
	ARG_OPTION_NUMBER,
	ARG_OPTION_ARRAY,
} ARG_OptionType;


typedef enum ARG_Status {
	ARG_STATUS_NONE,

	ARG_STATUS_OPTION_NOT_EXIST,
	ARG_STATUS_INPUT_NOT_COMPLETED,
	ARG_STATUS_INDEX_OVERED,

	ARG_STATUS_FAILED_TO_ALLOCATE_MEMORY,
	ARG_STATUS_HELP,
} ARG_Status;



/* Type Defines */

typedef struct ARG_Value ARG_Value;
typedef struct ARG_Option ARG_Option;
typedef struct ARG_Reader ARG_Reader;





/* Types */

typedef struct ARG_Option {
	const char* name;
	const char* shortname;
	const char* description;

	ARG_OptionType type;
	void* value;

	int arrayLength;        // 配列型オプション専用、配列の長さ

    // Relationship
	ARG_Option* front;
	ARG_Option* back;
} ARG_Option;



/* Variables */

const char* ARG_privateDescription = NULL;

ARG_Option* ARG_privateFirstOption = NULL;
ARG_Option* ARG_privateLastOption = NULL;

ARG_Option** ARG_privateIndexes = NULL;
int ARG_privateIndexesLength = 0;

int ARG_privateNowArg = 1;
int ARG_privateNowIndex = 0;

ARG_Status ARG_privateStatus = ARG_STATUS_NONE;
char ARG_privateErrorMsg[128];




/* ARG_Option メソッド */

// 配列オプションに値を追加する関数
int ARG_Option_ArrayAdd(ARG_Option* option, const char* value) {
    // 配列拡張
	if (option->arrayLength % 5 == 0) {
		const char** newBuffer = (const char**)realloc(option->value, (option->arrayLength + 1) * sizeof(const char*));

		if (newBuffer == NULL) {
			strcpy(ARG_privateErrorMsg, "Failed to allocate memory");
			return ARG_STATUS_FAILED_TO_ALLOCATE_MEMORY;
		}

		option->value = newBuffer;
	}

    // 値を追加
	((void**)option->value)[option->arrayLength++] = (void*)value;
	return 0;
}


// 文字列によってオプションに値を設定する関数
int ARG_Option_SetValueByString(ARG_Option* option, const char* value) {
	switch (option->type) {
		case ARG_OPTION_STRING:
			option->value = (void*)value;
			return 0;
		case ARG_OPTION_BOOLEAN:
			option->value = (void*)true;
			return 0;
		case ARG_OPTION_NUMBER:
			sscanf(value, "%lf", (double*)&option->value);
			return 0;
		case ARG_OPTION_ARRAY:
			ARG_Option_ArrayAdd(option, value);
			return 0;
		default:
			sprintf(ARG_privateErrorMsg, "Unknown type %d in '%s'", option->type, option->name);
			// ARG_PrintError()
			return -1;
	}
}


const char* ARG_StringOptionType(ARG_OptionType type) {
	switch (type) {
		case ARG_OPTION_STRING:
			return "String";
		case ARG_OPTION_BOOLEAN:
			return "Boolean";
		case ARG_OPTION_NUMBER:
			return "Number";
		case ARG_OPTION_ARRAY:
			return "String Array";
		default:
			return "Unknown";
	}
}

const char* ARG_StringOptionValue(ARG_Option* option) {
    static char buf[20];

    switch (option->type) {
        case ARG_OPTION_STRING:
            return (const char*)option->value;
        case ARG_OPTION_BOOLEAN:
            return option->value ? "true" : "false";
        case ARG_OPTION_NUMBER:
            sprintf(buf, "%lg", *(double*)&option->value);
            return buf;
        case ARG_OPTION_ARRAY:
            sprintf(buf, "string[%d]", option->arrayLength);
            return buf;
        default:
            return "Unknown";
    }
}



/* メソッド */

#define ARG_ForeachOptions(option) for (ARG_Option* option = ARG_privateFirstOption, *option##Next = option ? option->front : NULL; option; option = option##Next, option##Next = option ? option->front : NULL)


// Init
int ARG_Init(const char* description) {
    ARG_privateDescription = description;

	ARG_privateFirstOption = NULL;
	ARG_privateLastOption = NULL;

    ARG_privateIndexes = (ARG_Option**)malloc(5 * sizeof(ARG_Option*));
    ARG_privateIndexesLength = 0;

	ARG_privateNowArg = 1;
	ARG_privateNowIndex = 0;

    ARG_privateStatus = ARG_STATUS_NONE;
    return 0;
}

// Quit
void ARG_Quit() {
    // Destroy Options
    ARG_ForeachOptions(option) {
        if (option->type == ARG_OPTION_ARRAY) {
            free(option->value);
        }
        free(option);
    }

    // Destroy Indexes
    if (ARG_privateIndexes) {
        free(ARG_privateIndexes);
    }
}



/* GetError */

#define ARG_GetError() ARG_privateErrorMsg


/* GetOption */

// オプションを取得する関数
ARG_Option* ARG_GetOption(const char* name) {
	ARG_ForeachOptions(option) {
		if (strcmp(option->name, name) == 0) {
			return option;
		}
	}
	sprintf(ARG_privateErrorMsg, "Option '%s' not exists", name);
	return NULL;
}

// オプションから文字列を取得する関数
const char* ARG_GetOption_String(const char* name) {
	ARG_Option* option = ARG_GetOption(name);
	if (option == NULL) {
    #ifndef ARG_NODEBUG
        fprintf(stderr, "DEBUG: %s: Failed to get option! (%s)\n", __func__, ARG_GetError());
    #endif
		return "";
	}

	return (const char*)option->value;
}

// オプションから真偽値を取得する関数
bool ARG_GetOption_Boolean(const char* name) {
	ARG_Option* option = ARG_GetOption(name);
	if (!option) {
		return false;
	}

	return (bool)(long)option->value;
}

// オプションから数値を取得する関数
double ARG_GetOption_Number(const char* name) {
	ARG_Option* option = ARG_GetOption(name);
	if (!option) {
		return false;
	}

	return *(double*)&option->value;
}


// オプションから配列を取得する関数
const char** ARG_GetOption_Array(const char* name, int* retLength) {
	ARG_Option* option = ARG_GetOption(name);
	if (option == NULL) {
		return NULL;
	}

	*retLength = option->arrayLength;
	return (const char**)option->value;
}



/* CreateOption_<type> */

// オプションを作成する関数 (高度な関数、ARG_CreateOption_<type>() を使用することを推奨)
ARG_Option* ARG_CreateOption(const char* name, const char* shortname,
                        ARG_OptionType type, void* defaultValue, const char* description) {
	ARG_Option* option = (ARG_Option*)malloc(sizeof(ARG_Option));
	option->name = name;
	option->shortname = shortname ? shortname : "";
	option->description = description ? description : "";

	option->type = type;
	option->value = defaultValue;
    option->arrayLength = 0;
	
	option->front = NULL;
	option->back = ARG_privateLastOption;
	
	if (ARG_privateLastOption != NULL) {
		ARG_privateLastOption->front = option;
	}

	if (ARG_privateFirstOption == NULL) {
		ARG_privateFirstOption = option;
	}
	ARG_privateLastOption = option;

	return option;
}

// オプションを作成する関数 (文字列)
ARG_Option* ARG_CreateOption_String(const char* name, const char* shortname,
				const char* defaultValue, const char* description) {

	return ARG_CreateOption(name, shortname, ARG_OPTION_STRING, (void*)defaultValue, description);
}

// オプションを作成する関数 (真偽値)
ARG_Option* ARG_CreateOption_Boolean(const char* name, const char* shortname,
				const char* description) {

	return ARG_CreateOption(name, shortname, ARG_OPTION_BOOLEAN, 0, description);
}

// オプションを作成する関数 (数値)
ARG_Option* ARG_CreateOption_Number(const char* name, const char* shortname,
				double defaultValue, const char* description) {

	return ARG_CreateOption(name, shortname, ARG_OPTION_NUMBER, 0, description);
}

// オプションを作成する関数 (配列)
ARG_Option* ARG_CreateOption_Array(const char* name, const char* shortname,
				const char* description) {

	return ARG_CreateOption(name, shortname, ARG_OPTION_ARRAY, malloc(5 * sizeof(const char*)), description);
}




/* AddIndex */

/*
間接オプションを追加する関数
ターミナル例: program option1 option2 arrayOption ...
配列型オプション arrayOption を指定することで、それ以降の引数は配列に追加する

@param name オプションのフルーネーム
@return エラーコード
*/
int ARG_AddIndex(const char* name) {
    /* Get Option */
	ARG_Option* option = ARG_GetOption(name);
	if (option == NULL) {
		sprintf(ARG_privateErrorMsg, "Option '%s' not exists", name);
		return ARG_STATUS_OPTION_NOT_EXIST;
	}

    /* Add Index */
    // 配列拡張
	if (ARG_privateIndexesLength % 5 == 0) {
		ARG_Option** newBuffer = (ARG_Option**)realloc(ARG_privateIndexes, (ARG_privateIndexesLength + 5) * sizeof(ARG_Option*));

		if (newBuffer == NULL) {
			strcpy(ARG_privateErrorMsg, "Failed to allocate memory");
			return ARG_STATUS_FAILED_TO_ALLOCATE_MEMORY;
		}

		ARG_privateIndexes = newBuffer;
	}

    // 値を追加
	ARG_privateIndexes[ARG_privateIndexesLength++] = option;
	return ARG_STATUS_NONE;
}





/*
間接オプションを追加する関数
ターミナル例: program option1 option2 arrayOption ...
配列型オプション arrayOption を指定することで、それ以降の引数は配列に追加する

@param ... オプションのフルーネーム (NULL で終了)
@return エラーコード
*/
int ARG_AddIndexs(const char* name, ...) {
	va_list ap;
	va_start(ap, name);
	
	do {
		int ret = ARG_AddIndex(name);

		if (ret) {
		#ifndef ARG_NODEBUG
			fprintf(stderr, "DEBUG: %s: Failed to add index (%s)\n", __func__, ARG_GetError());
		#endif
			va_end(ap);
			return ret;
		}
	}
    while ((name = va_arg(ap, const char*)));

	va_end(ap);
	return ARG_STATUS_NONE;
}





/* String */

const char* ARG_StringGetOptionValue(const char* name) {
    ARG_Option* option = ARG_GetOption(name);
    if (option == NULL) {
        return "<ARG_STATUS_OPTION_NOT_EXIST>";
    }
    
    return ARG_StringOptionValue(option);
}






/*
パラメーターを読み取る

@param reader オプション設定済みの ARG_Reader
@param argc コマンドライン引数の数 (main() 第一パラメーター)
@param argv コマンドライン引数の配列 (main() 第二パラメーター)
@return エラーコード、ARG_privateStatus にも保存される
*/
int ARG_Read(int argc, char** argv) {
	ARG_Option* option;

	while (ARG_privateNowArg < argc) {
		char* word = argv[ARG_privateNowArg++];

		if (memcmp(word, "--", 2) == 0) {
			// 内蔵オプション
			if (strcmp(word, "--help") == 0) {
				return ARG_privateStatus = ARG_STATUS_HELP;
			}
			option = ARG_GetOption(word + 2);

			// ERROR: 存在しないオプション
			if (option == NULL) {
				// don't set error message since already set
				return ARG_privateStatus = ARG_STATUS_OPTION_NOT_EXIST;
			}

			// Boolean 特別処理法
			if (option->type == ARG_OPTION_BOOLEAN) {
				option->value = (void*)true;
				goto NextLoop;
			}
			// Boolean 以外の処理法
			if (ARG_privateNowArg < argc) {
				ARG_Option_SetValueByString(option, argv[ARG_privateNowArg++]);
				goto NextLoop;
			}

			// ERROR: 不完全な入力
			sprintf(ARG_privateErrorMsg, "'%s' は不完全な入力", word);
			return ARG_privateStatus = ARG_STATUS_INPUT_NOT_COMPLETED;
		}

		if (memcmp(word, "-", 1) == 0) {
			ARG_ForeachOptions(option) {
				if (strcmp(option->shortname, word + 1) != 0) {
					continue;
				}

				// Boolean 特別処理法
				if (option->type == ARG_OPTION_BOOLEAN) {
					option->value = (void*)true;
					goto NextLoop;
				}
				// Boolean 以外の処理法
				if (ARG_privateNowArg < argc) {
					ARG_Option_SetValueByString(option, argv[ARG_privateNowArg++]);
					goto NextLoop;
				}

				// ERROR: 不完全な入力
				sprintf(ARG_privateErrorMsg, "'%s' は不完全な入力", word);
				return ARG_privateStatus = ARG_STATUS_INPUT_NOT_COMPLETED;
			}

			// ERROR: 設定していないオプション
			sprintf(ARG_privateErrorMsg, "'%s' は見つかりません", word);
			return ARG_privateStatus = ARG_STATUS_OPTION_NOT_EXIST;
		}

		/* インデックスオプション解析 */
		if (ARG_privateNowIndex < ARG_privateIndexesLength) {
			ARG_Option* option = ARG_privateIndexes[ARG_privateNowIndex];
			
            if (option->type == ARG_OPTION_ARRAY) {
				ARG_Option_ArrayAdd(option, word);
				goto NextLoop;
			}

			if (option->type == ARG_OPTION_BOOLEAN) {
				option->value = (void*)true;
			} else {
				ARG_Option_SetValueByString(option, word);
			}

			ARG_privateNowIndex++;
			goto NextLoop;
		}

		// ERROR: インデックス引数が多すぎた
		strcpy(ARG_privateErrorMsg, "インデックス引数が多すぎた");
		return ARG_privateStatus = ARG_STATUS_INDEX_OVERED;

	NextLoop:
		continue;
	}

	return ARG_privateStatus = ARG_STATUS_NONE;
}




// ARG_Reader のすべてのオプションと説明を表示する
// @param reader 表示する ARG_Reader
// @return エラーコード
int ARG_PrintHelp() {
	puts(ARG_privateDescription);
	putchar('\n');

	puts("Options:");


	int longestNameLength = 0;
	int longestShortNameLength = 0;

	ARG_ForeachOptions(option) {
		int length = strlen(option->name);
		longestNameLength = MAX(longestNameLength, length);

		length = strlen(option->shortname);
		longestShortNameLength = MAX(longestShortNameLength, length);
	}

	longestNameLength += 2;
	longestShortNameLength += 1;

	char spaces1[50];
	char spaces2[50];

	ARG_ForeachOptions(option) {
		size_t nameLength = strlen(option->name);
		size_t shortNameLength = strlen(option->shortname);

		memset(spaces1, ' ', longestNameLength - nameLength);
		spaces1[longestNameLength - nameLength] = 0;

		memset(spaces2, ' ', longestShortNameLength - shortNameLength);
		spaces2[longestShortNameLength - shortNameLength] = 0;

		printf("--%s%s%c%s%s: %s\n",
				option->name,
				spaces1,
				shortNameLength ? '-' : ' ',
				option->shortname,
				spaces2,
				option->description
		);
	}
	putchar('\n');

	return ARG_STATUS_NONE;
}





// ARG_Read が失敗し、プログラムを終了する際に必要なことを行う。エラーの表示と、同時に ARG_Reader を解放する
// @return ERROR_NONE
int ARG_HandleError() {
	switch (ARG_privateStatus) {
		case ARG_STATUS_HELP:
			ARG_PrintHelp();
			ARG_Quit();
            return ARG_STATUS_NONE;
		default:
			fprintf(stderr, "ERROR: Failed to read paramter! (%s)\n", ARG_GetError());
			ARG_Quit();
			return ARG_privateStatus;
	}

	
}





#endif /* ARG_READER_H */