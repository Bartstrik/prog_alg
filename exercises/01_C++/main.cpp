// C++ is one-pass compiler. If we want to use a function/procedure/method, then it has to be
// defined or at least declared before.
// Here are 3 function declartions. The implementations of these functions are in different
// compilation units: findmax.cpp, summation.cpp, rangequery.cpp
// It's the task of the C++ linker to put all parts of a program together.

//////////////////////////////////////////////////////////////////////////////////////////////
// this function is implemented in summation.cpp
void summationTests();

//////////////////////////////////////////////////////////////////////////////////////////////
// this function is implemented in findmax.cpp
void findMaximumTests();

//////////////////////////////////////////////////////////////////////////////////////////////
// this function is implemented in rangequery.cpp
void rangeQueryTests();

int main() {
	summationTests();
	findMaximumTests();
	rangeQueryTests();
}
