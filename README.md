> ### Usage
> #### you can use the library in form of single header file in `header_only_src` directory. The files in `include` and`src` are exactly same but in the form of .hpp/.cpp files, which you need to compile and link with.

[![GitHub license](https://img.shields.io/github/license/geekyMrK/SQLiteCpp)](https://github.com/geekyMrK/SQLiteCpp/blob/main/LICENSE)
[![Made With Love](https://img.shields.io/badge/Made%20With-Love-orange.svg)](https://github.com/geekyMrK/SQLiteCpp)

# SQLiteCpp [![SQLite](https://img.shields.io/badge/sqlite-%2307405e.svg?style=for-the-badge&logo=sqlite&logoColor=white)](https://www.sqlite.org/index.html)[![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org)
A simple and efficient wrapper for SQLite3 for C++11 

## The Goals of SQLiteCpp are:
- to be a Simple and Efficient wrapper over SQLite3 
- to keep deficiencies to a minimum (SQLite3 and C++11 STL)
- to design a good Wrapper using Modern C++
- to support all SQLite constraints and clauses
- to support for-each loop 
- to support Unicode


## Examples

#### Creating a table, inserting values in it and then printing them
```cpp
try {
    //Creates a SQLite connection object
    Sqlite::SqliteConnection connection("myProfile.db");
    
    // sqliteExecute(connectionOb, "query"); executes a SQLite query, in this case creates a table
    sqliteExecute(connection, "create table myResume (skills text, proficiency int DEFAULT 5)");
    
    sqliteExecute(connection, "insert into myResume(skills, proficiency ) values (?, ?)", "C++", 7);
    sqliteExecute(connection, "insert into myResume(skills, proficiency ) values (?, ?)", "C", 6);
    
    sqliteExecute(connection, "insert into myResume(skills) values (?)", "libCurl");
    
    for (auto row : Sqlite::SqliteStatement(connection, "select skills, proficiency from myResume")) {
        std::cout << std::left << std::setw(8) << row.getString() << " : " << row.getInt(1) << std::endl;
    }
}
catch (const Sqlite::exception &e) {
    std::cout << e.errorMessage_ << '(' << e.errorCode_ << ')';
}
```
```
C++      : 7
C        : 6
libCurl  : 5
```
#### Creating a Sqlite::SqliteStatement and using it multiple times
```cpp
try {
    //Creates a in-memory database
    const Sqlite::SqliteConnection connection = Sqlite::SqliteConnection::memory();

    sqliteExecute(connection, "create table IF NOT EXISTS myResume ("
                        "skills text NOT NULL, "
                        "proficiency int DEFAULT 5 "
                        "CHECK (proficiency <= 10)"
                    ")");
                                      
    //Creates a SQLite statment and binds the values in it
    Sqlite::SqliteStatement statement(connection, "insert into myResume(skills, proficiency ) values (?, ?)", "C++", 7);
    //Executes SqliteStatement
    statement.execute();
    
    //Resets the values that are bind to the SqliteStatement
    statement.reset("C", 6);
    statement.execute();
    
    statement.reset("Boost", 7);
    statement.execute();
    
    statement.reset("MySQL", 6);
    statement.execute();
    
    statement.reset("SQLite", 7);
    statement.execute();
    
    std::cout << "the list of my skills in order of proficiency are:" << std::endl;
    for (auto row : Sqlite::SqliteStatement(connection, "select skills, proficiency from myResume ORDER BY proficiency DESC")) {
        std::cout << std::left << std::setw(8) << row.getString() << " : " << std::right << std::setw(6) << row.getInt(1) << std::endl;
    }
}
catch (const Sqlite::exception &e) {
    std::cout << e.errorMessage_ << '(' << e.errorCode_ << ')';
}
```
```
the list of my skills in order of proficiency are:
C++      :      7
Boost    :      7
SQLite   :      7
C        :      6
MySQL    :      6
```

## Contributing [![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/geekyMrK/SQLiteCpp)
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
Please make sure to update tests as appropriate
.
## License [![GitHub license](https://img.shields.io/github/license/geekyMrK/SQLiteCpp)](https://github.com/geekyMrK/SQLiteCpp/blob/main/LICENSE)
Distributed under the [MIT License](https://choosealicense.com/licenses/mit/)

#### Note on redistribution of SQLite source files

As stated by the MIT License, you are welcome to reuse, modify, and redistribute the SQLiteCpp source code the way you want it to, be it a git submodule, a subdirectory, or a selection of some source files.

I would love a mention in your README, a web link to the Sqlite repository, and a mention of the author, but none of those are mandatory.


