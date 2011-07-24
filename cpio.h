// Copyright (c) 2011 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "config.h"

/// CPIO archive reader
class CPIO {
    typedef uint8_t		value_type;
    typedef value_type*		pointer;
    typedef const value_type*	const_pointer;
    typedef pointer		iterator;
    typedef const_pointer	const_iterator;
public:
				CPIO (const char* filename);
				~CPIO (void);
    istream			File (const char* filename);
    inline uint32_t		size (void) const	{ return (m_DataSize); }
    inline const_iterator	begin (void) const	{ return (m_pData); }
    inline const_iterator	end (void) const	{ return (m_pData + size()); }
private:
    uint8_t*			m_pData;
    uint32_t			m_DataSize;
    int				m_fd;
};
