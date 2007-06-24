/******************************************************************************\
 * ManagedSQLite.cpp                                                          *
 * An Unicode and managed implementation of Rob Groves's CppSQLite3 wrapper   *
\******************************************************************************/

/*
    ManagedSQLite
    Copyright (c) 2007 ManagedSQLite Team.  All Rights Reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in the
          documentation and/or other materials provided with the distribution.
        * Neither the name of the ManagedSQLite Team nor the
          names of its contributors may be used to endorse or promote products
          derived from this software without specific prior written permission.

    Original incarnation:
        Rob Groves - rob.groves@btinternet.com
        http://www.codeproject.com/database/CppSQLite.asp

        Copyright (c) 2004 Rob Groves. All Rights Reserved.

        Permission to use, copy, modify, and distribute this software and its
        documentation for any purpose, without fee, and without a written
        agreement, is hereby granted, provided that the above copyright notice, 
        this paragraph and the following two paragraphs appear in all copies, 
        modifications, and distributions.

        IN NO EVENT SHALL THE AUTHOR BE LIABLE TO ANY PARTY FOR DIRECT,
        INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST
        PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
        EVEN IF THE AUTHOR HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

        THE AUTHOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
        LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
        PARTICULAR PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF
        ANY, PROVIDED HEREUNDER IS PROVIDED "AS IS". THE AUTHOR HAS NO OBLIGATION
        TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

    Support for Unicode added by:
        Tyushkov Nikolay at SoftVoile
        http://www.softvoile.com/development/CppSQLite3U/

        Copyright (c) 2006 Tyushkov Nikolay.  All Rights Reserved.
*/

#include "ManagedSQLite.h"

using namespace ManagedSQLite;

#pragma region Query
Query::Query()
{
    mpVM = 0;
    mbEof = true;
    mnCols = 0;
    mbOwnVM = false;
}
Query::Query(const Query %rQuery)
{
    mpVM = rQuery.mpVM;
    const_cast<Query%>(rQuery).mpVM = 0;
    mbEof = rQuery.mbEof;
    mnCols = rQuery.mnCols;
    mbOwnVM = rQuery.mbOwnVM;
}
Query::Query(sqlite3 *pDB,
             sqlite3_stmt *pVM,
             bool bEof,
             bool bOwnVM)
{
    mpDB = pDB;
    mpVM = pVM;
    mbEof = bEof;
    mnCols = sqlite3_column_count(mpVM);
    mbOwnVM = bOwnVM;
}
Query::Query(sqlite3 *pDB,
             sqlite3_stmt *pVM,
             bool bEof)
{
    mpDB = pDB;
    mpVM = pVM;
    mbEof = bEof;
    mnCols = sqlite3_column_count(mpVM);
    mbOwnVM = true;
}
Query::~Query()
{
    try
    {
        FinalizeQuery();
    }
    catch(...) { }
}

Query % Query::operator =(const Query %rQuery)
{
    try
    {
        FinalizeQuery();
    }
    catch(...) { }

    mpVM = rQuery.mpVM;
    const_cast<Query%>(rQuery).mpVM = 0;
    mbEof = rQuery.mbEof;
    mnCols = rQuery.mnCols;
    mbOwnVM = rQuery.mbOwnVM;

    return *this;
}

int Query::NumberOfFields()
{
    CheckVM();
    return mnCols;
}

String ^ Query::FieldValue(int nField)
{
    CheckVM();

    if (nField < 0 || nField > mnCols - 1)
    {
        throw gcnew Exception(_T("Invalid field index requested."),
                              MANAGEDSQLITE_ERROR);
    }

#ifdef UNICODE
    return UNMANAGED_TO_MANAGED_STRING((LPCTSTR)sqlite3_column_text16(mpVM, nField));
#else
    return UNMANAGED_TO_MANAGED_STRING((LPCTSTR)sqlite3_column_text(mpVM, nField));
#endif
};
String ^ Query::FieldValue(String ^szField)
{
    int nField = FieldIndex(szField);
    return FieldValue(nField);
}

int Query::GetIntField(int nField, int nNullValue)
{
    if (FieldDataType(nField) == SQLITE_NULL)
        return nNullValue;
    else
        return sqlite3_column_int(mpVM, nField);
}
int Query::GetIntField(String ^szField, int nNullValue)
{
    int nField = FieldIndex(szField);
    return GetIntField(nField, nNullValue);
}
int Query::GetIntField(int nField)
{
    return GetIntField(nField, 0);
}
int Query::GetIntField(String ^szField)
{
    return GetIntField(szField, 0);
}

