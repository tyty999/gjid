/* matrix.h
**
**	Defines a matrix template. Abstract and simple.
** If you want [] to check bounds, define CHECK_BOUNDS.
**
** includes:
**	mdefs.h		- for WORD
**	set.h		- base class
**	streamable.h	- base class
*/

#ifndef __MATRIX_H
#define __MATRIX_H

#include <mdefs.h>	 
#include <set.h>
#include <streamable.h>
#include <stdlib.h>
			  
typedef SetSizeType			MatrixSizeType;

template <class MatrixEl>
class Matrix : public Set<MatrixEl>, public Streamable {
private:
    MatrixSizeType			width;
    MatrixSizeType			height;
    
public:
    inline				Matrix (WORD h = 0, WORD w = 0);
    inline				Matrix (const Matrix<MatrixEl>& AMatrix);
    inline				Matrix (const MatrixEl * AMatrix, MatrixSizeType AHeight, MatrixSizeType AWidth);
			
    inline BOOL				operator== (const Matrix<MatrixEl>& toCompare) const;
    inline virtual MatrixEl *		operator[] (MatrixSizeType index) const;
    inline Matrix<MatrixEl>&		operator= (const Matrix<MatrixEl>& toBe);
    inline Matrix<MatrixEl>&		operator+ (const Matrix<MatrixEl>& toAdd) const;
    inline Matrix<MatrixEl>&		operator- (const Matrix<MatrixEl>& toSub) const;
    inline Matrix<MatrixEl>&		operator* (MatrixEl scalar) const;
    inline virtual Matrix<MatrixEl>&	operator* (const Matrix<MatrixEl>& toMul) const;
    
    inline void				Transpose (void);
    inline void				RowReduce (void);
    inline MatrixEl			Determinant (void);
    inline virtual void			Read (ifstream& is);
    inline virtual void			Write (ofstream& os) const;
    inline virtual void			Read (istream& is);
    inline virtual void			Write (ostream& os) const;
    inline virtual void			Resize (WORD h, WORD w);

    inline MatrixSizeType		Width (void) const
						{ return (width);};
    inline MatrixSizeType		Height (void) const
    						{ return (height);};
};

///////////////////////////////////////////////////////////////////////////
///////////////////// Functions ///////////////////////////////////////////

template <class MatrixEl>
inline Matrix<MatrixEl> :: Matrix 
(WORD h, WORD w) 
 : Set<MatrixEl> (h * w)
{   
    width = w;
    height = h; 
}

template <class MatrixEl>
inline Matrix<MatrixEl> :: Matrix
(const Matrix<MatrixEl>& AMatrix) : Set<MatrixEl> (AMatrix)
{   
    width = AMatrix.width;
    height = AMatrix.height;	   
};

template <class MatrixEl>
inline Matrix<MatrixEl> :: Matrix 
(const MatrixEl * AMatrix, MatrixSizeType AHeight, MatrixSizeType AWidth) 
 : Set<MatrixEl> (AMatrix, AHeight * AWidth)
{ 	   
    width = AWidth;
    height = AHeight;	   
};
 
template <class MatrixEl>
inline BOOL Matrix<MatrixEl> :: operator== 
(const Matrix<MatrixEl>& toCompare) const
{    
    if (toCompare.height != height || toCompare.width != width)
       return (FALSE);
	    
    for (MatrixSizeType i = 0; i < this->size; ++ i)
       if (this->data[i] != toCompare.data[i])
          return (FALSE);

    return (TRUE); 
}	       

template <class MatrixEl>
inline MatrixEl * Matrix<MatrixEl> :: operator[]
(WORD index) const
{
#ifdef CHECK_BOUNDS
    if (index < height)
#endif
       return (&this->data [index * width]);
       
    cout << "Matrix: index out of range.\n";
    exit (1);
}

template <class MatrixEl>
inline Matrix<MatrixEl>& Matrix<MatrixEl> :: operator= 
(const Matrix<MatrixEl>& toBe)
{	
MatrixSizeType i;
			    
    Resize (toBe.height, toBe.width);
    for (i = 0; i < this->size; ++ i)
       this->data[i] = toBe.data[i];

    return (*this);
}

template <class MatrixEl>
inline Matrix<MatrixEl>& Matrix<MatrixEl> :: operator+ 
(const Matrix<MatrixEl>& toAdd) const
{
MatrixSizeType row, col;				       
Matrix<MatrixEl> * sum;

    if (toAdd.height != height || toAdd.width != width) {
       cout << "Add: matrices must have same dimentions.\n";
       exit (1);
    }
		
    sum = new Matrix<MatrixEl> (height, width);
    
    for (row = 0; row < height; ++ row)
       for (col = 0; col < width; ++ col)
          sum->data [row * sum->width + col] = this->data [row * width + col] + toAdd.data [row * toAdd.width + col];
       
    return (*sum);
}

template <class MatrixEl>
inline Matrix<MatrixEl>& Matrix<MatrixEl> :: operator- 
(const Matrix<MatrixEl>& toSub) const
{
MatrixSizeType row, col;				       
Matrix<MatrixEl> * diff;

    if (toSub.height != height || toSub.width != width) {
       cout << "Sub: matrices must have same dimentions.\n";
       exit (1);
    }
		
    diff = new Matrix<MatrixEl> (height, width);
    
    for (row = 0; row < height; ++ row)
       for (col = 0; col < width; ++ col)
          diff->data [row * diff->width + col] = this->data [row * width + col] - toSub.data [row * toSub.width + col];
       
    return (*diff);
}

