////////////////////////////////////////////////////////////////////////
//
// File: SQLGuid.h
//
// Copyright (c) 1998-2018 ir. W.E. Huisman
// All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies 
// or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Last Revision:   20-04-2018
// Version number:  1.4.7
//
#pragma once
#include <sqltypes.h>   // Needed for conversions of SQLGUID

namespace SQLComponents
{

class SQLGuid
{
public:
  SQLGuid();
  SQLGuid(const SQLGuid& p_guid);
  SQLGuid(const SQLGUID* p_guid);
  SQLGuid(const CString  p_string);

  bool     New();
  // Set from external values
  bool     Set(const CString  p_string);
  bool     Set(const SQLGUID* p_guid);

  // Get the internals
  bool     IsValid();
  SQLGUID* AsGUID();
  CString  AsString();

  // Operators
  bool     operator ==(const SQLGuid& p_other);
  SQLGuid& operator  =(const SQLGuid& p_other);
private:
  bool     GenerateGUID();
  // DATA
  bool     m_initialized { false };
  SQLGUID  m_guid;
};

}