double Query::GetFloatField(int nField, double fNullValue)
{
    if (FieldDataType(nField) == SQLITE_NULL)
        return fNullValue;
    else
        return sqlite3_column_double(mpVM, nField);
}
double Query::GetFloatField(String ^szField, double fNullValue)
{
    int nField = FieldIndex(szField);
    return GetFloatField(nField, fNullValue);
}
double Query::GetFloatField(int nField)
{
    return GetFloatField(nField, 0.0);
}
double Query::GetFloatField(String ^szField)
{
    return GetFloatField(szField, 0.0);
}

String ^ Query::GetStringField(int nField, String ^szNullValue)
{
    if (FieldDataType(nField) == SQLITE_NULL)
        return szNullValue;
    else
    {
#ifdef UNICODE
        return UNMANAGED_TO_MANAGED_STRING((LPCTSTR)sqlite3_column_text16(mpVM, nField));
#else
        return UNMANAGED_TO_MANAGED_STRING((LPCTSTR)sqlite3_column_text(mpVM, nField));
#endif
    }
}
String ^ Query::GetStringField(String ^szField, String ^szNullValue)
{
    int nField = FieldIndex(szField);
	return GetStringField(nField, szNullValue);
}
String ^ Query::GetStringField(int nField)
{
    return GetStringField(nField, _T(""));
}
String ^ Query::GetStringField(String ^szField)
{
    return GetStringField(szField, _T(""));
}

const unsigned char* Query::GetBlobField(int nField, int &nLength)
{
    CheckVM();

    if (nField < 0 || nField > mnCols - 1)
    {
        throw gcnew Exception(_T("Invalid field index requested."),
                              MANAGEDSQLITE_ERROR);
    }

#ifdef UNICODE
    nLength = sqlite3_column_bytes16(mpVM, nField);
#else
    nLength = sqlite3_column_bytes(mpVM, nField);
#endif
    return (const unsigned char*)sqlite3_column_blob(mpVM, nField);
}
const unsigned char* Query::GetBlobField(String ^szField, int &nLength)
{
    int nField = FieldIndex(szField);
    return GetBlobField(nField, nLength);
}

bool Query::IsFieldIsNull(int nField)
{
    return (FieldDataType(nField) == SQLITE_NULL);
}

bool Query::IsFieldIsNull(String ^szField)
{
    int nField = FieldIndex(szField);
    return (FieldDataType(nField) == SQLITE_NULL);
}
int Query::FieldIndex(String ^szField)
{
	CheckVM();

	if (szField->Length > 0)
	{
		for (int nField = 0; nField < mnCols; nField++)
		{
#ifdef UNICODE
			String ^szTemp = UNMANAGED_TO_MANAGED_STRING((LPCTSTR)sqlite3_column_name16(mpVM, nField));
#else
            String ^szTemp = UNMANAGED_TO_MANAGED_STRING((LPCTSTR)sqlite3_column_name(mpVM, nField));
#endif
            if (String::Compare(szField, szTemp) == 0)
				return nField;
		}
	}
	throw gcnew Exception(_T("Invalid field name requested."),
                             MANAGEDSQLITE_ERROR);
}
String ^ Query::FieldName(int nColumn)
{
    CheckVM();

    if (nColumn < 0 || nColumn > mnCols - 1)
    {
        throw gcnew Exception(_T("Invalid field index requested."),
                                 MANAGEDSQLITE_ERROR);
    }

#ifdef UNICODE
    return UNMANAGED_TO_MANAGED_STRING((LPCTSTR)sqlite3_column_name16(mpVM, nColumn));
#else
    return UNMANAGED_TO_MANAGED_STRING((LPCTSTR)sqlite3_column_name(mpVM, nColumn));
#endif

}

String ^ Query::FieldDeclType(int nColumn)
{
    CheckVM();

    if (nColumn < 0 || nColumn > mnCols - 1)
    {
        throw gcnew Exception(_T("Invalid field index requested."),
                                 MANAGEDSQLITE_ERROR);
    }

#ifdef UNICODE
    return UNMANAGED_TO_MANAGED_STRING((LPCTSTR)sqlite3_column_decltype16(mpVM, nColumn));
#else
    return UNMANAGED_TO_MANAGED_STRING((LPCTSTR)sqlite3_column_decltype(mpVM, nColumn));
#endif
}
int Query::FieldDataType(int nColumn)
{
    CheckVM();

    if (nColumn < 0 || nColumn > mnCols - 1)
    {
        throw gcnew Exception(_T("Invalid field index requested."),
                              MANAGEDSQLITE_ERROR);
    }

    return sqlite3_column_type(mpVM, nColumn);
}
bool Query::EndOfFile()
{
    CheckVM();
    return mbEof;
}

