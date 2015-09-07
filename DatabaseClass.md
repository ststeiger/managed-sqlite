## Members ##
  * `Database()` Default constructor.
  * `Database(const Database% db)` Copy constructor.
  * `~Database()` Default destructor.
  * `Database % operator=(const Database% rhs)` Assignment operator.
  * `void Open(String ^szFile)` Opens and/or creates the database file located at `szFile`.
  * `void Close()` Closes the database file that was opened with `Open()`;
  * `bool IndexExists(String ^szIndex)` Returns `true` if the database file has an index named `szIndex`.  Returns `false` otherwise.
  * `bool TableExists(String ^szTable)` Returns `true` if the database file has an table named `szTable`.  Returns `false` otherwise.
  * `void BeginTransaction()` Begins a transaction.
  * `void CommitTransaction()` Ends and commits the transaction to the database file.
  * `void EndTransaction()` Ends and commits the transaction to the database file.
  * `int ExecuteDML(String ^szSQL)` Executes a data manipulation query on the open database.  Returns the number of changes.
  * `Query ^ ExecuteQuery(String ^szSQL)` Executes a query on the open database.  Returns a `Query` object as a recordset.
  * `int ExecuteScalar(String ^szSQL)` Executes a scalar query on the open database.  Returns the value of the result.

