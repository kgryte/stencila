#include <stencila/tables/query.hpp>
using namespace Stencila::Tables;

#include "py-extension.hpp"

//! @brief Convert a Python object into a Query element
//!
//! This function is a "raw_function" which recieves Python objects.
//! Its job is to convert those Python objects into Stencila Query elements
//! For example, a Python integer is converted into a Constant<int>
Element* Query_wrap(const object& o){
    //Boost.python's extract<float>(o).check() returns true even if o is an integer.
    //So have to use the Python PyXXX_Check functions to determine type
    const PyObject* p = o.ptr();
    if(PyBool_Check(p)) return new Constant<bool>(extract<bool>(o));
    if(PyInt_Check(p)) return new Constant<int>(extract<int>(o));
    if(PyFloat_Check(p)) return new Constant<float>(extract<float>(o));
    if(PyString_Check(p)) return new Constant<std::string>(extract<std::string>(o));
    
    //If the object is a Query element then just return it
    extract<Element*> e(o);
    if(e.check()) return e();
    
    //Any othe object type is converted to a string
    //! @todo Obtain the __repr__ or __str__ of the object
    return new Constant<std::string>(boost::lexical_cast<std::string>(p));
}

#define UNOP(name,type) \
    type Query_##name(Element& self){ \
        return type(&self); \
    }

UNOP(__neg__,Negative)
UNOP(__pos__,Positive)

#undef UNOP

#define BINOP(name,type) \
    type Query_##name(Element& self, object& other){ \
        return type(&self,Query_wrap(other)); \
    }

BINOP(__eq__,Equal)
BINOP(__ne__,NotEqual)
BINOP(__lt__,LessThan)
BINOP(__le__,LessEqual)
BINOP(__gt__,GreaterThan)
BINOP(__ge__,GreaterEqual)

BINOP(__add__,Add)
BINOP(__sub__,Subtract)
BINOP(__mul__,Multiply)
BINOP(__div__,Divide)

BINOP(__and__,And)
BINOP(__or__,Or)

#undef BINOP

void Query_define(void){
    
    // Because class Element has two "sql" methods use a typedef to remove ambiguity
    typedef std::string (Element::* Element_sql_type)(unsigned short phase) const;
    
    class_<Element>("Element")

        .def("dql",&Element::dql)
        .def("sql",Element_sql_type(&Element::sql))
        
        #define OP(name) .def(#name,Query_##name)
        
        OP(__pos__)
        OP(__neg__)

        OP(__eq__)
        OP(__ne__)
        OP(__lt__)
        OP(__le__)
        OP(__gt__)
        OP(__ge__)

        OP(__add__)
        OP(__sub__)
        OP(__mul__)
        OP(__div__)
        
        OP(__and__)
        OP(__or__)
        
        #undef OP
    ;
    
    class_<Column,bases<Element>>("Column",init<std::string>());
    
    //Define Python classes for each unary operator
    #define UNOP(name) class_<name,bases<Element>>(#name);

    UNOP(Negative)
    UNOP(Positive)
    
    #undef UNOP
    
    //Define Python classes for each binary operator
    #define BINOP(name) class_<name,bases<Element>>(#name);

    BINOP(Multiply)
    BINOP(Divide)
    BINOP(Add)
    BINOP(Subtract)

    BINOP(Equal)
    BINOP(NotEqual)
    BINOP(LessThan)
    BINOP(LessEqual)
    BINOP(GreaterThan)
    BINOP(GreaterEqual)

    BINOP(And)
    BINOP(Or)
    
    #undef BINOP
}