void Query::NextRow()
{
    CheckVM();

    int nRet = sqlite3_step(mpVM);

    if (nRet == SQLITE_DONE)
        mbEof = true;
    else if (nRet == SQLITE_ROW)
        return;
    else
    {
#ifdef UNICODE
        LPCTSTR szError = (LPCTSTR) sqlite3_errmsg16(mpDB);
#else
        LPCTSTR szError = (LPCTSTR) sqlite3_errmsg(mpDB);
#endif
        throw gcnew Exception(UNMANAGED_TO_MANAGED_STRING(szError),
                     nRet);
    }
}
void Query::FinalizeQuery()
{
    if (mpVM && mbOwnVM)
    {
        int nRet = sqlite3_finalize(mpVM);
        mpVM = 0;

        if (nRet != SQLITE_OK)
        {
#ifdef UNICODE
            LPCTSTR szError = (LPCTSTR) sqlite3_errmsg16(mpDB);
#else
            LPCTSTR szError = (LPCTSTR) sqlite3_errmsg(mpDB);
#endif
            throw gcnew Exception(UNMANAGED_TO_MANAGED_STRING(szError),
                                  nRet);
        }
    }
}
void Query::CheckVM()
{
    if (mpVM == 0)
    {
        throw gcnew Exception(_T("Null virtual machine pointer."),
                              MANAGEDSQLITE_ERROR);
    }
}
#pragma endregion

#pragma region Statement
Statement::Statement()
{
    mpDB = 0;
    mpVM = 0;
}
Statement::Statement(const Statement %rStatement)
{
    mpDB = rStatement.mpDB;
    mpVM = rStatement.mpVM;
    const_cast<Statement%>(rStatement).mpVM = 0;
}
Statement::Statement(sqlite3 *pDB, sqlite3_stmt *pVM)
{
    mpDB = pDB;
    mpVM = pVM;
}
Statement::~Statement()
{
    try
    {
        FinalizeStatement();
    }
    catch (...) { }
}

Statement % Statement::operator =(const Statement %rStatement)
{
    mpDB = rStatement.mpDB;
    mpVM = rStatement.mpVM;
    const_cast<Statement%>(rStatement).mpVM = 0;

    return *this;
}
int Statement::ExecuteDML()
{
    CheckDB();
    CheckVM();

    int nRet = sqlite3_step(mpVM);
    if (nRet == SQLITE_DONE)
    {
        int nRowsChanged = sqlite3_changes(mpDB);

        nRet = sqlite3_reset(mpVM);

        if (nRet != SQLITE_OK)
        {
#ifdef UNICODE
            LPCTSTR szError = (LPCTSTR) sqlite3_errmsg16(mpDB);
#else
            LPCTSTR szError = (LPCTSTR) sqlite3_errmsg(mpDB);
#endif
            throw gcnew Exception(UNMANAGED_TO_MANAGED_STRING(szError),
                                  nRet);
        }
        return nRowsChanged;
    }
    else
    {
        nRet = sqlite3_reset(mpVM);
#ifdef UNICODE
        LPCTSTR szError = (LPCTSTR) sqlite3_errmsg16(mpDB);
#else
        LPCTSTR szError = (LPCTSTR) sqlite3_errmsg(mpDB);
#endif
        throw gcnew Exception(UNMANAGED_TO_MANAGED_STRING(szError),
                              nRet);
    }
}
void Statement::Bind(int nParam, String ^szValue)
{
    CheckVM();
#ifdef UNICODE
   int nRes = sqlite3_bind_text16(mpVM,
                                  nParam,
                                  MANAGED_TO_UNMANAGED_STRING(szValue).c_str(),
                                  -1,
                                  SQLITE_TRANSIENT);
#else
    int nRes = sqlite3_bind_text(mpVM,
                                 nParam,
                                 MANAGED_TO_UNMANAGED_STRING(szValue).c_str(),
                                 -1,
                                 SQLITE_TRANSIENT);
#endif
    if (nRes != SQLITE_OK)
    {
        throw gcnew Exception(_T("Error binding string param."),
                              nRes);
    }
}

