#include "ArgReader2.h"

int main(int argc, char** argv) {
	// Init ARG_Reader
	ARG_Init("ArgReader Sample");

    ARG_CreateOption_String("string-option", "s", "No input", "String option");
    ARG_CreateOption_Boolean("boolean-option", "b", "Boolean Option");
    ARG_CreateOption_Number("number-option", "n", 0, "Number Option");
	ARG_CreateOption_Array("array-option", "a", "Array Option");

	ARG_AddIndexs("string-option", "boolean-option", "number-option", "array-option", NULL);

	// Read Parameters
	if (ARG_Read(argc, argv)) {
		return ARG_HandleError();
	}

	// Get Options
	const char* stringOption = ARG_GetOption_String("string-option");
	bool booleanOption = ARG_GetOption_Boolean("boolean-option");
	double numberOption = ARG_GetOption_Number("number-option");
	
	int arrayLength;
	const char** arrayOption = ARG_GetOption_Array("array-option", &arrayLength);

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