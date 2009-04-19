class Element;

#ifndef _ELEMENT_H
#define	_ELEMENT_H

#define ELEM_VALUE      0
#define ELEM_OPERATOR   1

class Element {
public:
    virtual ~Element();

    const int eltype;
protected:
    Element(int eltype);

};

#endif	/* _ELEMENT_H */