void Statement::Bind(int nParam, const int nValue)
{
    CheckVM();
    int nRes = sqlite3_bind_int(mpVM, nParam, nValue);
    if (nRes != SQLITE_OK)
    {
        throw gcnew Exception(_T("Error binding int param."),
                              nRes);
    }
}
void Statement::Bind(int nParam, const double dValue)
{
    CheckVM();
    int nRes = sqlite3_bind_double(mpVM, nParam, dValue);
    if (nRes != SQLITE_OK)
    {
        throw gcnew Exception(_T("Error binding double param."),
                              nRes);
    }
}
void Statement::Bind(int nParam,
                     const unsigned char* blobValue,
                     int nLen)
{
    CheckVM();
    int nRes = sqlite3_bind_blob(mpVM,
                                 nParam,
                                 (const void*) blobValue,
                                 nLen,
                                 SQLITE_TRANSIENT);
    if (nRes != SQLITE_OK)
    {
        throw gcnew Exception(_T("Error binding blob param."),
                              nRes);
    }
}
void Statement::BindNull(int nParam)
{
    CheckVM();
    int nRes = sqlite3_bind_null(mpVM, nParam);
    if (nRes != SQLITE_OK)
    {
        throw gcnew Exception(_T("Error binding null param."),
                              nRes);
    }
}

void Statement::Reset()
{
    if (mpVM)
    {
        int nRet = sqlite3_reset(mpVM);

        if (nRet != SQLITE_OK)
        {
#ifdef UNICODE
            LPCTSTR szError = (LPCTSTR) sqlite3_errmsg16(mpDB);
#else
            LPCTSTR szError = (LPCTSTR) sqlite3_errmsg(mpDB);
#endif
            throw gcnew Exception(UNMANAGED_TO_MANAGED_STRING(szError),
                                  nRet);
        }
    }
}
void Statement::FinalizeStatement()
{
    if (mpVM)
    {
        int nRet = sqlite3_finalize(mpVM);
        mpVM = 0;

        if (nRet != SQLITE_OK)
        {
#ifdef UNICODE
            LPCTSTR szError = (LPCTSTR) sqlite3_errmsg16(mpDB);
#else
            LPCTSTR szError = (LPCTSTR) sqlite3_errmsg(mpDB);
#endif
            throw gcnew Exception(UNMANAGED_TO_MANAGED_STRING(szError),
                                  nRet);
        }
    }
}

void Statement::CheckDB()
{
    if (mpDB == 0)
    {
        throw gcnew Exception(_T("Database not open."),
                              MANAGEDSQLITE_ERROR);
    }
}
void Statement::CheckVM()
{
    if (mpVM == 0)
    {
        throw gcnew Exception(_T("Null virtual machine pointer."),
                              MANAGEDSQLITE_ERROR);
    }
}
#pragma endregion

#pragma region Database
Database::Database()
{
    mpDB = 0;
    mnBusyTimeoutMilliseconds = 60000;
}
Database::Database(const Database% db)
{
    mpDB = db.mpDB;
    mnBusyTimeoutMilliseconds = 60000;
}
Database::~Database()
{
    Close();
}

Database % Database::operator =(const Database% db)
{
    mpDB = db.mpDB;
    mnBusyTimeoutMilliseconds = 60000;
    return *this;
}

void Database::Open(String ^szFile)
{
    int nRet = 0;

    sqlite3 *ptr = 0;
#ifdef UNICODE
    nRet = sqlite3_open16(MANAGED_TO_UNMANAGED_STRING(szFile).c_str(), &ptr);
#else
    nRet = sqlite3_open(MANAGED_TO_UNMANAGED_STRING(szFile).c_str(), &ptr);
#endif
    mpDB = ptr;

    if (nRet != SQLITE_OK)
    {
#ifdef UNICODE
        LPCTSTR szError = (LPCTSTR) sqlite3_errmsg16(mpDB);
#else
        LPCTSTR szError = (LPCTSTR) sqlite3_errmsg(mpDB);
#endif
        throw gcnew Exception(UNMANAGED_TO_MANAGED_STRING(szError),
                              nRet);
    }

    SetBusyTimeout(mnBusyTimeoutMilliseconds);
}

void Database::Close()
{
    if (mpDB)
    {
        int nRet = sqlite3_close(mpDB);

        if (nRet != SQLITE_OK)
        {
#ifdef UNICODE
            LPCTSTR szError = (LPCTSTR) sqlite3_errmsg16(mpDB);
#else
            LPCTSTR szError = (LPCTSTR) sqlite3_errmsg(mpDB);
#endif
            throw gcnew Exception(UNMANAGED_TO_MANAGED_STRING(szError),
                                  nRet);
        }

        mpDB = 0;
    }
}

Statement ^ Database::CompileStatement(String ^szSQL)
{
    CheckDB();

    sqlite3_stmt* pVM = Compile(szSQL);

    return %Statement(mpDB, pVM);
}

