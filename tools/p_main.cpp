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
	"<div class=\"" <<  clas  << "_" <<  i  << "\">call f() in ++ <div class=\"myclass\">" <<  i  << "</div></div>"
    );
}

std::string s()
{
    HTML
    (
	"<div>f(): <div class=\"myclass\">"; f(); ECHO "</div></div>"
    );
}

int main()
{
    std::string title("title");
    int i = 25;
    int a = 123;
    HTML
    (
	"<html><head><title>" << title << "</title></head><body>";  s();  ECHO "Test 123 <div class=\"myclass\">" <<  i  << "</div>";  f();  ECHO "<a href='#one_" <<  i  << "_modal' id='one_" <<  i  << "'>two_" <<  i  << "</a><div class='overlay' id='one_" <<  i  << "_overlay'></div><div class='modal' id='one_" <<  i  << "_modal'>    <a href='one_" <<  i  << "' id='one_" <<  i  << "_close'>X</a>    <a href='" <<  i  << "'></div><script type='text/javascript'>initModal('one_" <<  i  << "')</script></body></html>"
    );
}