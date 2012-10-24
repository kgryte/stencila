/*
Copyright (c) 2012, Stencila Ltd
Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is 
hereby granted, provided that the above copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD 
TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. 
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR 
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <iostream>

#ifdef STENCILA_TEST_SINGLE
    #define BOOST_TEST_MODULE tests
#endif
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <stencila/stencil.hpp>
#include <stencila/print.hpp>
#include <stencila/reflect.hpp>

using namespace Stencila;
using namespace Stencila::Reflect;


class DispatchDict : public Dispatch<void>{
private:
    typedef std::map<std::string,Proxy> Map;
    
public:	
    
	Proxy create(void){
        return Proxy(0,0);
    }
    
	std::string type(void){
		return "";
	};
    
	std::vector<std::string> keys(void){
		return {};
	};
    
	bool has(const std::string& name){
		return false;
	};
    
	Proxy get(void* object, const std::string& name){
        Map& map = *static_cast<Map*>(object);
        Map::iterator item = map.find(name);
        if(item!=map.end()) return item->second;
        else STENCILA_THROW(Exception,"object does not have key:"+name)
    }
    
    std::string repr(void* object){
		return "";
	};
    
} DispatcherDict;

Proxy Create(std::initializer_list<std::pair<std::string,Proxy>> map){
    return Proxy(
        new std::map<std::string,Proxy>(map.begin(),map.end()),
        &DispatcherDict
    );
}

class Person : public Reflector<Person> {
public:
	std::string name;
	int age;
    
    Person(std::string name_="", int age_=0):
        name(name_),
        age(age_){
    }
	
	std::string greet(void) const {
		return print()<<"Hello, my name is "<<name<<", I am "<<age<<" years old.";
	}

	template<class Reflection>
	void reflect(Reflection& r){
		r.data("name",&name,"Full name")
		 .data("age",&age,"Age in years")
		 .method("greet",&Person::greet,"Tells you about the person");
	}
};
Register<Person> Person_;

struct stencil1Fixture {
    
    Stencil stencil1;
    Stencil stencil2;
    
    stencil1Fixture(void){
        
        stencil1.load(
            "<span data-type='this'/>"
            "<span data-repr='this'/>"
        );
        
        stencil2.load(
            "<span data-type='this'/>"
            "<span data-repr='this'/>"
            "<div data-each__='this'>"
            "   <div data-include='stencil1'/>"
            "</div>"
        );
    }    
};

BOOST_FIXTURE_TEST_SUITE(stencil1,stencil1Fixture)

BOOST_AUTO_TEST_CASE(render){
    Person john("John",29);
    
    Proxy context = Create({
        std::make_pair("this",john.proxy())
    });
    //std::string report = stencil2.render(context).dump();
    //BOOST_CHECK_EQUAL(report,"");
}

BOOST_AUTO_TEST_SUITE_END()