////////////////////////////////////////////////////////////////////////
//
// File: TestNumeric.cpp
//
// Copyright (c) 1998-2016 ir. W.E. Huisman
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
// Last Revision:   04-12-2016
// Version number:  1.3.0
//
#include "stdafx.h"
#include "TestSQL.h"
#include "SQLQuery.h"
#include "SQLDatabase.h"
#include "SQLVariant.h"
#include "bcd.h"

#pragma warning(disable: 4477)

void TestNumeric()
{
  printf("Testing Numeric reading/writing:\n");
  printf("================================\n");

  SQLDatabase dbs;
  dbs.RegisterLogContext(LOGLEVEL_MAX,LogLevel,LogPrint,(void*)"");
  long beginTime = clock();

  try
  {
    g_dsn = "ok2b01";  // twoc09 = oracle / ok2b01 = MS-SQLServer / testing = MS-Access
    g_user = "k2b";
    g_password = "k2b";

    // Set options for the database
    dbs.SetLoginTimeout(0);
    dbs.SetMARS(false);

    if(dbs.Open(g_dsn,g_user,g_password))
    {
      printf("Database opened.\n");

      CString sql = "SELECT field2,field3 FROM test_number";
      SQLQuery query(&dbs);

      query.DoSQLStatement(sql);
      while(query.GetRecord())
      {
        double field1 = query[1];
        bcd    field2 = query[2];

        printf("Field 1 [%.4f] Field 2 [%s]\n",field1,field2.AsString());
      }


      // Do an update
      SQLTransaction trans(&dbs,"UpdNum");
      sql = "UPDATE test_number\n"
            "   SET field3 = ?\n"
            " WHERE id     = 1";
      bcd num("9876.99");

      // Parameter must match prec/scale of the database field
      query.SetParameter(1,num,14,2);

      query.DoSQLStatementNonQuery(sql);
      trans.Commit();
    }
    else
    {
      printf("Database ***NOT*** opened.\n");
    }
    dbs.Close();
  }
  catch(CString& s)
  {
    printf("Database error. Reason: %s\n",s);
  }
  catch(...)
  {
    printf("Unknown error in database test\n");
  }
  long endTime = clock();
  printf("NUMERIC test performed in: %.4f seconds\n",(double)(endTime - beginTime) / CLOCKS_PER_SEC);
}