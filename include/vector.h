/* vector.h
**
**	Defines a Vector class as a special case of the Matrix class.
** Specific operations are redefined such as the [] operator, dot
** product. New operations are Cross product.
**	If you want [] to check bounds define CHECK_BOUNDS.
**
** includes:
**	matrix.h	- base class
**	math.h		- for sqrt functions
*/

#ifndef __VECTOR_H
#define __VECTOR_H

#include <matrix.h>
#include <math.h>

typedef MatrixSizeType		VectorSizeType;

template <class VectorEl>
class Vector : public Matrix<VectorEl> {
public:
    inline                    		Vector (VectorSizeType s = 0);
   
    inline virtual Vector<VectorEl>&	operator= (const Vector<VectorEl>& toBe);
    inline virtual Vector<VectorEl>&	operator= (const Matrix<VectorEl>& toBe);
    inline virtual Vector<VectorEl>&	operator- (const Vector<VectorEl>& toSub);
    inline virtual VectorEl&		operator[] (VectorSizeType index);
    inline virtual VectorEl		operator* (const Vector<VectorEl>& toDot);

    inline virtual Vector<VectorEl>&	Cross (const Vector<VectorEl>& toCross);
    inline virtual VectorEl		Length (void) const;
    inline virtual void			Resize (VectorSizeType s);
};

////////////////////////////////////////////////////////////////////////
////////////////////////// Functions ///////////////////////////////////

template <class VectorEl>
inline Vector<VectorEl> :: Vector 
(VectorSizeType s) : Matrix<VectorEl> (s, 1)
{	
}

template <class VectorEl>
inline Vector<VectorEl>& Vector<VectorEl> :: operator=
(const Vector<VectorEl>& toBe)
{		  
    Resize (toBe.size);
    memcpy (data, toBe.data, size * sizeof(VectorEl));
    return (*this);
}

template <class VectorEl>
inline Vector<VectorEl>& Vector<VectorEl> :: operator=
(const Matrix<VectorEl>& toBe)
{
    if (toBe.Height() != 1 && toBe.Width() != 1) {
       cout << "Cannot equate matrix to vector!";
       exit(1);
    }
    Resize (toBe.Size());
    memcpy (data, (VectorEl*)toBe, size * sizeof(VectorEl));
    return (*this);
}

template <class VectorEl>
inline Vector<VectorEl>& Vector<VectorEl> :: operator-
(const Vector<VectorEl>& toSub)
{		  	      
Vector<VectorEl> * diff;
register int n;

    if (toSub.size != size) {
       cout << "Sub: vectors of different size!\n";
       exit(1);
    }
    
    diff = new Vector<VectorEl> (size);
    	
    for (n = 0; n < size; ++ n)
       diff->data[n] = data[n] - toSub.data[n];	 
    
    return (*diff);
}

template <class VectorEl>
inline VectorEl& Vector<VectorEl> :: operator[] 
(VectorSizeType index)
{      
#ifdef CHECK_BOUNDS
    if (index < size)
#endif
       return (data [index]);
       
    cout << "Vector: Index out of range.\n";
    exit (1);
}

template <class VectorEl>
inline VectorEl Vector<VectorEl> :: operator* 
(const Vector<VectorEl>& toDot)
{
MatrixSizeType el;
VectorEl product = 0;
     
    if (size != toDot.size)
    {
       cout << "Addition: vectors must be of the same dimention.\n";
       exit(1);
    }   
 	    
    for (el = 0; el < size; ++ el)
       product += data[el] * toDot.data[el];
       
    return (product);
}

template <class VectorEl>
inline Vector<VectorEl>& Vector<VectorEl> :: Cross 
(const Vector<VectorEl>& toCross)
{
MatrixSizeType el, i1, i2, tmp;
int sign = 1;
Vector<VectorEl> *product;
     
    if (size != toCross.size) {
       cout << "Cross: vectors must be of the same dimention.\n";
       exit(1);
    }   		  
    else if (size < 3) {
       cout << "Cross: vector dimention must be >= 3.\n";
       exit(1);
    }
    	    
    product = new Vector<VectorEl> (size);
    
    for (el = 0; el < size; ++ el) {				 
       i1 = (el + 1) % size;		// Compute indeces beforehand
       i2 = (el + 2) % size;	      
       tmp = i1;
       i1 = (i1 < i2 ? i1 : i2);	// i1 should be smaller than i2
       if (i1 != tmp)
          i2 = tmp;
       		      
       product->data [el] = sign * ((data [i1] * toCross.data [i2]) -
			             (data [i2] * toCross.data [i1]));
	
       sign *= (-1);
    }
       
    return (*product);
}

template <class VectorEl>
inline VectorEl Vector<VectorEl> :: Length (void) const
{	 
VectorEl SumOfSquares = 0;
MatrixSizeType el;
   
    for (el = 0; el < size; ++ el)
       SumOfSquares += data[el] * data[el];
       
    return ((VectorEl) sqrt (SumOfSquares));
}

template <class VectorEl>
inline void Vector<VectorEl> :: Resize
(VectorSizeType s)
{	 
    Matrix<VectorEl> :: Resize (s, 1);
}

#endif

