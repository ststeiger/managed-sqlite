/******************************************************************************\
 * ManagedSQLite.h                                                            *
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

    THIS SOFTWARE IS PROVIDED BY MANAGEDSQLITE TEAM ``AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE MANAGEDSQLITE TEAM BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

#pragma once
#include "StringTranslator.h"
#include "sqlite3.h"

#define MANAGEDSQLITE_ERROR 1000

namespace ManagedSQLite
{
    public ref class Exception : public System::Exception
    {
    public:
        Exception(String ^message,
                  int SQLite3ErrorCode) :
                  System::Exception(message)
        {
            this->_SQLite3ErrorCode = SQLite3ErrorCode;
        }
        property int SQLiteErrorCode
        {
            int get() { return _SQLite3ErrorCode; }
        }

        property String ^SQLiteErrorCodeToString
        {
            String ^get()
            {
                switch (_SQLite3ErrorCode)
                {
                    case SQLITE_OK          : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_OK"));
                    case SQLITE_ERROR       : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_ERROR"));
                    case SQLITE_INTERNAL    : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_INTERNAL"));
                    case SQLITE_PERM        : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_PERM"));
                    case SQLITE_ABORT       : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_ABORT"));
                    case SQLITE_BUSY        : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_BUSY"));
                    case SQLITE_LOCKED      : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_LOCKED"));
                    case SQLITE_NOMEM       : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_NOMEM"));
                    case SQLITE_READONLY    : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_READONLY"));
                    case SQLITE_INTERRUPT   : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_INTERRUPT"));
                    case SQLITE_IOERR       : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_IOERR"));
                    case SQLITE_CORRUPT     : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_CORRUPT"));
                    case SQLITE_NOTFOUND    : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_NOTFOUND"));
                    case SQLITE_FULL        : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_FULL"));
                    case SQLITE_CANTOPEN    : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_CANTOPEN"));
                    case SQLITE_PROTOCOL    : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_PROTOCOL"));
                    case SQLITE_EMPTY       : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_EMPTY"));
                    case SQLITE_SCHEMA      : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_SCHEMA"));
                    case SQLITE_TOOBIG      : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_TOOBIG"));
                    case SQLITE_CONSTRAINT  : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_CONSTRAINT"));
                    case SQLITE_MISMATCH    : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_MISMATCH"));
                    case SQLITE_MISUSE      : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_MISUSE"));
                    case SQLITE_NOLFS       : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_NOLFS"));
                    case SQLITE_AUTH        : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_AUTH"));
                    case SQLITE_FORMAT      : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_FORMAT"));
                    case SQLITE_RANGE       : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_RANGE"));
                    case SQLITE_ROW         : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_ROW"));
                    case SQLITE_DONE        : return UNMANAGED_TO_MANAGED_STRING(_T("SQLITE_DONE"));
                    case MANAGEDSQLITE_ERROR: return UNMANAGED_TO_MANAGED_STRING(_T("MANAGEDSQLITE_ERROR"));
                    default                 : return UNMANAGED_TO_MANAGED_STRING(_T("UNKNOWN_ERROR"));
                }
            }
        }
    private:
        int _SQLite3ErrorCode;
    };

    public ref class Query
    {
    public:
        Query();
        Query(const Query %rQuery);
        Query(sqlite3 *pDB,
              sqlite3_stmt *pVM,
              bool bEof,
              bool bOwnVM);
        Query(sqlite3 *pDB,
              sqlite3_stmt *pVM,
              bool bEof);
        virtual ~Query();

        Query% operator=(const Query %query);

        int NumberOfFields();

        int FieldIndex(String ^szField);
        String ^ FieldName(int nColumn);

        String ^ FieldDeclType(int nColumn);
        int FieldDataType(int nColumn);
        
        String ^ FieldValue(int nField);
        String ^ FieldValue(String ^szField);
        

        int GetIntField(int nField, int nNullValue);
        int GetIntField(String ^szField, int nNullValue);
        int GetIntField(int nField);
        int GetIntField(String ^szField);

        double GetFloatField(int nField, double fNullValue);
        double GetFloatField(String ^szField, double fNullValue);
        double GetFloatField(int nField);
        double GetFloatField(String ^szField);

        String ^ GetStringField(int nField, String ^szNullValue);
        String ^ GetStringField(String ^szField, String ^szNullValue);
        String ^ GetStringField(int nField);
        String ^ GetStringField(String ^szField);

        const unsigned char* GetBlobField(int nField, int& nLength);
        const unsigned char* GetBlobField(String ^szField, int& nLength);

        bool IsFieldIsNull(int nField);
        bool IsFieldIsNull(String ^szField);

        bool EndOfFile();
        void NextRow();
        void FinalizeQuery();
        
    private:
        void CheckVM();

        sqlite3 *mpDB;
        sqlite3_stmt *mpVM;
        bool mbEof;
        int mnCols;
        bool mbOwnVM;
    };

    public ref class Statement
    {
    public:
        Statement();
        Statement(const Statement %rStatement);
        Statement(sqlite3 *pDB, sqlite3_stmt *pVM);
        virtual ~Statement();

        Statement% operator=(const Statement %rStatement);

        int ExecuteDML();
        //Query^ ExecuteQuery();

        void Bind(int nParam, String ^szValue);
        void Bind(int nParam, const int nValue);
        void Bind(int nParam, const double dwValue);
        void Bind(int nParam,
                  const unsigned char* blobValue,
                  int nLen);
        void BindNull(int nParam);

        void Reset();
        void FinalizeStatement();

    private:
        void CheckDB();
        void CheckVM();

        sqlite3 *mpDB;
        sqlite3_stmt *mpVM;
    };

    public ref class Database
    {
    public:
        Database();
        virtual ~Database();

        void Open(String ^szFile);
        void Close();

        bool IndexExists(String ^szIndex);
        bool TableExists(String ^szTable);

        void BeginTransaction();
        void CommitTransaction();
        void EndTransaction();
       
        int ExecuteDML(String ^szSQL);
        Query ^ ExecuteQuery(String ^szSQL);
        int ExecuteScalar(String ^szSQL);

        Statement ^ CompileStatement(String ^szSQL);

        sqlite_int64 LastRowId();

        void Interrupt() { sqlite3_interrupt(mpDB); }
        void SetBusyTimeout(int nMilliseconds);

        static String ^ SQLiteVersion() { return _T(SQLITE_VERSION); }

    private:
        Database(const Database %db);
        Database % operator=(const Database %db);

        sqlite3_stmt * Compile(String ^szSQL);
        void CheckDB();

        sqlite3* mpDB;

        int mnBusyTimeoutMilliseconds;
    };
};