bool Database::IndexExists(String ^szIndex)
{
	String ^szSQL;

	szSQL  = _T("SELECT count(*) FROM `sqlite_master` ");
    szSQL += _T("WHERE `type` = 'index' AND `name` = ");
	szSQL += _T("'");
    szSQL += szIndex;
    szSQL += _T("';");

    int nRet = ExecuteScalar(szSQL);

	return (nRet > 0);
}

void Database::BeginTransaction()
{
    ExecuteQuery(_T("BEGIN TRANSACTION;"));
}
void Database::CommitTransaction()
{
    ExecuteQuery(_T("COMMIT TRANSACTION;"));
}
void Database::EndTransaction()
{
    ExecuteQuery(_T("END TRANSACTION;"));
}

bool Database::TableExists(String ^szTable)
{
	String ^szSQL;

	szSQL  = _T("SELECT count(*) FROM `sqlite_master` ");
    szSQL += _T("WHERE `type` = 'table' AND `name` = ");
	szSQL += _T("'");
    szSQL += szTable;
    szSQL += _T("';");

    int nRet = ExecuteScalar(szSQL);

	return (nRet > 0);
}

int Database::ExecuteDML(String ^szSQL)
{
    int nRet;
    int nChanges = 0;
    sqlite3_stmt* pVM;

    CheckDB();

    do
    {   
        pVM = Compile(szSQL);

        nRet = sqlite3_step(pVM);
        nChanges += sqlite3_changes(mpDB);

        if (nRet == SQLITE_ERROR)
        {
#ifdef UNICODE
            LPCTSTR szError = (LPCTSTR) sqlite3_errmsg16(mpDB);
#else
            LPCTSTR szError = (LPCTSTR) sqlite3_errmsg(mpDB);
#endif
            throw gcnew Exception(UNMANAGED_TO_MANAGED_STRING(szError),
                                  nRet);
        }

        nRet = sqlite3_finalize(pVM);
    }
    while (nRet == SQLITE_SCHEMA);

    return nChanges;
}

Query ^ Database::ExecuteQuery(String ^szSQL)
{
    int nRet;
    sqlite3_stmt* pVM;

    CheckDB();

    do
    {
        pVM = Compile(szSQL);

        nRet = sqlite3_step(pVM);

        if (nRet == SQLITE_DONE)
            return gcnew Query(mpDB, pVM, true);
        else if (nRet == SQLITE_ROW)
            return gcnew Query(mpDB, pVM, false);

        nRet = sqlite3_finalize(pVM);
    }
    while (nRet == SQLITE_SCHEMA);

#ifdef UNICODE
    LPCTSTR szError = (LPCTSTR) sqlite3_errmsg16(mpDB);
#else
    LPCTSTR szError = (LPCTSTR) sqlite3_errmsg(mpDB);
#endif
    throw gcnew Exception(UNMANAGED_TO_MANAGED_STRING(szError),
                          nRet);
}

int Database::ExecuteScalar(String ^szSQL)
{
    Query^ q = ExecuteQuery(szSQL);

    if (q->EndOfFile() || q->NumberOfFields() < 1)
    {
        throw gcnew Exception(_T("Invalid scalar query."),
                              MANAGEDSQLITE_ERROR);
    }

    return _tstoi(MANAGED_TO_UNMANAGED_STRING(q->FieldValue(0)).c_str());
}

sqlite_int64 Database::LastRowId()
{
    return sqlite3_last_insert_rowid(mpDB);
}

void Database::SetBusyTimeout(int nMilliseconds)
{
     mnBusyTimeoutMilliseconds = nMilliseconds;
     sqlite3_busy_timeout(mpDB, nMilliseconds);
}

void Database::CheckDB()
{
    if (!mpDB)
    {
        throw gcnew Exception(_T("Database not open."),
                              MANAGEDSQLITE_ERROR);
    }
}

sqlite3_stmt* Database::Compile(String ^szSQL)
{
    CheckDB();
    sqlite3_stmt* pVM;

    int nRet;

#ifdef UNICODE
    nRet = sqlite3_prepare16(mpDB, MANAGED_TO_UNMANAGED_STRING(szSQL).c_str(), -1, &pVM, 0);
#else
    nRet = sqlite3_prepare(mpDB, MANAGED_TO_UNMANAGED_STRING(szSQL).c_str(), -1, &pVM, 0);
#endif

    if (nRet != SQLITE_OK)
    {
        pVM = 0;

#ifdef UNICODE
        LPCTSTR szError = (LPCTSTR) sqlite3_errmsg16(mpDB);
#else
        LPCTSTR szError = (LPCTSTR) sqlite3_errmsg(mpDB);
#endif
        throw gcnew Exception(UNMANAGED_TO_MANAGED_STRING(szError),
                              nRet);
    }

    return pVM;
}
#pragma endregion
