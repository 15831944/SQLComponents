////////////////////////////////////////////////////////////////////////
//
// File: InfoDiscoveryTest.cpp
//
// Copyright (c) 1998-2017 ir. W.E. Huisman
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
// Last Revision:   08-01-2017
// Version number:  1.4.0
//
#include "stdafx.h"
#include "SQLComponents.h"
#include "SQLDatabase.h"
#include "SQLQuery.h"
#include "SQLInfoDB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace SQLComponents;

namespace DatabaseUnitTest
{
  void CALLBACK LogPrint(void* p_context,const char* p_text);
  int  CALLBACK LogLevel(void* p_context);
  extern CString g_dsn;
  extern CString g_user;
  extern CString g_password;
  extern CString g_schema;

  TEST_CLASS(DiscoveryInfo)
  {

    TEST_METHOD(Discovery)
    {
      Logger::WriteMessage("SQLInfo Discovery test for tables and procedures");
      Logger::WriteMessage("================================================");

      InitSQLComponents();

      OpenDatabase();

      // Types of objects we recognize
      MetaTypes();
      // Discover a few tables
      TableDiscovery("master");
      TableDiscovery("detail");
      // Discover a few procedures
      ProcedureDiscovery("multinout");
      ProcedureDiscovery("getdecimal");
      // Try out Native translation of the driver
      TranslateSQLtoNative("SELECT {fn UCASE(description)} FROM detail");

      CloseDatabase();
    }

    void MetaTypes()
    {
      Logger::WriteMessage("Meta types that can be discovered");
      MetaType(META_CATALOGS,"catalogs");
      MetaType(META_SCHEMAS, "schemata");
      MetaType(META_TABLES,  "tables");
    }

    void MetaType(int p_type,CString p_name)
    {
      SQLInfoDB* info = m_database->GetSQLInfoDB();
      MMetaMap objects;
      CString  errors;

      if(info->MakeInfoMetaTypes(objects,p_type,errors))
      {
        for(auto& obj : objects)
        {
          Logger::WriteMessage("Metatype " + p_name + " : " + obj.m_objectName);
        }
      }
      else
      {
        // Essentially not an error. It's an optional feature of ODBC
        Logger::WriteMessage("Cannot get the meta types for: " + p_name);
      }
    }

    void TableDiscovery(CString p_table)
    {
      Logger::WriteMessage("\nDO TABLE DISCOVERY: " + p_table);

      SQLInfoDB* info = m_database->GetSQLInfoDB();
      MTableMap tables;
      CString   errors;

      if(info->MakeInfoTableTablepart(p_table,tables,errors))
      {
        for(auto& table : tables)
        {
          Logger::WriteMessage("Table found: " + table.m_fullObjectName);
          ColumnsDiscovery(info);
          PrimaryDiscovery(info);
          ForeignDiscovery(info);
          IndicesDiscovery(info);
          SpecialDiscovery(info);
          TabPrivDiscovery(info);
        }
      }
      else if(!errors.IsEmpty())
      {
        Assert::Fail(L"Cannot get info for table");
      }
    }

    void ColumnsDiscovery(SQLInfoDB* p_info)
    {
      CString    errors;
      MColumnMap columns;

      if(p_info->MakeInfoTableColumns(columns,errors))
      {
        for(auto& column : columns)
        {
          Logger::WriteMessage("Column: " + column.m_column);
        }
      }
      else if(!errors.IsEmpty())
      {
        Assert::Fail(L"Cannot get column info for table");
      }
    }

    void PrimaryDiscovery(SQLInfoDB* p_info)
    {
      MPrimaryMap primaries;
      CString     errors;

      if(p_info->MakeInfoTablePrimary(primaries,errors))
      {

        for(auto& primary : primaries)
        {
          Logger::WriteMessage("Primary key constraint: " + primary.m_constraintName);

          CString text;
          text.Format("Primary key %d: %s"
                      ,primary.m_columnPosition
                      ,primary.m_columnName);
          Logger::WriteMessage(text);
        }
      }
      else if(!errors.IsEmpty())
      {
        Assert::Fail(L"Cannot get primary key info for table");
      }
    }
    
    void ForeignDiscovery(SQLInfoDB* p_info)
    {
      MForeignMap references;
      CString     errors;

      if(p_info->MakeInfoTableForeign(references,errors))
      {
        for(auto& ref : references)
        {
          Logger::WriteMessage("Foreign: " + ref.m_foreignConstraint);
        }
      }
      else if (!errors.IsEmpty())
      {
        Assert::Fail(L"Cannot get foreign key info for table");
      }
    }

    void IndicesDiscovery(SQLInfoDB* p_info)
    {
      MStatisticsMap statistics;
      CString        errors;

      if(p_info->MakeInfoTableStatistics(statistics,nullptr,errors))
      {
        for(auto& ind : statistics)
        {
          Logger::WriteMessage("Indices: " + ind.m_indexName);
        }
      }
      else if(!errors.IsEmpty())
      {
        Assert::Fail(L"Cannot get statistics info for table");
      }
    }

    void SpecialDiscovery(SQLInfoDB* p_info)
    {
      MSpecialColumnMap specials;
      CString errors;
      if(p_info->MakeInfoTableSpecials(specials,errors))
      {
        for(auto& spec : specials)
        {
          Logger::WriteMessage("Specials: " + spec.m_columnName);
        }
      }
      else if(!errors.IsEmpty())
      {
        Assert::Fail(L"Cannot get special info for table");
      }
    }

    void TabPrivDiscovery(SQLInfoDB* p_info)
    {
      MPrivilegeMap privileges;
      CString errors;

      if(p_info->MakeInfoTablePrivileges(privileges,errors))
      {
        for(auto& priv : privileges)
        {
          CString line;
          line.Format("Table privilege: %s was granted %s by %s"
                     ,priv.m_grantee
                     ,priv.m_privilege
                     ,priv.m_grantor);
          Logger::WriteMessage(line);
        }
      }
      else if(!errors.IsEmpty())
      {
        Assert::Fail(L"Cannot get table priviliges for table");
      }
    }

    void ProcedureDiscovery(CString p_procedure)
    {
      Logger::WriteMessage("\nDO PROCEDURE DISCOVERY: " + p_procedure);

      SQLInfoDB* info = m_database->GetSQLInfoDB();

      MProcedureMap procedures;
      CString errors;
      if(info->MakeInfoProcedureProcedurepart(p_procedure,procedures,errors))
      {
        for(auto& proc : procedures)
        {
          Logger::WriteMessage("Procedure : " + proc.m_procedureName);
        }
        ParametersDiscovery(info);
      }
      else if(!errors.IsEmpty())
      {
        Logger::WriteMessage("Cannot find procedure: " + p_procedure);
      }
    }

    void ParametersDiscovery(SQLInfoDB* p_info)
    {
      MProcColumnMap params;
      CString errors;

      if(p_info->MakeInfoProcedureParameters(params,errors))
      {
        for(auto& parm : params)
        {
          Logger::WriteMessage("Parameter : " + parm.m_columnName);
        }
      }
      else if(!errors.IsEmpty())
      {
        Assert::Fail(L"Cannot get parameters for procedure");
      }
    }

    void TranslateSQLtoNative(CString p_sql)
    {
      SQLInfoDB* info = m_database->GetSQLInfoDB();
      CString translated = info->NativeSQL(NULL,p_sql);

      Logger::WriteMessage("TRANSLATE : " + p_sql);
      Logger::WriteMessage("Translated: " + translated);
    }

//////////////////////////////////////////////////////////////////////////

    bool OpenDatabase()
    {
      Logger::WriteMessage("Opening database....");

      m_database = new SQLDatabase();
      m_database->RegisterLogContext(LOGLEVEL_MAX,LogLevel,LogPrint,(void*)"");

      try
      {
        m_database->SetMARS(false);
        m_database->SetLoginTimeout(0);
        if(m_database->Open(g_dsn,g_user,g_password))
        {
          Logger::WriteMessage("Database opened.");
        }
        else
        {
          Assert::Fail(L"Database ***NOT*** opened.");
        }
      }
      catch(CString& s)
      {
        Logger::WriteMessage(s);
        Assert::Fail(L"Database ***NOT*** opened.");
      }
      catch(...)
      {
        Assert::Fail(L"Database ***NOT*** opened.");
      }
      return true;
    }

    void CloseDatabase()
    {
      if(m_database)
      {
        Logger::WriteMessage("Closing database....");
        m_database->Close();
        delete m_database;
        m_database = nullptr;
      }
    }

  private:
    SQLDatabase* m_database { nullptr };
  };
}