template <class MatrixEl>
inline Matrix<MatrixEl>& Matrix<MatrixEl> :: operator* 
(MatrixEl scalar) const
{
MatrixSizeType row, col;
Matrix<MatrixEl> * product;

    product = new Matrix<MatrixEl> (height, width);
    
    for (row = 0; row < height; ++ row)
       for (col = 0; col < width; ++ col)
          product->data [row * product->width + col] = this->data [row * width + col] * scalar;
       
    return (*product);
}

template <class MatrixEl>
inline Matrix<MatrixEl>& Matrix<MatrixEl> :: operator* 
(const Matrix<MatrixEl>& toMul) const
{
MatrixSizeType row, col, idx;
Matrix<MatrixEl> * product;
MatrixSizeType ProductOffset, MyOffset, HisOffset;

    if (width != toMul.height) {
       cout << "Mul: width != height.\n";
       exit (1);
    }
		
    product = new Matrix<MatrixEl> (height, toMul.width);
    
    ProductOffset = 0;
    MyOffset = 0;
    
    for (row = 0; row < height; ++ row) {
       for (col = 0; col < toMul.width; ++ col) {
          HisOffset = col;
          product->data [ProductOffset] = 0;
    
          for (idx = 0; idx < width; ++ idx) {
             product->data [ProductOffset] += this->data [MyOffset + idx] * 
                                              toMul.data [HisOffset];
	     HisOffset += toMul.width;
	  }
	  ++ ProductOffset;   
       }
       MyOffset += width;
    }
       
    return (*product);
}
    
template <class MatrixEl>
inline void Matrix<MatrixEl> :: Transpose (void)
{		    
MatrixSizeType temp, x, y;
MatrixEl * NewValue;

    for (y = 0; y < height; ++ y)
       for (x = 0; x < width; ++ x)
          NewValue [x * height + y] = this->data [y * width + x];
    for (x = 0; x < width * height; ++ x)
       this->data [x] = NewValue [x];

    temp = width;
    width = height;
    height = width;	 
}

template <class MatrixEl>
inline void Matrix<MatrixEl> :: RowReduce (void)
{
    cout << "Row reduction is not yet implemented!\n";
    exit (1);
} 
 
template <class MatrixEl>
inline MatrixEl Matrix<MatrixEl> :: Determinant (void)
{
MatrixEl det = 1;
MatrixSizeType x;

    if (width != height) {
       cout << "Determinant: can only work with square matrices\n";
       exit (1);
    }
    RowReduce();
    for (x = 0; x < width; ++ x)
       det *= this->data [x * width + x];

    return (det);
} 
    	     				
template <class MatrixEl>
inline void Matrix<MatrixEl> :: Read
(ifstream& is)
{	    
MatrixSizeType row, col, SavedWidth, SavedHeight;
  						
    is.read ((char*) &SavedHeight, sizeof (MatrixSizeType));
    is.read ((char*) &SavedWidth, sizeof (MatrixSizeType));
    Resize (SavedHeight, SavedWidth);
    for (row = 0; row < height; ++ row)
       for (col = 0; col < width; ++ col)
          is.read ((char*) &this->data [row * width + col], sizeof(MatrixEl));
} 

template <class MatrixEl>
inline void Matrix<MatrixEl> :: Write
(ofstream& os) const
{	    
MatrixSizeType row, col;
  				       
    os.write ((const char*) &height, sizeof (MatrixSizeType));
    os.write ((const char*) &width, sizeof (MatrixSizeType));
    for (row = 0; row < height; ++ row)
       for (col = 0; col < width; ++ col)
          os.write ((const char*) &this->data [row * width + col], sizeof(MatrixEl));
} 
       
template <class MatrixEl>
inline void Matrix<MatrixEl> :: Read
(istream& is)
{	    
MatrixSizeType row, col, SavedWidth, SavedHeight;
  						
    is >> SavedHeight;
    is >> SavedWidth;			
    Resize (SavedHeight, SavedWidth);
    for (row = 0; row < height; ++ row) {
       for (col = 0; col < width; ++ col)
          is >> this->data [row * width + col];
    }
} 

template <class MatrixEl>
inline void Matrix<MatrixEl> :: Write
(ostream& os) const
{	    
MatrixSizeType row, col;
  		 
    os << height << " " << width << "\n";
    for (row = 0; row < height; ++ row) {
       for (col = 0; col < width; ++ col)
          os << this->data [row * width + col] << " ";
       os << "\n";
    }
} 
       
template <class MatrixEl>
inline void Matrix<MatrixEl> :: Resize
(WORD h, WORD w)
{		
MatrixEl * NewData;
MatrixSizeType row, col;
    
    NewData = new MatrixEl [h * w];

    for (row = 0; row < height; ++ row)
       for (col = 0; col < width; ++ col)
          NewData [row * w + col] = this->data [row * width + col];
    height = h;
    width = w;
    this->size = height * width;
    
    delete [] this->data;
    this->data = NewData;
}

#endif

