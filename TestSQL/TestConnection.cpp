#include "stdafx.h"
#include "TestSQL.h"
#include "SQLDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void TestConnection()
{
  printf("Testing the connection:\n");
  printf("=======================\n");

  SQLDatabase dbs;
  dbs.RegisterLogContext(LOGLEVEL_MAX,LogLevel,LogPrint,(void*)"");

  long beginTime = clock();

  try
  {
    // Set options for the database
    dbs.SetLoginTimeout(0);
    dbs.SetMARS(false);
    // dbs.AddConnectOption("FBS","600000");

    if(dbs.Open(g_dsn,g_user,g_password))
    {
      printf("Database opened.\n");
    }
    else
    {
      printf("Database ***NOT*** opened.\n");
    }
  }
  catch(CString& s)
  {
    printf("Database ***NOT*** opened. Reason:\n%s\n",s);
  }
  catch(...)
  {
    printf("Database ***NOT*** opened for unknown reasons\n");
  }
  long endTime = clock();
  printf("Open  test performed in: %.6f seconds\n", (double)(endTime - beginTime) / CLOCKS_PER_SEC);

  //////////////////////////////////////////////////////////////////////////
  //
  // CLOSING NOW
  //
  //////////////////////////////////////////////////////////////////////////

  beginTime = clock();
  if(dbs.IsOpen())
  {
    dbs.Close();
  }
  endTime = clock();
  printf("Close test performed in: %.6f seconds\n", (double)(endTime - beginTime) / CLOCKS_PER_SEC);
}

