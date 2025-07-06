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