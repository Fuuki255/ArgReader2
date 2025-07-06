# ArgReader2

ArgReader2 は、C言語で記述されたシンプルかつ柔軟なコマンドライン引数パーサー用ヘッダファイルです。オプションの定義、値の取得、エラー処理、ヘルプ表示など、コマンドラインツールの開発を効率化します。

| バージョン	| 2.0.0		|
|:--------------|:--------------|
| プログラマー	| ふうき		|

## 特徴

- **読みやすいデザイン**: コードの密度が高くて読みやすい、冗長な文字がない
- **多様な型サポート**: 文字列 (String)、真偽値 (Boolean)、数値 (Number)、配列型オプション (Array)
- **エラー処理・ヘルプ表示**: 標準的なエラー処理と自動的にヘルプ出力
- **インデックス引数対応**: オプションを指定しない入力に対して、設定されたインデックス順番にデータをゲットする

## サンプル

```c
#include "ArgReader2.h"

int main(int argc, char** argv) {
	// Init ARG_Reader
	ARG_Init("ArgReader Sample");

    ARG_CreateOption_String("string", "s", "No input", "String option"); // fullname, shortname, defaultvalue, description
    ARG_CreateOption_Boolean("boolean", "b", "Boolean Option");          // fullname, shortname, description
    ARG_CreateOption_Number("number", "n", 0, "Number Option");          // fullname, shortname, defaultvalue, description
	ARG_CreateOption_Array("array", "a", "Array Option");                // fullname, shortname, description

    // set index option order string, boolean, number and array
    // the NULL in last to tell ARG_AddIndexs there are no more value
	ARG_AddIndexs("string", "boolean", "number", "array", NULL);

	// Read Parameters
	if (ARG_Read(argc, argv)) {
		return ARG_HandleError();
	}

	// Get Options
	const char* stringOption = ARG_GetOption_String("string");
	bool booleanOption = ARG_GetOption_Boolean("boolean");
	double numberOption = ARG_GetOption_Number("number");
	
	int arrayLength;
	const char** arrayOption = ARG_GetOption_Array("array", &arrayLength);

	// Print Options
	printf("String Option: %s\n", stringOption);
	printf("Boolean Option: %s\n", booleanOption ? "true" : "false");
	printf("Number Option: %lg\n", numberOption);
	printf("Array Option:\n");

	for (int i = 0; i < arrayLength; i++) {
		printf("  %s\n", arrayOption[i]);
	}

	// Destroy ARG_Reader
	ARG_Quit();
	return 0;
}
```

## 使い方

### 1. インクルード

```c
#include "ArgReader2.h"
```

### 2. 初期化

```c
ARG_Init("コマンドの説明文");
```

### 3. オプションの定義

```c
ARG_CreateOption_String("input", "i", "default.txt", "入力ファイル");
ARG_CreateOption_Boolean("verbose", "v", "詳細表示");
ARG_CreateOption_Number("threshold", "t", 0.5, "しきい値");
ARG_CreateOption_Array("files", NULL, "ファイルリスト");
```

### 4. インデックス引数の設定（必要に応じて）

コマンドラインのオプションなしの入力に対して、このインデックス順番に応じて読み取ることができます

配列型オプションは後のすべてのオプションなし入力をゲットする

```c
ARG_AddIndexs("input", "threshold", NULL);	// 最初のデータは input で, 後は threshold, 最後は NULL でもう設定するオプションがないと示す
```

### 5. 引数のパース

```c
if (ARG_Read(argc, argv)) {	// 成功だと 0 を返す、失敗だとエラーコードが戻れる、ARG_privateStatus にも保存される
    return ARG_HandleError();
}
```

### 6. 値の取得

```c
const char* input = ARG_GetOption_String("input");

bool verbose = ARG_GetOption_Boolean("verbose");

double threshold = ARG_GetOption_Number("threshold");

int filesLen;
const char** files = ARG_GetOption_Array("files", &filesLen);
```

### 7. 終了処理

オプションなどでメモリが割り当てたため、`ARG_Quit()` でそれらを解放する

```c
ARG_Quit();
```

## エラー処理

- `ARG_HandleError()` を使うことで、エラー表示とリソース解放を自動で行います。

## ヘルプ表示

- `--help` オプションで自動的にヘルプが表示されます。

## 予定アップグレード (3.0.0)

1. **命名規則の変更**: 実際に `ArgReader2.h` はもう数年前の作品なので、今の僕が使うスタイルに変える 3.0.0 かいはつよていをする
2. **追加機能**: Dict オプションの追加、

## ライセンス

MIT License
