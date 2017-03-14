#include <string>
#include <iostream>

#define ECHO std::cout <<

#ifdef PREPROCESS
    #define HTML(_s) ECHO _s;
#else
    #define HTML(_s) ECHO #_s;
#endif

int f()
{
    std::string clas("my");
    int i = 25;
    HTML
    (
	<!++
	<div class="<v++ clas ++v>_<v++ i ++v>">
	    call f() in ++ <div class="myclass"><v++ i ++v></div>
	</div>
	++!>
    );
}

std::string s()
{
    HTML
    (
	<!++
	<div>
	    f(): <div class="myclass">
	    <f++ 
	    f(); 
	    ++f></div>
	</div>
	++!>
    );
}

int main()
{
    std::string title("title");
    int i = 25;
    int a = 123;
    HTML
    (
	<!++
	<html>
	<head>
	    <title>
	    <v++ 
		title 
	    ++v>
	    </title>
	</head>
	<body>
	<f++ s(); ++f>
	Test 123 <div class="myclass"><v++ i ++v></div>
	<f++ f(); ++f>
	<mw++ [one_<v++ i ++v>] [two_<v++ i ++v>]
	    <a href='<v++ i ++v>'>
	++mw>
	</body>
	</html>
	++!>
    );
}