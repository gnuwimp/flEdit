// Copyright 2024 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0
// This source file is an amalgamation of many files
// All empty lines and comments have been removed
#ifdef GNU_USE_SQLITE3
#include <cstdint>
#include <string>
#include <vector>
struct sqlite3;
struct sqlite3_stmt;
namespace gnu {
struct DBRow;
typedef std::vector<DBRow>      DBRowVector;
struct DBRow {
    std::string                 key;
    char*                       value;
    size_t                      size;
    int64_t                     time;
                                DBRow()
                                    { value = nullptr; size = 0; time = -1; }
                                DBRow(std::string KEY, char* VALUE, size_t SIZE, int64_t TIME = -1)
                                    { key = KEY; value = VALUE; size = SIZE; time = TIME; }
                                DBRow(std::string key, const char* value, size_t size, int64_t time = -1);
                                DBRow(const DBRow& r);
                                DBRow(DBRow&& r)
                                    { key = r.key; size = r.size; time = r.time; value = r.value; r.value = nullptr; }
    virtual                     ~DBRow()
                                    { free(value); }
    DBRow&                      operator=(const DBRow& r);
    DBRow&                      operator=(DBRow&& r)
                                    { free(value); key = r.key; size = r.size; time = r.time; value = r.value; r.value = nullptr; return *this; }
    void                        debug(bool print_value = false) const;
    static void                 Debug(const DBRowVector& v, bool print_value = false);
};
class DB {
public:
    std::string                 err_msg;
    int                         err_code;
                                DB(const DB&) = delete;
    DB&                         operator=(const DB&) = delete;
                                DB();
    explicit                    DB(std::string filename);
                                DB(DB&& other);
    virtual                     ~DB()
                                    { close(); }
    DB&                         operator=(DB&&  other);
    bool                        begin()
                                    { return execute("BEGIN TRANSACTION"); }
    bool                        close();
    bool                        commit()
                                    { return execute("COMMIT TRANSACTION"); }
    bool                        defrag()
                                    { return execute("VACUUM"); }
    bool                        execute(std::string sql);
    std::string                 filename() const
                                    { return _filename; }
    DBRow                       get(std::string key);
    DBRowVector                 get_all(std::string key = "", int64_t from_time = -1, int64_t to_time = -1)
                                    { return _get_all(key, "", from_time, to_time); }
    DBRowVector                 get_all(std::string key, std::string remove_string_from_key, int64_t from_time = -1, int64_t to_time = -1)
                                    { return _get_all(key, remove_string_from_key, from_time, to_time); }
    sqlite3*                    handle()
                                    { return _sql; }
    bool                        is_busy() const
                                    { return err_code == 5; }
    bool                        is_open() const
                                    { return _sql != nullptr; }
    bool                        key(std::string key);
    DBRowVector                 keys(std::string key = "", int64_t from_time = -1, int64_t to_time = -1)
                                    { return _keys(key, "", from_time, to_time); }
    DBRowVector                 keys(std::string key, std::string remove, int64_t from_time = -1, int64_t to_time = -1)
                                    { return _keys(key, remove, from_time, to_time); }
    bool                        open(std::string filename);
    bool                        put(std::string key, const char* in, size_t in_size, int64_t time = -1);
    bool                        put(std::string key, std::string value, int64_t time = -1)
                                    { return put(key, value.c_str(), value.length(), time); }
    bool                        put(const DBRow& r)
                                    { return put(r.key, r.value, r.size, r.time); }
    bool                        remove(std::string key)
                                    { return _remove(key, false) == 1; }
    int                         remove_many(std::string key)
                                    { return _remove(key, true); }
    bool                        rename(std::string key, std::string new_key);
    bool                        rollback()
                                    { return execute("ROLLBACK TRANSACTION"); }
    int64_t                     rows()
                                    { return _count("rows"); }
    int64_t                     size()
                                    { return _count("size"); }
    static bool                 Defrag(std::string& err, std::string filename);
    static DBRow                Load(std::string& err, std::string filename, std::string key);
    static DBRowVector          LoadRows(std::string& err, std::string filename, std::string key);
    static bool                 Save(std::string& err, std::string filename, const DBRow& row);
    static bool                 Save(std::string& err, std::string filename, std::string key, const char* value, size_t size, int64_t time = -1);
    static bool                 SaveRows(std::string& err, std::string filename, const DBRowVector& rows);
    static std::string          Version();
private:
    bool                        _clear_error_and_free_stmt();
    int                         _clear_error_and_free_stmt_and_return_changes();
    int64_t                     _count(std::string what);
    bool                        _error_invalid_arguments();
    DBRowVector                 _get_all(std::string key, std::string remove_string_from_key, int64_t from_time = -1, int64_t to_time = -1);
    DBRowVector                 _keys(std::string key, std::string remove_string_from_key, int64_t from_time = -1, int64_t to_time = -1);
    bool                        _prepare(const char* sql);
    int                         _remove(std::string key, bool many);
    bool                        _set_error_and_close_db();
    bool                        _set_error_and_free_stmt();
    sqlite3*                    _sql;
    sqlite3_stmt*               _stmt;
    std::string                 _filename;
};
}
#endif
#ifdef GNU_USE_PCRE
#include <string>
#include <vector>
namespace gnu {
class PCRE {
public:
                                PCRE(const PCRE&) = delete;
    PCRE&                       operator=(const PCRE&) = delete;
                                PCRE();
                                PCRE(PCRE&& other);
    explicit                    PCRE(std::string pattern, bool utf = false);
                                ~PCRE();
    PCRE&                       operator=(PCRE&& other);
    void                        clear();
    bool                        compile(std::string pattern, bool utf = false);
    void                        debug();
    int                         end(size_t match)
                                    { auto r = (match >= _matches) ? -1 : _off[match * 2 + 1]; return (r < 0 && r > (int) _subject.length()) ? -1 : r;}
    std::string                 error()
                                    { return _error; }
    size_t                      exec(std::string subject, bool not_bol = false, bool not_eol = false);
    size_t                      exec_next();
    bool                        has_error() const
                                    { return _error.length() > 0; }
    bool                        is_compiled() const
                                    { return _pcre != nullptr; }
    bool                        has_utf() const
                                    { return _utf; }
    size_t                      matches() const
                                    { return _matches; }
    std::string                 pattern() const
                                    { return _pattern; }
    bool                        offset(size_t match, int& start, int& end);
    bool                        offset(std::string name, int& start, int& end);
    std::string                 replace(size_t match, std::string replace);
    std::string                 replace(std::vector<std::string> replace, std::string skip = "");
    int                         start(size_t match)
                                    { auto r = (match >= _matches) ? -1 : _off[match * 2]; return (r < 0 && r > (int) _subject.length()) ? -1 : r;}
    std::string                 substr(size_t match);
    std::string                 substr(std::string name);
    std::string                 to_string() const;
    static std::string          Escape(std::string string);
    static std::string          Version();
private:
    static const size_t         MAX_CAPTURES = 60;
    int                         _off[MAX_CAPTURES];
    int                         _utf;
    size_t                      _matches;
    std::string                 _error;
    std::string                 _pattern;
    std::string                 _subject;
    void*                       _pcre;
};
}
#endif
#include <cstdint>
#include <map>
#include <string>
#include <vector>
namespace gnu {
struct PileBuf {
    char*                       p;
    size_t                      s;
                                PileBuf()
                                    { p = nullptr; s = 0; }
    explicit                    PileBuf(size_t S);
                                PileBuf(const char* p, size_t s);
                                PileBuf(const PileBuf& b);
                                PileBuf(PileBuf&& b)
                                    { p = b.p; s = b.s; b.p = nullptr; }
    PileBuf&                    operator=(const PileBuf& b);
    PileBuf&                    operator=(PileBuf&& b)
                                    { free(p); p = b.p; s = b.s; b.p = nullptr; return *this; }
    PileBuf&                    operator+=(const PileBuf& b);
    bool                        operator==(const PileBuf& other) const;
    virtual                     ~PileBuf()
                                    { free(p); }
    static inline PileBuf       Grab(char* P, size_t S)
                                    { auto res = PileBuf(); res.p = P; res.s = S; return res; }
};
class Pile {
public:
    explicit                    Pile(char* values = nullptr)
                                    { _buf[0] = 0; import_data(values); }
    void                        clear()
                                    { _values.clear(); }
    void                        debug() const;
    std::string                 export_data() const;
    PileBuf                     get_buf(std::string section, std::string key) const;
    double                      get_double(std::string section, std::string key, double def = 0.0) const;
    int64_t                     get_int(std::string section, std::string key, int64_t def = 0) const;
    std::string                 get_string(std::string section, std::string key, std::string def = "") const;
    size_t                      import_data(char* values);
    std::string                 make_key(unsigned key, uint8_t w = 3);
    std::vector<std::string>    keys(std::string section) const;
    std::vector<std::string>    sections() const;
    bool                        set(std::string section, std::string key, const char* value, size_t len);
    bool                        set_buf(std::string section, std::string key, const PileBuf& buf)
                                    { return set(section, key, buf.p, buf.s); }
    bool                        set_double(std::string section, std::string key, double value)
                                    { snprintf(_buf, 200, "%f", value); return set_string(section, key, _buf); }
    bool                        set_int(std::string section, std::string key, int64_t value)
                                    { snprintf(_buf, 200, "%lld", (long long int) value); return set_string(section, key, _buf); }
    bool                        set_string(std::string section, std::string key, std::string value);
    size_t                      size() const
                                    { return _values.size(); }
private:
    std::map<std::string, std::string> _values;
    char                            _buf[200];
};
}
#include <cstdint>
#include <string>
#include <vector>
namespace gnu {
namespace str {
std::string                     format(const char* format, ...);
std::string                     format_int(int64_t num, char del = ' ');
inline std::string              grab_string(char* cstr)
                                    { std::string res = (cstr != nullptr) ? cstr : ""; free(cstr); return res;}
bool                            is_whitespace(const std::string& str);
size_t                          list_append(std::vector<std::string>& strings, std::string string, size_t max_size);
size_t                          list_insert(std::vector<std::string>& strings, std::string string, size_t max_size);
std::string&                    replace(std::string& str, std::string find, std::string replace = "", size_t max = 999'999'999);
std::string                     replace_const(const std::string& str, std::string find, std::string replace = "", size_t max = 999'999'999);
const char*                     reverse(char* str, size_t len);
inline std::string&             reverse(std::string& str)
                                    { reverse(const_cast<char*>(str.c_str()), str.length()); return str; }
std::vector<std::string>        split(const std::string& str, char del);
std::vector<const char*>        split_fast(char* cstr, char split);
std::vector<std::string>        split_std(const std::string& str, std::string split);
std::string                     substr(const std::string& in, std::string::size_type pos, std::string::size_type size = std::string::npos, std::string def = "");
double                          to_double(std::string str, double def = 0.0);
long long int                   to_int(std::string str, long long int def = 0);
inline std::string              to_string(const char* cstr)
                                    { return (cstr != nullptr) ? cstr : ""; }
std::string&                    trim(std::string& str);
size_t                          utf_len(const char* p);
}
}
#include <cstdint>
#include <string>
namespace gnu {
class Time {
public:
    explicit                    Time(bool start = false)
                                    { reset(); if (start == true) this->start(); }
    unsigned                    count()
                                    { return _count; }
    void                        reset()
                                    { _micro = 0; _count = 0; _paused = true; }
    void                        start()
                                    { if (_paused == true) { _paused = false; _micro = Time::Micro() - _micro; } }
    void                        stop()
                                    { if (_paused == false) { _paused = true; _count++; _micro = Time::Micro() - _micro; } }
    int64_t                     micro()
                                    { return _micro; }
    int64_t                     milli()
                                    { return _micro / 1'000; }
    double                      sec()
                                    { return (double) _micro / 1'000'000.0; }
    double                      split_micro()
                                    { return (_count > 0) ? (double) _micro / (double) _count : (double) _micro; }
    double                      split_milli()
                                    { return split_micro() / 1'000.0; }
    double                      split_sec()
                                    { return (double) split_micro() / 1'000'000.0; }
    static double               Clock();
    static std::string          FormatMilliToTime(int64_t milliseconds);
    static std::string          FormatUnixToISO(int64_t seconds, bool utc = false, bool date_only = false);
    static int64_t              Micro();
    static inline int64_t       Milli()
                                    { return Time::Micro() / 1'000;  }
    static inline double        Sec()
                                    { return (double) Time::Micro() / 1'000'000.0;  }
    static void                 SleepMilli(unsigned milliseconds);
private:
    int64_t                     _micro;
    unsigned                    _count;
    bool                        _paused;
};
}
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
namespace gnu {
class File;
typedef bool (*CallbackCopy)(int64_t size, int64_t copied, void* data);
typedef std::vector<File> FileVector;
struct FileBuf {
    char*                       p;
    size_t                      s;
                                FileBuf()
                                    { p = nullptr; s = 0; }
    explicit                    FileBuf(size_t S);
                                FileBuf(const char* P, size_t S);
                                FileBuf(const FileBuf& b);
                                FileBuf(FileBuf&& b)
                                    { p = b.p; s = b.s; b.p = nullptr; }
                                FileBuf(const std::string& S)
                                    { p = nullptr; add(S.c_str(), S.length()); }
    virtual                     ~FileBuf()
                                    { free(p); }
    FileBuf&                    operator=(const FileBuf& b)
                                    { return set(b.p, b.s); }
    FileBuf&                    operator=(FileBuf&& b)
                                    { free(p); p = b.p; s = b.s; b.p = nullptr; return *this; }
    FileBuf&                    operator=(const std::string& S)
                                    { free(p); p = nullptr; add(S.c_str(), S.length()); return *this; }
    FileBuf&                    operator+=(const FileBuf& b)
                                    { return add(b.p, b.s); }
    bool                        operator==(const FileBuf& other) const;
    bool                        operator!=(const FileBuf& other) const
                                    { return (*this == other) == false; }
    FileBuf&                    add(const char* P, size_t S);
    void                        clear()
                                    { free(p); p = nullptr; s = 0; }
    void                        count(size_t count[257]) const
                                    { FileBuf::Count(p, s, count); }
    void                        debug() const
                                    { printf("gnu::FileBuf(0x%p, %u)\n", p, (unsigned) s); }
    uint64_t                    fletcher64() const
                                    { return FileBuf::Fletcher64(p, s); }
    FileBuf&                    grab(char* P, size_t S)
                                    { free(p); p = P; s = S; return *this; }
    FileBuf                     insert_cr(bool dos = true, bool trailing = false) const
                                    { return FileBuf::InsertCR(p, s, dos, trailing); }
    char*                       release()
                                    { auto res = p; p = nullptr; s = 0; return res; }
    FileBuf                     remove_cr() const
                                    { return FileBuf::RemoveCR(p, s); }
    FileBuf&                    set(const char* P, size_t S);
    bool                        write(std::string filename) const;
    static void                 Count(const char* P, size_t S, size_t count[257]);
    static uint64_t             Fletcher64(const char* p, size_t s);
    static inline FileBuf       Grab(char* P)
                                    { auto res = FileBuf(); res.p = P; res.s = strlen(P); return res; }
    static inline FileBuf       Grab(char* P, size_t S)
                                    { auto res = FileBuf(); res.p = P; res.s = S; return res; }
    static FileBuf              InsertCR(const char* P, size_t S, bool dos, bool trailing = false);
    static FileBuf              RemoveCR(const char* P, size_t S);
};
class File {
public:
#ifdef _WIN32
    static const int            DEFAULT_DIR_MODE  = 0x00000080;
    static const int            DEFAULT_FILE_MODE = 0x00000080;
#else
    static const int            DEFAULT_DIR_MODE  = 0755;
    static const int            DEFAULT_FILE_MODE = 0664;
#endif
    enum class TYPE {
                                MISSING,
                                DIR,
                                FILE,
                                OTHER,
    };
    File::TYPE                  type;
    bool                        link;
    int                         mode;
    int64_t                     ctime;
    int64_t                     mtime;
    int64_t                     size;
    std::string                 ext;
    std::string                 filename;
    std::string                 name;
    std::string                 path;
                                File()
                                    { update(""); }
    explicit                    File(std::string in, bool realpath = false)
                                    { update(in, realpath); }
    File&                       operator=(std::string in)
                                    { return update(in); }
    File&                       operator+=(std::string in)
                                    { filename += in; return *this; }
    bool                        operator==(const File& other) const
                                    { return filename == other.filename; }
    bool                        operator<(const File& other) const
                                    { return filename < other.filename; }
    bool                        operator<=(const File& other) const
                                    { return filename <= other.filename; }
    const char*                 c_str() const
                                    { return filename.c_str(); }
    std::string                 canonicalname() const
                                    { return File::CanonicalName(filename); }
    void                        debug(bool short_version = true) const
                                    { printf("%s\n", to_string(short_version).c_str()); fflush(stdout); }
    bool                        is_circular() const;
    bool                        is_dir() const
                                    { return type == TYPE::DIR; }
    bool                        is_file() const
                                    { return type == TYPE::FILE; }
    bool                        is_link() const
                                    { return link; }
    bool                        is_missing() const
                                    { return type == TYPE::MISSING; }
    bool                        is_other() const
                                    { return type == TYPE::OTHER; }
    std::string                 linkname() const;
    std::string                 name_without_ext() const;
    File&                       update();
    File&                       update(std::string in, bool realpath = false);
    std::string                 type_name() const;
    std::string                 to_string(bool short_version = true) const;
    static char*                Allocate(char* resize_or_null, size_t size, bool exception = true);
    static std::string          CanonicalName(std::string filename);
    static bool                 ChDir(std::string path);
    static std::string          CheckFilename(std::string filename);
    static bool                 ChMod(std::string path, int mode);
    static FileBuf              CloseStderr();
    static FileBuf              CloseStdout();
    static bool                 Copy(std::string from, std::string to, CallbackCopy callback = nullptr, void* data = nullptr);
    static File                 HomeDir();
    static bool                 MkDir(std::string path);
    static bool                 ModTime(std::string path, int64_t time);
    static FILE*                Open(std::string path, std::string mode);
    static std::string          OS();
    static FILE*                Popen(std::string cmd, bool write = false);
    static FileBuf              Read(std::string path);
    static FileBuf*             Read2(std::string path);
    static FileVector           ReadDir(std::string path);
    static FileVector           ReadDirRec(std::string path);
    static bool                 RedirectStderr();
    static bool                 RedirectStdout();
    static bool                 Remove(std::string path);
    static bool                 RemoveRec(std::string path);
    static bool                 Rename(std::string from, std::string to);
    static int                  Run(std::string cmd, bool background, bool hide_win32_window = false);
    static File                 TmpDir();
    static File                 TmpFile(std::string prepend = "");
    static File                 WorkDir();
    static bool                 Write(std::string filename, const char* in, size_t in_size);
    static inline bool          Write(std::string filename, const FileBuf& b)
                                    { return Write(filename, b.p, b.s); }
    };
}
#include <string>
#include <vector>
#include <cmath>
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_PostScript.H>
#ifdef DEBUG
#include <iostream>
#include <iomanip>
#define FLW_LINE                        { ::printf("\033[31m%6u: \033[34m%s::%s\033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_RED                         { ::printf("\033[7m\033[31m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_GREEN                       { ::printf("\033[7m\033[32m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_BLUE                        { ::printf("\033[7m\033[34m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_PRINT(...)                  FLW_PRINT_MACRO(__VA_ARGS__, FLW_PRINT7, FLW_PRINT6, FLW_PRINT5, FLW_PRINT4, FLW_PRINT3, FLW_PRINT2, FLW_PRINT1)(__VA_ARGS__);
#define FLW_PRINT1(A)                   { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << std::endl; fflush(stdout); }
#define FLW_PRINT2(A,B)                 { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINT3(A,B,C)               { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << "" << std::endl; fflush(stdout); }
#define FLW_PRINT4(A,B,C,D)             { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << ",  \033[32m4=" << (D) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINT5(A,B,C,D,E)           { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << ",  \033[32m4=" << (D) << "\033[0m,  5=" << (E) << std::endl; fflush(stdout); }
#define FLW_PRINT6(A,B,C,D,E,F)         { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << ",  \033[32m4=" << (D) << "\033[0m,  5=" << (E) << ",  \033[32m6=" << (F) << "\033[0m " << std::endl; fflush(stdout); }
#define FLW_PRINT7(A,B,C,D,E,F,G)       { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << ",  \033[32m4=" << (D) << "\033[0m,  5=" << (E) << ",  \033[32m6=" << (F) << "\033[0m,  7=" << (G) << std::endl; fflush(stdout); }
#define FLW_PRINT_MACRO(A,B,C,D,E,F,G,N,...) N
#define FLW_PRINTV(...)                 FLW_PRINTV_MACRO(__VA_ARGS__, FLW_PRINTV7, FLW_PRINTV6, FLW_PRINTV5, FLW_PRINTV4, FLW_PRINTV3, FLW_PRINTV2, FLW_PRINTV1)(__VA_ARGS__);
#define FLW_PRINTV1(A)                  { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << "" << std::endl; fflush(stdout); }
#define FLW_PRINTV2(A,B)                { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINTV3(A,B,C)              { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << "" << std::endl; fflush(stdout); }
#define FLW_PRINTV4(A,B,C,D)            { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << ",  \033[32m" #D "=" << (D) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINTV5(A,B,C,D,E)          { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << ",  \033[32m" #D "=" << (D) << "\033[0m,  " #E "=" << (E) << "" << std::endl; fflush(stdout); }
#define FLW_PRINTV6(A,B,C,D,E,F)        { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << ",  \033[32m" #D "=" << (D) << "\033[0m,  " #E "=" << (E) << ",  \033[32m" #F "=" << (F) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINTV7(A,B,C,D,E,F,G)      { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << ",  \033[32m" #D "=" << (D) << "\033[0m,  " #E "=" << (E) << ",  \033[32m" #F "=" << (F) << "\033[0m,  " #G "=" << (G) << "" << std::endl; fflush(stdout); }
#define FLW_PRINTV_MACRO(A,B,C,D,E,F,G,N,...) N
#define FLW_PRINTD(...)                 FLW_PRINTD_MACRO(__VA_ARGS__, FLW_PRINTD4, FLW_PRINTD3, FLW_PRINTD2, FLW_PRINTD1)(__VA_ARGS__);
#define FLW_PRINTD1(A)                  { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %.10f\n", __LINE__, __func__, #A, static_cast<double>(A)); fflush(stdout); }
#define FLW_PRINTD2(A,B)                { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %.10f,  \033[32m%s = %.10f\033[0m\n", __LINE__, __func__, #A, static_cast<double>(A), #B, static_cast<double>(B)); fflush(stdout); }
#define FLW_PRINTD3(A,B,C)              { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %.10f,  \033[32m%s = %.10f\033[0m,  %s = %.10f\n", __LINE__, __func__, #A, static_cast<double>(A), #B, static_cast<double>(B), #C, static_cast<double>(C)); fflush(stdout); }
#define FLW_PRINTD4(A,B,C,D)            { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %.10f,  \033[32m%s = %.10f\033[0m,  %s = %.10f, \033[32m %s = %.10f\033[0m\n", __LINE__, __func__, #A, static_cast<double>(A), #B, static_cast<double>(B), #C, static_cast<double>(C), #D, static_cast<double>(D)); fflush(stdout); }
#define FLW_PRINTD_MACRO(A,B,C,D,N,...) N
#define FLW_PRINTDS(...)                FLW_PRINTDS_MACRO(__VA_ARGS__, FLW_PRINTDS4, FLW_PRINTDS3, FLW_PRINTDS2, FLW_PRINTDS1)(__VA_ARGS__);
#define FLW_PRINTDS1(A)                 { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %s\n", __LINE__, __func__, #A, flw::util::format_double(static_cast<double>(A), 0, '\'').c_str()); fflush(stdout); }
#define FLW_PRINTDS2(A,B)               { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %s,  \033[32m%s = %s\033[0m\n", __LINE__, __func__, #A, flw::util::format_double(static_cast<double>(A), 0, '\'').c_str(), #B, flw::util::format_double(static_cast<double>(B), 0, '\'').c_str()); fflush(stdout); }
#define FLW_PRINTDS3(A,B,C)             { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %s,  \033[32m%s = %s\033[0m,  %s = %s\n", __LINE__, __func__, #A, flw::util::format_double(static_cast<double>(A), 0, '\'').c_str(), #B, flw::util::format_double(static_cast<double>(B), 0, '\'').c_str(), #C, flw::util::format_double(static_cast<double>(C), 0, '\'').c_str()); fflush(stdout); }
#define FLW_PRINTDS4(A,B,C,D)           { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %s,  \033[32m%s = %s\033[0m,  %s = %s,  \033[32m%s = %s\033[0m\n", __LINE__, __func__, #A, flw::util::format_double(static_cast<double>(A), 0, '\'').c_str(), #B, flw::util::format_double(static_cast<double>(B), 0, '\'').c_str(), #C, flw::util::format_double(static_cast<double>(C), 0, '\'').c_str(), #D, flw::util::format_double(static_cast<double>(D), 0, '\'').c_str()); fflush(stdout); }
#define FLW_PRINTDS_MACRO(A,B,C,D,N,...) N
#define FLW_NL                          { ::printf("\n"); fflush(stdout); }
#define FLW_ASSERT(X,Y)                 flw::debug::test(X,Y,__LINE__,__func__);
#define FLW_TRUE(X)                     flw::debug::test(X,__LINE__,__func__);
#define FLW_ASSERTD(X,Y,Z)              flw::debug::test(X,Y,Z,__LINE__,__func__);
#else
#define FLW_LINE
#define FLW_RED
#define FLW_GREEN
#define FLW_BLUE
#define FLW_PRINT(...)
#define FLW_PRINTV(...)
#define FLW_PRINTD(...)
#define FLW_PRINTDS(...)
#define FLW_NL
#define FLW_ASSERT(X,Y)
#define FLW_TRUE(X)
#define FLW_ASSERTD(X,Y,Z)
#endif
namespace flw {
extern int                      PREF_FIXED_FONT;
extern std::string              PREF_FIXED_FONTNAME;
extern int                      PREF_FIXED_FONTSIZE;
extern Fl_Font                  PREF_FONT;
extern int                      PREF_FONTSIZE;
extern std::string              PREF_FONTNAME;
extern std::vector<char*>       PREF_FONTNAMES;
extern std::string              PREF_THEME;
extern const char* const        PREF_THEMES[];
typedef std::vector<std::string> StringVector;
typedef std::vector<void*>       VoidVector;
typedef std::vector<Fl_Widget*>  WidgetVector;
typedef bool (*PrintCallback)(void* data, int pw, int ph, int page);
namespace debug {
    void                        print(const Fl_Widget* widget);
    void                        print(const Fl_Widget* widget, std::string& indent);
    bool                        test(bool val, int line, const char* func);
    bool                        test(const char* ref, const char* val, int line, const char* func);
    bool                        test(int64_t ref, int64_t val, int line, const char* func);
    bool                        test(double ref, double val, double diff, int line, const char* func);
}
namespace menu {
    void                        enable_item(Fl_Menu_* menu, const char* text, bool value);
    Fl_Menu_Item*               get_item(Fl_Menu_* menu, const char* text);
    Fl_Menu_Item*               get_item(Fl_Menu_* menu, void* v);
    bool                        item_value(Fl_Menu_* menu, const char* text);
    void                        set_item(Fl_Menu_* menu, const char* text, bool value);
    void                        setonly_item(Fl_Menu_* menu, const char* text);
}
namespace util {
    void                        center_window(Fl_Window* window, Fl_Window* parent = nullptr);
    double                      clock();
    int                         count_decimals(double number);
    Fl_Widget*                  find_widget(Fl_Group* group, std::string label);
    std::string                 fix_menu_string(std::string in);
    std::string                 format(const char* format, ...);
    std::string                 format_double(double num, int decimals = 0, char del = ' ');
    std::string                 format_int(int64_t num, char del = ' ');
    bool                        is_whitespace_or_empty(const char* str);
    void                        labelfont(Fl_Widget* widget, Fl_Font fn = flw::PREF_FONT, int fs = flw::PREF_FONTSIZE);
    int64_t                     microseconds();
    int32_t                     milliseconds();
    bool                        png_save(std::string opt_name, Fl_Window* window, int X = 0, int Y = 0, int W = 0, int H = 0);
    std::string                 print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data);
    std::string                 print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data, int from, int to);
    std::string                 remove_browser_format(const char* text);
    std::string&                replace_string(std::string& string, std::string find, std::string replace);
    void                        sleep(int milli);
    StringVector                split_string(const std::string& string, std::string split);
    std::string                 substr(std::string in, std::string::size_type pos, std::string::size_type size = std::string::npos);
    double                      to_double(std::string s, double def = INFINITY);
    long long                   to_long(std::string s, long long def = 0);
    static inline std::string   to_string(const char* text)
                                    { return text != nullptr ? text : ""; }
    void*                       zero_memory(char* mem, size_t size);
}
namespace theme {
    bool                        is_dark();
    bool                        load(std::string name);
    int                         load_font(std::string requested_font);
    void                        load_fonts(bool iso8859_only = true);
    void                        load_icon(Fl_Window* win, int win_resource, const char** xpm_resource = nullptr, const char* name = nullptr);
    void                        load_theme_pref(Fl_Preferences& pref);
    void                        load_win_pref(Fl_Preferences& pref, Fl_Window* window, int show_0_1_2 = 1, int defw = 800, int defh = 600, std::string basename = "gui.");
    bool                        parse(int argc, const char** argv);
    void                        save_theme_pref(Fl_Preferences& pref);
    void                        save_win_pref(Fl_Preferences& pref, Fl_Window* window, std::string basename = "gui.");
    enum {
                                THEME_DEFAULT,
                                THEME_GLEAM,
                                THEME_GLEAM_BLUE,
                                THEME_GLEAM_DARK,
                                THEME_GLEAM_TAN,
                                THEME_GTK,
                                THEME_GTK_BLUE,
                                THEME_GTK_DARK,
                                THEME_GTK_TAN,
                                THEME_OXY,
                                THEME_OXY_TAN,
                                THEME_PLASTIC,
                                THEME_PLASTIC_TAN,
                                THEME_NIL,
    };
}
namespace color {
    extern Fl_Color             BEIGE;
    extern Fl_Color             CHOCOLATE;
    extern Fl_Color             CRIMSON;
    extern Fl_Color             DARKOLIVEGREEN;
    extern Fl_Color             DODGERBLUE;
    extern Fl_Color             FORESTGREEN;
    extern Fl_Color             GOLD;
    extern Fl_Color             GRAY;
    extern Fl_Color             INDIGO;
    extern Fl_Color             OLIVE;
    extern Fl_Color             PINK;
    extern Fl_Color             ROYALBLUE;
    extern Fl_Color             SIENNA;
    extern Fl_Color             SILVER;
    extern Fl_Color             SLATEGRAY;
    extern Fl_Color             TEAL;
    extern Fl_Color             TURQUOISE;
    extern Fl_Color             VIOLET;
}
class PrintText {
public:
                                PrintText(std::string filename,
                                    Fl_Paged_Device::Page_Format format = Fl_Paged_Device::Page_Format::A4,
                                    Fl_Paged_Device::Page_Layout layout = Fl_Paged_Device::Page_Layout::PORTRAIT,
                                    Fl_Font font = FL_COURIER,
                                    Fl_Fontsize fontsize = 14,
                                    Fl_Align align = FL_ALIGN_LEFT,
                                    bool wrap = true,
                                    bool border = false,
                                    int line_num = 0);
                                ~PrintText();
    Fl_Fontsize                 fontsize() const
                                    { return _fontsize; }
    int                         page_count() const
                                    { return _page_count; }
    std::string                 print(const char* text, unsigned replace_tab_with_space = 0);
    std::string                 print(const std::string& text, unsigned replace_tab_with_space = 0);
    std::string                 print(const StringVector& lines, unsigned replace_tab_with_space = 0);
private:
    void                        check_for_new_page();
    void                        measure_lw_lh(const std::string& text);
    void                        print_line(const std::string& line);
    void                        print_wrapped_line(const std::string& line);
    std::string                 start();
    std::string                 stop();
    Fl_Align                    _align;
    Fl_Font                     _font;
    Fl_Fontsize                 _fontsize;
    Fl_PostScript_File_Device*  _printer;
    Fl_Paged_Device::Page_Format _page_format;
    Fl_Paged_Device::Page_Layout _page_layout;
    FILE*                       _file;
    bool                        _border;
    bool                        _wrap;
    int                         _lh;
    int                         _line_count;
    int                         _line_num;
    int                         _lw;
    int                         _nw;
    int                         _page_count;
    int                         _ph;
    int                         _pw;
    int                         _px;
    int                         _py;
    std::string                 _filename;
};
}
#include <FL/Fl_Group.H>
namespace flw {
class GridGroup : public Fl_Group {
public:
    explicit                    GridGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~GridGroup();
    void                        add(Fl_Widget* widget, int X, int Y, int W, int H);
    void                        adjust(Fl_Widget* widget, int L = 0, int R = 0, int T = 0, int B = 0);
    void                        clear();
    void                        do_layout()
                                    { resize(x(), y(), w(), h()); Fl::redraw(); }
    int                         handle(int event) override;
    Fl_Widget*                  remove(Fl_Widget* widget);
    void                        resize(int X, int Y, int W, int H) override;
    void                        resize(Fl_Widget* widget, int X, int Y, int W, int H);
    int                         size() const
                                    { return _size; }
    void                        size(int size)
                                    { _size = (size >= 4 && size <= 72) ? size : 0; }
private:
    void                        _last_active_widget(Fl_Widget** first, Fl_Widget** last);
    VoidVector                  _widgets;
    int                         _size;
};
}
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Menu_Button.H>
namespace flw {
class ScrollBrowser : public Fl_Hold_Browser {
public:
                                ScrollBrowser(const ScrollBrowser&) = delete;
                                ScrollBrowser(ScrollBrowser&&) = delete;
    ScrollBrowser&              operator=(const ScrollBrowser&) = delete;
    ScrollBrowser&              operator=(ScrollBrowser&&) = delete;
    explicit                    ScrollBrowser(int scroll = 9, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    bool                        enable_menu() const
                                    { return _flag_menu; }
    void                        enable_menu(bool menu)
                                    { _flag_menu = menu; }
    bool                        enable_pagemove() const
                                    { return _flag_move; }
    void                        enable_pagemove(bool move)
                                    { _flag_move = move; }
    int                         handle(int event) override;
    Fl_Menu_Button*             menu()
                                    { return _menu; }
    std::string                 text2() const
                                    { return util::remove_browser_format(text(value())); }
    std::string                 text2(int line) const
                                    { return util::remove_browser_format(text(line)); }
    void                        update_pref()
                                    { update_pref(flw::PREF_FONT, flw::PREF_FONTSIZE); }
    void                        update_pref(Fl_Font text_font, Fl_Fontsize text_size);
    static void                 Callback(Fl_Widget*, void*);
private:
    Fl_Menu_Button*             _menu;
    bool                        _flag_menu;
    bool                        _flag_move;
    int                         _scroll;
};
}
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Hor_Fill_Slider.H>
#include <FL/Fl_Toggle_Button.H>
namespace flw {
namespace dlg {
extern const char*              PASSWORD_CANCEL;
extern const char*              PASSWORD_OK;
void                            center_message_dialog();
StringVector                    check(std::string title, const StringVector& list, Fl_Window* parent = nullptr);
int                             choice(std::string title, const StringVector& list, int selected = 0, Fl_Window* parent = nullptr);
bool                            font(Fl_Font& font, Fl_Fontsize& fontsize, std::string& fontname, bool limit_to_default = false);
void                            html(std::string title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            list(std::string title, const StringVector& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
void                            list(std::string title, const std::string& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
void                            list_file(std::string title, std::string file, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
void                            panic(std::string message);
bool                            password1(std::string title, std::string& password, Fl_Window* parent = nullptr);
bool                            password2(std::string title, std::string& password, Fl_Window* parent = nullptr);
bool                            password3(std::string title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
bool                            password4(std::string title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
void                            print(std::string title, PrintCallback cb, void* data = nullptr, int from = 1, int to = 0, Fl_Window* parent = nullptr);
bool                            print_text(std::string title, const std::string& text, Fl_Window* parent = nullptr);
bool                            print_text(std::string title, const StringVector& text, Fl_Window* parent = nullptr);
int                             select(std::string title, const StringVector& list, int select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
int                             select(std::string title, const StringVector& list, const std::string& select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
bool                            slider(std::string title, double min, double max, double& value, double step = 1.0, Fl_Window* parent = nullptr);
void                            text(std::string title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
bool                            text_edit(std::string title, std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            theme(bool enable_font = false, bool enable_fixedfont = false, Fl_Window* parent = nullptr);
class AbortDialog : public Fl_Double_Window {
    using Fl_Double_Window::show;
public:
                                AbortDialog(const AbortDialog&) = delete;
                                AbortDialog(AbortDialog&&) = delete;
    AbortDialog&                operator=(const AbortDialog&) = delete;
    AbortDialog&                operator=(AbortDialog&&) = delete;
    explicit                    AbortDialog(std::string label = "", double min = 0.0, double max = 0.0);
    bool                        check(int milliseconds = 200);
    bool                        check(double value, double min, double max, int milliseconds = 200);
    bool                        aborted()
                                    { return _abort; }
    void                        range(double min, double max);
    void                        resize(int X, int Y, int W, int H) override
                                    { Fl_Double_Window::resize(X, Y, W, H); _grid->resize(0, 0, W, H); }
    void                        show(const std::string& label, Fl_Window* parent = nullptr);
    void                        value(double value);
    static void                 Callback(Fl_Widget* w, void* o);
private:
    Fl_Button*                  _button;
    Fl_Hor_Fill_Slider*         _progress;
    GridGroup*                  _grid;
    bool                        _abort;
    int64_t                     _last;
};
class FontDialog : public Fl_Double_Window {
public:
                                FontDialog(const FontDialog&) = delete;
                                FontDialog(FontDialog&&) = delete;
    FontDialog&                 operator=(const FontDialog&) = delete;
    FontDialog&                 operator=(FontDialog&&) = delete;
                                FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& label, bool limit_to_default = false);
                                FontDialog(std::string font, Fl_Fontsize fontsize, std::string label, bool limit_to_default = false);
    void                        activate_font()
                                    { static_cast<Fl_Widget*>(_fonts)->activate(); }
    void                        deactivate_font()
                                    { static_cast<Fl_Widget*>(_fonts)->deactivate(); }
    void                        deactivate_fontsize()
                                    { static_cast<Fl_Widget*>(_sizes)->deactivate(); }
    int                         font()
                                    { return _font; }
    std::string                 fontname()
                                    { return _fontname; }
    int                         fontsize()
                                    { return _fontsize; }
    void                        resize(int X, int Y, int W, int H) override
                                    { Fl_Double_Window::resize(X, Y, W, H); _grid->resize(0, 0, W, H); }
    bool                        run(Fl_Window* parent = nullptr);
    static void                 Callback(Fl_Widget* w, void* o);
private:
    void                        _activate();
    void                        _create(Fl_Font font, std::string fontname, Fl_Fontsize fontsize, std::string label, bool limit_to_default);
    void                        _select_name(std::string font_name);
    Fl_Box*                     _label;
    Fl_Button*                  _cancel;
    Fl_Button*                  _select;
    GridGroup*                  _grid;
    ScrollBrowser*              _fonts;
    ScrollBrowser*              _sizes;
    bool                        _ret;
    int                         _font;
    int                         _fontsize;
    std::string                 _fontname;
};
class WorkDialog : public Fl_Double_Window {
public:
                                WorkDialog(const char* title, Fl_Window* parent, bool cancel, bool pause, int W = 40, int H = 10);
    void                        resize(int X, int Y, int W, int H) override
                                    { Fl_Double_Window::resize(X, Y, W, H); _grid->resize(0, 0, W, H); }
    bool                        run(double update_time, const StringVector& messages);
    bool                        run(double update_time, const std::string& message);
    static void                 Callback(Fl_Widget* w, void* o);
private:
    Fl_Button*                  _cancel;
    Fl_Hold_Browser*            _label;
    Fl_Toggle_Button*           _pause;
    GridGroup*                  _grid;
    bool                        _ret;
    double                      _last;
    std::string                 _message;
};
}
}
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Button.H>
namespace flw {
class _InputMenu;
class InputMenu : public Fl_Group {
public:
                                InputMenu(const InputMenu&) = delete;
                                InputMenu(InputMenu&&) = delete;
    InputMenu&                  operator=(const InputMenu&) = delete;
    InputMenu&                  operator=(InputMenu&&) = delete;
    explicit                    InputMenu(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        clear();
    bool                        enable_menu() const
                                    { return _menu->visible() != 0; }
    void                        enable_menu(bool value)
                                    { if (value == true) _menu->show(); else _menu->hide(); resize(x(), y(), w(), h()); }
    StringVector                get_history() const;
    Fl_Input*                   input()
                                    { return reinterpret_cast<Fl_Input*>(_input); }
    void                        insert(std::string string, int max_list_len);
    Fl_Menu_Button*             menu()
                                    { return _menu; }
    void                        resize(int X, int Y, int W, int H) override;
    void                        update_pref(Fl_Font text_font = flw::PREF_FONT, Fl_Fontsize text_size = flw::PREF_FONTSIZE);
    const char*                 value() const;
    void                        value(const char* string);
    void                        values(const StringVector& list, bool copy_first_to_input = true);
    static void                 Callback(Fl_Widget*, void*);
private:
    void                        _add(bool insert, const std::string& string, int max_list_len);
    void                        _add(bool insert, const StringVector& list);
    _InputMenu*                 _input;
    Fl_Menu_Button*             _menu;
};
}
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Preferences.H>
namespace flw {
    class RecentMenu {
    public:
                                        RecentMenu(Fl_Menu_* menu, Fl_Callback* file_callback, void* userdata, std::string base_label = "&File/Open recent", std::string clear_label = "/Clear");
        void                            append(std::string file)
                                            { return _add(file, true); }
        static void                     CallbackClear(Fl_Widget*, void* o);
        void                            insert(std::string file)
                                            { return _add(file, false); }
        StringVector                    items() const
                                            { return _files; }
        void                            max_items(size_t max)
                                            { if (max > 0 && max <= 100) _max = max; }
        Fl_Menu_*                       menu()
                                            { return _menu; }
        void                            load_pref(Fl_Preferences& pref, std::string base_name = "files");
        void                            save_pref(Fl_Preferences& pref, std::string base_name = "files");
    private:
        void                            _add(std::string file, bool append);
        size_t                          _add_string(StringVector& in, size_t max_size, std::string string);
        size_t                          _insert_string(StringVector& in, size_t max_size, std::string string);
        std::string                     _base;
        Fl_Callback*                    _callback;
        std::string                     _clear;
        StringVector                    _files;
        size_t                          _max;
        Fl_Menu_*                       _menu;
        void*                           _user;
    };
}
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
namespace flw {
    class SplitGroup : public Fl_Group {
    public:
        enum class CHILD {
                                FIRST,
                                SECOND,
        };
        enum class DIRECTION {
                                HORIZONTAL,
                                VERTICAL,
        };
        explicit                SplitGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                    add(Fl_Widget* widget, SplitGroup::CHILD child);
        Fl_Widget*              child(SplitGroup::CHILD child)
                                    { return (child == SplitGroup::CHILD::FIRST) ? _widgets[0] : _widgets[1]; }
        void                    clear();
        DIRECTION               direction() const
                                    { return _direction; }
        void                    direction(SplitGroup::DIRECTION direction);
        void                    do_layout()
                                    { SplitGroup::resize(x(), y(), w(), h()); Fl::redraw(); }
        int                     handle(int event) override;
        int                     min_pos() const
                                    { return _min; }
        void                    min_pos(int value)
                                    { _min = value; }
        void                    resize(int X, int Y, int W, int H) override;
        int                     split_pos() const
                                    { return _split_pos; }
        void                    split_pos(int split_pos)
                                    { _split_pos = split_pos; }
        void                    swap()
                                    { auto tmp = _widgets[0]; _widgets[0] = _widgets[1]; _widgets[1] = tmp; do_layout(); }
        void                    toggle(SplitGroup::CHILD child, SplitGroup::DIRECTION direction, int second_size = -1);
        void                    toggle(SplitGroup::CHILD child, int second_size = -1)
                                    { toggle(child, _direction, second_size); }
    private:
        DIRECTION               _direction;
        Fl_Widget*              _widgets[2];
        bool                    _drag;
        int                     _min;
        int                     _split_pos;
    };
}
#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>
namespace flw {
class TabsGroup : public Fl_Group {
public:
    static const int            DEFAULT_SPACE = 2;
    enum class TABS {
                                NORTH,
                                SOUTH,
                                WEST,
                                EAST,
    };
    explicit                    TabsGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        add(std::string label, Fl_Widget* widget, const Fl_Widget* after =  nullptr);
    void                        border(int n = 0, int s = 0, int w = 0, int e = 0)
                                    { _n = n; _s = s; _w = w; _e = e; do_layout(); }
    Fl_Widget*                  child(int num) const;
    int                         children() const
                                    { return (int) _widgets.size(); }
    void                        clear();
    void                        debug() const;
    void                        do_layout()
                                    { TabsGroup::resize(x(), y(), w(), h()); Fl::redraw(); }
    void                        draw() override;
    int                         find(const Fl_Widget* widget) const;
    int                         handle(int event) override;
    void                        hide_tabs();
    void                        insert(std::string label, Fl_Widget* widget, const Fl_Widget* before = nullptr);
    bool                        is_tabs_visible() const
                                    { return _scroll->visible(); }
    std::string                 label(Fl_Widget* widget);
    void                        label(std::string label, Fl_Widget* widget);
    Fl_Widget*                  remove(int num);
    Fl_Widget*                  remove(Fl_Widget* widget)
                                    { return TabsGroup::remove(find(widget)); }
    void                        resize(int X, int Y, int W, int H) override;
    void                        show_tabs();
    void                        sort(bool ascending = true, bool casecompare = false);
    void                        swap(int from, int to);
    TABS                        tabs() const
                                    { return _tabs; }
    void                        tabs(TABS value, int space_max_20 = TabsGroup::DEFAULT_SPACE);
    void                        update_pref(unsigned characters = 10, Fl_Font font = flw::PREF_FONT, Fl_Fontsize fontsize = flw::PREF_FONTSIZE);
    Fl_Widget*                  value() const;
    void                        value(int num);
    void                        value(Fl_Widget* widget)
                                    { value(find(widget)); }
    static void                 Callback(Fl_Widget* sender, void* object);
    static const char*          Help();
private:
    Fl_Widget*                  _active_button();
    void                        _resize_east_west(int X, int Y, int W, int H);
    void                        _resize_north_south(int X, int Y, int W, int H);
    void                        _resize_widgets();
    Fl_Pack*                    _pack;
    Fl_Rect                     _area;
    Fl_Scroll*                  _scroll;
    TABS                        _tabs;
    WidgetVector                _widgets;
    bool                        _drag;
    int                         _active;
    int                         _e;
    int                         _n;
    int                         _pos;
    int                         _s;
    int                         _space;
    int                         _w;
};
}
namespace flw {
    class WaitCursor {
        static WaitCursor*              WAITCURSOR;
    public:
                                        WaitCursor(const WaitCursor&) = delete;
                                        WaitCursor(WaitCursor&&) = delete;
                                        WaitCursor& operator=(const WaitCursor&) = delete;
                                        WaitCursor& operator=(WaitCursor&&) = delete;
                                        WaitCursor();
                                        ~WaitCursor();
    };
}
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <inttypes.h>
class Fl_Menu_Button;
class Fl_Preferences;
class Fl_Window;
namespace flw {
class ScrollBrowser;
class SplitGroup;
}
namespace fle {
class Editor;
class FindReplace;
class Message;
class TextBuffer;
class View;
typedef std::vector<int>                        IntVector;
typedef std::set<std::string>                   StringSet;
typedef std::unordered_map<std::string, int>    StringIntHash;
typedef std::unordered_set<std::string>         StringHash;
typedef std::vector<Message*>                   MessageVector;
enum class FBINFILE {
    NO,
    TEXT,
    HEX,
};
enum class FCASE {
    LOWER,
    UPPER,
};
enum class FCASECOMPARE {
    NO,
    YES,
};
enum class FCOPY {
    COPY_LINE,
    CUT_LINE,
};
enum class FDELKEY {
    NIL,
    BACKSPACE,
    DEL,
};
enum class FDELTEXT {
    WORD,
    LINE,
};
enum class FHIDEFIND {
    NO,
    YES,
};
enum class FINDENT {
    BREAKLINE,
    ADDLINE,
};
enum class FFINDLINES {
    GETINPUT,
    RUNAGAIN,
};
enum FKEY {
    FKEY_KOMMAND_START,
    FKEY_KOMMAND_STOP,
    FKEY_AUTOCOMPLETE,
    FKEY_BOOKMARKS_NEXT,
    FKEY_BOOKMARKS_PREV,
    FKEY_BOOKMARKS_TOGGLE,
    FKEY_CASE_LOWER,
    FKEY_CASE_UPPER,
    FKEY_COMMENT_BLOCK,
    FKEY_COMMENT_LINE,
    FKEY_COPY_TEXT,
    FKEY_CUT_TEXT,
    FKEY_DELETE_LINE_LEFT,
    FKEY_DELETE_LINE_RIGHT,
    FKEY_DELETE_WORD_LEFT,
    FKEY_DELETE_WORD_RIGHT,
    FKEY_DUP_TEXT,
    FKEY_ENTER1,
    FKEY_ENTER2,
    FKEY_FIND_LINES,
    FKEY_FIND_LINES_AGAIN,
    FKEY_GOTO_LINE,
    FKEY_HELP,
    FKEY_HOME,
    FKEY_MENU,
    FKEY_MOVE_DOWN,
    FKEY_MOVE_UP,
    FKEY_OUTPUT_NEXT,
    FKEY_OUTPUT_PREV,
    FKEY_OUTPUT_TOGGLE,
    FKEY_QUICK_FIND,
    FKEY_REDO,
    FKEY_REPLACE,
    FKEY_SEARCH_BACKWARD,
    FKEY_SEARCH_FORWARD,
    FKEY_SELECT_LINE,
    FKEY_SELECT_PAIR1,
    FKEY_SELECT_PAIR2,
    FKEY_SELECT_STYLE,
    FKEY_SHIFT_LEFT,
    FKEY_SHIFT_RIGHT,
    FKEY_SHOW_FIND_REPLACE,
    FKEY_SORT_ASCENDING,
    FKEY_SORT_DESCENDING,
    FKEY_UNDO,
    FKEY_VIEW_1,
    FKEY_VIEW_2,
    FKEY_VIEW_CLOSE,
    FKEY_SIZE,
};
enum class FLINEENDING {
    UNIX,
    WINDOWS,
};
enum class FMOVEH {
    LEFT,
    RIGHT,
};
enum class FMOVEV {
    UP,
    DOWN,
};
enum class FNLTAB {
    NO,
    FIND,
    REPLACE,
    YES,
};
enum class FOUTPUT {
    SHOW_HORIZONTAL,
    SHOW_VERTICAL,
    SHOW,
    TOGGLE,
};
enum class FREGEX {
    NO,
    YES,
};
enum class FREGEXTYPE {
    REPLACE,
    INSERT,
    APPEND,
};
enum class FSAVEWORD {
    NO,
    YES,
};
enum class FSEARCHDIR {
    FORWARD,
    BACKWARD,
};
enum class FSCRIPT {
    GETINPUT,
    RUNAGAIN,
};
enum class FSCRIPTRES {
    OUTPUT,
    CLIPBOARD,
    INSERT,
    REPLACE,
};
enum class FSELECTION {
    NO,
    YES,
};
enum class FSORT {
    ASCENDING,
    DESCENDING,
};
enum class FSPLITVIEW {
    NO,
    VERTICAL,
    HORIZONTAL,
};
enum class FTAB {
    HARD,
    SOFT,
};
enum class FTRIM {
    NO,
    YES,
};
enum class FUNDO {
    FLE,
    FLTK,
    NONE,
};
enum class FWORDCOMPARE {
    NO,
    YES,
};
enum class FWRAP {
    NO,
    YES,
};
namespace ascii {
    constexpr const unsigned char BACKSLASH    = '\\';
    constexpr const unsigned char CLOSE_PARENT = ')';
    constexpr const unsigned char NEWLINE      = '\n';
    constexpr const unsigned char OPEN_PARENT  = '(';
    constexpr const unsigned char QUOTE        = '"';
    constexpr const unsigned char SINGLE_QUOTE = '\'';
    constexpr const unsigned char SPACE        = ' ';
}
namespace errors {
    constexpr const char* FILE_DELETED        = "error: file has been deleted!";
    constexpr const char* FILE_IS_DIR         = "error: can't open <%s> because it is a directory!";
    constexpr const char* FILE_TOO_LARGE      = "error: file too large <%s>!";
    constexpr const char* HEX_TOO_LARGE       = "error: binary file too large to view as hex <%s>!";
    constexpr const char* LOADING_BIN         = "error: can't load binary file <%s>! Binary file loading has been turned off.";
    constexpr const char* LOADING_FILE        = "error: failed to read file <%s>!";
    constexpr const char* NO_FILENAME         = "error: no filename!";
    constexpr const char* SAVE_FILE           = "error: failed to save file <%s>!";
    constexpr const char* TEXT_DIFF_FROM_FILE = "error: file is not same as on disk (%s)";
    constexpr const char* TEXT_IS_READ_ONLY   = "error: text is in readonly mode!";
}
namespace info {
    constexpr const char* ASK_RELOAD           = "File %s has been changed on disk\nWould you like to load it again and discard changes?";
    constexpr const char* BACKUP_LOADED        = "file is missing but found backup file!";
    constexpr const char* BIN_LOADED           = "binary file has been loaded as text";
    constexpr const char* FILE_RELOAD          = "file changed on disk and reloaded";
    constexpr const char* FILE_WRAPPED         = "word wrapping has been turned on due to very long text lines";
    constexpr const char* FOUND_LINES          = "found %u lines in %d mS";
    constexpr const char* FOUND_STRING_POS     = "found string at pos %d";
    constexpr const char* FOUND_STRING_LINE    = "found string at line %d";
    constexpr const char* FOUND_STRING_LINECOL = "found string at line %d, column %d";
    constexpr const char* HEX_LOADED           = "binary file has been loaded as hex";
    constexpr const char* NO_SPACES_REPLACED   = "unable to convert spaces to tabs!";
    constexpr const char* NO_STRINGS_REPLACED  = "unable to replace <%s> with <%s>";
    constexpr const char* NO_STRING_FOUND      = "unable to find <%s>";
    constexpr const char* NO_TABS_REPLACED     = "unable to convert tabs to spaces!";
    constexpr const char* REDID_CHANGES        = "redid %d changes";
    constexpr const char* REMOVED_TRAILING     = "removed whitespace from %u lines";
    constexpr const char* REPLACED_STRINGS     = "replaced %u strings in %d mS";
    constexpr const char* SPACES_REPLACED      = "replaced spaces with %d tabs";
    constexpr const char* STYLE_OFF            = "styling has been turned off!";
    constexpr const char* TABS_REPLACED        = "replaced %d tabs with spaces";
    constexpr const char* TEXT_SAME_AS_FILE    = "file and buffer are the same";
    constexpr const char* UNDID_CHANGES        = "undid %d changes";
    constexpr const char* UNDO_MEMORY          = "warning: memory usage for undo has reached %lld bytes";
}
namespace message {
    const std::string DND_EVENT               = "message: editor has received an dnd event (p = bool*)";
    const std::string EDITOR_FOCUS            = "message: editor has focus (p = Editor*)";
    const std::string FILE_LOADED             = "message: editor has loaded a file (p = Editor*)";
    const std::string FONTSIZE2_CHANGED       = "message: fontsize overruled";
    const std::string HIDE_FIND               = "message: hide find widget (p = FindReplace*)";
    const std::string PREF_CHANGED            = "message: pref has been changed";
    const std::string RESET_INSERT_MODE       = "message: reset insert mode to default";
    const std::string SHOW_FIND               = "message: show find widget (p = FindReplace*)";
    const std::string STATUSBAR_LINE_UNIX     = "message: unix line ending";
    const std::string STATUSBAR_LINE_WIN      = "message: windows line ending";
    const std::string STATUSBAR_STYLE_CHANGED = "message: style has changed (p = style name)";
    const std::string STATUSBAR_TAB_CHANGED   = "message: tab has changed";
    const std::string STATUSBAR_TO_SPACES     = "message: convert to spaces";
    const std::string STATUSBAR_TO_TABS       = "message: convert to tabs";
    const std::string TEXT_CHANGED            = "message: text has changed saved status (p = Editor*)";
    const std::string UNDO_MODE_CHANGED       = "message: undo mode has changed";
}
namespace strings {
    constexpr const char* COMMAND_MODE = "Command mode";
    constexpr const char* SOFT_TABS[]  = {
        "",
        " ",
        "  ",
        "   ",
        "    ",
        "     ",
        "      ",
        "       ",
        "        ",
        "         ",
        "          ",
        "           ",
        "            ",
        "             ",
        "              ",
    };
}
enum {
    LIMIT_MIN,
    LIMIT_VAL,
    LIMIT_MAX,
    LIMIT_DEF,
};
namespace limits {
    extern const size_t         FIND_LIST_MAX;
    extern const size_t         HEXFILE_DIVIDER;
    extern const size_t         MOUSE_SCROLL_MAX;
    extern const size_t         TAB_WIDTH_MAX;
    extern const size_t         UNDO_WARNING;
    extern const size_t         WRAP_DEF;
    extern const size_t         WRAP_MAX;
    extern const size_t         WRAP_MIN;
    extern size_t               AUTOCOMPLETE_FILESIZE_DEF;
    extern size_t               AUTOCOMPLETE_FILESIZE_MAX;
    extern size_t               AUTOCOMPLETE_FILESIZE_MIN;
    extern size_t               AUTOCOMPLETE_FILESIZE_STEP;
    extern size_t               AUTOCOMPLETE_FILESIZE_VAL;
    extern size_t               AUTOCOMPLETE_LINES_DEF;
    extern size_t               AUTOCOMPLETE_LINES_MAX;
    extern size_t               AUTOCOMPLETE_LINES_MIN;
    extern size_t               AUTOCOMPLETE_LINES_STEP;
    extern size_t               AUTOCOMPLETE_LINES_VAL;
    extern size_t               AUTOCOMPLETE_WORD_SIZE_DEF;
    extern size_t               AUTOCOMPLETE_WORD_SIZE_MAX;
    extern size_t               AUTOCOMPLETE_WORD_SIZE_MIN;
    extern size_t               AUTOCOMPLETE_WORD_SIZE_STEP;
    extern size_t               AUTOCOMPLETE_WORD_SIZE_VAL;
    extern size_t               COUNT_CHAR_DEF;
    extern size_t               COUNT_CHAR_MAX;
    extern size_t               COUNT_CHAR_MIN;
    extern size_t               COUNT_CHAR_STEP;
    extern size_t               COUNT_CHAR_VAL;
    extern size_t               FILE_BACKUP_SIZE_DEF;
    extern size_t               FILE_BACKUP_SIZE_MAX;
    extern size_t               FILE_BACKUP_SIZE_MIN;
    extern size_t               FILE_BACKUP_SIZE_STEP;
    extern size_t               FILE_BACKUP_SIZE_VAL;
    extern size_t               FILE_SIZE_DEF;
    extern size_t               FILE_SIZE_MAX;
    extern size_t               FILE_SIZE_MIN;
    extern size_t               FILE_SIZE_STEP;
    extern size_t               FILE_SIZE_VAL;
    extern size_t               FORCE_RESTYLING;
    extern size_t               OUTPUT_LINES_DEF;
    extern size_t               OUTPUT_LINES_MAX;
    extern size_t               OUTPUT_LINES_MIN;
    extern size_t               OUTPUT_LINES_STEP;
    extern size_t               OUTPUT_LINES_VAL;
    extern size_t               OUTPUT_LINE_LENGTH_DEF;
    extern size_t               OUTPUT_LINE_LENGTH_MAX;
    extern size_t               OUTPUT_LINE_LENGTH_MIN;
    extern size_t               OUTPUT_LINE_LENGTH_STEP;
    extern size_t               OUTPUT_LINE_LENGTH_VAL;
    extern size_t               STYLE_FILESIZE_DEF;
    extern size_t               STYLE_FILESIZE_MAX;
    extern size_t               STYLE_FILESIZE_MIN;
    extern size_t               STYLE_FILESIZE_STEP;
    extern size_t               STYLE_FILESIZE_VAL;
    extern size_t               WRAP_LINE_LENGTH_DEF;
    extern size_t               WRAP_LINE_LENGTH_MAX;
    extern size_t               WRAP_LINE_LENGTH_MIN;
    extern size_t               WRAP_LINE_LENGTH_STEP;
    extern size_t               WRAP_LINE_LENGTH_VAL;
}
class Bookmarks {
public:
    explicit                    Bookmarks(Editor* editor)
                                    { _editor = editor; }
                                Bookmarks(Editor* editor, std::string bookmarks);
                                Bookmarks() = delete;
                                Bookmarks(const Bookmarks&) = delete;
                                Bookmarks(Bookmarks&& other) = delete;
    Bookmarks&                  operator=(const Bookmarks&) = delete;
    Bookmarks&                  operator=(Bookmarks&&  other)
                                    { _editor = other._editor; _vec    = other._vec; return *this; }
    bool                        add(int pos);
    void                        clear()
                                    { _vec.clear(); }
    void                        debug() const;
    int                         find_next(int pos, size_t& index);
    int                         find_prev(int pos, size_t& index);
    void                        goto_next();
    void                        goto_prev();
    size_t                      size() const
                                    { return _vec.size(); }
    void                        toggle();
    std::string                 tostring() const;
    size_t                      update(int pos, int inserted, int deleted);
private:
    Editor*                     _editor;
    IntVector                   _vec;
};
struct Config {
    Editor*                     active;
    FBINFILE                    pref_binary;
    FUNDO                       pref_undo;
    bool                        disable_autoreload;
    bool                        disable_lineending;
    bool                        disable_style;
    bool                        disable_tab;
    bool                        pref_autocomplete;
    bool                        pref_autoreload;
    bool                        pref_indentation;
    bool                        pref_insert;
    bool                        pref_linenumber;
    bool                        pref_statusbar;
    gnu::File                   pref_backup;
    int                         pref_cursor;
    int                         pref_shrink_status;
    int                         pref_tmp_fontsize;
    std::string                 pref_scheme;
    unsigned                    pref_mouse_scroll;
    unsigned                    pref_wrap;
    std::vector<std::string>    find_list;
    std::vector<std::string>    replace_list;
                                Config();
                                Config(const Config&) = delete;
                                Config(Config&&) = delete;
    Config&                     operator=(const Config&) = delete;
    Config&                     operator=(Config&&) = delete;
    bool                        add_find_word(std::string word, bool append = false);
    int                         add_receiver(Message* object);
    bool                        add_replace_word(std::string word, bool append = false);
    std::string                 backup_name(std::string filename);
    void                        debug() const;
    void                        load_pref(Fl_Preferences& preferences, FindReplace* findreplace = nullptr);
    void                        remove_receiver(const Message* object);
    void                        save_pref(Fl_Preferences& preferences, FindReplace* findreplace = nullptr);
    void                        send_message(std::string message, std::string s = "", const void* p = nullptr);
private:
    int                         _id;
    bool                        _del;
    MessageVector               _list;
};
struct CursorPos {
    int                         pos1;
    int                         pos2;
    int                         drag;
    int                         start;
    int                         top1;
    int                         top2;
    int                         end;
    bool                        swap;
                                CursorPos();
                                CursorPos(int pos1, int pos2, int start, int end, bool swap);
                                CursorPos(int pos1, int pos2, int drag, int start, int end, bool swap);
    bool                        operator==(const CursorPos& pos)
                                    { return pos1 == pos.pos1 && pos2 == pos.pos2; }
    bool                        operator!=(const CursorPos& pos)
                                    { return pos1 != pos.pos1 || pos2 != pos.pos2; }
    void                        clear_selection()
                                    { start = end = drag = -1; }
    void                        debug(int line = 0, const char* file = "") const;
    bool                        has_cursor() const
                                    { return pos1 >= 0 || pos2 >= 0; }
    bool                        text_has_selection() const
                                    { return start >= 0 && end > start; }
    int                         len() const
                                    { return (start >= 0 && end > start) ? end - start : 0; }
    void                        set_drag()
                                    { drag = (pos1 > start) ? start : end; }
    void                        to_default()
                                    { _convert(false); }
    void                        to_swapped()
                                    { _convert(true); }
private:
    void                        _convert(bool swapped);
};
struct EditorFlags {
    FSEARCHDIR                  fsearchdir;
    FSPLITVIEW                  fsplitview;
    FTAB                        tab_mode;
    FWRAP                       fwrap;
    bool                        dnd;
    bool                        kommand;
    bool                        ro;
    unsigned                    tab_width;
    unsigned                    wrap_col;
                                EditorFlags();
    void                        debug() const;
    void                        set_tab_from_string(std::string s);
};
struct FileInfo {
    FLINEENDING                 flineending;
    bool                        binary;
    gnu::File                   fi;
    int64_t                     reload_time;
    uint64_t                    fletcher64;
                                FileInfo();
    void                        debug() const;
    std::string                 to_string() const
                                    { return gnu::str::format("%s, bin=%d, type=%d", fi.name.c_str(), binary, static_cast<int>(flineending)); }
};
namespace help {
    std::string                 find_lines();
    std::string                 flags(const Config& config);
    std::string                 general();
    std::string                 pcre();
    std::string                 replace_text();
}
struct KeyConf {
    static const int            VERSION = 0;
    FKEY                        num;
    bool                        alt_d;
    bool                        alt_u;
    bool                        ctrl_d;
    bool                        ctrl_u;
    bool                        shift_d;
    bool                        shift_u;
    bool                        kommand_d;
    bool                        kommand_u;
    int                         key_d;
    int                         key_u;
    const char*                 help;
    bool                        has_custom_key() const;
    int                         to_int() const;
    std::string                 to_string() const;
    std::string                 make_help(int w) const;
    static std::string          KeyDescr(int key);
    static void                 LoadPref(Fl_Preferences& preferences);
    static void                 SavePref(Fl_Preferences& preferences);
};
extern KeyConf                  KEYS[FKEY_SIZE];
class Message {
public:
    enum class CTRL {
                                CONTINUE,
                                ABORT,
    };
    static const std::string    EMPTY_STRING;
                                Message() = delete;
                                Message(const Message&) = delete;
                                Message(Message&&) = delete;
    Message&                    operator=(const Message&) = delete;
    Message&                    operator=(Message&&) = delete;
                                Message(Config& config);
    virtual                     ~Message();
    virtual Message::CTRL       message(const std::string& message, const std::string& s = EMPTY_STRING, const void* p = nullptr) = 0;
    int                         object_id() const
                                    { return _id; }
private:
    int                         _id;
    Config&                     _config;
};
struct StatusBarInfo {
    int                         col;
    int                         end;
    int                         pos;
    int                         row;
    int                         rows;
    int                         start;
                                StatusBarInfo();
    void                        debug() const;
    bool                        has_selection() const
                                    { return start != end; }
};
namespace string {
    gnu::FileBuf                binary_to_hex(const char* in, size_t in_size);
    gnu::FileBuf                binary_to_text(const char* in, size_t in_size);
    std::string                 fnltab(std::string text);
    bool                        is_one_char(const char* in);
    void                        replace_char(char* in, char find, char replace);
    int                         toints(const std::string& string, int numbers[], int size, int def = -1);
    std::string                 tolower(std::string in);
    int                         wordlist(const char* text, StringSet& words);
}
class Token {
public:
    enum TYPE {
                                NIL           =     0,
                                CTRL          =     1,
                                NEWLINE       =     2,
                                SPACE         =     4,
                                PUNCTUATOR    =     8,
                                DECIMAL       =    16,
                                LETTER        =    32,
                                IDENT1        =    64,
                                IDENT2        =   128,
                                HEX           =   256,
                                NUM_SEPARATOR =   512,
                                VALUE         =  1024,
                                PRAGMA        =  2048,
                                UTF1          =  4096,
                                UTF2          =  8192,
    };
                                Token();
                                Token(const Token& other);
                                Token(Token&& other);
    Token&                      operator=(const Token& other);
    Token&                      operator=(Token&& other);
    int                         operator[](uint8_t index) const
                                    { return _char[index]; }
    int                         get(uint8_t index) const
                                    { return _char[index]; }
    Token&                      set(uint8_t index, uint16_t value);
    Token&                      set(uint8_t index_from, uint8_t index_to, uint16_t value);
    static void                 Debug(unsigned t);
    static Token                MakeWord();
private:
    uint16_t                    _char[256];
};
}
#include <FL/Fl_Text_Display.H>
#include <assert.h>
namespace fle {
class Style;
namespace style {
enum STYLE {
    STYLE_FG                    = 'A',
    STYLE_BG                    = 'B',
    STYLE_BG_SEL                = 'C',
    STYLE_FG_NUM                = 'D',
    STYLE_BG_NUM                = 'E',
    STYLE_CURSOR                = 'F',
    STYLE_KEYWORD               = 'G',
    STYLE_VAR                   = 'H',
    STYLE_FUNCTION              = 'I',
    STYLE_PRAGMA                = 'J',
    STYLE_TYPE                  = 'K',
    STYLE_NUMBER                = 'L',
    STYLE_PUNCTUATOR            = 'M',
    STYLE_STRING                = 'N',
    STYLE_RAW_STRING            = 'O',
    STYLE_COMMENT               = 'P',
    STYLE_BLOCK_COMMENT         = 'Q',
    STYLE_INIT                  = 'R',
    STYLE_SIZE                  = 18,
    STYLE_LAST                  = 16,
};
static constexpr const char* STYLE_FONTS[] = {
    "Foreground",
    nullptr,
    nullptr,
    "",
    nullptr,
    nullptr,
    "Keyword",
    "Var",
    "Function",
    "Pragma",
    "Type",
    "Number",
    "Punctuator",
    "String",
    "Raw string",
    "Line comment",
    "Block comment",
    nullptr,
    nullptr,
};
static constexpr const char* STYLE_NAMES[] = {
    "Foreground",
    "Background",
    "Background selection",
    "Foreground linenumbers",
    "Background linenumbers",
    "Cursor",
    "Keyword",
    "Var",
    "Function",
    "Pragma",
    "Type",
    "Number",
    "Punctuator",
    "String",
    "Raw string",
    "Line comment",
    "Block comment",
    "INIT",
    "SIZE",
};
enum WORD {
                                WORD_GROUP0 = 0,
                                WORD_GROUP1 = 1,
                                WORD_GROUP2 = 2,
                                WORD_GROUP4 = 4,
                                WORD_GROUP8 = 8,
};
extern const char*              FILE_FILTER;
extern const char*              SCHEME_BLUE;
extern const char*              SCHEME_DARK;
extern const char*              SCHEME_DEF;
extern const char*              SCHEME_LIGHT;
extern const char*              SCHEME_NEON;
extern const char*              SCHEME_TAN;
extern const char*              BAT;
extern FTAB                     BAT_TAB;
extern unsigned                 BAT_TAB_WIDTH;
extern const char*              CPP;
extern FTAB                     CPP_TAB;
extern unsigned                 CPP_TAB_WIDTH;
extern const char*              CS;
extern FTAB                     CS_TAB;
extern unsigned                 CS_TAB_WIDTH;
extern const char*              GO;
extern FTAB                     GO_TAB;
extern unsigned                 GO_TAB_WIDTH;
extern const char*              JAVA;
extern FTAB                     JAVA_TAB;
extern unsigned                 JAVA_TAB_WIDTH;
extern const char*              JS;
extern FTAB                     JS_TAB;
extern unsigned                 JS_TAB_WIDTH;
extern const char*              KOTLIN;
extern FTAB                     KOTLIN_TAB;
extern unsigned                 KOTLIN_TAB_WIDTH;
extern const char*              LUA;
extern FTAB                     LUA_TAB;
extern unsigned                 LUA_TAB_WIDTH;
extern const char*              MAKEFILE;
extern FTAB                     MAKEFILE_TAB;
extern unsigned                 MAKEFILE_TAB_WIDTH;
extern const char*              MARKUP;
extern FTAB                     MARKUP_TAB;
extern unsigned                 MARKUP_TAB_WIDTH;
extern const char*              PHP;
extern FTAB                     PHP_TAB;
extern unsigned                 PHP_TAB_WIDTH;
extern const char*              PYTHON;
extern FTAB                     PYTHON_TAB;
extern unsigned                 PYTHON_TAB_WIDTH;
extern const char*              RUBY;
extern FTAB                     RUBY_TAB;
extern unsigned                 RUBY_TAB_WIDTH;
extern const char*              RUST;
extern FTAB                     RUST_TAB;
extern unsigned                 RUST_TAB_WIDTH;
extern const char*              SHELL;
extern FTAB                     SHELL_TAB;
extern unsigned                 SHELL_TAB_WIDTH;
extern const char*              TEXT;
extern FTAB                     TEXT_TAB;
extern const char*              TEXT;
extern const char*              TS;
extern FTAB                     TS_TAB;
extern unsigned                 TS_TAB_WIDTH;
extern const char*              WREN;
extern FTAB                     WREN_TAB;
extern unsigned                 WREN_TAB_WIDTH;
}
class Style {
public:
                                Style(const Style&) = delete;
                                Style(Style&&) = delete;
    Style&                      operator=(const Style&) = delete;
    Style&                      operator=(Style&&) = delete;
    explicit                    Style(std::string name = style::TEXT);
    virtual                     ~Style();
    std::string                 block_end() const
                                    { return _block_end; }
    int                         block_end_size() const
                                    { return _block_end_size; }
    std::string                 block_start() const
                                    { return _block_start; }
    int                         block_start_size() const
                                    { return _block_start_size; }
    void                        debug() const;
    bool                        insert_word(std::string word, int word_type);
    bool                        is_paused() const
                                    { return _pause; }
    std::string                 line_comment() const
                                    { return _line_comment; }
    int                         line_comment_size() const
                                    { return _line_comment_size; }
    void                        make_words();
    std::string                 name() const
                                    { return _name; }
    void                        pause(bool pause)
                                    { _pause = pause; }
    void                        set_buffers(TextBuffer* text_buffer, TextBuffer* style_buffer)
                                    { _text  = text_buffer; _style = style_buffer; }
    FTAB                        tab_mode() const;
    void                        tab_mode(FTAB tab);
    unsigned                    tab_width() const;
    void                        tab_width(unsigned width);
    int                         update();
    virtual int                 update(int pos, int inserted_size, int deleted_size, const char* deleted_text, const char* deleted_style, Editor* editor);
    StringSet                   words()
                                    { return _words; }
protected:
    static const size_t         MAX_RAW = 5;
    StringIntHash               _lookup;
    StringSet                   _custom;
    StringSet                   _words;
    TextBuffer*                 _style;
    TextBuffer*                 _text;
    Token                       _tokens;
    bool                        _bin;
    bool                        _hex;
    bool                        _oct;
    bool                        _pause;
    bool                        _pragma;
    bool                        _raw_escape[MAX_RAW];
    const char*                 _block_end;
    const char*                 _block_start;
    const char*                 _line_comment;
    const char*                 _raw_end[MAX_RAW];
    const char*                 _raw_start[MAX_RAW];
    std::string                 _name;
    unsigned                    _block_end_size;
    unsigned                    _block_start_size;
    unsigned                    _line_comment_size;
    unsigned                    _raw_end_size[MAX_RAW];
    unsigned                    _raw_start_size[MAX_RAW];
    unsigned                    _single_quote_str;
};
class StyleDef : public Style {
public:
    explicit                    StyleDef(std::string name);
    int                         update(int pos, int inserted_size, int deleted_size, const char* deleted_text, const char* deleted_style, Editor* editor) override;
protected:
    int                         _expand_left(int& start);
    int                         _expand_right(int& end);
    virtual int                 _update(int start, int end);
};
class StyleBat : public StyleDef {
public:
                                StyleBat();
};
class StyleCpp : public StyleDef {
public:
                                StyleCpp();
};
class StyleCS : public StyleDef {
public:
                                StyleCS();
};
class StyleGo : public StyleDef {
public:
                                StyleGo();
};
class StyleJava : public StyleDef {
public:
                                StyleJava();
};
class StyleJS : public StyleDef {
public:
                                StyleJS(std::string name = style::JS);
};
class StyleKotlin : public StyleDef {
public:
                                StyleKotlin();
};
class StyleLua : public StyleDef {
public:
                                StyleLua();
};
class StyleMakefile : public StyleDef {
public:
                                StyleMakefile();
};
class StyleMarkup : public StyleDef {
public:
                                StyleMarkup();
    int                         update(int pos, int inserted_size, int deleted_size, const char* deleted_text, const char* deleted_style, Editor* editor) override;
protected:
    void                        _tag(int& start, int& end, int last, bool term);
    int                         _update(int start, int end) override;
};
class StylePHP : public StyleDef {
public:
                                StylePHP();
};
class StylePython : public StyleDef {
public:
                                StylePython();
};
class StyleRuby : public StyleDef {
public:
                                StyleRuby();
};
class StyleRust : public StyleDef {
public:
                                StyleRust();
};
class StyleShell : public StyleDef {
public:
                                StyleShell();
};
class StyleTS : public StyleJS {
public:
                                StyleTS();
};
class StyleWren : public StyleDef {
public:
                                StyleWren();
};
struct StyleProp {
    Fl_Color                    color_d;
    Fl_Color                    color_u;
    unsigned                    attr_d;
    unsigned                    attr_u;
    bool                        bold_u;
    bool                        bold_d;
    bool                        italic_u;
    bool                        italic_d;
                                StyleProp();
                                StyleProp(Fl_Color color, unsigned attr = 0, bool bold = false, bool italic = false);
    unsigned                    attr() const
                                    { return (attr_u != attr_d) ? attr_u : attr_d; }
    Fl_Color                    color() const
                                    { return (color_u != color_d) ? color_u : color_d; }
    int                         font() const;
    void                        reset();
};
typedef StyleProp StyleProperties[style::STYLE_SIZE];
typedef Fl_Text_Display::Style_Table_Entry Style_Table_Entry;
namespace style {
StyleProp*                      get_style_prop(std::string scheme, style::STYLE style);
FTAB                            get_tab_type(std::string name);
unsigned                        get_tab_width(std::string name);
const Style_Table_Entry*        get_table(std::string scheme, Fl_Fontsize fs);
inline unsigned                 index(STYLE style)
                                    { assert(style < STYLE_INIT); return style - STYLE_FG; }
void                            load_pref(Fl_Preferences& preferences);
Style*                          make_from_file(const gnu::File& file);
Style*                          make_from_name(std::string name);
void                            reset_all_styles();
void                            reset_style(std::string scheme);
void                            save_pref(Fl_Preferences& preferences);
void                            set_tab_type(std::string name, FTAB tab);
void                            set_tab_width(std::string name, unsigned width);
}
}
namespace fle {
class UndoEvent {
public:
    enum {
                                FLAG_NIL       =   0,
                                FLAG_INSERT    =   1,
                                FLAG_DELETE    =   2,
                                FLAG_REPLACE   =   4,
                                FLAG_BACKSPACE =   8,
                                FLAG_SELECTED  =  16,
                                FLAG_CUSTOM1   =  32,
                                FLAG_CUSTOM2   =  64,
    };
                                UndoEvent()
                                    { _flag = 0; _group = 0; _pos = 0; }
                                UndoEvent(uint8_t flag, uint16_t group, int pos, const char* str1, const char* str2 = "") : _str1(str1), _str2(str2 ? str2 : "")
                                    { _flag = flag; _group = group; _pos = pos; }
    void                        append_str1(const char* in)
                                    { _str1 += in; }
    const char*                 c_str1() const
                                    { return _str1.c_str(); }
    const char*                 c_str2() const
                                    { return _str2.c_str(); }
    void                        debug(int buffer_pos = 0, int count = 0) const;
    uint8_t                     flag() const
                                    { return _flag; }
    void                        flag(uint8_t flag)
                                    { _flag = flag; }
    uint16_t                    group() const
                                    { return _group; }
    void                        insert_str1(const char* in)
                                    { _str1 = in + _str1; }
    bool                        is_backspace() const
                                    { return _flag & FLAG_BACKSPACE; }
    bool                        is_custom1() const
                                    { return _flag & FLAG_CUSTOM1; }
    bool                        is_custom2() const
                                    { return _flag & FLAG_CUSTOM2; }
    bool                        is_delete() const
                                    { return _flag & FLAG_DELETE; }
    bool                        is_insert() const
                                    { return _flag & FLAG_INSERT; }
    bool                        is_null() const
                                    { return _str1.length() == 0; }
    bool                        is_replace() const
                                    { return _flag & FLAG_REPLACE; }
    bool                        is_selected() const
                                    { return _flag & FLAG_SELECTED; }
    int                         len1() const
                                    { return (int) _str1.length(); }
    int                         len2() const
                                    { return (int) _str2.length(); }
    int32_t                     pos() const
                                    { return _pos; }
    void                        pos(int32_t pos)
                                    { _pos = pos; }
    uint32_t                    size() const
                                    { return (uint32_t) (UndoEvent::NumberSize() + _str1.length() + 1 + _str2.length() + 1); }
    std::string                 str1() const
                                    { return _str1; }
    void                        str1(const char* in)
                                    { _str1 = in; }
    int                         str1_toint(int r[3]) const
                                    { return string::toints(_str1, r, 3); }
    std::string                 str2() const
                                    { return _str2; }
    void                        str2(const char* in)
                                    { _str2 = in; }
    int                         str2_toint(int r[3]) const
                                    { return string::toints(_str2, r, 3); }
    static inline uint32_t      NumberSize()
                                    { return (uint32_t) (sizeof(_flag) + sizeof(_group) + sizeof(_pos)); }
private:
    uint8_t                     _flag;
    uint16_t                    _group;
    int32_t                     _pos;
    std::string                 _str1;
    std::string                 _str2;
};
class UndoBuffer {
friend class Undo;
public:
                                UndoBuffer(const UndoBuffer&) = delete;
                                UndoBuffer(UndoBuffer&&) = delete;
    UndoBuffer&                 operator=(const UndoBuffer&) = delete;
    UndoBuffer&                 operator=(UndoBuffer&&) = delete;
                                UndoBuffer()
                                    { _buf = nullptr; clear(); }
                                ~UndoBuffer()
                                    { free(_buf); }
    bool                        buffer_decrease();
    bool                        buffer_increase(int64_t requested_bcap);
    int64_t                     capacity() const
                                    { return _bcap; }
    void                        clear();
    int64_t                     cursor() const
                                    { auto left = _bcur; go_left(left); return _move < UndoBuffer::MOVING_RIGHT ? left : _bcur; }
    int64_t                     debug(int all = 0) const;
    const UndoEvent             get_node() const
                                    { return get_node(_bcur); }
    const UndoEvent             get_node(int64_t cursor) const;
    bool                        go_left();
    bool                        go_left(int64_t& cursor) const;
    bool                        go_right();
    bool                        go_right(int64_t& cursor) const;
    void                        go_right_before_cut();
    bool                        has_left() const
                                    { auto test = _bcur; go_left(test); return _move == UndoBuffer::MOVING_RIGHT || test >= 0; }
    bool                        has_right() const
                                    { auto test = _bcur; go_right(test); return _move == UndoBuffer::MOVING_LEFT || test < _bend; }
    bool                        is_cursor_at_end() const
                                    { return _bcur == _bend; }
    const UndoEvent             peek_left() const;
    const UndoEvent             peek_right() const;
    void                        pop();
    void                        set_node(const UndoEvent& node);
private:
    static const int            MOVING_LEFT  = -1;
    static const int            MOVING_END   =  0;
    static const int            MOVING_RIGHT =  1;
    char*                       _buf;
    int                         _move;
    int64_t                     _bcap;
    int64_t                     _bcur;
    int64_t                     _bend;
};
class Undo {
public:
                                Undo(const Undo&) = delete;
                                Undo(Undo&&) = delete;
    Undo&                       operator=(const Undo&) = delete;
    Undo&                       operator=(Undo&&) = delete;
                                Undo();
    void                        add(FDELKEY delkey, bool selection, int pos, const char* inserted_text, const int inserted_size, const char* deleted_text = nullptr, const int deleted_size = 0);
    void                        add_custom1(std::string str1)
                                    { auto custom = UndoEvent(UndoEvent::FLAG_CUSTOM1, _group, -1, str1.c_str()); _push_node_to_buf(custom); }
    int64_t                     capacity() const
                                    { return _buf.capacity(); }
    void                        clear();
    void                        clear_custom1()
                                    { _custom1 = UndoEvent(); }
    int64_t                     debug(int all = 0);
    void                        group_add()
                                    { if (_group_lock == false) _group++; }
    void                        group_lock()
                                    { _group_lock = true; }
    void                        group_unlock()
                                    { _group_lock = false; }
    void                        group_unlock_and_add()
                                    { _group_lock = false; _group++; }
    bool                        has_redo() const
                                    { return _buf.has_right(); }
    bool                        has_undo() const
                                    { return _buf.has_left(); }
    bool                        is_at_save_point() const
                                    { return _save_point == _buf.cursor(); }
    bool                        is_group_locked() const
                                    { return _group_lock; }
    UndoEvent                   peek_redo() const
                                    { return _buf.peek_right(); }
    UndoEvent                   peek_undo() const
                                    { return _buf.peek_left(); }
    void                        prepare_custom1(std::string str1)
                                    { _custom1 = UndoEvent(UndoEvent::FLAG_CUSTOM1, _group, -1, str1.c_str()); }
    void                        prepare_custom2(const char* str2)
                                    { _custom2 = str2; }
    UndoEvent                   redo()
                                    { _prev_type = Token::NIL; return (_buf.go_right() == true) ? _buf.get_node() : UndoEvent(); }
    void                        set_save_point()
                                    { _save_point = _buf.cursor(); }
    UndoEvent                   undo()
                                    { _prev_type = Token::NIL; return (_buf.go_left() == true) ? _buf.get_node() : UndoEvent(); }
private:
    void                        _add(UndoEvent* last, uint8_t flag, int pos, const char* str1, const char* str2 = "");
    bool                        _append_to_node(UndoEvent* last, uint8_t flag, int pos, const char* str1);
    bool                        _is_cursor_at_end() const
                                    { return _buf.is_cursor_at_end(); }
    void                        _push_node_to_buf(UndoEvent node);
    Token                       _tokens;
    UndoBuffer                  _buf;
    UndoEvent                   _custom1;
    bool                        _group_lock;
    int                         _prev_type;
    int64_t                     _save_point;
    std::string                 _custom2;
    uint16_t                    _group;
};
}
#include <FL/Fl_Text_Buffer.H>
namespace fle {
class Editor;
struct CursorPos;
class BufferController {
public:
                                BufferController() = delete;
                                BufferController(const BufferController&) = delete;
                                BufferController(BufferController&&) = delete;
    BufferController&           operator=(const BufferController&) = delete;
    BufferController&           operator=(BufferController&&) = delete;
                                BufferController(TextBuffer* buffer, int timeout, bool start_group_lock);
                                ~BufferController()
                                    { stop(); }
    void                        check_timeout();
    void                        stop();
    int                         time() const
                                    { return _time; }
private:
    TextBuffer*                 _buffer;
    bool                        _running;
    bool                        _stopped;
    flw::WaitCursor*            _wc;
    int                         _time;
    int                         _timeout;
};
class TextBuffer : public Fl_Text_Buffer {
    friend class BufferController;
public:
    static int                  TIMEOUT_LONG;
    static int                  TIMEOUT_SHORT;
                                TextBuffer(const TextBuffer&) = delete;
                                TextBuffer(TextBuffer&&) = delete;
    TextBuffer&                 operator=(const TextBuffer&) = delete;
    TextBuffer&                 operator=(TextBuffer&&) = delete;
                                TextBuffer(Editor* editor, Config& config);
                                ~TextBuffer();
    uint32_t                    adler32() const;
    uint64_t                    calc_fletcher64() const;
    void                        callback_connect()
                                    { add_modify_callback(TextBuffer::CallbackUndo, this); }
    void                        callback_disconnect()
                                    { remove_modify_callback(TextBuffer::CallbackUndo, this); }
    CursorPos                   case_for_selection(FCASE fcase);
    void                        clear_key()
                                    { _fdelkey = FDELKEY::NIL; }
    CursorPos                   comment_block(std::string comment_start, std::string comment_end);
    CursorPos                   comment_line(std::string comment_string);
    int                         count_changes()
                                    { return _count_changes; }
    void                        count_changes(int count)
                                    { _count_changes = count; }
    bool                        cut_or_copy_line(int pos, FCOPY action);
    void                        debug() const;
    int                         delete_indent(int pos, FTAB ftab, unsigned tab_width);
    int                         delete_text_left(int pos, FDELTEXT del);
    int                         delete_text_right(int pos, FDELTEXT del);
    CursorPos                   duplicate_text();
    Editor*                     editor()
                                    { return _editor; }
    size_t                      find_lines(std::string filename, std::string find, gnu::PCRE* re, FTRIM ftrim, std::vector<std::string>& out);
    CursorPos                   find_replace(std::string find, const char* replace, FSEARCHDIR fsearchdir, FCASECOMPARE fcasecompare, FWORDCOMPARE fwordcompare, FNLTAB fnltab);
    CursorPos                   find_replace_all(std::string find, std::string replace, FSELECTION fselection, FCASECOMPARE fcase, FWORDCOMPARE fword, FNLTAB fnltab);
    CursorPos                   find_replace_regex(std::string find, const char* replace, FNLTAB fnltab);
    CursorPos                   find_replace_regex_all(gnu::PCRE* regex, std::string replace, FSELECTION fselection, FNLTAB fnltab);
    gnu::FileBuf                get(FLINEENDING flineending = FLINEENDING::UNIX, bool trim_whitespace = false) const;
    std::string                 get_first(int pos) const;
    std::string                 get_indent(int pos) const;
    std::string                 get_line(int pos) const;
    void                        get_line_pos(int pos, int& start, int& end) const;
    void                        get_line_pos_with_nl(int pos, int& start, int& end) const;
    uint64_t                    get_saved_fletcher64() const
                                    { return _fletcher64; }
    bool                        get_selection(int& start, int& end, bool expand);
    std::string                 get_text_range_string(int start, int end) const;
    int                         get_word_end(int pos) const;
    char*                       get_word_left(int pos) const;
    int                         get_word_start(int pos, bool move_left) const;
    bool                        has_callback() const
                                    { return mModifyProcs != nullptr; }
    bool                        has_fle_undo() const
                                    { return _undo != nullptr; }
    bool                        has_multiline_selection();
    bool                        has_restyle() const
                                    { return _style_text; }
    bool                        has_selection()
                                    { return _has_selection == true || selected() != 0; }
    int                         home(int pos);
    CursorPos                   indent(FINDENT findent);
    CursorPos                   insert_tab(CursorPos pos, FTAB ftab, unsigned tab_width);
    CursorPos                   insert_tab_multiline(CursorPos pos, FMOVEH fmoveh, FTAB ftab, unsigned tab_width);
    bool                        is_dirty() const
                                    { return _dirty; }
    bool                        is_word(int start, int end, int word_type);
    CursorPos                   move_line(FMOVEV move);
    int                         peek_token(int pos) const;
    CursorPos                   sort(FSORT order);
    CursorPos                   select_color();
    CursorPos                   select_line(bool exclude_newline);
    CursorPos                   select_pair(bool move_cursor);
    CursorPos                   select_word();
    void                        set(const char* text, uint64_t fletcher64);
    void                        set_backspace_key()
                                    { _fdelkey = FDELKEY::BACKSPACE; }
    void                        set_delete_key()
                                    { _fdelkey = FDELKEY::DEL; }
    void                        set_dirty(bool value, bool force_send = false);
    int                         token(const char* string) const;
    Undo*                       undo()
                                    { return _undo; }
    CursorPos                   undo_back(bool all, CursorPos cursor);
    bool                        undo_check_save_point();
    void                        undo_cursor_move_to_statusbar_row(CursorPos& cursor, const UndoEvent& node, bool undo);
    CursorPos                   undo_forward(bool all, CursorPos cursor);
    FUNDO                       undo_mode() const
                                    { return (_undo != nullptr) ? FUNDO::FLE : (mCanUndo != 0) ? FUNDO::FLTK : FUNDO::NONE; }
    void                        undo_set_mode_using_config();
    void                        update_saved_fletcher64()
                                    { _fletcher64 = calc_fletcher64(); }
    inline bool                 compare(unsigned int start, const char* string, unsigned int string_len) {
                                    auto end = start + string_len - 1;
                                    if (end >= (unsigned int) mLength) {
                                        return false;
                                    }
                                    while (string_len-- > 0) {
                                        auto buf = mBuf + start;
                                        if (start >= (unsigned int) mGapStart) {
                                            buf += mGapEnd;
                                            buf -= mGapStart;
                                        }
                                        if (*string != *buf) {
                                            return false;
                                        }
                                        start++;
                                        string++;
                                    }
                                    return true;
                                }
    inline bool                 compare_lowercase(unsigned int start, const char* string, unsigned int string_len) {
                                    auto end = start + string_len - 1;
                                    if (end >= (unsigned int) mLength) {
                                        return false;
                                    }
                                    while (string_len-- > 0) {
                                        auto buf = mBuf + start;
                                        if (start >= (unsigned int) mGapStart) {
                                            buf += mGapEnd;
                                            buf -= mGapStart;
                                        }
                                        auto c = tolower(*buf);
                                        if (*string != c) {
                                            return false;
                                        }
                                        start++;
                                        string++;
                                    }
                                    return true;
                                }
    inline unsigned char        peek(unsigned int pos) const {
                                    if (pos < (unsigned int) mGapStart) {
                                        return *(mBuf + pos);
                                    }
                                    else if (pos < (unsigned int) mLength) {
                                        auto buf = static_cast<const char*>(mBuf + pos);
                                        buf += mGapEnd;
                                        buf -= mGapStart;
                                        return *buf;
                                    }
                                    return 0;
                                }
    inline void                 poke(unsigned int pos, char c) {
                                    if (pos < (unsigned int) mGapStart) {
                                        *(mBuf + pos) = c;
                                    }
                                    else if (pos < (unsigned int) mLength) {
                                        auto buf = static_cast<char*>(mBuf + pos + mGapEnd - mGapStart);
                                        *buf = c;
                                    }
                                }
    inline void                 poke(unsigned int start, unsigned int end, char c) {
                                    auto len = end - start;
                                    if (end > (unsigned int) mLength || start > end) {
                                    }
                                    else if (end < (unsigned int) mGapStart) {
                                        memset(mBuf + start, c, len);
                                    }
                                    else if (start >= (unsigned int) mGapStart) {
                                        memset(mBuf + start + mGapEnd - mGapStart, c, len);
                                    }
                                    else {
                                        auto len1 = mGapStart - start;
                                        auto len2 = len - len1;
                                        memset(mBuf + start, c, len1);
                                        memset(mBuf + mGapEnd, c, len2);
                                    }
                                }
    int                         token(unsigned char c) const
                                    { return _word.get(c); }
    static void                 CallbackUndo(const int pos, const int inserted_size, const int deleted_size, const int restyled_size, const char* deleted_text, void* v);
#ifdef DEBUG
    CursorPos                   _find_replace_regex(gnu::PCRE* regex, const std::string replace, int from, int to, FREGEXTYPE fregextype, bool selection);
#endif
private:
#ifndef DEBUG
    CursorPos                   _find_replace_regex(gnu::PCRE* regex, const std::string replace, int from, int to, FREGEXTYPE fregextype, bool selection);
#endif
    Config&                     _config;
    Editor*                     _editor;
    FDELKEY                     _fdelkey;
    Token                       _word;
    Undo*                       _undo;
    bool                        _dirty;
    bool                        _has_selection;
    bool                        _pause_undo;
    bool                        _style_text;
    char                        _buf[256];
    int                         _count_changes;
    uint64_t                    _fletcher64;
};
}
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Int_Input.H>
namespace fle {
class Editor;
class FindReplace;
class StatusBar;
class AutoCompleteDialog : public Fl_Double_Window {
public:
    explicit                    AutoCompleteDialog(Fl_Fontsize fontsize);
    int                         handle(int event) override;
    int                         populate(const StringSet& words, std::string word);
    void                        resize(int X, int Y, int W, int H) override;
    std::string                 run(int x, int y, int w, int h);
    static void                 Callback(Fl_Widget* w, void* o);
private:
    Fl_Hold_Browser*            _browser;
};
class FindBar : public Fl_Group {
public:
    explicit                    FindBar(Config& config);
    FindReplace&                findreplace()
                                    { return *_findreplace; }
    int                         height() const;
    void                        hide() override;
    void                        resize(int X, int Y, int W, int H) override;
    void                        show() override;
    StatusBar&                  statusbar()
                                    { return *_statusbar; }
    void                        update_pref();
    bool                        visible() const;
private:
    Config&                     _config;
    FindReplace*                _findreplace;
    StatusBar*                  _statusbar;
};
class FindDialog : public Fl_Double_Window {
public:
    static FREGEX               REGEX;
    static FTRIM                TRIM;
                                FindDialog(std::string label, const std::vector<std::string>& find_list);
    std::string                 run();
    bool                        test_pcre();
    static void                 Callback(Fl_Widget* w, void* o);
private:
    Fl_Button*                  _cancel;
    Fl_Button*                  _help;
    Fl_Button*                  _ok;
    Fl_Button*                  _test;
    Fl_Check_Button*            _regex;
    Fl_Check_Button*            _trim;
    bool                        _ret;
    flw::GridGroup*             _grid;
    flw::InputMenu*             _find;
};
class FindReplace : public flw::GridGroup {
public:
    explicit                    FindReplace(Config& config);
    void                        add_find_word(std::string word);
    void                        add_replace_word(std::string word);
    void                        callback(Fl_Callback* cb, void* obj);
    void                        enable_buttons();
    FCASECOMPARE                fcasecompare() const
                                    { return (_case->value() != 0 && _case->active() != 0) ? FCASECOMPARE::YES : FCASECOMPARE::NO; }
    void                        fcasecompare(FCASECOMPARE value)
                                    { _case->value((value == FCASECOMPARE::NO) ? 0 : 1); }
    Fl_Widget*                  find_input() const
                                    { return static_cast<Fl_Widget*>(_find_input); }
    std::string                 find_string() const
                                    { return _find_input->value(); }
    void                        find_string(std::string value)
                                    { _find_input->value(value.c_str()); }
    FNLTAB                      fnltab() const;
    void                        fnltab(FNLTAB fnltab);
    FREGEX                      fregex() const
                                    { return (_regex->value() != 0) ? FREGEX::YES : FREGEX::NO; }
    void                        fregex(FREGEX value)
                                    { _regex->value((value == FREGEX::NO) ? 0 : 1); }
    FSELECTION                  fselection() const
                                    { return (_selection->value() != 0) ? FSELECTION::YES : FSELECTION::NO; }
    void                        fselection(FSELECTION value)
                                    { _selection->value((value == FSELECTION::NO) ? 0 : 1); }
    FWORDCOMPARE                fwordcompare() const
                                    { return (_word->value() != 0 && _word->active() != 0) ? FWORDCOMPARE::YES : FWORDCOMPARE::NO; }
    void                        fwordcompare(FWORDCOMPARE value)
                                    { _word->value((value == FWORDCOMPARE::NO) ? 0 : 1); }
    int                         handle(int event) override;
    Fl_Widget*                  next_button() const
                                    { return static_cast<Fl_Widget*>(_find_next); }
    Fl_Widget*                  prev_button() const
                                    { return static_cast<Fl_Widget*>(_find_prev); }
    void                        show() override;
    Fl_Widget*                  replace_all_button() const
                                    { return static_cast<Fl_Widget*>(_replace_all); }
    Fl_Widget*                  replace_button() const
                                    { return static_cast<Fl_Widget*>(_replace); }
    Fl_Widget*                  replace_input() const
                                    { return static_cast<Fl_Widget*>(_replace_input); }
    std::string                 replace_string() const
                                    { return _replace_input->value(); }
    void                        replace_string(std::string value)
                                    { _replace_input->value(value.c_str()); }
    void                        resize(int X, int Y, int W, int H) override;
    void                        take_focus();
    void                        update_lists(bool find = true, bool replace = true);
    void                        update_pref();
    static void                 CallbackCheckButton(Fl_Widget* w, void* o);
    static inline int           Height()
                                    { return flw::PREF_FONTSIZE * 5.5; }
private:
    Config&                     _config;
    Fl_Button*                  _find_next;
    Fl_Button*                  _find_prev;
    Fl_Button*                  _replace;
    Fl_Button*                  _replace_all;
    Fl_Check_Button*            _case;
    Fl_Check_Button*            _find_nl;
    Fl_Check_Button*            _regex;
    Fl_Check_Button*            _replace_nl;
    Fl_Check_Button*            _selection;
    Fl_Check_Button*            _word;
    flw::InputMenu*             _find_input;
    flw::InputMenu*             _replace_input;
};
class LineNumDialog : public Fl_Double_Window {
public:
                                LineNumDialog();
    int                         handle(int event) override;
    void                        resize(int X, int Y, int W, int H) override;
    int                         run();
    static void                 Callback(Fl_Widget* w, void* o);
private:
    Fl_Int_Input*               _input;
    int                         _line;
};
class ReplaceDialog : public Fl_Double_Window {
public:
    static FCASECOMPARE         CASECOMPARE;
    static FNLTAB               NLTAB;
    static FREGEX               REGEX;
    static FSELECTION           SELECTION;
    static FWORDCOMPARE         WORDCOMPARE;
                                ReplaceDialog(std::string label, std::string& find, std::string& replace, const std::vector<std::string>& find_list, const std::vector<std::string>& replace_list);
    void                        check_buttons();
    int                         handle(int event) override;
    bool                        run();
    bool                        test_pcre();
    static void                 Callback(Fl_Widget* w, void* o);
private:
    Fl_Button*                  _cancel;
    Fl_Button*                  _help;
    Fl_Button*                  _ok;
    Fl_Button*                  _test;
    Fl_Check_Button*            _case;
    Fl_Check_Button*            _find_nl;
    Fl_Check_Button*            _regex;
    Fl_Check_Button*            _replace_nl;
    Fl_Check_Button*            _selection;
    Fl_Check_Button*            _word;
    bool                        _ret;
    flw::GridGroup*             _grid;
    flw::InputMenu*             _find_input;
    flw::InputMenu*             _replace_input;
    std::string&                _find;
    std::string&                _replace;
};
class StatusBar : public flw::GridGroup {
public:
    explicit                    StatusBar(Config& config);
    void                        callback_line();
    void                        callback_syntax();
    void                        callback_tab();
    void                        callback_tab_def();
    int                         height() const
                                    { return _h; }
    bool                        label_cursor(std::string val);
    bool                        label_cursor_mode(std::string val);
    std::string                 label_message() const
                                    { return _label_message->label(); }
    bool                        label_message(std::string val);
    void                        update_menus(Editor* editor);
    void                        update_pref();
private:
    Config&                     _config;
    Fl_Box*                     _label_cursor;
    Fl_Box*                     _label_cursor_mode;
    Fl_Box*                     _label_message;
    Fl_Menu_Button*             _line_menu;
    Fl_Menu_Button*             _style_menu;
    Fl_Menu_Button*             _tab_menu;
    int                         _h;
};
namespace dlg {
    void                        config(Config& config);
    bool                        edit(Config& config, std::string title, std::string& text, std::string style = style::TEXT, int W = 60, int H = 40);
    bool                        edit_file(Config& config, std::string title, std::string file, std::string style = "", int W = 60, int H = 40);
    void                        keyboard(Config& config);
    void                        scheme(Config& config);
    void                        tweaks();
    bool                        view(Config& config, std::string title, std::string& text, std::string style = style::TEXT, int W = 60, int H = 40);
    bool                        view_file(Config& config, std::string title, std::string file, std::string style = "", int W = 60, int H = 40);
}
}
#include <FL/Fl_Text_Editor.H>
namespace fle {
class View : public Fl_Text_Editor, public Message {
public:
    explicit                    View(Config& config, Editor* editor);
                                ~View();
    void                        callback_connect();
    void                        callback_disconnect();
    void                        display_insert()
                                    { Fl_Text_Editor::display_insert(); }
    int                         drag_pos() const
                                    { return dragPos; }
    void                        drag_set_pos(int pos = 0, int type = 0)
                                    { dragPos = pos; dragType = type; }
    int                         drag_type() const
                                    { return dragType; }
    int                         handle(int event) override;
    void                        init(View* view1);
    int                         pos_to_line_and_col(int pos, int& row, int& col)
                                    { return position_to_linecol(pos, &row, &col); }
    Message::CTRL               message(const std::string& message, const std::string& s, const void* p) override;
    int                         take_focus();
    int                         top_line() const
                                    { return get_absolute_top_line_number(); }
    void                        top_set_line(int line_num)
                                    { scroll(line_num, 1); }
    bool                        update_pref(bool wrap_for_view2);
    static void                 CallbackStyleBuffer(const int pos, const int inserted_size, const int deleted_size, const int restyled_size, const char* deleted_text, void* v);
private:
    bool                        _handle_dnd();
    int                         _handle_key();
    bool                        _handle_mousewheel();
    Editor*                     _editor;
    Config&                     _config;
};
}
#include <assert.h>
#include <FL/fl_ask.H>
namespace fle {
class Editor : public Fl_Group, Message {
public:
                                Editor(Config& config, FindBar* findbar, int X = 0, int Y = 0, int W = 0, int H = 0);
                                ~Editor();
    void                        activate();
    Bookmarks&                  bookmarks()
                                    { return _bookmarks; }
    TextBuffer&                 buffer()
                                    { return *_buf1; }
    void                        callback_connect()
                                    { _view1->callback_connect(); _buf1->callback_connect(); }
    void                        callback_disconnect()
                                    { _view1->callback_disconnect(); _buf1->callback_disconnect(); }
    void                        callback_output(int add_line = 0);
    Config&                     config()
                                    { return _config; }
    CursorPos                   cursor(bool set_top_line);
    int                         cursor_insert_position() const
                                    { assert(_view); return _view->insert_position(); }
    void                        cursor_move(CursorPos cursor);
    void                        cursor_move_to_pos(int pos, bool force_unselect);
    void                        cursor_move_to_rowcol(int row, int column);
    void                        cursor_move_to_statusbar_row()
                                     { cursor_move_to_rowcol(_statusbar_info.row, 1); }
    int                         cursor_pos_to_line_and_col(int pos, int& row, int& col)
                                    { assert(_view); return _view->pos_to_line_and_col(pos, row, col); }
    void                        cursor_save()
                                    { _saved_cursor = cursor(true); }
    CursorPos                   cursor_saved() const
                                    { return _saved_cursor; }
    void                        deactivate();
    void                        debug(int what = 0);
    std::string                 debug_save_style(std::string filename = "", bool add_pos = true) const;
    void                        do_layout()
                                    { _main->do_layout(); Fl::redraw(); }
    std::string                 file_changed_name() const
                                    { return (text_is_dirty() == true) ? "*" + _file_info.fi.name + "*": _file_info.fi.name; }
    void                        file_check_reload();
    void                        file_compare_buffer();
    uint64_t                    file_fletcher64() const
                                    { return _file_info.fletcher64; }
    bool                        file_is_empty() const
                                    { return text_is_dirty() == false && _file_info.fi.filename == ""; }
    FLINEENDING                 file_line_ending() const
                                    { return _file_info.flineending; }
    std::string                 file_load(std::string filename);
    uint64_t                    file_mtime() const
                                    { return _file_info.fi.mtime; }
    std::string                 file_name() const
                                    { return _file_info.fi.name; }
    std::string                 file_save();
    std::string                 file_save_as(std::string filename);
    void                        file_set_new_filename(std::string filename)
                                    { _file_info.fi = filename; text_set_dirty(true); }
    std::string                 filename() const
                                    { return _file_info.fi.filename; }
    std::string                 filepath() const
                                    { return _file_info.fi.path; }
    size_t                      find_lines(std::string find, FREGEX fregex, FTRIM ftrim);
    size_t                      find_lines(std::string find, FREGEX fregex, FTRIM ftrim, std::vector<std::string>& out);
    void                        find_quick();
    bool                        find_replace(FSEARCHDIR fdir, bool replace_text = false);
    size_t                      find_replace_all(std::string find, std::string replace, FNLTAB fnltab, FSELECTION fselection,  FCASECOMPARE fcase, FWORDCOMPARE fword, FREGEX fregex, FSAVEWORD fsave, FHIDEFIND fhide, bool disable_message = false);
    FSEARCHDIR                  find_search_dir() const
                                    { return _editor_flags.fsearchdir; }
    FindBar&                    findbar()
                                    { return *_findbar; }
    FindReplace&                findreplace()
                                    { return _findbar->findreplace(); }
    int                         handle(int event) override;
    void                        help() const;
    bool                        home();
    size_t                      memory_usage(size_t& buffer, size_t& style, size_t& undo) const;
    Message::CTRL               message(const std::string& message, const std::string& s, const void* p) override;
    void                        resize(int X, int Y, int W, int H) override
                                    { Fl_Group::resize(X, Y, W, H); _main->resize(X, Y, W, H); Fl::redraw(); }
    void                        select_color();
    void                        select_pair(bool move_cursor);
    void                        show_autocomplete();
    void                        show_find()
                                    { _config.send_message(message::SHOW_FIND, "", &_findbar->findreplace()); }
    size_t                      show_find_lines_dialog_or_run_again(FFINDLINES find);
    void                        show_line_dialog();
    void                        show_menu();
    void                        show_output(FOUTPUT foutput);
    void                        show_print();
    std::string                 statusbar_message() const
                                    { return _findbar->statusbar().label_message(); }
    std::string                 statusbar_set_message(std::string label)
                                    { _findbar->statusbar().label_message(label); return label; }
    Style&                      style()
                                    { return *_style; }
    void                        style(Style* style);
    TextBuffer&                 style_buffer()
                                    { return *_buf2; }
    void                        style_from_filename()
                                    { style(style::make_from_file(_file_info.fi)); }
    void                        style_from_language(std::string name)
                                    { style(style::make_from_name(name)); }
    void                        style_resize_buffer();
    int                         take_focus()
                                    { assert(_view); return _view->take_focus(); }
    void                        text_comment_block();
    void                        text_comment_line();
    void                        text_convert_case(FCASE transform);
    void                        text_copy_to_clipboard() const
                                    { auto text1 = _buf1->selection_text(); Fl::copy(text1, strlen(text1), 2); free(text1); }
    int                         text_count_selection_len(bool* bytes = nullptr) const;
    void                        text_cut_to_clipboard() const
                                    { auto text1 = _buf1->selection_text(); _buf1->remove_selection(); Fl::copy(text1, strlen(text1), 2); free(text1); }
    void                        text_duplicate_line_or_selection();
    gnu::FileBuf                text_get_buffer() const
                                    { return _buf1->get(); }
    gnu::FileBuf                text_get_buffer_with_trim_and_line() const
                                    { return _buf1->get(_file_info.flineending, false); }
    char*                       text_get_selection() const
                                    { return _buf1->selection_text(); }
    std::string                 text_get_selection_string() const
                                    { auto s = _buf1->selection_text(); std::string res = s; free(s); return res; }
    bool                        text_has_dnd_event() const
                                    { return _editor_flags.dnd; }
    bool                        text_has_kommand() const
                                    { return _editor_flags.kommand; }
    bool                        text_has_selection()
                                    { return _statusbar_info.start != _statusbar_info.end || _buf1->has_selection(); }
    void                        text_insert_from_clipboard()
                                    { assert(_view); Fl::paste(*_view, 1); }
    void                        text_insert_tab_or_move_lines_left_right(FMOVEH move);
    bool                        text_is_dirty() const
                                    { return _buf1->is_dirty(); }
    bool                        text_is_readonly() const
                                    { return _editor_flags.ro; }
    int                         text_length() const
                                    { return _buf1->length(); }
    void                        text_move_lines(FMOVEV move);
    int                         text_remove_trailing();
    void                        text_select_line();
    void                        text_select_word();
    void                        text_set(const char* text = "", FLINEENDING flineending = FLINEENDING::UNIX, uint64_t fletcher64 = 0);
    void                        text_set_dirty(bool value, bool force_send = false)
                                    { _buf1->set_dirty(value, force_send); }
    void                        text_set_dnd_event(bool value)
                                    {  _editor_flags.dnd = value; }
    void                        text_set_kommand(bool value)
                                    { _editor_flags.kommand = value; }
    void                        text_set_readonly(bool value);
    void                        text_sort_lines(FSORT order = FSORT::ASCENDING);
    FTAB                        text_tab_mode() const
                                    { return _editor_flags.tab_mode; }
    void                        text_tab_mode(FTAB value)
                                    { _editor_flags.tab_mode = value; }
    void                        text_tab_width(unsigned value)
                                    { if (value >= 1 && value <= limits::TAB_WIDTH_MAX) _editor_flags.tab_width = value; }
    unsigned                    text_tab_width() const
                                    { return _editor_flags.tab_width; }
    void                        text_to_space();
    void                        text_to_tab();
    void                        undo_back(bool all = false);
    void                        undo_forward(bool all = false);
    FUNDO                       undo_mode() const
                                    { return _buf1->undo_mode(); }
    void                        update_after_focus();
    void                        update_autocomplete(const char* text = nullptr);
    void                        update_pref();
    void                        update_statusbar();
    void                        update_textinfo();
    View&                       view()
                                    { assert(_view); return *_view; }
    View*                       view1()
                                    { return _view1; }
    View*                       view2()
                                    { return _view2; }
    void                        view_activate(int num = 1)
                                    { _view = (num == 2 && _view2 != nullptr) ? _view2 : _view1; }
    View*                       view_not_active()
                                    { return (_view == _view1) ? _view2 : _view1; }
    void                        view_reset_split_size()
                                    { _editors->split_pos(-1); }
    void                        view_set(View* view)
                                    { assert(view); _view = view; }
    void                        view_set_split(FSPLITVIEW fsplit);
    FSPLITVIEW                  view_split() const
                                    { return (_view2 == nullptr) ? FSPLITVIEW::NO : (_editors->direction() == flw::SplitGroup::DIRECTION::VERTICAL) ? FSPLITVIEW::VERTICAL : FSPLITVIEW::HORIZONTAL; }
    unsigned                    wrap_col() const
                                    { return _editor_flags.wrap_col; }
    FWRAP                       wrap_mode() const
                                    { return _editor_flags.fwrap; }
    void                        wrap_set_col(unsigned value)
                                    { _editor_flags.wrap_col = value; }
    void                        wrap_set_mode(FWRAP fwrap)
                                    { _editor_flags.fwrap = fwrap; _editor_flags.wrap_col = 0; update_pref();  }
    void                        wrap_toggle_mode();
    static inline void          ShowKeyboardSetup(Config& config)
                                    { dlg::keyboard(config); }
    static inline void          ShowScheme(Config& config)
                                    { dlg::scheme(config); }
    static inline void          ShowSetup(Config& config)
                                    { dlg::config(config); }
    static inline void          ShowTweaks()
                                    { dlg::tweaks(); }
private:
    static void                 CallbackFind(Fl_Widget* sender, void* data);
    static void                 CallbackOutput(Fl_Widget* w, void* o);
    Bookmarks                   _bookmarks;
    Config&                     _config;
    CursorPos                   _saved_cursor;
    EditorFlags                 _editor_flags;
    FileInfo                    _file_info;
    FindBar*                    _findbar;
    Fl_Menu_Button*             _menu;
    StatusBarInfo               _statusbar_info;
    StringSet                   _words;
    Style*                      _style;
    TextBuffer*                 _buf1;
    TextBuffer*                 _buf2;
    View*                       _view1;
    View*                       _view2;
    View*                       _view;
    flw::ScrollBrowser*         _output;
    flw::SplitGroup*            _editors;
    flw::SplitGroup*            _main;
    gnu::PCRE*                  _regex;
    std::string                 _scheme;
};
}
#ifdef GNU_USE_SQLITE3
#include "sqlite3.h"
#include <cstring>
#include <sys/timeb.h>
#include <time.h>
namespace gnu {
static int64_t _db_secs_since_epoch() {
#ifdef _WIN32
    struct timeb timeVal;
    ftime(&timeVal);
    return timeVal.time;
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec;
#endif
}
DBRow::DBRow(const std::string KEY, const char* VALUE, const size_t SIZE, const int64_t TIME) {
    value = nullptr;
    size  = SIZE;
    key   = KEY;
    time  = TIME;
    if (VALUE == nullptr) {
        return;
    }
    value = static_cast<char*>(calloc(SIZE + 1, 1));
    if (value == nullptr) {
        throw "error: memory allocation failed";
    }
    memcpy(value, VALUE, SIZE);
}
DBRow::DBRow(const DBRow& r) {
    value = nullptr;
    size  = r.size;
    key   = r.key;
    time  = r.time;
    if (r.value == nullptr) {
        return;
    }
    value = static_cast<char*>(calloc(r.size + 1, 1));
    if (value == nullptr) {
        throw "error: memory allocation failed";
    }
    memcpy(value, r.value, r.size);
}
DBRow& DBRow::operator=(const DBRow& r) {
    if (this == &r) {
        return *this;
    }
    free(value);
    value = nullptr;
    size  = r.size;
    key   = r.key;
    time  = r.time;
    if (r.value == nullptr) {
        return *this;
    }
    value = static_cast<char*>(calloc(r.size + 1, 1));
    if (value == nullptr) {
        throw "error: memory allocation failed";
    }
    memcpy(value, r.value, r.size);
    return *this;
}
void DBRow::debug(bool print_value) const {
    printf("DBRow(%s): size(%u), time(%lld), value(%s)\n", key.c_str(), (unsigned) size, (long long int) time, (print_value == true) ? (value != nullptr) ? value : "NULL" : "not shown");
    fflush(stdout);
}
void DBRow::Debug(const DBRowVector& v, bool print_value) {
    printf("\nDBRow[%d]:\n", (int) v.size());
    for (const auto& k : v) k.debug(print_value);
}
DB::DB() {
    _sql  = nullptr;
    _stmt = nullptr;
    err_msg  = "error: database is closed";
    err_code = SQLITE_ERROR;
}
DB::DB(std::string filename) {
    err_code = 0;
    _sql     = nullptr;
    _stmt    = nullptr;
    open(filename);
}
DB::DB(DB&& other) {
    err_code    = other.err_code;
    err_msg     = other.err_msg;
    _filename   = other._filename;
    _sql        = other._sql;
    _stmt       = other._stmt;
    other._sql  = nullptr;
    other._stmt = nullptr;
}
DB& DB::operator=(DB&& other) {
    err_code    = other.err_code;
    err_msg     = other.err_msg;
    _filename   = other._filename;
    _sql        = other._sql;
    _stmt       = other._stmt;
    other._sql  = nullptr;
    other._stmt = nullptr;
    return *this;
}
bool DB::_clear_error_and_free_stmt() {
    _clear_error_and_free_stmt_and_return_changes();
    return true;
}
int DB::_clear_error_and_free_stmt_and_return_changes() {
    sqlite3_finalize(_stmt);
    _stmt = nullptr;
    err_msg = "";
    err_code = 0;
    return (_sql != nullptr) ? sqlite3_changes(_sql) : 0;
}
bool DB::close() {
    _filename = "";
    if (_sql == nullptr) {
        return _error_invalid_arguments();
    }
    if (sqlite3_close(_sql) != SQLITE_OK) {
        _sql = nullptr;
        return _set_error_and_free_stmt();
    }
    _sql = nullptr;
    return _clear_error_and_free_stmt();
}
int64_t DB::_count(std::string what) {
    if (_sql == nullptr) {
        _error_invalid_arguments();
        return -1;
    }
    if (what == "size") {
        _prepare("SELECT SUM(LENGTH(value)) FROM kv");
    }
    else {
        _prepare("SELECT COUNT(rowid) FROM kv");
    }
    if (_stmt == nullptr) {
        return -1;
    }
    if (sqlite3_step(_stmt) != SQLITE_ROW) {
        _set_error_and_free_stmt();
        return -1;
    }
    auto res = sqlite3_column_int64(_stmt, 0);
    _clear_error_and_free_stmt();
    return res;
}
bool DB::Defrag(std::string& err, const std::string filename) {
    auto db  = DB(filename);
    auto res = false;
    if (db.is_open() == true) {
        res = db.defrag();
    }
    err = db.err_msg;
    return res;
}
bool DB::_error_invalid_arguments() {
    err_msg  = (_sql == nullptr) ? "error: database is closed" : "error: invalid arguments";
    err_code = SQLITE_ERROR;
    return false;
}
bool DB::execute(const std::string sql) {
    if (_sql == nullptr || sql == "") {
        return _error_invalid_arguments();
    }
    if (sqlite3_exec(_sql, sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        return _set_error_and_free_stmt();
    }
    return _clear_error_and_free_stmt();
}
DBRow DB::get(const std::string key) {
    if (_sql == nullptr || key == "") {
        _error_invalid_arguments();
        return DBRow();
    }
    if (_prepare("SELECT time, value FROM kv WHERE key = ?") == false) {
        return DBRow();
    }
    if (sqlite3_bind_text(_stmt, 1, key.c_str(), key.size(), SQLITE_STATIC) != SQLITE_OK) {
        _set_error_and_free_stmt();
        return DBRow();
    }
    if (sqlite3_step(_stmt) != SQLITE_ROW) {
        _set_error_and_free_stmt();
        return DBRow();
    }
    auto t = sqlite3_column_int64(_stmt, 0);
    auto s = sqlite3_column_bytes(_stmt, 1);
    auto v = static_cast<const char*>(sqlite3_column_blob(_stmt, 1));
    auto r   = DBRow(key, v, s, t);
    _clear_error_and_free_stmt();
    return r;
}
DBRowVector DB::_get_all(const std::string key, const std::string remove_string_from_key, const int64_t from_time, const int64_t to_time) {
    auto res = DBRowVector();
    auto time = from_time >= 0 && to_time >= 0;
    if (_sql == nullptr) {
        _error_invalid_arguments();
        return res;
    }
    if (key == "" && time == true && (
            _prepare("SELECT key, time, value FROM kv WHERE time >= ? AND time <= ? ORDER BY key") == false ||
            sqlite3_bind_int64(_stmt, 1, from_time) != SQLITE_OK ||
            sqlite3_bind_int64(_stmt, 2, to_time) != SQLITE_OK)
        ) {
        _set_error_and_free_stmt();
        return res;
    }
    if (key == "" && time == false && _prepare("SELECT key, time, value FROM kv ORDER BY key") == false) {
        return res;
    }
    if (key != "" && time == true && (
            _prepare("SELECT key, time, value FROM kv WHERE key like ? AND time >= ? AND time <= ? ORDER BY key") == false ||
            sqlite3_bind_text(_stmt, 1, key.c_str(), key.size(), SQLITE_STATIC) != SQLITE_OK ||
            sqlite3_bind_int64(_stmt, 2, from_time) != SQLITE_OK ||
            sqlite3_bind_int64(_stmt, 3, to_time) != SQLITE_OK)
        ) {
        _set_error_and_free_stmt();
        return res;
    }
    if (key != "" && time == false && (
            _prepare("SELECT key, time, value FROM kv WHERE key like ? ORDER BY key") == false ||
            sqlite3_bind_text(_stmt, 1, key.c_str(), key.size(), SQLITE_STATIC) != SQLITE_OK)
        ) {
        _set_error_and_free_stmt();
        return res;
    }
    while (sqlite3_step(_stmt) == SQLITE_ROW) {
        auto k = (std::string) reinterpret_cast<const char*>(sqlite3_column_text(_stmt, 0));
        auto t = sqlite3_column_int64(_stmt, 1);
        auto s = sqlite3_column_bytes(_stmt, 2);
        auto v = static_cast<const char*>(sqlite3_column_blob(_stmt, 2));
        if (remove_string_from_key.length() > 0 && remove_string_from_key.length() < k.length() && k.find(remove_string_from_key) == 0) {
            k = k.substr(remove_string_from_key.length());
        }
        res.push_back(DBRow(k, v, s, t));
    }
    if (res.size() == 0) {
        _set_error_and_free_stmt();
        return res;
    }
    _clear_error_and_free_stmt();
    return res;
}
bool DB::key(const std::string key) {
    if (_sql == nullptr || key == "") {
        _error_invalid_arguments();
        return false;
    }
    if (_prepare("SELECT time FROM kv WHERE key = ?") == false) {
        return false;
    }
    if (sqlite3_bind_text(_stmt, 1, key.c_str(), key.size(), SQLITE_STATIC) != SQLITE_OK) {
        _set_error_and_free_stmt();
        return false;
    }
    if (sqlite3_step(_stmt) != SQLITE_ROW) {
        _set_error_and_free_stmt();
        return false;
    }
    _clear_error_and_free_stmt();
    return true;
}
DBRowVector DB::_keys(const std::string key, const std::string remove_string_from_key, const int64_t from_time, const int64_t to_time) {
    auto res = DBRowVector();
    auto time = from_time >= 0 && to_time >= 0;
    if (_sql == nullptr) {
        _error_invalid_arguments();
        return res;
    }
    if (key == "" && time == true && (
            _prepare("SELECT key, LENGTH(value), time FROM kv WHERE time >= ? AND time <= ? ORDER BY key") == false ||
            sqlite3_bind_int64(_stmt, 1, from_time) != SQLITE_OK ||
            sqlite3_bind_int64(_stmt, 2, to_time) != SQLITE_OK)
        ) {
        _set_error_and_free_stmt();
        return res;
    }
    if (key == "" && time == false && _prepare("SELECT key, LENGTH(value), time FROM kv ORDER BY key") == false) {
        _set_error_and_free_stmt();
        return res;
    }
    if (key != "" && time == true && (
            _prepare("SELECT key, LENGTH(value), time FROM kv WHERE key like ? AND time >= ? AND time <= ? ORDER BY key") == false ||
            sqlite3_bind_text(_stmt, 1, key.c_str(), key.size(), SQLITE_STATIC) != SQLITE_OK ||
            sqlite3_bind_int64(_stmt, 2, from_time) != SQLITE_OK ||
            sqlite3_bind_int64(_stmt, 3, to_time) != SQLITE_OK)
        ) {
        _set_error_and_free_stmt();
        return res;
    }
    if (key != "" && time == false && (
            _prepare("SELECT key, LENGTH(value), time FROM kv WHERE key like ? ORDER BY key") == false ||
            sqlite3_bind_text(_stmt, 1, key.c_str(), key.size(), SQLITE_STATIC) != SQLITE_OK)
        ) {
        _set_error_and_free_stmt();
        return res;
    }
    while (sqlite3_step(_stmt) == SQLITE_ROW) {
        auto k = (std::string) reinterpret_cast<const char*>(sqlite3_column_text(_stmt, 0));
        auto s = sqlite3_column_int64(_stmt, 1);
        auto t = sqlite3_column_int64(_stmt, 2);
        if (remove_string_from_key.length() > 0 && remove_string_from_key.length() < k.length() && k.find(remove_string_from_key) == 0) {
            k = k.substr(remove_string_from_key.length());
        }
        res.push_back(DBRow(k, (const char*) nullptr, (size_t) s, t));
    }
    _clear_error_and_free_stmt();
    return res;
}
DBRow DB::Load(std::string& err, const std::string filename, const std::string key) {
    auto res = DBRow();
    auto db = DB(filename);
    if (db.is_open() == true) {
        res = db.get(key);
    }
    err = db.err_msg;
    return res;
}
DBRowVector DB::LoadRows(std::string& err, const std::string filename, const std::string key) {
    auto res = DBRowVector();
    auto db = DB(filename);
    if (db.is_open() == true) {
        res = db.get_all(key);
    }
    err = db.err_msg;
    return res;
}
bool DB::open(const std::string filename) {
    close();
    if (filename == "") {
        err_msg  = "error: empty filename";
        err_code = SQLITE_ERROR;
        return false;
    }
    if (sqlite3_open(filename.c_str(), &_sql) != SQLITE_OK) {
        return _set_error_and_close_db();
    }
    if (execute("CREATE TABLE IF NOT EXISTS kv(key TEXT PRIMARY KEY, time INTEGER NOT NULL, value BLOB NOT NULL)") == false) {
        return _set_error_and_close_db();
    }
    if (execute("CREATE INDEX IF NOT EXISTS kv_time ON kv(time)") == false) {
        return _set_error_and_close_db();
    }
    execute("PRAGMA synchronous = normal");
    execute("PRAGMA temp_store = memory");
    execute("PRAGMA case_sensitive_like = ON");
    _filename = filename;
    return _clear_error_and_free_stmt();
}
bool DB::_prepare(const char* sql) {
    if (sqlite3_prepare_v2(_sql, sql, -1, &_stmt, 0) != SQLITE_OK) {
        _set_error_and_free_stmt();
        return false;
    }
    return true;
}
bool DB::put(std::string key, const char* in, size_t in_size, int64_t time) {
    if (_sql == nullptr || key == "" || in == nullptr) {
        return _error_invalid_arguments();
    }
    if (time < 0) {
        time = _db_secs_since_epoch();
    }
    if (_prepare("REPLACE INTO kv(key, time, value) VALUES(?, ?, ?)") == false) {
        return false;
    }
    if (sqlite3_bind_text(_stmt, 1, key.c_str(), key.size(), SQLITE_STATIC) != SQLITE_OK) {
        return _set_error_and_free_stmt();
    }
    if (sqlite3_bind_int64(_stmt, 2, time) != SQLITE_OK) {
        return _set_error_and_free_stmt();
    }
    if (sqlite3_bind_blob(_stmt, 3, in, in_size, SQLITE_STATIC) != SQLITE_OK) {
        return _set_error_and_free_stmt();
    }
    if (sqlite3_step(_stmt) != SQLITE_DONE) {
        return _set_error_and_free_stmt();
    }
    return _clear_error_and_free_stmt();
}
int DB::_remove(const std::string key, bool many) {
    if (_sql == nullptr || key == "") {
        return _error_invalid_arguments();
    }
    if (many == false && _prepare("DELETE FROM kv WHERE key = ?") == false) {
        return 0;
    }
    else if (many == true && _prepare("DELETE FROM kv WHERE key like ?") == false) {
        return 0;
    }
    if (sqlite3_bind_text(_stmt, 1, key.c_str(), key.size(), SQLITE_STATIC) != SQLITE_OK) {
        return _set_error_and_free_stmt();
    }
    if (sqlite3_step(_stmt) != SQLITE_DONE) {
        return _set_error_and_free_stmt();
    }
    return _clear_error_and_free_stmt_and_return_changes();
}
bool DB::rename(const std::string key, const std::string new_key) {
    if (_sql == nullptr || key == "" || new_key == "" || key == new_key) {
        return _error_invalid_arguments();
    }
    if (_prepare("UPDATE kv SET key = ? WHERE key = ?") == false) {
        return false;
    }
    if (sqlite3_bind_text(_stmt, 1, new_key.c_str(), new_key.size(), SQLITE_STATIC) != SQLITE_OK) {
        return _set_error_and_free_stmt();
    }
    if (sqlite3_bind_text(_stmt, 2, key.c_str(), key.size(), SQLITE_STATIC) != SQLITE_OK) {
        return _set_error_and_free_stmt();
    }
    if (sqlite3_step(_stmt) != SQLITE_DONE) {
        return _set_error_and_free_stmt();
    }
    return _clear_error_and_free_stmt_and_return_changes();
}
bool DB::Save(std::string& err, const std::string filename, const DBRow& row) {
    auto db  = DB(filename);
    auto res = false;
    if (db.is_open() == true) {
        res = db.put(row);
    }
    err = db.err_msg;
    return res;
}
bool DB::Save(std::string& err, const std::string filename, const std::string key, const char* value, const size_t size, const int64_t time) {
    auto db  = DB(filename);
    auto res = false;
    if (db.is_open() == true) {
        res = db.put(key, value, size, time);
    }
    err = db.err_msg;
    return res;
}
bool DB::SaveRows(std::string& err, const std::string filename, const DBRowVector& rows) {
    auto db    = DB(filename);
    auto count = (size_t) 0;
    err = "";
    if (db.is_open() == false) {
        err = db.err_msg;
        return false;
    }
    db.begin();
    for (auto& r : rows) {
        count += db.put(r);
        if (err != "") {
            err = db.err_msg;
        }
    }
    if (count != rows.size() || err != "") {
        db.rollback();
        return false;
    }
    else {
        auto res = db.commit();
        err = db.err_msg;
        return res;
    }
}
bool DB::_set_error_and_close_db() {
    if (_sql == nullptr) {
        err_msg = "error: database could not be opened";
        err_code = SQLITE_ERROR;
    }
    else {
        _set_error_and_free_stmt();
        sqlite3_close(_sql);
        _sql = nullptr;
    }
    return false;
}
bool DB::_set_error_and_free_stmt() {
    if (_sql != nullptr) {
        err_msg = sqlite3_errmsg(_sql);
        err_code = sqlite3_errcode(_sql);
    }
    else {
        err_msg = "error: database is closed";
        err_code = SQLITE_ERROR;
    }
    sqlite3_finalize(_stmt);
    _stmt = nullptr;
    return false;
}
std::string DB::Version() {
    return SQLITE_VERSION;
}
}
#endif
#include <cstring>
#include <assert.h>
namespace gnu {
static const char _PILE_SKEY = '#';
static const char _PILE_SDAT = '=';
static const char _PILE_TSTR = '^';
static const char _PILE_TESC = '!';
static int8_t _PILE_BIN[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
static const char _PILE_HEX[513] =
    "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f"
    "404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f"
    "808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
    "c0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";
static bool _pile_check_key(const std::string& a, const std::string& b) {
    if (a == "" || b == "") {
        return false;
    }
    for (unsigned char c : a) {
        if (c < 33 || c == _PILE_SKEY || c == _PILE_SDAT) return false;
    }
    for (unsigned char c : b) {
        if (c < 33 || c == _PILE_SKEY || c == _PILE_SDAT) return false;
    }
    return true;
}
static bool _pile_check_escape(const std::string& str) {
    if (str == "") return false;
    for (unsigned char c : str) {
        if (c >= 10 && c <= 13) return true;
    }
    return false;
}
static PileBuf _pile_from_hex(const char* value, size_t len) {
    if (value == nullptr || len == 0) return PileBuf();
    if (len % 2 != 0) return PileBuf();
    auto   buf = PileBuf(len / 2);
    size_t pos = 0;
    for (size_t f = 0; f < len; f += 2) {
        int8_t c1 = _PILE_BIN[(uint8_t) value[f]];
        int8_t c2 = _PILE_BIN[(uint8_t) value[f + 1]];
        if (c1 < 0 || c2 < 0) return PileBuf();
        buf.p[pos++] = c1 * 16 + c2;
    }
    return buf;
}
static std::string _pile_to_hex(const char* value, size_t len) {
    if (value == nullptr || len == 0) return "";
    std::string res;
    res.reserve(len * 2 + 1);
    for (size_t f = 0; f < len; f++) {
        uint8_t b = value[f];
        int     c = b * 2;
        res += _PILE_HEX[c++];
        res += _PILE_HEX[c];
    }
    return res;
}
static std::string _pile_from_escaped(const std::string& str) {
    std::string res;
    res.reserve(str.length());
    for (size_t f = 1; f < str.length(); f++) {
        char c = str[f];
        char n = str[f + 1];
        char u = 0;
        if (c == '\\') {
            if (n == 'n')      u = '\n';
            else if (n == 'v') u = '\v';
            else if (n == 'f') u = '\f';
            else if (n == 'r') u = '\r';
        }
        if (u != 0) {
            res += u;
            f++;
        }
        else res += c;
    }
    return res;
}
static std::string _pile_to_escaped(const std::string& str) {
    std::string res;
    res.reserve((size_t) (str.length() * 1.1));
    for (unsigned c : str) {
        if (c == 10) res += "\\n";
        else if (c == 11) res += "\\v";
        else if (c == 12) res += "\\f";
        else if (c == 13) res += "\\r";
        else res += c;
    }
    return res;
}
static std::vector<std::string> _pile_split(char* string, char split) {
    assert(string && split);
    auto res   = std::vector<std::string>();
    auto found = strchr(string, split);
    while (found != nullptr) {
        *found = 0;
        res.push_back(string);
        *found = split;
        string = found + 1;
        found = strchr(string, split);
    }
    res.push_back(string);
    return res;
}
PileBuf::PileBuf(size_t S) {
    p = (S < SIZE_MAX) ? static_cast<char*>(calloc(S + 1, 1)) : nullptr;
    if (p == nullptr) {
        throw "error: memory allocation failed";
    }
    s = S;
}
PileBuf::PileBuf(const char* P, size_t S) {
    if (P == nullptr) {
        p = nullptr;
        s = 0;
        return;
    }
    p = (S < SIZE_MAX) ? static_cast<char*>(calloc(S + 1, 1)) : nullptr;
    s = 0;
    if (p == nullptr) {
        throw "error: memory allocation failed";
    }
    memcpy(p, P, S);
    s = S;
}
PileBuf::PileBuf(const PileBuf& b) {
    if (b.p == nullptr) {
        p = nullptr;
        s = 0;
        return;
    }
    p = (b.s < SIZE_MAX) ? static_cast<char*>(calloc(b.s + 1, 1)) : nullptr;
    s = 0;
    if (p == nullptr) {
        throw "error: memory allocation failed";
    }
    memcpy(p, b.p, b.s);
    s = b.s;
}
PileBuf& PileBuf::operator=(const PileBuf& b) {
    if (this == &b) {
        return *this;
    }
    else if (b.p == nullptr) {
        free(p);
        p = nullptr;
        s = 0;
        return *this;
    }
    free(p);
    p = (b.s < SIZE_MAX) ? static_cast<char*>(calloc(b.s + 1, 1)) : nullptr;
    s = 0;
    if (p == nullptr) {
        throw "error: memory allocation failed";
    }
    memcpy(p, b.p, b.s);
    s = b.s;
    return *this;
}
PileBuf& PileBuf::operator+=(const PileBuf& b) {
    if (b.p == nullptr) {
        return *this;
    }
    if (p == nullptr) {
        *this = b;
        return *this;
    }
    auto t = (b.s < SIZE_MAX) ? static_cast<char*>(calloc(s + b.s + 1, 1)) : nullptr;
    if (t == nullptr) {
        throw "error: memory allocation failed";
    }
    memcpy(t, p, s);
    memcpy(t + s, b.p, b.s);
    free(p);
    p = t;
    s += b.s;
    return *this;
}
bool PileBuf::operator==(const PileBuf& other) const {
    return p != nullptr && s == other.s && memcmp(p, other.p, s) == 0;
}
void Pile::debug() const {
    int c = 1;
    for (auto& v : _values) {
        if (v.second.length() > 140) {
            printf("%5d: %-20s| %s ...\n", c++, v.first.c_str(), v.second.substr(0, 140).c_str());
        }
        else {
            printf("%5d: %-20s| %s\n", c++, v.first.c_str(), v.second.c_str());
        }
    }
    fflush(stdout);
}
std::string Pile::export_data() const {
    size_t s = 0;
    for (auto& v : _values) {
        s += v.first.length() + v.second.length() + 2;
    }
    auto res = std::string();
    res.reserve(s + 5);
    for (const auto& m : _values) {
        res += m.first + _PILE_SDAT + m.second + "\n";
    }
    return res;
}
PileBuf Pile::get_buf(std::string section, std::string key) const {
    if (_pile_check_key(section, key) == false) return PileBuf();
    auto v = _values.find(section + _PILE_SKEY + key);
    if (v == _values.end() || v->second.length() == 0 || v->second.front() == _PILE_TSTR || v->second.front() == _PILE_TESC) {
        return PileBuf();
    }
    return _pile_from_hex(v->second.c_str(), v->second.length());
}
double Pile::get_double(std::string section, std::string key, double def) const {
    std::string n = get_string(section, key, "");
    try {
        return std::stod(n);
    }
    catch (...) {
        return def;
    }
}
int64_t Pile::get_int(std::string section, std::string key, int64_t def) const {
    std::string n = get_string(section, key, "");
    try {
        return std::stoll(n);
    }
    catch (...) {
        return def;
    }
}
std::string Pile::get_string(std::string section, std::string key, std::string def) const {
    if (_pile_check_key(section, key) == false) return def;
    auto v = _values.find(section + _PILE_SKEY + key);
    if (v == _values.end() || v->second.length() == 0 || (v->second.front() != _PILE_TSTR && v->second.front() != _PILE_TESC)) {
        return def;
    }
    else if (v->second.front() == _PILE_TESC) {
        return _pile_from_escaped(v->second);
    }
    else {
        return v->second.substr(1);
    }
}
size_t Pile::import_data(char* values) {
    clear();
    if (values == nullptr || *values == 0) {
        return 0;
    }
    auto lines = _pile_split(values, '\n');
    for (auto& line : lines) {
        auto l  = const_cast<char*>(line.c_str());
        auto ik = static_cast<char*>(strchr(l, _PILE_SKEY));
        auto iv = static_cast<char*>(strchr(l, _PILE_SDAT));
        if (iv != nullptr && ik != nullptr) {
            auto section = l;
            auto key     = ik + 1;
            auto value   = iv + 1;
            *ik = 0;
            *iv = 0;
            if (_pile_check_key(section, key) == true) {
                *ik = _PILE_SKEY;
                _values[l] = value;
            }
        }
    }
    return _values.size();
}
std::vector<std::string> Pile::keys(std::string section) const {
    auto res = std::vector<std::string>();
    if (section == "") {
        for (auto& m : _values) {
            res.push_back(m.first);
        }
    }
    else {
        auto name = std::string(section) + _PILE_SKEY;
        auto stop = false;
        for (auto& m : _values) {
            if (m.first.find(name) == 0) {
                res.push_back(m.first.substr(name.length()));
                stop = true;
            }
            else if (stop == true) {
                return res;
            }
        }
    }
    return res;
}
std::string Pile::make_key(unsigned key, uint8_t w) {
    char b[50];
    snprintf(b, 50, "%0*u", w, key);
    return b;
}
std::vector<std::string> Pile::sections() const {
    std::vector<std::string> res;
    std::string              section;
    for (const auto& m : _values) {
        auto s = m.first;
        auto p = s.find(_PILE_SKEY);
        if (p != std::string::npos) {
            s = s.substr(0, p);
            if (s != section) {
                res.push_back(s);
            }
            section = s;
        }
    }
    return res;
}
bool Pile::set(std::string section, std::string key, const char* value, size_t value_len) {
    if (_pile_check_key(section, key) == false) {
        return false;
    }
    _values[section + _PILE_SKEY + key] = _pile_to_hex(value, value_len);
    return true;
}
bool Pile::set_string(std::string section, std::string key, std::string value) {
    if (_pile_check_key(section, key) == false) {
        return false;
    }
    if (_pile_check_escape(value) == true) {
        _values[section + _PILE_SKEY + key] = _PILE_TESC + _pile_to_escaped(value);
    }
    else {
        _values[section + _PILE_SKEY + key] = _PILE_TSTR + value;
    }
    return true;
}
}
#ifdef GNU_USE_PCRE
#include "pcre8.h"
#include <algorithm>
#include <cstring>
#include <cstdarg>
namespace gnu {
static std::string _pcre_format(const char* format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    auto n = vsnprintf(buf, 256, format, args);
    va_end(args);
    return (n < 0 || n >= 256) ? "" : buf;
}
void _pcre_replace(std::string& string, const std::string& find, const std::string& replace) {
    size_t start = 0;
    while ((start = string.find(find, start)) != std::string::npos) {
        string.replace(start, find.length(), replace);
        start += replace.length();
    }
}
PCRE::PCRE() {
    _pcre = nullptr;
    clear();
    pcre_config(PCRE_CONFIG_UTF8, &_utf);
}
PCRE::PCRE(PCRE&& other) {
    _error   = other._error;
    _matches = other._matches;
    _pattern = other._pattern;
    _pcre    = other._pcre;
    _subject = other._subject;
    _utf     = other._utf;
    memcpy(other._off, _off, PCRE::MAX_CAPTURES * sizeof(int));
    other._pcre = nullptr;
}
PCRE::PCRE(std::string pattern, bool utf) {
    _pcre = nullptr;
    clear();
    pcre_config(PCRE_CONFIG_UTF8, &_utf);
    compile(pattern, utf);
}
PCRE::~PCRE() {
    pcre_free(_pcre);
}
PCRE& PCRE::operator=(PCRE&& other) {
    pcre_free(_pcre);
    _error   = other._error;
    _matches = other._matches;
    _pattern = other._pattern;
    _pcre    = other._pcre;
    _subject = other._subject;
    _utf     = other._utf;
    memcpy(_off, other._off, PCRE::MAX_CAPTURES * sizeof(int));
    other._pcre = nullptr;
    return *this;
}
void PCRE::clear() {
    pcre_free(_pcre);
    memset(_off, 0, PCRE::MAX_CAPTURES * sizeof(int));
    _matches = 0;
    _pattern = "";
    _pcre    = nullptr;
    _subject = "";
    _error   = "";
}
bool PCRE::compile(std::string pattern, bool utf) {
    clear();
    if (_utf != 1 && utf == true) {
        _error = "error: utf is requested but has not been turned on in library";
        return false;
    }
    if (pattern == "") {
        _error = "error: empty pattern string";
        return false;
    }
    auto error_str = (const char*) nullptr;
    auto error_off = 0;
    auto flags     = (utf == true && _utf == 1) ? (PCRE_UTF8 | PCRE_UCP) : 0;
    _pattern = pattern;
    _pcre    = pcre_compile(pattern.c_str(), flags, &error_str, &error_off, nullptr);
    if (_pcre == nullptr) {
        _error = _pcre_format("error: %s at char %d", error_str, error_off);
        return false;
    }
    return true;
}
void PCRE::debug() {
    printf("PCRE(%d.%d):\n", PCRE_MAJOR, PCRE_MINOR);
    printf("    compiled: %s\n", (is_compiled() == true) ? "true" : "false");
    printf("    utf:      %s\n", _utf == 1 ? "true" : "false");
    printf("    pattern:  \"%s\"\n", _pattern.c_str());
    printf("    subject:  \"%s\"\n", _subject.c_str());
    printf("    error:    %s\n", _error.c_str());
    printf("    length:   %d\n", (int) _subject.length());
    printf("    matches:  %u\n", (unsigned) _matches);
    for (size_t f = 0; f < _matches; f++) {
        auto Start = 0;
        auto Stop  = 0;
        offset(f, Start, Stop);
        printf("    match:    %u: (%2d, %2d)  \"%s\"\n", (unsigned) f, Start, Stop, substr(f).c_str());
    }
    printf("\n");
    fflush(stdout);
}
std::string PCRE::Escape(std::string string) {
    _pcre_replace(string, "\\", "\\\\");
    _pcre_replace(string, "$", "\\$");
    _pcre_replace(string, "(", "\\(");
    _pcre_replace(string, ")", "\\)");
    _pcre_replace(string, "*", "\\*");
    _pcre_replace(string, "+", "\\+");
    _pcre_replace(string, "-", "\\-");
    _pcre_replace(string, ".", "\\.");
    _pcre_replace(string, "?", "\\?");
    _pcre_replace(string, "[", "\\[");
    _pcre_replace(string, "]", "\\]");
    _pcre_replace(string, "^", "\\^");
    _pcre_replace(string, "{", "\\{");
    _pcre_replace(string, "|", "\\|");
    _pcre_replace(string, "}", "\\}");
    return string;
}
size_t PCRE::exec(std::string subject, bool not_bol, bool not_eol) {
    _error   = "";
    _subject = "";
    _matches = 0;
    if (_pcre == nullptr) {
        _error = "error: pcre is null";
        return 0;
    }
    memset(_off, 0, PCRE::MAX_CAPTURES * sizeof(int));
    auto option  = ((not_bol == true) ? PCRE_NOTBOL : 0) | ((not_eol == true) ? PCRE_NOTEOL : 0);
    auto Matches = pcre_exec(static_cast<pcre*>(_pcre), nullptr, subject.c_str(), subject.length(), 0,  option, _off, PCRE::MAX_CAPTURES);
    if (Matches > 0) {
        _subject = subject;
        _matches = Matches;
    }
    return _matches;
}
size_t PCRE::exec_next() {
    _error = "";
    if (_pcre == nullptr) {
        _error = "error: pcre is null";
        _matches = 0;
        return 0;
    }
    if (_matches < 1) {
        _error = "error: no matches";
        _matches = 0;
        return 0;
    }
    auto End = this->end(_matches - 1);
    if (End < 0) {
        _matches = 0;
        return 0;
    }
    memset(_off, 0, PCRE::MAX_CAPTURES * sizeof(int));
    auto option  = 0;
    auto Matches = pcre_exec(static_cast<pcre*>(_pcre), nullptr, _subject.c_str(), _subject.length(), End, option, _off, PCRE::MAX_CAPTURES);
    _matches = (Matches < 1) ? 0 : Matches;
    return _matches;
}
bool PCRE::offset(size_t match, int& start, int& end) {
    start = -1;
    end   = -1;
    _error = "";
    if (_pcre == nullptr) {
        _error = "error: pcre is null";
        return false;
    }
    if (match >= _matches) {
        _error = "error: match is out of range";
        return false;
    }
    match *= 2;
    start  = _off[match];
    end    = _off[match + 1];
    if (start < 0 || end < 0 || start > end || end > (int) _subject.length()) {
        _error = "error: match is out of range";
        start  = -1;
        end    = -1;
        return false;
    }
    return true;
}
bool PCRE::offset(std::string name, int& start, int& end) {
    return offset(pcre_get_stringnumber(static_cast<pcre*>(_pcre), name.c_str()), start, end);
}
std::string PCRE::replace(size_t match, std::string replace) {
    std::string res = _subject;
    _error = "";
    if (_pcre == nullptr) {
        _error = "error: pcre is null";
        return _subject;
    }
    try {
        auto Start = 0;
        auto Stop  = 0;
        if (offset(match, Start, Stop) == true && Stop - Start > 0) {
            res.replace(Start, Stop - Start, replace);
        }
        return res;
    }
    catch(...) {
        return res;
    }
}
std::string PCRE::replace(std::vector<std::string> replace, std::string skip) {
    std::string res = _subject;
    _error = "";
    if (_pcre == nullptr) {
        _error = "error: pcre is null";
        return _subject;
    }
    size_t match = replace.size();
    std::reverse(replace.begin(), replace.end());
    for (auto& s : replace) {
        try {
            auto Start = 0;
            auto Stop  = 0;
            if (skip.length() > 0 && s == skip) {
            }
            else if (offset(match, Start, Stop) == true && Stop - Start > 0) {
                res.replace(Start, Stop - Start, s);
            }
        }
        catch(...) {
            _error = "error: exception in replace";
        }
        match--;
    }
    return res;
}
std::string PCRE::substr(size_t match) {
    std::string res;
    _error = "";
    if (_pcre == nullptr) {
        _error = "error: pcre is null";
        return "";
    }
    auto cap = (const char*) nullptr;
    if (pcre_get_substring(_subject.c_str(), _off, _matches, match, &cap) < 0) {
        _error = "error: string not found";
        return "";
    }
    res = cap;
    pcre_free_substring(cap);
    return res;
}
std::string PCRE::substr(std::string name) {
    std::string res;
    _error = "";
    if (_pcre == nullptr) {
        _error = "error: pcre is null";
        return "";
    }
    auto cap = (const char*) nullptr;
    if (pcre_get_named_substring(static_cast<pcre*>(_pcre), _subject.c_str(), _off, _matches, name.c_str(), &cap) < 0) {
        _error = "error: string not found";
        return "";
    }
    res = cap;
    pcre_free_substring(cap);
    return res;
}
std::string PCRE::to_string() const {
    return _pcre_format("PCRE(%s): matches=%d, error=%s", _pattern.c_str(), (int) _matches, _error.c_str());
}
std::string PCRE::Version() {
    return _pcre_format("%d.%d", PCRE_MAJOR, PCRE_MINOR);
}
}
#endif
#include <assert.h>
#include <algorithm>
#include <cstring>
#include <cstdarg>
#include <cmath>
namespace gnu {
static char* _str_replace(const char* in, size_t in_len, const std::string& find, const std::string& replace, size_t replacements) {
    auto res      = (char*) nullptr;
    auto buf_len  = in_len;
    auto cur_len  = (size_t) 0;
    auto iter     = in;
    auto iter_end = in + in_len;
    auto first    = *find.c_str();
    auto f_len    = find.length();
    auto f_str    = find.c_str();
    auto r_len    = replace.length();
    auto r_str    = replace.c_str();
    auto count    = replacements;
    if (in_len == 0 || f_len == 0) {
        return (char*) calloc(1, 1);
    }
    if (r_len > f_len) {
        buf_len *= 2;
    }
    res = static_cast<char*>(malloc(buf_len + 1));
    if (res == nullptr) {
        throw "error: memory allocation failed";
    }
    while (iter < iter_end) {
        if (count > 0 && *iter == first && strncmp(iter, f_str, f_len) == 0) {
            size_t size = cur_len + r_len;
            if (size >= buf_len) {
                while (size >= buf_len) {
                    buf_len *= 2;
                }
                auto res2 = static_cast<char*>(realloc(res, buf_len));
                if (res2 == nullptr) {
                    free(res);
                    throw "error: memory allocation failed";
                }
                else {
                    res = res2;
                }
            }
            memcpy(res + cur_len, r_str, r_len);
            count--;
            cur_len += r_len;
            iter += f_len;
        }
        else {
            if (cur_len + 1 >= buf_len) {
                buf_len *= 2;
                auto res2 = static_cast<char*>(realloc(res, buf_len));
                if (res2 == nullptr) {
                    free(res);
                    throw "error: memory allocation failed";
                }
                else {
                    res = res2;
                }
            }
            res[cur_len] = *iter;
            cur_len++;
            iter++;
        }
    }
    res[cur_len] = 0;
    return res;
}
std::string str::format(const char* format, ...) {
    assert(format);
    int     l = 128;
    int     n = 0;
    va_list args;
    char    buf1[128];
    char*   buf2;
    va_start(args, format);
    n = vsnprintf(buf1, l, format, args);
    va_end(args);
    if (n <= 0) {
        return "";
    }
    else if (n < l) {
        return buf1;
    }
    l = n + 1;
    buf2 = (char*) malloc(l);
    va_start(args, format);
    n = vsnprintf(buf2, l, format, args);
    va_end(args);
    std::string res = buf2;
    free(buf2);
    return res;
}
std::string str::format_int(const int64_t num, char del) {
    auto pos = 0;
    char tmp1[32];
    char tmp2[32];
    if (del < 1) {
        del = 32;
    }
    memset(tmp2, 0, 32);
    snprintf(tmp1, 32, "%lld", (long long int) num);
    auto len = strlen(tmp1);
    for (int f = len - 1, i = 0; f >= 0 && pos < 32; f--, i++) {
        char c = tmp1[f];
        if ((i % 3) == 0 && i > 0 && c != '-') {
            tmp2[pos++] = del;
        }
        tmp2[pos++] = c;
    }
    std::string r = tmp2;
    std::reverse(r.begin(), r.end());
    return r;
}
size_t str::list_append(std::vector<std::string>& strings, std::string string, size_t max_size) {
    for (auto it = strings.begin(); it != strings.end(); ++it) {
        if (*it == string) {
            strings.erase(it);
            break;
        }
    }
    strings.push_back(string);
    while (strings.size() > max_size) {
        strings.erase(strings.begin());
    }
    return strings.size();
}
size_t str::list_insert(std::vector<std::string>& strings, std::string string, size_t max_size) {
    for (auto it = strings.begin(); it != strings.end(); ++it) {
        if (*it == string) {
            strings.erase(it);
            break;
        }
    }
    strings.insert(strings.begin(), string);
    while (strings.size() > max_size) {
        strings.pop_back();
    }
    return strings.size();
}
bool str::is_whitespace(const std::string& str) {
    for (auto c : str) {
        if (c != 9 && c != 32) return false;
    }
    return true;
}
std::string& str::replace(std::string& str, std::string find, std::string replace, size_t max) {
    auto p = _str_replace(str.c_str(), str.length(), find, replace, max);
    if (p) str = p;
    free(p);
    return str;
}
std::string str::replace_const(const std::string& str, std::string find, std::string replace, size_t max) {
    auto p = _str_replace(str.c_str(), str.length(), find, replace, max);
    std::string res;
    if (p) res = p;
    free(p);
    return res;
}
const char* str::reverse(char* str, size_t len) {
    auto middle = (size_t) (len / 2);
    for (size_t f = 0; f < middle; f++) {
        auto e = len - f - 1;
        auto c = str[f];
        str[f] = str[e];
        str[e] = c;
    }
    return str;
}
std::vector<std::string> str::split(const std::string& str, char del) {
    auto res  = std::vector<std::string>();
    auto prev = (std::string::size_type) 0;
    auto pos  = (std::string::size_type) 0;
    while((pos = str.find(del, pos)) != std::string::npos) {
        auto substring = str.substr(prev, pos - prev);
        res.push_back(substring);
        prev = ++pos;
    }
    res.push_back(str.substr(prev, pos - prev));
    return res;
}
std::vector<const char*> str::split_fast(char* cstr, char split) {
    assert(cstr);
    auto res   = std::vector<const char*>();
    auto start = cstr;
    auto end   = cstr + strlen(cstr);
    auto found = strchr(start, split);
    if (split == 0) {
        res.push_back(cstr);
        return res;
    }
    while (found != nullptr) {
        *found = 0;
        res.push_back(start);
        start = found + 1;
        found = strchr(start, split);
    }
    if (start <= end) {
        res.push_back(start);
    }
    return res;
}
std::vector<std::string> str::split_std(const std::string& str, std::string split) {
    auto res = std::vector<std::string>();
    try {
        if (split == "") {
            res.push_back(str);
            return res;
        }
        auto pos1 = (std::string::size_type) 0;
        auto pos2 = str.find(split);
        while (pos2 != std::string::npos) {
            res.push_back(str.substr(pos1, pos2 - pos1));
            pos1 = pos2 + split.size();
            pos2 = str.find(split, pos1);
        }
        if (pos1 <= str.size()) {
            res.push_back(str.substr(pos1));
        }
    }
    catch(...) {
        res.clear();
    }
    return res;
}
std::string str::substr(const std::string& str, std::string::size_type pos, std::string::size_type size, std::string def) {
    try { return str.substr(pos, size); }
    catch(...) { return def; }
}
double str::to_double(std::string str, double def) {
    try { return std::stod(str, 0); }
    catch (...) { return def; }
}
long long int str::to_int(std::string str, long long int def) {
    try { return std::stoll(str, 0, 0); }
    catch (...) { return def; }
}
std::string& str::trim(std::string& str) {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](auto c) { return !std::isspace(c);} ));
    str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) { return !std::isspace(ch); }).base(), str.end());
    return str;
}
size_t str::utf_len(const char* p) {
    auto count = (size_t) 0;
    auto f     = (size_t) 0;
    auto u     = reinterpret_cast<const unsigned char*>(p);
    auto c     = (unsigned) u[0];
    while (c != 0) {
        if (c >= 128) {
            if (c >= 194 && c <= 223) {
                c = u[++f];
                if (c < 128 || c > 191) return 0;
            }
            else if (c >= 224 && c <= 239) {
                c = u[++f];
                if (c < 128 || c > 191) return 0;
                c = u[++f];
                if (c < 128 || c > 191) return 0;
            }
            else if (c >= 240 && c <= 244) {
                c = u[++f];
                if (c < 128 || c > 191) return 0;
                c = u[++f];
                if (c < 128 || c > 191) return 0;
                c = u[++f];
                if (c < 128 || c > 191) return 0;
            }
            else {
                return 0;
            }
        }
        count++;
        c = u[++f];
    }
    return count;
}
}
#include <ctime>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif
namespace gnu {
std::string Time::FormatMilliToTime(int64_t milliseconds) {
    auto h = (int64_t) milliseconds / 3'600'000;
    auto m = (int64_t) 0;
    auto s = (int64_t) 0;
    char buffer[100];
    milliseconds = milliseconds % 3'600'000;
    m            = milliseconds / 60'000;
    milliseconds = milliseconds % 60'000;
    s            = milliseconds / 1'000;
    milliseconds = milliseconds % 1'000;
    if (h == 0) {
        snprintf(buffer, 100, "%02u:%02u.%03u", (unsigned) m, (unsigned) s, (unsigned) milliseconds);
    }
    else {
        snprintf(buffer, 100, "%02u:%02u:%02u", (unsigned) h, (unsigned) m, (unsigned) s);
    }
    return buffer;
}
double Time::Clock() {
#ifdef _WIN32
    struct timeb timeVal;
    ftime(&timeVal);
    return (double) timeVal.time + (double) (timeVal.millitm / 1'000.0);
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double) (ts.tv_sec) + (ts.tv_nsec / 1'000'000'000.0);
#endif
}
std::string Time::FormatUnixToISO(int64_t seconds, bool utc, bool date_only) {
    const time_t rawtime  = (time_t) seconds;
    const tm*    timeinfo = (utc == true) ? gmtime(&rawtime) : localtime(&rawtime);
    char         buffer[100];
    if (timeinfo == nullptr) {
        return "";
    }
    if (date_only == true) {
        snprintf(buffer, 100, "%04d-%02d-%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
    }
    else {
        snprintf(buffer, 100, "%04d-%02d-%02d %02d:%02d:%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    }
    return buffer;
}
int64_t Time::Micro() {
#if defined(_WIN32)
    LARGE_INTEGER starting_time;
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&starting_time);
    starting_time.QuadPart *= 1'000'000;
    starting_time.QuadPart /= frequency.QuadPart;
    return starting_time.QuadPart;
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1'000'000) + (t.tv_nsec / 1'000);
#endif
}
void Time::SleepMilli(unsigned milliseconds) {
#ifdef _WIN32
    ::Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}
}
#include <algorithm>
#include <assert.h>
#include <ctime>
#include <dirent.h>
#include <unistd.h>
#ifdef _WIN32
    #include <shlobj.h>
    #include <time.h>
#else
    #include <sys/stat.h>
    #include <utime.h>
#endif
#ifndef __APPLE__
    #include <filesystem>
#endif
#ifndef PATH_MAX
    #define PATH_MAX 1050
#endif
namespace gnu {
static std::string _FILE_STDOUT_NAME = "";
static std::string _FILE_STDERR_NAME = "";
#ifdef _WIN32
static char* _file_from_wide(const wchar_t* in) {
    auto out_len = WideCharToMultiByte(CP_UTF8, 0, in, -1, nullptr, 0, nullptr, nullptr);
    auto out     = File::Allocate(nullptr, out_len + 1);
    WideCharToMultiByte(CP_UTF8, 0, in, -1, (LPSTR) out, out_len, nullptr, nullptr);
    return (char*) out;
}
static wchar_t* _file_to_wide(const char* in) {
    auto out_len = MultiByteToWideChar(CP_UTF8, 0, in , -1, nullptr , 0);
    auto out     = reinterpret_cast<wchar_t*>(File::Allocate(nullptr, out_len * sizeof(wchar_t) + sizeof(wchar_t)));
    MultiByteToWideChar(CP_UTF8, 0, in , -1, out, out_len);
    return out;
}
static int64_t _file_time(FILETIME* ft) {
    int64_t res = (int64_t) ft->dwHighDateTime << 32 | (int64_t) ft->dwLowDateTime;
    res = res / 10000000;
    res = res - 11644473600;
    return res;
}
#endif
static FileBuf _file_close_redirect(int type) {
    std::string fname;
    FILE* fhandle;
    if (type == 2) {
        if (_FILE_STDERR_NAME == "") return FileBuf();
        fname = _FILE_STDERR_NAME;
        fhandle = stderr;
        _FILE_STDERR_NAME = "";
    }
    else {
        if (_FILE_STDOUT_NAME == "") return FileBuf();
        fname = _FILE_STDOUT_NAME;
        fhandle = stdout;
        _FILE_STDOUT_NAME = "";
    }
#ifdef _WIN32
    fflush(fhandle);
    freopen("CON", "w", fhandle);
#else
    fflush(fhandle);
    auto r = freopen("/dev/tty", "w", fhandle);
    (void) r;
#endif
    auto res = File::Read(fname);
    File::Remove(fname);
    return res;
}
static bool _file_open_redirect(int type) {
    bool res = false;
    std::string fname;
    FILE* fhandle = nullptr;
    if (type == 2) {
        if (_FILE_STDERR_NAME != "") return res;
        fname = _FILE_STDERR_NAME = File::TmpFile("stderr_").filename;
        fhandle = stderr;
    }
    else {
        if (_FILE_STDOUT_NAME != "") return res;
        fname = _FILE_STDOUT_NAME = File::TmpFile("stdout_").filename;
        fhandle = stdout;
    }
#ifdef _WIN32
        auto wpath = _file_to_wide(fname.c_str());
        auto wmode = _file_to_wide("wb");
        res = _wfreopen(wpath, wmode, fhandle) != nullptr;
        free(wpath);
        free(wmode);
#else
        res = freopen(fname.c_str(), "wb", fhandle) != nullptr;
#endif
    if (res == false && type == 1) _FILE_STDOUT_NAME = "";
    else if (res == false && type == 2) _FILE_STDERR_NAME = "";
    return res;
}
static int _file_rand() {
    static bool INIT = false;
    if (INIT == false) srand(time(nullptr));
    INIT = true;
    return rand() % 10'000;
}
static std::string& _file_replace_all(std::string& string, const std::string& find, const std::string& replace) {
    if (find == "") {
        return string;
    }
    else {
        size_t start = 0;
        while ((start = string.find(find, start)) != std::string::npos) {
            string.replace(start, find.length(), replace);
            start += replace.length();
        }
        return string;
    }
}
static std::string _file_substr(const std::string& in, std::string::size_type pos, std::string::size_type size = std::string::npos) {
    try { return in.substr(pos, size); }
    catch(...) { return ""; }
}
static void _file_sync(FILE* file) {
    if (file != nullptr) {
#ifdef _WIN32
        auto handle = (HANDLE) _get_osfhandle(_fileno(file));
        if (handle != INVALID_HANDLE_VALUE) {
            FlushFileBuffers(handle);
        }
#else
        fsync(fileno(file));
#endif
    }
}
static const std::string _file_to_absolute_path(const std::string& in, bool realpath) {
    std::string res;
    auto name = in;
    if (name == "") {
        return "";
    }
#ifdef _WIN32
    if (name.find("\\\\") == 0) {
        res = name;
        return name;
    }
    else if (name.size() < 2 || name[1] != ':') {
        auto work = File(File::WorkDir());
        res = work.filename;
        res += "\\";
        res += name;
    }
    else {
        res = name;
    }
    _file_replace_all(res, "\\", "/");
    auto len = res.length();
    _file_replace_all(res, "//", "/");
    while (len > res.length()) {
        len = res.length();
        _file_replace_all(res, "//", "/");
    }
    while (res.size() > 3 && res.back() == '/') {
        res.pop_back();
    }
#else
    if (name[0] != '/') {
        auto work = File(File::WorkDir());
        res = work.filename;
        res += "/";
        res += name;
    }
    else {
        res = name;
    }
    auto len = res.length();
    _file_replace_all(res, "//", "/");
    while (len > res.length()) {
        len = res.length();
        _file_replace_all(res, "//", "/");
    }
    while (res.size() > 1 && res.back() == '/') {
        res.pop_back();
    }
#endif
    return (realpath == true) ? File::CanonicalName(res) : res;
}
static void _file_split_paths(std::string filename, std::string& path, std::string& name, std::string& ext) {
    path = "";
    name = "";
    ext  = "";
    if (filename == "") {
        return;
    }
#ifdef _WIN32
    auto sep = '/';
    if (filename.find("\\\\") == 0) {
        sep = '\\';
        if (filename.back() == '\\') {
            return;
        }
    }
    auto pos1 = filename.find_last_of(sep);
    if (pos1 != std::string::npos) {
        if (filename.length() != 3) {
            path = _file_substr(filename, 0, pos1);
        }
        name = _file_substr(filename, pos1 + 1);
    }
    auto pos2 = name.find_last_of('.');
    if (pos2 != std::string::npos && pos2 != 0) {
        ext = _file_substr(name, pos2 + 1);
    }
    if (path.back() == ':') {
        path += sep;
        return;
    }
#else
    auto pos1 = filename.find_last_of('/');
    if (pos1 != std::string::npos) {
        if (pos1 > 0) {
            path = _file_substr(filename, 0, pos1);
        }
        else if (filename != "/") {
            path = "/";
        }
        if (filename != "/") {
            name = _file_substr(filename, pos1 + 1);
        }
    }
    auto pos2 = filename.find_last_of('.');
    if (pos2 != std::string::npos && pos2 > pos1 + 1) {
        ext = _file_substr(filename, pos2 + 1);
    }
#endif
}
static void _file_read(std::string path, FileBuf& buf) {
    File file(path);
    if (file.is_file() == false || (long long unsigned int) file.size > SIZE_MAX) {
        return;
    }
    auto out = File::Allocate(nullptr, file.size + 1);
    if (file.size == 0) {
        buf.p = out;
        return;
    }
    auto handle = File::Open(file.filename, "rb");
    if (handle == nullptr) {
        free(out);
        return;
    }
    else if (fread(out, 1, file.size, handle) != (size_t) file.size) {
        fclose(handle);
        free(out);
    }
    else {
        fclose(handle);
        buf.p = out;
        buf.s = file.size;
    }
}
static void _file_read_dir_rec(FileVector& res, FileVector& files) {
    for (auto& file : files) {
        res.push_back(file);
        if (file.type == File::TYPE::DIR && file.link == false && file.is_circular() == false) {
            auto v = File::ReadDir(file.filename);
            _file_read_dir_rec(res, v);
        }
    }
}
FileBuf::FileBuf(size_t S) {
    p = File::Allocate(nullptr, S + 1);
    s = S;
}
FileBuf::FileBuf(const char* P, size_t S) {
    if (P == nullptr) {
        p = nullptr;
        s = 0;
        return;
    }
    p = File::Allocate(nullptr, S + 1);
    s = S;
    std::memcpy(p, P, S);
}
FileBuf::FileBuf(const FileBuf& b) {
    if (b.p == nullptr) {
        p = nullptr;
        s = 0;
        return;
    }
    p = File::Allocate(nullptr, b.s + 1);
    s = b.s;
    std::memcpy(p, b.p, b.s);
}
bool FileBuf::operator==(const FileBuf& other) const {
    return p != nullptr && s == other.s && std::memcmp(p, other.p, s) == 0;
}
FileBuf& FileBuf::add(const char* P, size_t S) {
    if (p == P || P == nullptr) {
    }
    else if (p == nullptr) {
        p = File::Allocate(nullptr, S + 1);
        std::memcpy(p, P, S);
        s = S;
    }
    else if (S > 0) {
        auto t = File::Allocate(nullptr, s + S + 1);
        std::memcpy(t, p, s);
        std::memcpy(t + s, P, S);
        free(p);
        p = t;
        s += S;
    }
    return *this;
}
void FileBuf::Count(const char* P, size_t S, size_t count[257]) {
    assert(P);
    auto max_line     = 0;
    auto current_line = 0;
    std::memset(count, 0, sizeof(size_t) * 257);
    for (size_t f = 0; f < S; f++) {
        auto c = (unsigned char) P[f];
        count[c] += 1;
        if (current_line > max_line) {
            max_line = current_line;
        }
        if (c == 0 ||c == 10 || c == 13) {
            current_line = 0;
        }
        else {
            current_line++;
        }
    }
    count[256] = max_line;
}
uint64_t FileBuf::Fletcher64(const char* P, size_t S) {
    if (P == nullptr || S == 0) {
        return 0;
    }
    auto u8data = reinterpret_cast<const uint8_t*>(P);
    auto dwords = (uint64_t) S / 4;
    auto sum1   = (uint64_t) 0;
    auto sum2   = (uint64_t) 0;
    auto data32 = reinterpret_cast<const uint32_t*>(u8data);
    auto left   = (uint64_t) 0;
    for (size_t f = 0; f < dwords; ++f) {
        sum1 = (sum1 + data32[f]) % UINT32_MAX;
        sum2 = (sum2 + sum1) % UINT32_MAX;
    }
    left = S - dwords * 4;
    if (left > 0) {
        auto tmp  = (uint32_t) 0;
        auto byte = reinterpret_cast<uint8_t*>(&tmp);
        for (auto f = (uint64_t) 0; f < left; ++f) {
            byte[f] = u8data[dwords * 4 + f];
        }
        sum1 = (sum1 + tmp) % UINT32_MAX;
        sum2 = (sum2 + sum1) % UINT32_MAX;
    }
    return (sum2 << 32) | sum1;
}
FileBuf FileBuf::InsertCR(const char* P, size_t S, bool dos, bool trailing) {
    if (P == nullptr || S == 0 || (trailing == false && dos == false)) {
        return FileBuf();
    }
    auto res_size = S;
    if (dos == true) {
        for (size_t f = 0; f < S; f++) {
            res_size += (P[f] == '\n');
        }
    }
    auto res     = File::Allocate(nullptr, res_size + 1);
    auto restart = std::string::npos;
    auto res_pos = (size_t) 0;
    auto p       = (unsigned char) 0;
    for (size_t f = 0; f < S; f++) {
        auto c = (unsigned char) P[f];
        if (trailing == true) {
            if (c == '\n') {
                if (restart != std::string::npos) {
                    res_pos = restart;
                }
                restart = std::string::npos;
            }
            else if (restart == std::string::npos && (c == ' ' || c == '\t')) {
                restart = res_pos;
            }
            else if (c != ' ' && c != '\t') {
                restart = std::string::npos;
            }
        }
        if (dos == true && c == '\n' && p != '\r') {
            res[res_pos++] = '\r';
        }
        res[res_pos++] = c;
        p = c;
    }
    res[res_pos] = 0;
    if (restart != std::string::npos) {
        res[restart] = 0;
        res_pos = restart;
    }
    return FileBuf::Grab(res, res_pos);
}
FileBuf FileBuf::RemoveCR(const char* P, size_t S) {
    auto res = FileBuf(S);
    for (size_t f = 0, e = 0; f < S; f++) {
        auto c = P[f];
        if (c != 13) {
            res.p[e++] = c;
        }
        else {
            res.s--;
        }
    }
    return res;
}
FileBuf& FileBuf::set(const char* P, size_t S) {
    if (p == P) {
    }
    else if (P == nullptr) {
        free(p);
        p = nullptr;
        s = 0;
    }
    else {
        free(p);
        p = File::Allocate(nullptr, S + 1);
        s = S;
        std::memcpy(p, P, S);
    }
    return *this;
}
bool FileBuf::write(std::string filename) const {
    return File::Write(filename, p, s);
}
char* File::Allocate(char* resize_or_null, size_t size, bool exception) {
    void* res = nullptr;
    if (resize_or_null == nullptr) {
        res = calloc(size, 1);
    }
    else {
        res = realloc(resize_or_null, size);
    }
    if (res == nullptr && exception == true) {
        throw "error: memory allocation failed in File::Allocate()";
    }
    return (char*) res;
}
std::string File::CanonicalName(std::string filename) {
#if defined(_WIN32)
    wchar_t wres[PATH_MAX];
    auto    wpath = _file_to_wide(filename.c_str());
    auto    len   = GetFullPathNameW(wpath, PATH_MAX, wres, nullptr);
    if (len > 0 && len < PATH_MAX) {
        auto cpath = _file_from_wide(wres);
        auto res   = std::string(cpath);
        free(cpath);
        free(wpath);
        _file_replace_all(res, "\\", "/");
        return res;
    }
    else {
        free(wpath);
        return filename;
    }
#elif defined(__linux__)
    auto path = canonicalize_file_name(filename.c_str());
    auto res  = (path != nullptr) ? std::string(path) : filename;
    free(path);
    return res;
#else
    auto path = realpath(filename.c_str(), nullptr);
    auto res  = (path != nullptr) ? std::string(path) : filename;
    free(path);
    return res;
#endif
    return "";
}
bool File::ChDir(std::string path) {
#ifdef _WIN32
    auto wpath = _file_to_wide(path.c_str());
    auto res   = _wchdir(wpath);
    free(wpath);
    return res == 0;
#else
    return ::chdir(path.c_str()) == 0;
#endif
}
std::string File::CheckFilename(std::string filename) {
    static const std::string ILLEGAL = "<>:\"/\\|?*\n\t\r";
    std::string res;
    for (auto& c : filename) {
        if (ILLEGAL.find(c) == std::string::npos) {
            res += c;
        }
    }
    return res;
}
bool File::ChMod(std::string path, int mode) {
    auto res = false;
    if (mode < 0) {
        return false;
    }
#ifdef _WIN32
    auto wpath = _file_to_wide(path.c_str());
    res = SetFileAttributesW(wpath, mode);
    free(wpath);
#else
    res = ::chmod(path.c_str(), mode) == 0;
#endif
    return res;
}
FileBuf File::CloseStderr() {
    return _file_close_redirect(2);
}
FileBuf File::CloseStdout() {
    return _file_close_redirect(1);
}
bool File::Copy(std::string from, std::string to, CallbackCopy callback, void* data) {
#ifdef DEBUG
    static const size_t BUF_SIZE = 16384;
#else
    static const size_t BUF_SIZE = 131072;
#endif
    auto file1 = File(from);
    auto file2 = File(to);
    if (file1 == file2) {
        return false;
    }
    auto buf   = File::Allocate(nullptr, BUF_SIZE);
    auto read  = File::Open(from, "rb");
    auto write = File::Open(to, "wb");
    auto count = (int64_t) 0;
    auto size  = (size_t) 0;
    if (read == nullptr || write == nullptr) {
        if (read != nullptr) {
            fclose(read);
        }
        if (write != nullptr) {
            fclose(write);
            File::Remove(to);
        }
        free(buf);
        return false;
    }
    while ((size = fread(buf, 1, BUF_SIZE, read)) > 0) {
        if (fwrite(buf, 1, size, write) != size) {
            break;
        }
        count += size;
        if (callback != nullptr && callback(file1.size, count, data) == false && count != file1.size) {
            break;
        }
    }
    fclose(read);
    _file_sync(write);
    fclose(write);
    free(buf);
    if (count != file1.size) {
        File::Remove(to);
        return false;
    }
    File::ModTime(to, file1.mtime);
    File::ChMod(to, file1.mode);
    return true;
}
File File::HomeDir() {
    std::string res;
#ifdef _WIN32
    wchar_t wpath[PATH_MAX];
    if (SHGetFolderPathW(nullptr, CSIDL_PROFILE, nullptr, 0, wpath) == S_OK) {
        auto path = _file_from_wide(wpath);
        res = path;
        free(path);
    }
#else
    const char* tmp = getenv("HOME");
    res = tmp ? tmp : "";
#endif
    return File(res);
}
bool File::is_circular() const {
    if (type == TYPE::DIR && link == true) {
        auto l = canonicalname() + "/";
        return filename.find(l) == 0;
    }
    return false;
}
std::string File::linkname() const {
#ifdef _WIN32
    return "";
#else
    char tmp[PATH_MAX + 1];
    auto tmp_size = readlink(filename.c_str(), tmp, PATH_MAX);
    if (tmp_size > 0 && tmp_size < PATH_MAX) {
        tmp[tmp_size] = 0;
        return path + "/" + tmp;
    }
    return "";
#endif
}
bool File::MkDir(std::string path) {
    bool res = false;
#ifdef _WIN32
    auto wpath = _file_to_wide(path.c_str());
    res = _wmkdir(wpath) == 0;
    free(wpath);
#else
    res = ::mkdir(path.c_str(), File::DEFAULT_DIR_MODE) == 0;
#endif
    return res;
}
bool File::ModTime(std::string path, int64_t time) {
    auto res = false;
#ifdef _WIN32
    auto wpath  = _file_to_wide(path.c_str());
    auto handle = CreateFileW(wpath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (handle != INVALID_HANDLE_VALUE) {
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        auto     lm = (LONGLONG) 0;
        lm = Int32x32To64((time_t) time, 10000000) + 116444736000000000;
        ftLastAccessTime.dwLowDateTime  = (DWORD)lm;
        ftLastAccessTime.dwHighDateTime = lm >> 32;
        ftLastWriteTime.dwLowDateTime   = (DWORD)lm;
        ftLastWriteTime.dwHighDateTime  = lm >> 32;
        res = SetFileTime(handle, nullptr, &ftLastAccessTime, &ftLastWriteTime);
        CloseHandle(handle);
    }
    free(wpath);
#else
    utimbuf ut;
    ut.actime  = (time_t) time;
    ut.modtime = (time_t) time;
    res        = utime(path.c_str(), &ut) == 0;
#endif
    return res;
}
std::string File::name_without_ext() const {
    auto dot = name.find_last_of(".");
    return (dot == std::string::npos) ? name : name.substr(0, dot);
}
FILE* File::Open(std::string path, std::string mode) {
    FILE* res = nullptr;
#ifdef _WIN32
    auto wpath = _file_to_wide(path.c_str());
    auto wmode = _file_to_wide(mode.c_str());
    res = _wfopen(wpath, wmode);
    free(wpath);
    free(wmode);
#else
    res = fopen(path.c_str(), mode.c_str());
#endif
    return res;
}
std::string File::OS() {
#if defined(_WIN32)
    return "windows";
#elif defined(__APPLE__)
    return "macos";
#elif defined(__linux__)
    return "linux";
#elif defined(__unix__)
    return "unix";
#else
    return "unknown";
#endif
}
FILE* File::Popen(std::string cmd, bool write) {
    FILE* file = nullptr;
#ifdef _WIN32
    auto wpath = _file_to_wide(cmd.c_str());
    auto wmode = _file_to_wide(write ? "wb" : "rb");
    file = _wpopen(wpath, wmode);
    free(wpath);
    free(wmode);
#else
    ::fflush(nullptr);
    file = ::popen(cmd.c_str(), (write == true) ? "w" : "r");
#endif
    return file;
}
FileBuf File::Read(std::string path) {
    FileBuf buf;
    _file_read(path, buf);
    return buf;
}
FileBuf* File::Read2(std::string path) {
    auto buf = new FileBuf();
    _file_read(path, *buf);
    return buf;
}
FileVector File::ReadDir(std::string path) {
    auto file = File(path, true);
    auto res  = FileVector();
    if (file.type != TYPE::DIR) {
        return res;
    }
#ifdef _WIN32
    auto wpath = _file_to_wide(file.filename.c_str());
    auto dirp  = _wopendir(wpath);
    auto sep   = '/';
    if (file.filename.find("\\\\") == 0) {
        sep = '\\';
    }
    if (dirp != nullptr) {
        auto entry = _wreaddir(dirp);
        while (entry != nullptr) {
            auto cpath = _file_from_wide(entry->d_name);
            if (strcmp(cpath, ".") != 0 && strcmp(cpath, "..") != 0) {
                auto name = (file.name == ".") ? file.path + sep + cpath : file.filename + sep + cpath;
                res.push_back(File(name));
            }
            free(cpath);
            entry = _wreaddir(dirp);
        }
        _wclosedir(dirp);
    }
    free(wpath);
#else
    auto dirp = ::opendir(file.filename.c_str());
    if (dirp != nullptr) {
        auto entry = ::readdir(dirp);
        while (entry != nullptr) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                auto name = (file.name == ".") ? file.path + "/" + entry->d_name : file.filename + "/" + entry->d_name;
                res.push_back(File(name));
            }
            entry = ::readdir(dirp);
        }
        ::closedir(dirp);
    }
#endif
    std::sort(res.begin(), res.end());
    return res;
}
FileVector File::ReadDirRec(std::string path) {
    auto res   = FileVector();
    auto files = File::ReadDir(path);
    _file_read_dir_rec(res, files);
    return res;
}
bool File::RedirectStderr() {
    return _file_open_redirect(2);
}
bool File::RedirectStdout() {
    return _file_open_redirect(1);
}
bool File::Remove(std::string path) {
    auto f = File(path);
    if (f.type == TYPE::MISSING && f.link == false) {
        return false;
    }
    auto res = false;
#ifdef _WIN32
    auto wpath = _file_to_wide(path.c_str());
    if (f.type == TYPE::DIR) {
        res = RemoveDirectoryW(wpath);
    }
    else {
        res = DeleteFileW(wpath);
    }
    if (res == false) {
        if (f.type == TYPE::DIR) {
            File::ChMod(path, File::DEFAULT_DIR_MODE);
            res = RemoveDirectoryW(wpath);
        }
        else {
            File::ChMod(path, File::DEFAULT_FILE_MODE);
            res = DeleteFileW(wpath);
        }
    }
    free(wpath);
#else
    if (f.type == TYPE::DIR && f.link == false) {
        res = ::rmdir(path.c_str()) == 0;
    }
    else {
        res = ::unlink(path.c_str()) == 0;
    }
#endif
    return res;
}
bool File::RemoveRec(std::string path) {
    auto file = File(path, true);
    if (file == File::HomeDir() || file.path == "") {
        return false;
    }
    auto files = File::ReadDirRec(path);
    std::reverse(files.begin(), files.end());
    for (const auto& file : files) {
        File::Remove(file.filename);
    }
    return File::Remove(path);
}
bool File::Rename(std::string from, std::string to) {
    auto res    = false;
    auto from_f = File(from);
    auto to_f   = File(to);
    if (from_f == to_f) {
        return false;
    }
#ifdef _WIN32
    auto wfrom = _file_to_wide(from_f.filename.c_str());
    auto wto   = _file_to_wide(to_f.filename.c_str());
    if (to_f.type == TYPE::DIR) {
        File::RemoveRec(to_f.filename);
        res = MoveFileExW(wfrom, wto, MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);
    }
    else if (to_f.type == TYPE::MISSING) {
        res = MoveFileExW(wfrom, wto, MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);
    }
    else {
        res = ReplaceFileW(wto, wfrom, nullptr, REPLACEFILE_WRITE_THROUGH, 0, 0);
    }
    free(wfrom);
    free(wto);
#else
    if (to_f.type == TYPE::DIR) {
        File::RemoveRec(to_f.filename);
    }
    res = ::rename(from_f.filename.c_str(), to_f.filename.c_str()) == 0;
#endif
    return res;
}
int File::Run(std::string cmd, bool background, bool hide_win32_window) {
#ifdef _WIN32
    wchar_t*            cmd_w = _file_to_wide(cmd.c_str());
    STARTUPINFOW        startup_info;
    PROCESS_INFORMATION process_info;
    ZeroMemory(&startup_info, sizeof(STARTUPINFOW));
    ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));
    startup_info.cb          = sizeof(STARTUPINFOW);
    startup_info.dwFlags     = STARTF_USESHOWWINDOW;
    startup_info.wShowWindow = (hide_win32_window == true) ? SW_HIDE : SW_SHOW;
    if (CreateProcessW(nullptr, cmd_w, nullptr, nullptr, false, 0, nullptr, nullptr, &startup_info, &process_info) == 0) {
        free(cmd_w);
        return -1;
    }
    else {
        ULONG rc = 0;
        if (background == false) {
            WaitForSingleObject(process_info.hProcess, INFINITE);
            GetExitCodeProcess(process_info.hProcess, &rc);
        }
        CloseHandle(process_info.hThread);
        CloseHandle(process_info.hProcess);
        free(cmd_w);
        return (int) rc;
    }
#else
    (void) hide_win32_window;
    auto cmd2 = cmd;
    if (background == true) {
        cmd2 += " 2>&1 > /dev/null &";
    }
    return system(cmd2.c_str());
#endif
}
File File::TmpDir() {
    std::string res;
    try {
#if defined(_WIN32)
        auto path = std::filesystem::temp_directory_path();
        auto utf  = _file_from_wide(path.c_str());
        res = utf;
        free(utf);
#elif defined(__APPLE__)
        res = "/tmp";
#else
        auto path = std::filesystem::temp_directory_path();
        res = path.c_str();
#endif
    }
    catch(...) {
        return File::WorkDir();
    }
    return File(res);
}
File File::TmpFile(std::string prepend) {
    assert(prepend.length() < 50);
    char buf[100];
    snprintf(buf, 100, "%s%04d%04d%04d", prepend.c_str(), _file_rand(), _file_rand(), _file_rand());
    return File(TmpDir().filename + "/" + buf);
}
std::string File::to_string(bool short_version) const {
    char tmp[PATH_MAX + 100];
    int n = 0;
    if (short_version == true) {
        n = snprintf(tmp, PATH_MAX + 100, "File(filename=%s, type=%s, %ssize=%lld, mtime=%lld)",
            filename.c_str(),
            type_name().c_str(),
            link ? "LINK, " : "",
            (long long int) size,
            (long long int) mtime);
    }
    else {
        n = snprintf(tmp, PATH_MAX + 100, "File(filename=%s, name=%s, ext=%s, path=%s, type=%s, link=%s, size=%lld, mtime=%lld, mode=%o)",
            filename.c_str(),
            name.c_str(),
            ext.c_str(),
            path.c_str(),
            type_name().c_str(),
            link ? "YES" : "NO",
            (long long int) size,
            (long long int) mtime,
            mode > 0 ? mode : 0);
    }
    return (n > 0 && n < PATH_MAX + 100) ? tmp : "";
}
std::string File::type_name() const {
    static const char* NAMES[] = { "Missing", "Directory", "File", "Other", "", };
    return NAMES[static_cast<size_t>(type)];
}
File& File::update() {
    ctime = -1;
    link  = false;
    mode  = -1;
    mtime = -1;
    size  = -1;
    type  = TYPE::MISSING;
    if (filename == "") {
        return *this;
    }
#ifdef _WIN32
    auto wpath = _file_to_wide(filename.c_str());
    WIN32_FILE_ATTRIBUTE_DATA attr;
    if (GetFileAttributesExW(wpath, GetFileExInfoStandard, &attr) != 0) {
        if (attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            type = TYPE::DIR;
            size = 0;
        }
        else {
            type = TYPE::FILE;
            size = (attr.nFileSizeHigh * 4294967296) + attr.nFileSizeLow;
        }
        mtime = _file_time(&attr.ftLastWriteTime);
        ctime = _file_time(&attr.ftCreationTime);
        if (attr.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            link = true;
            HANDLE handle = CreateFileW(wpath, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
            if (handle != INVALID_HANDLE_VALUE) {
                size = GetFileSize(handle, NULL);
                FILETIME ftCreationTime;
                FILETIME ftLastAccessTime;
                FILETIME ftLastWriteTime;
                if (GetFileTime(handle, &ftCreationTime, &ftLastAccessTime, &ftLastWriteTime) != 0) {
                    mtime = _file_time(&ftLastWriteTime);
                    ctime = _file_time(&ftCreationTime);
                }
                CloseHandle(handle);
            }
        }
    }
    free(wpath);
#else
    struct stat st;
    char        tmp[PATH_MAX + 1];
    if (::stat(filename.c_str(), &st) == 0) {
        size  = st.st_size;
        ctime = st.st_ctime;
        mtime = st.st_mtime;
        if (S_ISDIR(st.st_mode)) {
            type = TYPE::DIR;
        }
        else if (S_ISREG(st.st_mode)) {
            type = TYPE::FILE;
        }
        else {
            type = TYPE::OTHER;
        }
        snprintf(tmp, PATH_MAX, "%o", st.st_mode);
        auto l = strlen(tmp);
        if (l > 2) {
            mode = strtol(tmp + (l - 3), nullptr, 8);
        }
        if (lstat(filename.c_str(), &st) == 0 && S_ISLNK(st.st_mode)) {
            link = true;
        }
    }
    else {
        auto tmp_size = readlink(filename.c_str(), tmp, PATH_MAX);
        if (tmp_size > 0 && tmp_size < PATH_MAX) {
            link = true;
        }
    }
#endif
    return *this;
}
File& File::update(std::string in, bool realpath) {
    filename = (in != "") ? _file_to_absolute_path(in, realpath) : "";
    _file_split_paths(filename, path, name, ext);
    update();
    return *this;
}
File File::WorkDir() {
    std::string res;
#ifdef _WIN32
    auto wpath = _wgetcwd(nullptr, 0);
    if (wpath != nullptr) {
        auto path = _file_from_wide(wpath);
        free(wpath);
        res = path;
        free(path);
    }
#else
    auto path = getcwd(nullptr, 0);
    if (path != nullptr) {
        res = path;
        free(path);
    }
#endif
    return File(res);
}
bool File::Write(std::string filename, const char* in, size_t in_size) {
    if (File(filename).type == TYPE::DIR) {
        return false;
    }
    auto tmpfile = filename + ".~tmp";
    auto file    = File::Open(tmpfile, "wb");
    if (file == nullptr) {
        return false;
    }
    auto wrote = fwrite(in, 1, in_size, file);
    _file_sync(file);
    fclose(file);
    if (wrote != in_size) {
        File::Remove(tmpfile);
        return false;
    }
    else if (File::Rename(tmpfile, filename) == false) {
        File::Remove(tmpfile);
        return false;
    }
    return true;
}
}
#include <algorithm>
#include <cstdint>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Tooltip.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#ifdef _WIN32
    #include <FL/x.H>
    #include <windows.h>
#else
    #include <unistd.h>
#endif
#ifdef FLW_USE_PNG
    #include <FL/Fl_PNG_Image.H>
    #include <FL/fl_draw.H>
#endif
namespace flw {
std::vector<char*>          PREF_FONTNAMES;
int                         PREF_FIXED_FONT         = FL_COURIER;
std::string                 PREF_FIXED_FONTNAME     = "FL_COURIER";
int                         PREF_FIXED_FONTSIZE     = 14;
int                         PREF_FONT               = FL_HELVETICA;
int                         PREF_FONTSIZE           = 14;
std::string                 PREF_FONTNAME           = "FL_HELVETICA";
std::string                 PREF_THEME              = "default";
const char* const           PREF_THEMES[]           = {
                                "default",
                                "gleam",
                                "blue gleam",
                                "dark gleam",
                                "tan gleam",
                                "gtk",
                                "blue gtk",
                                "dark gtk",
                                "tan gtk",
                                "oxy",
                                "tan oxy",
                                "plastic",
                                "tan plastic",
                                nullptr,
};
const char* const           PREF_THEMES2[]           = {
                                "default",
                                "gleam",
                                "blue_gleam",
                                "dark_gleam",
                                "tan_gleam",
                                "gtk",
                                "blue_gtk",
                                "dark_gtk",
                                "tan_gtk",
                                "oxy",
                                "tan_oxy",
                                "plastic",
                                "tan_plastic",
                                nullptr,
};
static std::string _flw_print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data, int from, int to) {
    bool                      cont = true;
    FILE*                     file = nullptr;
    Fl_PostScript_File_Device printer;
    int                       ph;
    int                       pw;
    std::string               res;
    if ((file = fl_fopen(ps_filename.c_str(), "wb")) == nullptr) {
        return "error: could not open file!";
    }
    printer.begin_job(file, 0, format, layout);
    while (cont == true) {
        if (printer.begin_page() != 0) {
            res = "error: couldn't create new page!";
            goto ERR;
        }
        if (printer.printable_rect(&pw, &ph) != 0) {
            res = "error: couldn't retrieve page size!";
            goto ERR;
        }
        fl_push_clip(0, 0, pw, ph);
        cont = cb(data, pw, ph, from);
        fl_pop_clip();
        if (printer.end_page() != 0) {
            res = "error: couldn't end page!";
            goto ERR;
        }
        if (from > 0) {
            from++;
            if (from > to) {
                cont = false;
            }
        }
    }
ERR:
    printer.end_job();
    fclose(file);
    return res;
}
void debug::print(const Fl_Widget* widget) {
    std::string indent;
    debug::print(widget, indent);
}
void debug::print(const Fl_Widget* widget, std::string& indent) {
    if (widget == nullptr) {
        puts("flw::debug::print() => null widget");
    }
    else {
        printf("%sx=%4d, y=%4d, w=%4d, h=%4d, %c, \"%s\"\n", indent.c_str(), widget->x(), widget->y(), widget->w(), widget->h(), widget->visible() ? 'V' : 'H', widget->label() ? widget->label() : "NULL");
        auto group = widget->as_group();
        if (group != nullptr) {
            indent += "\t";
            for (int f = 0; f < group->children(); f++) {
                debug::print(group->child(f), indent);
            }
            indent.pop_back();
        }
    }
    fflush(stdout);
}
bool debug::test(bool val, int line, const char* func) {
    if (val == false) {
        fprintf(stderr, "error: test failed at line %d in %s\n", line, func);
        fflush(stderr);
        return false;
    }
    return true;
}
bool debug::test(const char* ref, const char* val, int line, const char* func) {
    if (ref == nullptr && val == nullptr) {
        return true;
    }
    else if (ref == nullptr || val == nullptr || strcmp(ref, val) != 0) {
        fprintf(stderr, "error: test failed '%s' != '%s' at line %d in %s\n", ref ? ref : "NULL", val ? val : "NULL", line, func);
        fflush(stderr);
        return false;
    }
    return true;
}
bool debug::test(int64_t ref, int64_t val, int line, const char* func) {
    if (ref != val) {
        fprintf(stderr, "error: test failed '%lld' != '%lld' at line %d in %s\n", (long long int) ref, (long long int) val, line, func);
        fflush(stderr);
        return false;
    }
    return true;
}
bool debug::test(double ref, double val, double diff, int line, const char* func) {
    if (fabs(ref - val) > diff) {
        fprintf(stderr, "error: test failed '%f' != '%f' at line %d in %s\n", ref, val, line, func);
        fflush(stderr);
        return false;
    }
    return true;
}
namespace menu {
static Fl_Menu_Item* _item(Fl_Menu_* menu, const char* text, void* v = nullptr) {
    const Fl_Menu_Item* item;
    if (v == nullptr) {
        assert(menu && text);
        item = menu->find_item(text);
    }
    else {
        item = menu->find_item_with_user_data(v);
    }
#ifdef DEBUG
    if (item == nullptr) fprintf(stderr, "error: cant find menu item <%s>\n", text);
#endif
    return const_cast<Fl_Menu_Item*>(item);
}
}
void menu::enable_item(Fl_Menu_* menu, const char* text, bool value) {
    auto item = _item(menu, text);
    if (item == nullptr) return;
    if (value == true) item->activate();
    else item->deactivate();
}
Fl_Menu_Item* menu::get_item(Fl_Menu_* menu, const char* text) {
    return _item(menu, text);
}
Fl_Menu_Item* menu::get_item(Fl_Menu_* menu, void* v) {
    return _item(menu, nullptr, v);
}
bool menu::item_value(Fl_Menu_* menu, const char* text) {
    auto item = _item(menu, text);
    if (item == nullptr) return false;
    return item->value();
}
void menu::set_item(Fl_Menu_* menu, const char* text, bool value) {
    auto item = _item(menu, text);
    if (item == nullptr) return;
    if (value == true) item->set();
    else item->clear();
}
void menu::setonly_item(Fl_Menu_* menu, const char* text) {
    auto item = _item(menu, text);
    if (item == nullptr) return;
    menu->setonly(item);
}
void util::center_window(Fl_Window* window, Fl_Window* parent) {
    if (parent != nullptr) {
        int x = parent->x() + parent->w() / 2;
        int y = parent->y() + parent->h() / 2;
        window->resize(x - window->w() / 2, y - window->h() / 2, window->w(), window->h());
    }
    else {
        window->resize((Fl::w() / 2) - (window->w() / 2), (Fl::h() / 2) - (window->h() / 2), window->w(), window->h());
    }
}
double util::clock() {
#ifdef _WIN32
    struct timeb timeVal;
    ftime(&timeVal);
    return (double) timeVal.time + (double) (timeVal.millitm / 1000.0);
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double) (ts.tv_sec) + (ts.tv_nsec / 1000000000.0);
#endif
}
int util::count_decimals(double num) {
    num = fabs(num);
    if (num > 9'223'372'036'854'775'807.0) {
        return 0;
    }
    int    res     = 0;
    char*  end     = 0;
    double inum = static_cast<int64_t>(num);
    double fnum = num - inum;
    char   buffer[100];
    if (num > 999'999'999'999'999) {
        snprintf(buffer, 100, "%.1f", fnum);
    }
    else if (num > 9'999'999'999'999) {
        snprintf(buffer, 100, "%.2f", fnum);
    }
    else if (num > 999'999'999'999) {
        snprintf(buffer, 100, "%.3f", fnum);
    }
    else if (num > 99'999'999'999) {
        snprintf(buffer, 100, "%.4f", fnum);
    }
    else if (num > 9'999'999'999) {
        snprintf(buffer, 100, "%.5f", fnum);
    }
    else if (num > 999'999'999) {
        snprintf(buffer, 100, "%.6f", fnum);
    }
    else if (num > 99'999'999) {
        snprintf(buffer, 100, "%.7f", fnum);
    }
    else if (num > 9'999'999) {
        snprintf(buffer, 100, "%.8f", fnum);
    }
    else {
        snprintf(buffer, 100, "%.9f", fnum);
    }
    size_t len = strlen(buffer);
    end = buffer + len - 1;
    while (*end == '0') {
        *end = 0;
        end--;
    }
    res = strlen(buffer) - 2;
    return res;
}
Fl_Widget* util::find_widget(Fl_Group* group, std::string label) {
    for (int f = 0; f < group->children(); f++) {
        auto w = group->child(f);
        if (w->label() != nullptr && label == w->label()) {
            return w;
        }
    }
    for (int f = 0; f < group->children(); f++) {
        auto w = group->child(f);
        auto g = w->as_group();
        if (g != nullptr) {
            w = util::find_widget(g, label);
            if (w != nullptr) {
                return w;
            }
        }
    }
    return nullptr;
}
std::string util::fix_menu_string(std::string in) {
    std::string res = in;
    util::replace_string(res, "\\", "\\\\");
    util::replace_string(res, "_", "\\_");
    util::replace_string(res, "/", "\\/");
    util::replace_string(res, "&", "&&");
    return res;
}
std::string util::format(const char* format, ...) {
    if (format == nullptr || *format == 0) return "";
    int         l   = 128;
    int         n   = 0;
    char*       buf = static_cast<char*>(calloc(l, 1));
    std::string res;
    va_list     args;
    va_start(args, format);
    n = vsnprintf(buf, l, format, args);
    va_end(args);
    if (n < 0) {
        free(buf);
        return res;
    }
    if (n < l) {
        res = buf;
        free(buf);
        return res;
    }
    free(buf);
    l = n + 1;
    buf = static_cast<char*>(calloc(l, 1));
    if (buf == nullptr) return res;
    va_start(args, format);
    vsnprintf(buf, l, format, args);
    va_end(args);
    res = buf;
    free(buf);
    return res;
}
std::string util::format_double(double num, int decimals, char del) {
    if (num > 9'223'372'036'854'775'807.0) {
        return "err";
    }
    if (decimals < 0) {
        decimals = util::count_decimals(num);
    }
    if (del < 32) {
        del = 32;
    }
    if (decimals == 0 || decimals > 9) {
        return util::format_int(static_cast<int64_t>(num), del);
    }
    char res[100];
    char fr_str[100];
    auto int_num    = static_cast<int64_t>(fabs(num));
    auto double_num = static_cast<double>(fabs(num) - int_num);
    auto int_str    = util::format_int(int_num, del);
    auto len        = snprintf(fr_str, 99, "%.*f", decimals, double_num);
    *res = 0;
    if (len > 0 && len < 100) {
        if (num < 0.0) {
            res[0] = '-';
            res[1] = 0;
        }
        strncat(res, int_str.c_str(), 99);
        strncat(res, fr_str + 1, 99);
    }
    return res;
}
std::string util::format_int(int64_t num, char del) {
    auto pos = 0;
    char tmp1[32];
    char tmp2[32];
    if (del < 1) {
        del = 32;
    }
    memset(tmp2, 0, 32);
    snprintf(tmp1, 32, "%lld", (long long int) num);
    auto len = strlen(tmp1);
    for (int f = len - 1, i = 0; f >= 0 && pos < 32; f--, i++) {
        char c = tmp1[f];
        if ((i % 3) == 0 && i > 0 && c != '-') {
            tmp2[pos++] = del;
        }
        tmp2[pos++] = c;
    }
    std::string r = tmp2;
    std::reverse(r.begin(), r.end());
    return r;
}
bool util::is_whitespace_or_empty(const char* str) {
    while (*str != 0) {
        if (*str != 9 && *str != 32) {
            return false;
        }
        str++;
    }
    return true;
}
void util::labelfont(Fl_Widget* widget, Fl_Font fn, int fs) {
    widget->labelfont(fn);
    widget->labelsize(fs);
    auto group = widget->as_group();
    if (group != nullptr) {
        for (auto f = 0; f < group->children(); f++) {
            util::labelfont(group->child(f), fn, fs);
        }
    }
}
int64_t util::microseconds() {
#if defined(_WIN32)
    LARGE_INTEGER StartingTime;
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);
    StartingTime.QuadPart *= 1000000;
    StartingTime.QuadPart /= Frequency.QuadPart;
    return StartingTime.QuadPart;
#else
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000000 + t.tv_nsec / 1000);
#endif
}
int32_t util::milliseconds() {
#if defined(_WIN32)
    LARGE_INTEGER StartingTime;
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);
    StartingTime.QuadPart *= 1000000;
    StartingTime.QuadPart /= Frequency.QuadPart;
    return StartingTime.QuadPart / 1000;
#else
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000000 + t.tv_nsec / 1000) / 1000;
#endif
}
bool util::png_save(std::string opt_name, Fl_Window* window, int X, int Y, int W, int H) {
    auto res = false;
#ifdef FLW_USE_PNG
    auto filename = (opt_name == "") ? fl_file_chooser("Save To PNG File", "All Files (*)\tPNG Files (*.png)", "") : opt_name.c_str();
    if (filename != nullptr) {
        window->make_current();
        if (X == 0 && Y == 0 && W == 0 && H == 0) {
            X = window->x();
            Y = window->y();
            W = window->w();
            H = window->h();
        }
        auto image = fl_read_image(nullptr, X, Y, W, H);
        if (image != nullptr) {
            auto ret = fl_write_png(filename, image, W, H);
            if (ret == 0) {
                res = true;
            }
            else if (ret == -1) {
                fl_alert("%s", "error: missing libraries");
            }
            else if (ret == -2) {
                fl_alert("error: failed to save image to %s", filename);
            }
            delete []image;
        }
        else {
            fl_alert("%s", "error: failed to grab image");
        }
    }
#else
    (void) opt_name;
    (void) window;
    (void) X;
    (void) Y;
    (void) W;
    (void) H;
    fl_alert("error: flw not compiled with FLW_USE_PNG flag");
#endif
    return res;
}
std::string util::print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data) {
    return flw::_flw_print(ps_filename, format, layout, cb, data, 0, 0);
}
std::string util::print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data, int from, int to) {
    if (from < 1 || from > to) {
        return "error: invalid from/to range";
    }
    return flw::_flw_print(ps_filename, format, layout, cb, data, from, to);
}
std::string util::remove_browser_format(const char* text) {
    auto res = std::string((text != nullptr) ? text : "");
    auto f   = res.find_last_of("@");
    if (f != std::string::npos) {
        auto tmp = res.substr(f + 1);
        if (tmp[0] == '.' || tmp[0] == 'l' || tmp[0] == 'm' || tmp[0] == 's' || tmp[0] == 'b' || tmp[0] == 'i' || tmp[0] == 'f' || tmp[0] == 'c' || tmp[0] == 'r' || tmp[0] == 'u' || tmp[0] == '-') {
            res = tmp.substr(1);
        }
        else if (tmp[0] == 'B' || tmp[0] == 'C' || tmp[0] == 'F' || tmp[0] == 'S') {
            auto s = std::string();
            auto e = false;
            tmp = tmp.substr(f + 1);
            for (auto c : tmp) {
                if (e == false && c >= '0' && c <= '9') {
                }
                else {
                    e = true;
                    s += c;
                }
            }
            res = s;
        }
        else {
            res = res.substr(f);
        }
    }
    return res;
}
std::string& util::replace_string(std::string& string, std::string find, std::string replace) {
    if (find == "") {
        return string;
    }
    try {
        size_t start = 0;
        while ((start = string.find(find, start)) != std::string::npos) {
            string.replace(start, find.length(), replace);
            start += replace.length();
        }
    }
    catch(...) {
        string = "";
    }
    return string;
}
void util::sleep(int milli) {
#ifdef _WIN32
    Sleep(milli);
#else
    usleep(milli * 1000);
#endif
}
flw::StringVector util::split_string(const std::string& string, std::string split) {
    auto res = StringVector();
    try {
        if (split != "") {
            auto pos1 = (std::string::size_type) 0;
            auto pos2 = string.find(split);
            while (pos2 != std::string::npos) {
                res.push_back(string.substr(pos1, pos2 - pos1));
                pos1 = pos2 + split.size();
                pos2 = string.find(split, pos1);
            }
            if (pos1 <= string.size()) {
                res.push_back(string.substr(pos1));
            }
        }
    }
    catch(...) {
        res.clear();
    }
    return res;
}
std::string util::substr(std::string in, std::string::size_type pos, std::string::size_type size) {
    try {
        return in.substr(pos, size);
    }
    catch(...) {
        return "";
    }
}
double util::to_double(std::string num, double def) {
    try {
        return std::stod(num);
    }
    catch(...) {
        return def;
    }
}
long long util::to_long(std::string num, long long def) {
    try {
        return std::stoll(num);
    }
    catch(...) {
        return def;
    }
}
void* util::zero_memory(char* mem, size_t size) {
    if (mem == nullptr || size == 0) return mem;
#ifdef _WIN32
    RtlSecureZeroMemory(mem, size);
#else
    auto p = reinterpret_cast<volatile unsigned char*>(mem);
    while (size--) {
        *p = 0;
        p++;
    }
#endif
    return mem;
}
Fl_Color color::BEIGE            = fl_rgb_color(245, 245, 220);
Fl_Color color::CHOCOLATE        = fl_rgb_color(210, 105,  30);
Fl_Color color::CRIMSON          = fl_rgb_color(220,  20,  60);
Fl_Color color::DARKOLIVEGREEN   = fl_rgb_color( 85, 107,  47);
Fl_Color color::DODGERBLUE       = fl_rgb_color( 30, 144, 255);
Fl_Color color::FORESTGREEN      = fl_rgb_color( 34, 139,  34);
Fl_Color color::GOLD             = fl_rgb_color(255, 215,   0);
Fl_Color color::GRAY             = fl_rgb_color(128, 128, 128);
Fl_Color color::INDIGO           = fl_rgb_color( 75,   0, 130);
Fl_Color color::OLIVE            = fl_rgb_color(128, 128,   0);
Fl_Color color::PINK             = fl_rgb_color(255, 192, 203);
Fl_Color color::ROYALBLUE        = fl_rgb_color( 65, 105, 225);
Fl_Color color::SIENNA           = fl_rgb_color(160,  82,  45);
Fl_Color color::SILVER           = fl_rgb_color(192, 192, 192);
Fl_Color color::SLATEGRAY        = fl_rgb_color(112, 128, 144);
Fl_Color color::TEAL             = fl_rgb_color(  0, 128, 128);
Fl_Color color::TURQUOISE        = fl_rgb_color( 64, 224, 208);
Fl_Color color::VIOLET           = fl_rgb_color(238, 130, 238);
namespace theme {
static unsigned char _OLD_R[256]  = { 0 };
static unsigned char _OLD_G[256]  = { 0 };
static unsigned char _OLD_B[256]  = { 0 };
static bool          _IS_DARK     = false;
static bool          _SAVED_COLOR = false;
static int           _SCROLLSIZE  = Fl::scrollbar_size();
static void _additional_colors(bool dark) {
    color::BEIGE            = fl_rgb_color(245, 245, 220);
    color::CHOCOLATE        = fl_rgb_color(210, 105,  30);
    color::CRIMSON          = fl_rgb_color(220,  20,  60);
    color::DARKOLIVEGREEN   = fl_rgb_color( 85, 107,  47);
    color::DODGERBLUE       = fl_rgb_color( 30, 144, 255);
    color::FORESTGREEN      = fl_rgb_color( 34, 139,  34);
    color::GOLD             = fl_rgb_color(255, 215,   0);
    color::GRAY             = fl_rgb_color(128, 128, 128);
    color::INDIGO           = fl_rgb_color( 75,   0, 130);
    color::OLIVE            = fl_rgb_color(128, 128,   0);
    color::PINK             = fl_rgb_color(255, 192, 203);
    color::ROYALBLUE        = fl_rgb_color( 65, 105, 225);
    color::SIENNA           = fl_rgb_color(160,  82,  45);
    color::SILVER           = fl_rgb_color(192, 192, 192);
    color::SLATEGRAY        = fl_rgb_color(112, 128, 144);
    color::TEAL             = fl_rgb_color(  0, 128, 128);
    color::TURQUOISE        = fl_rgb_color( 64, 224, 208);
    color::VIOLET           = fl_rgb_color(238, 130, 238);
    if (dark == true) {
        color::BEIGE            = fl_darker(color::BEIGE);
        color::CHOCOLATE        = fl_darker(color::CHOCOLATE);
        color::CRIMSON          = fl_darker(color::CRIMSON);
        color::DARKOLIVEGREEN   = fl_darker(color::DARKOLIVEGREEN);
        color::DODGERBLUE       = fl_darker(color::DODGERBLUE);
        color::FORESTGREEN      = fl_darker(color::FORESTGREEN);
        color::GOLD             = fl_darker(color::GOLD);
        color::GRAY             = fl_darker(color::GRAY);
        color::INDIGO           = fl_darker(color::INDIGO);
        color::OLIVE            = fl_darker(color::OLIVE);
        color::PINK             = fl_darker(color::PINK);
        color::ROYALBLUE        = fl_darker(color::ROYALBLUE);
        color::SIENNA           = fl_darker(color::SIENNA);
        color::SILVER           = fl_darker(color::SILVER);
        color::SLATEGRAY        = fl_darker(color::SLATEGRAY);
        color::TEAL             = fl_darker(color::TEAL);
        color::TURQUOISE        = fl_darker(color::TURQUOISE);
        color::VIOLET           = fl_darker(color::VIOLET);
    }
}
static void _blue_colors() {
    Fl::set_color(0,   228, 228, 228);
    Fl::set_color(7,    79,  86,  94);
    Fl::set_color(8,   108, 113, 125);
    Fl::set_color(15,  241, 196,  126);
    Fl::set_color(56,    0,   0,   0);
    Fl::background(48, 56, 65);
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    for (int f = 32; f < 49; f++) {
        Fl::set_color(f, r, g, b);
        if (f == 32) {
            r = 0;
            g = 9;
            b = 18;
        }
        else {
            r += 2 + (f < 44);
            g += 2 + (f < 44);
            b += 2 + (f < 44);
        }
    }
}
static void _dark_colors() {
    Fl::set_color(0,   200, 200, 200);
    Fl::set_color(7,    64,  64,  64);
    Fl::set_color(8,   100, 100, 100);
    Fl::set_color(15,  177, 227, 177);
    Fl::set_color(56,    0,   0,   0);
    Fl::set_color(49, 43, 43, 43);
    Fl::background(43, 43, 43);
    unsigned char c = 0;
    for (int f = 32; f < 49; f++) {
        Fl::set_color(f, c, c, c);
        c += 2;
        if (f > 40) c++;
    }
}
static void _make_default_colors_darker() {
    Fl::set_color(FL_GREEN, fl_darker(Fl::get_color(FL_GREEN)));
    Fl::set_color(FL_DARK_GREEN, fl_darker(Fl::get_color(FL_DARK_GREEN)));
    Fl::set_color(FL_RED, fl_darker(Fl::get_color(FL_RED)));
    Fl::set_color(FL_DARK_RED, fl_darker(Fl::get_color(FL_DARK_RED)));
    Fl::set_color(FL_YELLOW, fl_darker(Fl::get_color(FL_YELLOW)));
    Fl::set_color(FL_DARK_YELLOW, fl_darker(Fl::get_color(FL_DARK_YELLOW)));
    Fl::set_color(FL_BLUE, fl_darker(Fl::get_color(FL_BLUE)));
    Fl::set_color(FL_DARK_BLUE, fl_darker(Fl::get_color(FL_DARK_BLUE)));
    Fl::set_color(FL_CYAN, fl_darker(Fl::get_color(FL_CYAN)));
    Fl::set_color(FL_DARK_CYAN, fl_darker(Fl::get_color(FL_DARK_CYAN)));
    Fl::set_color(FL_MAGENTA, fl_darker(Fl::get_color(FL_MAGENTA)));
    Fl::set_color(FL_DARK_MAGENTA, fl_darker(Fl::get_color(FL_DARK_MAGENTA)));
}
static void _restore_colors() {
    if (_SAVED_COLOR == true) {
        for (int f = 0; f < 256; f++) {
            Fl::set_color(f, theme::_OLD_R[f], theme::_OLD_G[f], theme::_OLD_B[f]);
        }
    }
}
static void _save_colors() {
    if (_SAVED_COLOR == false) {
        for (int f = 0; f < 256; f++) {
            unsigned char r1, g1, b1;
            Fl::get_color(f, r1, g1, b1);
            theme::_OLD_R[f] = r1;
            theme::_OLD_G[f] = g1;
            theme::_OLD_B[f] = b1;
        }
        _SAVED_COLOR = true;
    }
}
static void _tan_colors() {
    Fl::set_color(0,     0,   0,   0);
    Fl::set_color(7,   255, 255, 255);
    Fl::set_color(8,    85,  85,  85);
    Fl::set_color(15,  188, 114,  50);
    Fl::background(206, 202, 187);
}
void _load_default() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("none");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_DEFAULT];
    _IS_DARK = false;
}
void _load_gleam() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM];
    _IS_DARK = false;
}
void _load_gleam_blue() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_make_default_colors_darker();
    theme::_blue_colors();
    theme::_additional_colors(true);
    Fl::set_color(255, 101, 117, 125);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_BLUE];
    _IS_DARK = true;
}
void _load_gleam_dark() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_make_default_colors_darker();
    theme::_dark_colors();
    theme::_additional_colors(true);
    Fl::set_color(255, 112, 112, 112);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_DARK];
    _IS_DARK = true;
}
void _load_gleam_tan() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_tan_colors();
    theme::_additional_colors(false);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_TAN];
    _IS_DARK = false;
}
void _load_gtk() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK];
    _IS_DARK = false;
}
void _load_gtk_blue() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_make_default_colors_darker();
    theme::_blue_colors();
    theme::_additional_colors(true);
    Fl::set_color(255, 101, 117, 125);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_BLUE];
    _IS_DARK = true;
}
void _load_gtk_dark() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_make_default_colors_darker();
    theme::_dark_colors();
    theme::_additional_colors(true);
    Fl::set_color(255, 112, 112, 112);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_DARK];
    _IS_DARK = true;
}
void _load_gtk_tan() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_tan_colors();
    theme::_additional_colors(false);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_TAN];
    _IS_DARK = false;
}
void _load_oxy() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("oxy");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_OXY];
    _IS_DARK = false;
}
void _load_oxy_tan() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_tan_colors();
    theme::_additional_colors(false);
    Fl::scheme("oxy");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_OXY_TAN];
    _IS_DARK = false;
}
void _load_plastic() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("plastic");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_PLASTIC];
    _IS_DARK = false;
}
void _load_plastic_tan() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_tan_colors();
    theme::_additional_colors(false);
    Fl::scheme("plastic");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_PLASTIC_TAN];
    _IS_DARK = false;
}
void _scrollbar() {
    if (flw::PREF_FONTSIZE < 12 || flw::PREF_FONTSIZE > 16) {
        auto f = (double) flw::PREF_FONTSIZE / 14.0;
        auto s = (int) (f * theme::_SCROLLSIZE);
        Fl::scrollbar_size(s);
    }
    else if (theme::_SCROLLSIZE > 0) {
        Fl::scrollbar_size(theme::_SCROLLSIZE);
    }
}
}
bool theme::is_dark() {
    if (flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GLEAM_BLUE] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GLEAM_DARK] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GTK_BLUE] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GTK_DARK]) {
        return true;
    }
    else {
        return false;
    }
}
bool theme::load(std::string name) {
    if (theme::_SCROLLSIZE == 0) {
        theme::_SCROLLSIZE = Fl::scrollbar_size();
    }
    if (name == flw::PREF_THEMES[theme::THEME_DEFAULT] || name == flw::PREF_THEMES2[theme::THEME_DEFAULT]) {
        theme::_load_default();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM] || name == flw::PREF_THEMES2[theme::THEME_GLEAM]) {
        theme::_load_gleam();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_BLUE] || name == flw::PREF_THEMES2[theme::THEME_GLEAM_BLUE]) {
        theme::_load_gleam_blue();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_DARK] || name == flw::PREF_THEMES2[theme::THEME_GLEAM_DARK]) {
        theme::_load_gleam_dark();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_TAN] || name == flw::PREF_THEMES2[theme::THEME_GLEAM_TAN]) {
        theme::_load_gleam_tan();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK] || name == flw::PREF_THEMES2[theme::THEME_GTK]) {
        theme::_load_gtk();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_BLUE] || name == flw::PREF_THEMES2[theme::THEME_GTK_BLUE]) {
        theme::_load_gtk_blue();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_DARK] || name == flw::PREF_THEMES2[theme::THEME_GTK_DARK]) {
        theme::_load_gtk_dark();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_TAN] || name == flw::PREF_THEMES2[theme::THEME_GTK_TAN]) {
        theme::_load_gtk_tan();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_OXY] || name == flw::PREF_THEMES2[theme::THEME_OXY]) {
        theme::_load_oxy();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_OXY_TAN] || name == flw::PREF_THEMES2[theme::THEME_OXY_TAN]) {
        theme::_load_oxy_tan();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_PLASTIC] || name == flw::PREF_THEMES2[theme::THEME_PLASTIC]) {
        theme::_load_plastic();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_PLASTIC_TAN] || name == flw::PREF_THEMES2[theme::THEME_PLASTIC_TAN]) {
        theme::_load_plastic_tan();
    }
    else {
        return false;
    }
    theme::_scrollbar();
    return true;
}
int theme::load_font(std::string requested_font) {
    theme::load_fonts();
    auto count = 0;
    for (auto font : flw::PREF_FONTNAMES) {
        auto font2 = util::remove_browser_format(font);
        if (requested_font == font2) {
            return count;
        }
        count++;
    }
    return -1;
}
void theme::load_fonts(bool iso8859_only) {
    if (flw::PREF_FONTNAMES.size() == 0) {
        auto fonts = Fl::set_fonts((iso8859_only == true) ? nullptr : "-*");
        for (int f = 0; f < fonts; f++) {
            auto attr  = 0;
            auto name1 = Fl::get_font_name((Fl_Font) f, &attr);
            auto name2 = std::string();
            if (attr & FL_BOLD) {
                name2 = std::string("@b");
            }
            if (attr & FL_ITALIC) {
                name2 += std::string("@i");
            }
            name2 += std::string("@.");
            name2 += name1;
            flw::PREF_FONTNAMES.push_back(strdup(name2.c_str()));
        }
    }
}
void theme::load_icon(Fl_Window* win, int win_resource, const char** xpm_resource, const char* name) {
    assert(win);
    if (win->shown() != 0) {
        fl_alert("%s", "warning: load icon before showing window!");
    }
#if defined(_WIN32)
    win->icon(reinterpret_cast<char*>(LoadIcon(fl_display, MAKEINTRESOURCE(win_resource))));
    (void) name;
    (void) xpm_resource;
    (void) name;
#elif defined(__linux__)
    assert(xpm_resource);
    Fl_Pixmap    pix(xpm_resource);
    Fl_RGB_Image rgb(&pix, Fl_Color(0));
    win->icon(&rgb);
    win->xclass((name != nullptr) ? name : "FLTK");
    (void) win_resource;
#else
    (void) win;
    (void) win_resource;
    (void) xpm_resource;
    (void) name;
#endif
}
void theme::load_theme_pref(Fl_Preferences& pref) {
    auto val = 0;
    char buffer[4000];
    pref.get("regular_name", buffer, "", 4000);
    if (*buffer != 0 && strcmp("FL_HELVETICA", buffer) != 0) {
        auto font = theme::load_font(buffer);
        if (font != -1) {
            flw::PREF_FONT     = font;
            flw::PREF_FONTNAME = buffer;
        }
    }
    pref.get("regular_size", val, flw::PREF_FONTSIZE);
    if (val >= 6 && val <= 72) {
        flw::PREF_FONTSIZE = val;
    }
    pref.get("mono_name", buffer, "", 1000);
    if (*buffer != 0 && strcmp("FL_COURIER", buffer) != 0) {
        auto font = theme::load_font(buffer);
        if (font != -1) {
            flw::PREF_FIXED_FONT     = font;
            flw::PREF_FIXED_FONTNAME = buffer;
        }
    }
    pref.get("mono_size", val, flw::PREF_FIXED_FONTSIZE);
    if (val >= 6 && val <= 72) {
        flw::PREF_FIXED_FONTSIZE = val;
    }
    pref.get("theme", buffer, "oxy", 4000);
    theme::load(buffer);
    Fl_Tooltip::font(flw::PREF_FONT);
    Fl_Tooltip::size(flw::PREF_FONTSIZE);
    _scrollbar();
}
void theme::load_win_pref(Fl_Preferences& pref, Fl_Window* window, int show_0_1_2, int defw, int defh, std::string basename) {
    assert(window);
    int  x, y, w, h, f, m;
    pref.get((basename + "x").c_str(), x, 80);
    pref.get((basename + "y").c_str(), y, 60);
    pref.get((basename + "w").c_str(), w, defw);
    pref.get((basename + "h").c_str(), h, defh);
    pref.get((basename + "fullscreen").c_str(), f, 0);
    pref.get((basename + "maximized").c_str(), m, 0);
    if (w == 0 || h == 0) {
        w = 800;
        h = 600;
    }
    if (x + w <= 0 || y + h <= 0 || x >= Fl::w() || y >= Fl::h()) {
        x = 80;
        y = 60;
    }
    if (show_0_1_2 > 1 && window->shown() == 0) {
        window->show();
    }
    window->resize(x, y, w, h);
    if (f == 1) {
        window->fullscreen();
    }
    else if (m == 1) {
        window->maximize();
    }
    if (show_0_1_2 == 1 && window->shown() == 0) {
        window->show();
    }
}
bool theme::parse(int argc, const char** argv) {
    auto res = false;
    for (auto f = 1; f < argc; f++) {
        if (res == false) {
            res = theme::load(argv[f]);
        }
        auto fontsize = atoi(argv[f]);
        if (fontsize >= 6 && fontsize <= 72) {
            flw::PREF_FONTSIZE = fontsize;
        }
    }
    flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
    Fl_Tooltip::font(flw::PREF_FONT);
    Fl_Tooltip::size(flw::PREF_FONTSIZE);
    return res;
}
void theme::save_theme_pref(Fl_Preferences& pref) {
    pref.set("theme", flw::PREF_THEME.c_str());
    pref.set("regular_name", flw::PREF_FONTNAME.c_str());
    pref.set("regular_size", flw::PREF_FONTSIZE);
    pref.set("mono_name", flw::PREF_FIXED_FONTNAME.c_str());
    pref.set("mono_size", flw::PREF_FIXED_FONTSIZE);
}
void theme::save_win_pref(Fl_Preferences& pref, Fl_Window* window, std::string basename) {
    assert(window);
    pref.set((basename + "x").c_str(), window->x());
    pref.set((basename + "y").c_str(), window->y());
    pref.set((basename + "w").c_str(), window->w());
    pref.set((basename + "h").c_str(), window->h());
    pref.set((basename + "fullscreen").c_str(), window->fullscreen_active() ? 1 : 0);
    pref.set((basename + "maximized").c_str(), window->maximize_active() ? 1 : 0);
}
PrintText::PrintText(std::string filename,
    Fl_Paged_Device::Page_Format page_format,
    Fl_Paged_Device::Page_Layout page_layout,
    Fl_Font font,
    Fl_Fontsize fontsize,
    Fl_Align align,
    bool wrap,
    bool border,
    int line_num) {
    _align       = FL_ALIGN_INSIDE | FL_ALIGN_TOP;
    _align      |= (align == FL_ALIGN_CENTER || align == FL_ALIGN_RIGHT) ? align : FL_ALIGN_LEFT;
    _border      = border;
    _file        = nullptr;
    _filename    = filename;
    _font        = font;
    _fontsize    = fontsize;
    _line_num    = (align == FL_ALIGN_LEFT) ? line_num : 0;
    _page_format = page_format;
    _page_layout = page_layout;
    _printer     = nullptr;
    _wrap        = wrap;
}
PrintText::~PrintText() {
    stop();
}
void PrintText::check_for_new_page() {
    if (_py + _lh > _ph || _page_count == 0) {
        if (_page_count > 0) {
            fl_pop_clip();
            if (_printer->end_page() != 0) {
                throw "error: couldn't end current page";
            }
        }
        if (_printer->begin_page() != 0) {
            throw "error: couldn't create new page!";
        }
        if (_printer->printable_rect(&_pw, &_ph) != 0) {
            throw "error: couldn't retrieve page size!";
        }
        fl_font(_font, _fontsize);
        fl_color(FL_BLACK);
        fl_line_style(FL_SOLID, 1);
        fl_push_clip(0, 0, _pw, _ph);
        if (_border == false) {
            _px = 0;
            _py = 0;
        }
        else {
            fl_rect(0, 0, _pw, _ph);
            measure_lw_lh("M");
            _px  = _lw;
            _py  = _lh;
            _pw -= _lw * 2;
            _ph -= _lh * 2;
        }
        _page_count++;
    }
}
void PrintText::measure_lw_lh(const std::string& text) {
    _lw = _lh = 0;
    fl_measure(text.c_str(), _lw, _lh, 0);
}
std::string PrintText::print(const char* text, unsigned replace_tab_with_space) {
    return print(util::split_string(text, "\n"), replace_tab_with_space);
}
std::string PrintText::print(const std::string& text, unsigned replace_tab_with_space) {
    return print(util::split_string(text.c_str(), "\n"), replace_tab_with_space);
}
std::string PrintText::print(const StringVector& lines, unsigned replace_tab_with_space) {
    std::string res;
    std::string tab;
    while (replace_tab_with_space > 0 && replace_tab_with_space <= 16) {
        tab += " ";
        replace_tab_with_space--;
    }
    try {
        auto wc = WaitCursor();
        res = start();
        if (res == "") {
            for (auto& line : lines) {
                if (tab != "") {
                    auto l = line;
                    util::replace_string(l, "\t", "    ");
                    print_line(l == "" ? " " : l);
                }
                else {
                    print_line(line == "" ? " " : line);
                }
            }
            res = stop();
        }
    }
    catch (const char* ex) {
        res = ex;
    }
    catch (...) {
        res = "error: unknown exception!";
    }
    return res;
}
void PrintText::print_line(const std::string& line) {
    _line_count++;
    check_for_new_page();
    if (_line_num > 0) {
        auto num = util::format("%*d: ", _line_num, _line_count);
        measure_lw_lh(num);
        fl_draw(num.c_str(), _px, _py, _pw, _lh, _align, nullptr, 0);
        _nw = _lw;
    }
    measure_lw_lh(line);
    if (_wrap == true && _lw > _pw - _nw) {
        print_wrapped_line(line);
    }
    else {
        fl_draw(line.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
        _py += _lh;
    }
}
void PrintText::print_wrapped_line(const std::string& line) {
    auto p1 = line.c_str();
    auto s1 = std::string();
    auto s2 = std::string();
    while (*p1 != 0) {
        auto cl = fl_wcwidth(p1);
        if (cl > 1) {
            for (auto f = 0; f < cl && *p1 != 0; f++) {
                s1 += *p1;
                p1++;
            }
        }
        else {
            s1 += *p1;
            p1++;
        }
        auto c = s1.back();
        if (c == ' ' || c == '\t' || c == ',' || c == ';' || c == '.') {
            s2 = s1;
        }
        measure_lw_lh(s1);
        if (_lw >= _pw - _nw) {
            check_for_new_page();
            if (s2 != "") {
                fl_draw(s2.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
                s1 = (s2.length() < s1.length()) ? s1.substr(s2.length()) : "";
                s2 = "";
            }
            else {
                std::string s;
                if (s1.length() > 1) {
                    s  = s1.substr(s1.length() - 1);
                    s1 = s1.substr(0, s1.length() - 1);
                }
                fl_draw(s1.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
                s1 = s;
            }
            _py += _lh;
        }
    }
    if (s1 != "") {
        check_for_new_page();
        fl_draw(s1.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
        _py += _lh;
    }
}
std::string PrintText::start() {
    if ((_file = fl_fopen(_filename.c_str(), "wb")) == nullptr) {
        return "error: could not open file!";
    }
    _lh         = 0;
    _line_count = 0;
    _lw         = 0;
    _nw         = 0;
    _page_count = 0;
    _ph         = 0;
    _pw         = 0;
    _px         = 0;
    _py         = 0;
    _printer    = new Fl_PostScript_File_Device();
    _printer->begin_job(_file, 0, _page_format, _page_layout);
    return "";
}
std::string PrintText::stop() {
    std::string res = "";
    if (_printer != nullptr) {
        if (_page_count > 0) {
            fl_pop_clip();
            if (_printer->end_page() != 0) {
                res = "error: could not end page!";
            }
        }
        _printer->end_job();
        delete _printer;
        fclose(_file);
        _file    = nullptr;
        _printer = nullptr;
    }
    return res;
}
}
#ifdef _WIN32
    #include <windows.h>
#else
#endif
#include <math.h>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Tooltip.H>
namespace flw {
namespace theme {
void _load_default();
void _load_gleam();
void _load_gleam_blue();
void _load_gleam_dark();
void _load_gleam_tan();
void _load_gtk();
void _load_gtk_blue();
void _load_gtk_dark();
void _load_gtk_tan();
void _load_oxy();
void _load_oxy_blue();
void _load_oxy_tan();
void _load_plastic();
void _load_plastic_tan();
void _scrollbar();
}
namespace dlg {
static void _init_printer_formats(Fl_Choice* format, Fl_Choice* layout) {
    format->add("A0 format");
    format->add("A1 format");
    format->add("A2 format");
    format->add("A3 format");
    format->add("A4 format");
    format->add("A5 format");
    format->add("A6 format");
    format->add("A7 format");
    format->add("A8 format");
    format->add("A9 format");
    format->add("B0 format");
    format->add("B1 format");
    format->add("B2 format");
    format->add("B3 format");
    format->add("B4 format");
    format->add("B5 format");
    format->add("B6 format");
    format->add("B7 format");
    format->add("B8 format");
    format->add("B9 format");
    format->add("Executive format");
    format->add("Folio format");
    format->add("Ledger format");
    format->add("Legal format");
    format->add("Letter format");
    format->add("Tabloid format");
    format->tooltip("Select paper format.");
    format->value(4);
    layout->add("Portrait");
    layout->add("Landscape");
    layout->tooltip("Select paper layout.");
    layout->value(0);
}
void center_message_dialog() {
    int X, Y, W, H;
    Fl::screen_xywh(X, Y, W, H);
    fl_message_position(W / 2, H / 2, 1);
}
bool font(Fl_Font& font, Fl_Fontsize& fontsize, std::string& fontname, bool limit_to_default) {
    auto dlg = dlg::FontDialog(font, fontsize, "Select Font", limit_to_default);
    if (dlg.run() == false) {
        return false;
    }
    font     = dlg.font();
    fontsize = dlg.fontsize();
    fontname = dlg.fontname();
    return true;
}
void panic(std::string message) {
    fl_alert("panic! I have to quit\n%s", message.c_str());
    exit(1);
}
class _DlgCheck : public Fl_Double_Window {
    Fl_Button*                  _all;
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Button*                  _invert;
    Fl_Button*                  _none;
    Fl_Scroll*                  _scroll;
    GridGroup*                  _grid;
    const StringVector&         _labels;
    WidgetVector                _checkbuttons;
    bool                        _ret;
public:
    _DlgCheck(const char* title, Fl_Window* parent, const StringVector& strings) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 36, flw::PREF_FONTSIZE * 20),
    _labels(strings) {
        end();
        _ret    = false;
        _all    = new Fl_Button(0, 0, 0, 0, "All on");
        _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _close  = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
        _grid   = new GridGroup(0, 0, w(), h());
        _invert = new Fl_Button(0, 0, 0, 0, "Invert");
        _none   = new Fl_Button(0, 0, 0, 0, "All off");
        _scroll = new Fl_Scroll(0, 0, 0, 0);
        _grid->add(_scroll,   1,  1, -1, -7);
        _grid->add(_invert, -67, -5, 10,  4);
        _grid->add(_none,   -56, -5, 10,  4);
        _grid->add(_all,    -45, -5, 10,  4);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_close,  -17, -5, 16,  4);
        add(_grid);
        for (auto& l : _labels) {
            auto b = new Fl_Check_Button(0, 0, 0, 0, l.c_str() + 1);
            b->value(*l.c_str() == '1');
            _checkbuttons.push_back(b);
            _scroll->add(b);
        }
        _all->callback(_DlgCheck::Callback, this);
        _cancel->callback(_DlgCheck::Callback, this);
        _close->callback(_DlgCheck::Callback, this);
        _invert->callback(_DlgCheck::Callback, this);
        _none->callback(_DlgCheck::Callback, this);
        _scroll->box(FL_BORDER_BOX);
        util::labelfont(this);
        callback(_DlgCheck::Callback, this);
        copy_label(title);
        set_modal();
        resizable(_grid);
        size_range(flw::PREF_FONTSIZE * 36, flw::PREF_FONTSIZE * 12);
        util::center_window(this, parent);
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgCheck*>(o);
        if (w == self || w == self->_cancel) {
            self->hide();
        }
        else if (w == self->_close) {
            self->_ret = true;
            self->hide();
        }
        else if (w == self->_all) {
            for (auto b : self->_checkbuttons) {
                static_cast<Fl_Check_Button*>(b)->value(1);
            }
        }
        else if (w == self->_none) {
            for (auto b : self->_checkbuttons) {
                static_cast<Fl_Check_Button*>(b)->value(0);
            }
        }
        else if (w == self->_invert) {
            for (auto b : self->_checkbuttons) {
                static_cast<Fl_Check_Button*>(b)->value(static_cast<Fl_Check_Button*>(b)->value() ? 0 : 1);
            }
        }
    }
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        _grid->resize(0, 0, W, H);
        auto y = _scroll->y() + 4;
        for (auto b : _checkbuttons) {
            b->resize(_scroll->x() + 4, y, _scroll->w() - Fl::scrollbar_size() - 8, flw::PREF_FONTSIZE * 2);
            y += flw::PREF_FONTSIZE * 2;
        }
    }
    StringVector run() {
        StringVector res;
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        if (_ret == true) {
            for (auto b : _checkbuttons) {
                res.push_back(std::string((static_cast<Fl_Check_Button*>(b)->value() == 0) ? "0" : "1") + b->label());
            }
        }
        return res;
    }
};
StringVector check(std::string title, const StringVector& list, Fl_Window* parent) {
    _DlgCheck dlg(title.c_str(), parent, list);
    return dlg.run();
}
class _DlgChoice : public Fl_Double_Window {
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Choice*                  _choice;
    GridGroup*                  _grid;
    int                         _ret;
public:
    _DlgChoice(const char* title, Fl_Window* parent, const StringVector& strings, int selected) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 6) {
        end();
        _ret    = -1;
        _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _choice = new Fl_Choice(0, 0, 0, 0);
        _close  = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
        _grid   = new GridGroup(0, 0, w(), h());
        _grid->add(_choice,   1,  1, -1,  4);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_close,  -17, -5, 16,  4);
        add(_grid);
        for (const auto& string : strings) {
            _choice->add(string.c_str());
        }
        _cancel->callback(_DlgChoice::Callback, this);
        _choice->textfont(flw::PREF_FONT);
        _choice->textsize(flw::PREF_FONTSIZE);
        _choice->value(selected);
        _close->callback(_DlgChoice::Callback, this);
        util::labelfont(this);
        callback(_DlgChoice::Callback, this);
        copy_label(title);
        set_modal();
        resizable(_grid);
        size_range(flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 6);
        util::center_window(this, parent);
        _grid->do_layout();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgChoice*>(o);
        if (w == self || w == self->_cancel) {
            self->hide();
        }
        else if (w == self->_close) {
            self->_ret = self->_choice->value();
            self->hide();
        }
    }
    int run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        return _ret;
    }
};
int choice(std::string title, const StringVector& list, int selected, Fl_Window* parent) {
    _DlgChoice dlg(title.c_str(), parent, list, selected);
    return dlg.run();
}
class _DlgHtml  : public Fl_Double_Window {
    Fl_Help_View*               _html;
    Fl_Return_Button*           _close;
    GridGroup*                  _grid;
public:
    _DlgHtml(const char* title, const char* text, Fl_Window* parent, int W, int H) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W,flw::PREF_FONTSIZE * H) {
        end();
        _close = new Fl_Return_Button(0, 0, 0, 0, "&Close");
        _grid  = new GridGroup(0, 0, w(), h());
        _html  = new Fl_Help_View(0, 0, 0, 0);
        _grid->add(_html,    1,  1, -1, -6);
        _grid->add(_close, -17, -5, 16,  4);
        add(_grid);
        _close->callback(_DlgHtml::Callback, this);
        _close->labelfont(flw::PREF_FONT);
        _close->labelsize(flw::PREF_FONTSIZE);
        _html->textfont(flw::PREF_FONT);
        _html->textsize(flw::PREF_FONTSIZE);
        _html->value(text);
        callback(_DlgHtml::Callback, this);
        copy_label(title);
        size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
        set_modal();
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgHtml*>(o);
        if (w == self || w == self->_close) {
            self->hide();
        }
    }
    void run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
    }
};
void html(std::string title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgHtml dlg(title.c_str(), text.c_str(), parent, W, H);
    dlg.run();
}
class _DlgList : public Fl_Double_Window {
    Fl_Return_Button*           _close;
    GridGroup*                  _grid;
    ScrollBrowser*              _list;
public:
    _DlgList(const char* title, const StringVector& list, std::string file, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 50, int H = 20) :
    Fl_Double_Window(0, 0, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H) {
        end();
        _close = new Fl_Return_Button(0, 0, 0, 0, "&Close");
        _grid  = new GridGroup(0, 0, w(), h());
        _list  = new ScrollBrowser();
        _grid->add(_list,     1,   1,  -1,  -6);
        _grid->add(_close,  -17,  -5,  16,   4);
        add(_grid);
        _close->callback(_DlgList::Callback, this);
        _close->labelfont(flw::PREF_FONT);
        _close->labelsize(flw::PREF_FONTSIZE);
        _list->take_focus();
        if (fixed_font == true) {
            _list->textfont(flw::PREF_FIXED_FONT);
            _list->textsize(flw::PREF_FIXED_FONTSIZE);
        }
        else {
            _list->textfont(flw::PREF_FONT);
            _list->textsize(flw::PREF_FONTSIZE);
        }
        callback(_DlgList::Callback, this);
        copy_label(title);
        size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
        set_modal();
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
        if (list.size() > 0) {
            for (const auto& s : list) {
                _list->add(s.c_str());
            }
        }
        else if (file != "") {
            _list->load(file.c_str());
        }
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgList*>(o);
        if (w == self || w == self->_close) {
            self->hide();
        }
    }
    void run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
    }
};
void list(std::string title, const StringVector& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), list, "", parent, fixed_font, W, H);
    dlg.run();
}
void list(std::string title, const std::string& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    auto list2 = util::split_string( list, "\n");
    _DlgList dlg(title.c_str(), list2, "", parent, fixed_font, W, H);
    dlg.run();
}
void list_file(std::string title, std::string file, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), flw::StringVector(), file, parent, fixed_font, W, H);
    dlg.run();
}
const char* PASSWORD_CANCEL = "&Cancel";
const char* PASSWORD_OK     = "&Ok";
class _DlgPassword : public Fl_Double_Window {
public:
    enum class TYPE {
                                ASK_PASSWORD,
                                ASK_PASSWORD_AND_KEYFILE,
                                CONFIRM_PASSWORD,
                                CONFIRM_PASSWORD_AND_KEYFILE,
    };
private:
    Fl_Button*                  _browse;
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Input*                   _file;
    Fl_Input*                   _password1;
    Fl_Input*                   _password2;
    GridGroup*                  _grid;
    _DlgPassword::TYPE          _mode;
    bool                        _ret;
public:
    _DlgPassword(const char* title, Fl_Window* parent, _DlgPassword::TYPE mode) :
    Fl_Double_Window(0, 0, 10, 10) {
        end();
        _browse    = new Fl_Button(0, 0, 0, 0, "&Browse");
        _cancel    = new Fl_Button(0, 0, 0, 0, PASSWORD_CANCEL);
        _close     = new Fl_Return_Button(0, 0, 0, 0, PASSWORD_OK);
        _file      = new Fl_Output(0, 0, 0, 0, "Key file");
        _grid      = new GridGroup(0, 0, w(), h());
        _password1 = new Fl_Secret_Input(0, 0, 0, 0, "Password");
        _password2 = new Fl_Secret_Input(0, 0, 0, 0, "Enter password again");
        _mode      = mode;
        _ret       = false;
        _grid->add(_password1,  1,   3,  -1,  4);
        _grid->add(_password2,  1,  10,  -1,  4);
        _grid->add(_file,       1,  17,  -1,  4);
        _grid->add(_browse,   -51,  -5,  16,  4);
        _grid->add(_cancel,   -34,  -5,  16,  4);
        _grid->add(_close,    -17,  -5,  16,  4);
        add(_grid);
        _browse->callback(_DlgPassword::Callback, this);
        _cancel->callback(_DlgPassword::Callback, this);
        _close->callback(_DlgPassword::Callback, this);
        _close->deactivate();
        _file->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _file->textfont(flw::PREF_FIXED_FONT);
        _file->textsize(flw::PREF_FONTSIZE);
        _file->tooltip("Select optional key file");
        _password1->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _password1->callback(_DlgPassword::Callback, this);
        _password1->textfont(flw::PREF_FIXED_FONT);
        _password1->textsize(flw::PREF_FONTSIZE);
        _password1->when(FL_WHEN_CHANGED);
        _password2->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _password2->callback(_DlgPassword::Callback, this);
        _password2->textfont(flw::PREF_FIXED_FONT);
        _password2->textsize(flw::PREF_FONTSIZE);
        _password2->when(FL_WHEN_CHANGED);
        auto W = flw::PREF_FONTSIZE * 35;
        auto H = flw::PREF_FONTSIZE * 13.5;
        if (_mode == _DlgPassword::TYPE::ASK_PASSWORD) {
            _password2->hide();
            _browse->hide();
            _file->hide();
            H = flw::PREF_FONTSIZE * 6.5;
        }
        else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD) {
            _browse->hide();
            _file->hide();
            H = flw::PREF_FONTSIZE * 10;
        }
        else if (_mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {
            _password2->hide();
            _grid->resize(_file, 1, 10, -1, 4);
            H = flw::PREF_FONTSIZE * 10;
        }
        resizable(_grid);
        util::labelfont(this);
        callback(_DlgPassword::Callback, this);
        label(title);
        size(W, H);
        size_range(W, H);
        set_modal();
        util::center_window(this, parent);
        _grid->do_layout();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgPassword*>(o);
        if (w == self) {
            ;
        }
        else if (w == self->_password1) {
            self->check();
        }
        else if (w == self->_password2) {
            self->check();
        }
        else if (w == self->_browse) {
            auto filename = fl_file_chooser("Select Key File", nullptr, nullptr, 0);
            if (filename) {
                self->_file->value(filename);
            }
            else {
                self->_file->value("");
            }
        }
        else if (w == self->_cancel) {
            self->_ret = false;
            self->hide();
        }
        else if (w == self->_close) {
            self->_ret = true;
            self->hide();
        }
    }
    void check() {
        auto p1 = _password1->value();
        auto p2 = _password2->value();
        if (_mode == _DlgPassword::TYPE::ASK_PASSWORD ||
            _mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {
            if (strlen(p1)) {
                _close->activate();
            }
            else {
                _close->deactivate();
            }
        }
        else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD ||
                 _mode == _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE) {
            if (strlen(p1) && strcmp(p1, p2) == 0) {
                _close->activate();
            }
            else {
                _close->deactivate();
            }
        }
    }
    bool run(std::string& password, std::string& file) {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        if (_ret) {
            file = _file->value();
            password = _password1->value();
        }
        util::zero_memory(const_cast<char*>(_password1->value()), strlen(_password1->value()));
        util::zero_memory(const_cast<char*>(_password2->value()), strlen(_password2->value()));
        util::zero_memory(const_cast<char*>(_file->value()), strlen(_file->value()));
        return _ret;
    }
};
bool password1(std::string title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD);
    return dlg.run(password, file);
}
bool password2(std::string title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD);
    return dlg.run(password, file);
}
bool password3(std::string title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}
bool password4(std::string title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}
class _DlgPrint : public Fl_Double_Window {
    Fl_Button*                  _close;
    Fl_Button*                  _file;
    Fl_Button*                  _print;
    Fl_Choice*                  _format;
    Fl_Choice*                  _layout;
    Fl_Hor_Slider*              _from;
    Fl_Hor_Slider*              _to;
    GridGroup*                  _grid;
    PrintCallback               _cb;
    void*                       _data;
public:
    _DlgPrint(std::string title, Fl_Window* parent, PrintCallback cb, void* data, int from, int to) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 18) {
        end();
        _close  = new Fl_Button(0, 0, 0, 0, "&Close");
        _file   = new Fl_Button(0, 0, 0, 0, "output.ps");
        _format = new Fl_Choice(0, 0, 0, 0);
        _from   = new Fl_Hor_Slider(0, 0, 0, 0);
        _grid   = new GridGroup(0, 0, w(), h());
        _layout = new Fl_Choice(0, 0, 0, 0);
        _print  = new Fl_Button(0, 0, 0, 0, "&Print");
        _to     = new Fl_Hor_Slider(0, 0, 0, 0);
        _cb     = cb;
        _data   = data;
        _grid->add(_from,     1,   3,  -1,   4);
        _grid->add(_to,       1,  10,  -1,   4);
        _grid->add(_format,   1,  15,  -1,   4);
        _grid->add(_layout,   1,  20,  -1,   4);
        _grid->add(_file,     1,  25,  -1,   4);
        _grid->add(_print,  -34,  -5,  16,   4);
        _grid->add(_close,  -17,  -5,  16,   4);
        add(_grid);
        util::labelfont(this);
        _close->callback(_DlgPrint::Callback, this);
        _file->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _file->callback(_DlgPrint::Callback, this);
        _file->tooltip("Select output PostScript file.");
        _from->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _from->callback(_DlgPrint::Callback, this);
        _from->color(FL_BACKGROUND2_COLOR);
        _from->range(from, to);
        _from->precision(0);
        _from->value(from);
        _from->tooltip("Start page number.");
        _print->callback(_DlgPrint::Callback, this);
        _to->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _to->callback(_DlgPrint::Callback, this);
        _to->color(FL_BACKGROUND2_COLOR);
        _to->range(from, to);
        _to->precision(0);
        _to->value(to);
        _to->tooltip("End page number.");
        if (from < 1 || from > to) {
            _from->deactivate();
            _from->range(0, 1);
            _from->value(0);
            _to->deactivate();
            _to->range(0, 1);
            _to->value(0);
        }
        else if (from == to) {
            _from->deactivate();
            _to->deactivate();
        }
        dlg::_init_printer_formats(_format, _layout);
        _DlgPrint::Callback(_from, this);
        _DlgPrint::Callback(_to, this);
        callback(_DlgPrint::Callback, this);
        copy_label(title.c_str());
        size_range(flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 18);
        set_modal();
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgPrint*>(o);
        if (w == self) {
            self->hide();
        }
        else if (w == self->_close) {
            self->hide();
        }
        else if (w == self->_file) {
            auto filename = fl_file_chooser("Save To PostScript File", "PostScript Files (*.ps)\tAll Files (*)", self->_file->label());
            if (filename != nullptr) {
                self->_file->copy_label(filename);
            }
        }
        else if (w == self->_from || w == self->_to) {
            auto l = util::format("%s page: %d", (w == self->_from) ? "From" : "To", (int) static_cast<Fl_Hor_Slider*>(w)->value());
            w->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_print) {
            self->print();
        }
    }
    void print() {
        auto from   = static_cast<int>(_from->value());
        auto to     = static_cast<int>(_to->value());
        auto format = static_cast<Fl_Paged_Device::Page_Format>(_format->value());
        auto layout = (_layout->value() == 0) ? Fl_Paged_Device::Page_Layout::PORTRAIT : Fl_Paged_Device::Page_Layout::LANDSCAPE;
        auto file   = _file->label();
        auto err    = (from == 0) ? util::print(file, format, layout, _cb, _data) : util::print(file, format, layout, _cb, _data, from, to);
        if (err != "") {
            fl_alert("Printing failed!\n%s", err.c_str());
            return;
        }
        hide();
    }
    void run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
    }
};
void print(std::string title, PrintCallback cb, void* data, int from, int to, Fl_Window* parent) {
    _DlgPrint dlg(title, parent, cb, data, from, to);
    dlg.run();
}
class _DlgPrintText : public Fl_Double_Window {
    Fl_Box*                     _label;
    Fl_Button*                  _close;
    Fl_Button*                  _file;
    Fl_Button*                  _fonts;
    Fl_Button*                  _print;
    Fl_Check_Button*            _border;
    Fl_Check_Button*            _wrap;
    Fl_Choice*                  _align;
    Fl_Choice*                  _format;
    Fl_Choice*                  _layout;
    Fl_Font                     _font;
    Fl_Fontsize                 _fontsize;
    Fl_Hor_Slider*              _line;
    Fl_Hor_Slider*              _tab;
    GridGroup*                  _grid;
    bool                        _ret;
    const StringVector&         _text;
    std::string                 _label2;
public:
    _DlgPrintText(std::string title, Fl_Window* parent, const StringVector& text) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 35),
    _text(text) {
        end();
        _align    = new Fl_Choice(0, 0, 0, 0);
        _border   = new Fl_Check_Button(0, 0, 0, 0, "Print border");
        _close    = new Fl_Button(0, 0, 0, 0, "&Close");
        _file     = new Fl_Button(0, 0, 0, 0, "output.ps");
        _fonts    = new Fl_Button(0, 0, 0, 0, "Courier - 14");
        _format   = new Fl_Choice(0, 0, 0, 0);
        _grid     = new GridGroup(0, 0, w(), h());
        _label    = new Fl_Box(0, 0, 0, 0);
        _layout   = new Fl_Choice(0, 0, 0, 0);
        _line     = new Fl_Hor_Slider(0, 0, 0, 0);
        _print    = new Fl_Button(0, 0, 0, 0, "&Print");
        _tab      = new Fl_Hor_Slider(0, 0, 0, 0);
        _wrap     = new Fl_Check_Button(0, 0, 0, 0, "Wrap lines");
        _ret      = false;
        _font     = FL_COURIER;
        _fontsize = 14;
        _grid->add(_border,   1,   1,  -1,   4);
        _grid->add(_wrap,     1,   6,  -1,   4);
        _grid->add(_line,     1,  13,  -1,   4);
        _grid->add(_tab,      1,  20,  -1,   4);
        _grid->add(_format,   1,  25,  -1,   4);
        _grid->add(_layout,   1,  30,  -1,   4);
        _grid->add(_align,    1,  35,  -1,   4);
        _grid->add(_fonts,    1,  40,  -1,   4);
        _grid->add(_file,     1,  45,  -1,   4);
        _grid->add(_label,    1,  50,  -1,   13);
        _grid->add(_print,  -34,  -5,  16,   4);
        _grid->add(_close,  -17,  -5,  16,   4);
        add(_grid);
        util::labelfont(this);
        _align->add("Left align");
        _align->add("Center align");
        _align->add("Right align");
        _align->tooltip("Line numbers are only used for left aligned text.");
        _align->value(0);
        _border->tooltip("Print line border around the print area.");
        _close->callback(_DlgPrintText::Callback, this);
        _file->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _file->callback(_DlgPrintText::Callback, this);
        _file->tooltip("Select output PostScript file.");
        _fonts->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _fonts->callback(_DlgPrintText::Callback, this);
        _fonts->tooltip("Select font to use.");
        _label->align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _label->box(FL_BORDER_BOX);
        _label->box(FL_THIN_DOWN_BOX);
        _line->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _line->callback(_DlgPrintText::Callback, this);
        _line->color(FL_BACKGROUND2_COLOR);
        _line->range(0, 6);
        _line->precision(0);
        _line->value(0);
        _line->tooltip("Set minimum line number width.\nSet to 0 to disable.");
        _print->callback(_DlgPrintText::Callback, this);
        _tab->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _tab->callback(_DlgPrintText::Callback, this);
        _tab->color(FL_BACKGROUND2_COLOR);
        _tab->range(0, 16);
        _tab->precision(0);
        _tab->value(0);
        _tab->tooltip("Replace tabs with spaces.\nSet to 0 to disable.");
        _wrap->tooltip("Wrap long lines or they will be clipped.");
        dlg::_init_printer_formats(_format, _layout);
        _DlgPrintText::Callback(_line, this);
        _DlgPrintText::Callback(_tab, this);
        callback(_DlgPrintText::Callback, this);
        copy_label(title.c_str());
        size_range(flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 35);
        set_modal();
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
        set_label();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgPrintText*>(o);
        if (w == self) {
            self->hide();
        }
        else if (w == self->_close) {
            self->hide();
        }
        else if (w == self->_file) {
            auto filename = fl_file_chooser("Save To PostScript File", "PostScript Files (*.ps)\tAll Files (*)", self->_file->label());
            if (filename != nullptr) {
                self->_file->copy_label(filename);
            }
        }
        else if (w == self->_fonts) {
            auto dlg = FontDialog(self->_font, self->_fontsize, "Select Print Font", true);
            if (dlg.run() == true) {
                auto l = util::format("%s - %d", dlg.fontname().c_str(), dlg.fontsize());
                self->_fonts->copy_label(l.c_str());
                self->_font     = dlg.font();
                self->_fontsize = dlg.fontsize();
            }
        }
        else if (w == self->_line) {
            auto l = util::format("Line number: %d", (int) self->_line->value());
            self->_line->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_print) {
            self->print();
        }
        else if (w == self->_tab) {
            auto l = util::format("Tab replacement: %d", (int) self->_tab->value());
            self->_tab->copy_label(l.c_str());
            self->redraw();
        }
    }
    void print() {
        auto border  = _border->value();
        auto wrap    = _wrap->value();
        auto line    = static_cast<int>(_line->value());
        auto tab     = static_cast<int>(_tab->value());
        auto format  = static_cast<Fl_Paged_Device::Page_Format>(_format->value());
        auto layout  = (_layout->value() == 0) ? Fl_Paged_Device::Page_Layout::PORTRAIT : Fl_Paged_Device::Page_Layout::LANDSCAPE;
        auto align   = (_align->value() == 0) ? FL_ALIGN_LEFT : (_align->value() == 1) ? FL_ALIGN_CENTER : FL_ALIGN_RIGHT;
        auto file    = _file->label();
        auto printer = PrintText(file, format, layout, _font, _fontsize, align, wrap, border, line);
        auto err     = printer.print(_text, tab);
        if (err == "") {
            auto s = _label2;
            s += util::format("\n%d page%s was printed.", printer.page_count(), printer.page_count() > 1 ? "s" : "");
            _label->copy_label(s.c_str());
            _ret = true;
        }
        else {
            auto s = _label2;
            s += util::format("\nPrinting failed!\n%s", err.c_str());
            _label->copy_label(s.c_str());
            _ret = false;
        }
        redraw();
    }
    bool run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        return _ret;
    }
    void set_label() {
        auto len = 0;
        for (auto& s : _text) {
            auto l = fl_utf_nb_char((unsigned char*) s.c_str(), (int) s.length());
            if (l > len) {
                len = l;
            }
        }
        _label2 = util::format("Text contains %u lines.\nMax line length are %u characters.", (unsigned) _text.size(), (unsigned) len);
        _label->copy_label(_label2.c_str());
    }
};
bool print_text(std::string title, const std::string& text, Fl_Window* parent) {
    auto lines = util::split_string(text, "\n");
    _DlgPrintText dlg(title, parent, lines);
    return dlg.run();
}
bool print_text(std::string title, const StringVector& text, Fl_Window* parent) {
    _DlgPrintText dlg(title, parent, text);
    return dlg.run();
}
class _DlgSelect : public Fl_Double_Window {
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Input*                   _filter;
    GridGroup*                  _grid;
    ScrollBrowser*              _list;
    const StringVector&         _strings;
public:
    _DlgSelect(const char* title, Fl_Window* parent, const StringVector& strings, int selected_string_index, std::string selected_string, bool fixed_font, int W, int H) :
    Fl_Double_Window(0, 0, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H),
    _strings(strings) {
        end();
        _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _close  = new Fl_Return_Button(0, 0, 0, 0, "&Select");
        _filter = new Fl_Input(0, 0, 0, 0);
        _grid   = new GridGroup(0, 0, w(), h());
        _list   = new ScrollBrowser(0, 0, 0, 0);
        _grid->add(_filter,   1,  1, -1,  4);
        _grid->add(_list,     1,  6, -1, -6);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_close,  -17, -5, 16,  4);
        add(_grid);
        _cancel->callback(_DlgSelect::Callback, this);
        _close->callback(_DlgSelect::Callback, this);
        _filter->callback(_DlgSelect::Callback, this);
        _filter->tooltip("Enter text to filter rows that macthes the text.\nPress tab to switch focus between input and list widget.");
        _filter->when(FL_WHEN_CHANGED);
        _list->callback(_DlgSelect::Callback, this);
        _list->tooltip("Use Page Up or Page Down in list to scroll faster,");
        if (fixed_font == true) {
            _filter->textfont(flw::PREF_FIXED_FONT);
            _filter->textsize(flw::PREF_FIXED_FONTSIZE);
            _list->textfont(flw::PREF_FIXED_FONT);
            _list->textsize(flw::PREF_FIXED_FONTSIZE);
        }
        else {
            _filter->textfont(flw::PREF_FONT);
            _filter->textsize(flw::PREF_FONTSIZE);
            _list->textfont(flw::PREF_FONT);
            _list->textsize(flw::PREF_FONTSIZE);
        }
        {
            auto r = 0;
            auto f = 0;
            for (const auto& string : _strings) {
                _list->add(string.c_str());
                if (string == selected_string) {
                    r = f + 1;
                }
                f++;
            }
            if (selected_string_index > 0 && selected_string_index <= (int) _strings.size()) {
                _list->value(selected_string_index);
                _list->middleline(selected_string_index);
            }
            else if (r > 0) {
                _list->value(r);
                _list->middleline(r);
            }
            else {
                _list->value(1);
            }
        }
        _filter->take_focus();
        util::labelfont(this);
        callback(_DlgSelect::Callback, this);
        copy_label(title);
        activate_button();
        size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
        set_modal();
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
    }
    void activate_button() {
        if (_list->value() == 0) {
            _close->deactivate();
        }
        else {
            _close->activate();
        }
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgSelect*>(o);
        if (w == self || w == self->_cancel) {
            self->_list->deselect();
            self->hide();
        }
        else if (w == self->_filter) {
            self->filter(self->_filter->value());
            self->activate_button();
        }
        else if (w == self->_list) {
            self->activate_button();
            if (Fl::event_clicks() > 0 && self->_close->active()) {
                Fl::event_clicks(0);
                self->hide();
            }
        }
        else if (w == self->_close) {
            self->hide();
        }
    }
    void filter(const char* filter) {
        _list->clear();
        for (const auto& string : _strings) {
            if (*filter == 0) {
                _list->add(string.c_str());
            }
            else if (strstr(string.c_str(), filter) != nullptr) {
                _list->add(string.c_str());
            }
        }
        _list->value(1);
    }
    int handle(int event) override {
        if (event == FL_KEYDOWN) {
            if (Fl::event_key() == FL_Enter) {
                if (_list->value() > 0) {
                    hide();
                }
                return 1;
            }
            else if (Fl::event_key() == FL_Tab) {
                if (Fl::focus() == _list || Fl::focus() == _list->menu()) {
                    _filter->take_focus();
                }
                else {
                    _list->take_focus();
                }
                return 1;
            }
        }
        return Fl_Double_Window::handle(event);
    }
    int run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        auto row = _list->value();
        if (row > 0) {
            auto selected = _list->text(row);
            for (int f = 0; f < (int) _strings.size(); f++) {
                const auto& string = _strings[f];
                if (string == selected) {
                    return f + 1;
                }
            }
        }
        return 0;
    }
};
int select(std::string title, const StringVector& list, int selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, selected_row, "", fixed_font, W, H);
    return dlg.run();
}
int select(std::string title, const StringVector& list, const std::string& selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, 0, selected_row, fixed_font, W, H);
    return dlg.run();
}
class _DlgSlider : public Fl_Double_Window {
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Hor_Value_Slider*        _slider;
    GridGroup*                  _grid;
    bool                        _ret;
    double&                     _value;
public:
    _DlgSlider(const char* title, Fl_Window* parent, double min, double max, double& value, double step) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 40, flw::PREF_FONTSIZE * 6),
    _value(value) {
        end();
        _ret    = false;
        _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _slider = new Fl_Hor_Value_Slider(0, 0, 0, 0);
        _close  = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
        _grid   = new GridGroup(0, 0, w(), h());
        _grid->add(_slider,   1,  1, -1,  4);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_close,  -17, -5, 16,  4);
        add(_grid);
        _cancel->callback(_DlgSlider::Callback, this);
        _slider->align(FL_ALIGN_LEFT    );
        _slider->callback(_DlgSlider::Callback, this);
        _slider->range(min, max);
        _slider->value(value);
        _slider->step(step);
        _slider->textfont(flw::PREF_FONT);
        _slider->textsize(flw::PREF_FONTSIZE);
        _close->callback(_DlgSlider::Callback, this);
        util::labelfont(this);
        callback(_DlgSlider::Callback, this);
        copy_label(title);
        set_modal();
        resizable(_grid);
        size_range(flw::PREF_FONTSIZE * 40, flw::PREF_FONTSIZE * 6);
        util::center_window(this, parent);
        _grid->do_layout();
        _slider->value_width((max >= 100'000) ? flw::PREF_FONTSIZE * 10 : flw::PREF_FONTSIZE * 6);
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgSlider*>(o);
        if (w == self || w == self->_cancel) {
            self->hide();
        }
        else if (w == self->_close) {
            self->_ret = true;
            self->_value = self->_slider->value();
            self->hide();
        }
    }
    int run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        return _ret;
    }
};
bool slider(std::string title, double min, double max, double& value, double step, Fl_Window* parent) {
    _DlgSlider dlg(title.c_str(), parent, min, max, value, step);
    return dlg.run();
}
class _DlgText : public Fl_Double_Window {
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Button*                  _save;
    Fl_Text_Buffer*             _buffer;
    Fl_Text_Display*            _text;
    GridGroup*                  _grid;
    bool                        _edit;
    char*                       _res;
public:
    _DlgText(const char* title, const char* text, bool edit, Fl_Window* parent, int W, int H) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W, flw::PREF_FONTSIZE * H) {
        end();
        _buffer = new Fl_Text_Buffer();
        _cancel = new Fl_Button(0, 0, 0, 0, "C&ancel");
        _close  = (edit == false) ? new Fl_Return_Button(0, 0, 0, 0, "&Close") : new Fl_Button(0, 0, 0, 0, "&Update");
        _grid   = new GridGroup(0, 0, w(), h());
        _save   = new Fl_Button(0, 0, 0, 0, "&Save");
        _text   = (edit == false) ? new Fl_Text_Display(0, 0, 0, 0) : new Fl_Text_Editor(0, 0, 0, 0);
        _edit   = edit;
        _res    = nullptr;
        _grid->add(_text,     1,   1,  -1,  -6);
        _grid->add(_cancel, -51,  -5,  16,   4);
        _grid->add(_save,   -34,  -5,  16,   4);
        _grid->add(_close,  -17,  -5,  16,   4);
        add(_grid);
        _buffer->text(text);
        _cancel->callback(_DlgText::Callback, this);
        _cancel->tooltip("Close and abort all changes.");
        _close->callback(_DlgText::Callback, this);
        _close->tooltip(_edit == true ? "Update text and close window." : "Close window.");
        _save->callback(_DlgText::Callback, this);
        _save->tooltip("Save text to file.");
        _text->buffer(_buffer);
        _text->linenumber_align(FL_ALIGN_RIGHT);
        _text->linenumber_bgcolor(FL_BACKGROUND_COLOR);
        _text->linenumber_fgcolor(FL_FOREGROUND_COLOR);
        _text->linenumber_font(flw::PREF_FIXED_FONT);
        _text->linenumber_size(flw::PREF_FIXED_FONTSIZE);
        _text->linenumber_width(flw::PREF_FIXED_FONTSIZE * 3);
        _text->take_focus();
        _text->textfont(flw::PREF_FIXED_FONT);
        _text->textsize(flw::PREF_FIXED_FONTSIZE);
        util::labelfont(this);
        if (edit == false) {
            _cancel->hide();
        }
        callback(_DlgText::Callback, this);
        copy_label(title);
        size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
        set_modal();
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
    }
    ~_DlgText() {
       _text->buffer(nullptr);
        delete _buffer;
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgText*>(o);
        if (w == self || w == self->_cancel) {
            self->hide();
        }
        else if (w == self->_save) {
            auto filename = fl_file_chooser("Select Destination File", nullptr, nullptr, 0);
            if (filename != nullptr && self->_buffer->savefile(filename) != 0) {
                fl_alert("error: failed to save text to %s", filename);
            }
        }
        else if (w == self->_close) {
            if (self->_edit == true) {
                self->_res = self->_buffer->text();
            }
            self->hide();
        }
    }
    char* run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        return _res;
    }
};
void text(std::string title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgText dlg(title.c_str(), text.c_str(), false, parent, W, H);
    dlg.run();
}
bool text_edit(std::string title, std::string& text, Fl_Window* parent, int W, int H) {
    auto dlg = _DlgText(title.c_str(), text.c_str(), true, parent, W, H);
    auto res = dlg.run();
    if (res == nullptr) {
        return false;
    }
    text = res;
    free(res);
    return true;
}
class _DlgTheme : public Fl_Double_Window {
    Fl_Box*                     _fixed_label;
    Fl_Box*                     _font_label;
    Fl_Browser*                 _theme;
    Fl_Button*                  _close;
    Fl_Button*                  _fixedfont;
    Fl_Button*                  _font;
    GridGroup*                  _grid;
    int                         _theme_row;
public:
    _DlgTheme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) :
    Fl_Double_Window(0, 0, 10, 10, "Set Theme") {
        end();
        _close       = new Fl_Return_Button(0, 0, 0, 0, "&Close");
        _fixedfont   = new Fl_Button(0, 0, 0, 0, "&Mono font");
        _fixed_label = new Fl_Box(0, 0, 0, 0);
        _font        = new Fl_Button(0, 0, 0, 0, "&Regular font");
        _font_label  = new Fl_Box(0, 0, 0, 0);
        _grid        = new GridGroup(0, 0, w(), h());
        _theme       = new Fl_Hold_Browser(0, 0, 0, 0);
        _theme_row   = 0;
        _grid->add(_theme,         1,   1,  -1, -16);
        _grid->add(_font_label,    1, -15,  -1,   4);
        _grid->add(_fixed_label,   1, -10,  -1,   4);
        _grid->add(_font,        -51,  -5,  16,   4);
        _grid->add(_fixedfont,   -34,  -5,  16,   4);
        _grid->add(_close,       -17,  -5,  16,   4);
        add(_grid);
        if (enable_font == false) {
          _font->deactivate();
        }
        if (enable_fixedfont == false) {
          _fixedfont->deactivate();
        }
        _close->callback(Callback, this);
        _fixed_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _fixed_label->box(FL_BORDER_BOX);
        _fixed_label->color(FL_BACKGROUND2_COLOR);
        _fixedfont->callback(Callback, this);
        _font->callback(Callback, this);
        _font_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _font_label->box(FL_BORDER_BOX);
        _font_label->color(FL_BACKGROUND2_COLOR);
        _theme->box(FL_BORDER_BOX);
        _theme->callback(Callback, this);
        _theme->textfont(flw::PREF_FONT);
        for (size_t f = 0; f < 100; f++) {
            auto t = flw::PREF_THEMES[f];
            if (t != nullptr) {
                _theme->add(t);
            }
            else {
                break;
            }
        }
        resizable(_grid);
        callback(Callback, this);
        set_modal();
        update_pref();
        util::center_window(this, parent);
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgTheme*>(o);
        if (w == self) {
            self->hide();
        }
        else if (w == self->_fixedfont) {
            FontDialog fd(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE, "Select Monospaced Font");
            if (fd.run(Fl::first_window()) == true) {
                flw::PREF_FIXED_FONT     = fd.font();
                flw::PREF_FIXED_FONTSIZE = fd.fontsize();
                flw::PREF_FIXED_FONTNAME = fd.fontname();
                if (self->_font->active() == 0) {
                    flw::PREF_FONTSIZE = flw::PREF_FIXED_FONTSIZE;
                }
                self->update_pref();
            }
        }
        else if (w == self->_font) {
            FontDialog fd(flw::PREF_FONT, flw::PREF_FONTSIZE, "Select Regular Font");
            if (fd.run(Fl::first_window()) == true) {
                flw::PREF_FONT     = fd.font();
                flw::PREF_FONTSIZE = fd.fontsize();
                flw::PREF_FONTNAME = fd.fontname();
                Fl_Tooltip::font(flw::PREF_FONT);
                Fl_Tooltip::size(flw::PREF_FONTSIZE);
                if (self->_fixedfont->active() == 0) {
                    flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
                }
                self->update_pref();
            }
        }
        else if (w == self->_theme) {
            auto row = self->_theme->value() - 1;
            if (row == theme::THEME_GLEAM) {
                theme::_load_gleam();
            }
            else if (row == theme::THEME_GLEAM_BLUE) {
                theme::_load_gleam_blue();
            }
            else if (row == theme::THEME_GLEAM_DARK) {
                theme::_load_gleam_dark();
            }
            else if (row == theme::THEME_GLEAM_TAN) {
                theme::_load_gleam_tan();
            }
            else if (row == theme::THEME_GTK) {
                theme::_load_gtk();
            }
            else if (row == theme::THEME_GTK_BLUE) {
                theme::_load_gtk_blue();
            }
            else if (row == theme::THEME_GTK_DARK) {
                theme::_load_gtk_dark();
            }
            else if (row == theme::THEME_GTK_TAN) {
                theme::_load_gtk_tan();
            }
            else if (row == theme::THEME_OXY) {
                theme::_load_oxy();
            }
            else if (row == theme::THEME_OXY_TAN) {
                theme::_load_oxy_tan();
            }
            else if (row == theme::THEME_PLASTIC) {
                theme::_load_plastic();
            }
            else if (row == theme::THEME_PLASTIC_TAN) {
                theme::_load_plastic_tan();
            }
            else {
                theme::_load_default();
            }
            self->update_pref();
        }
        else if (w == self->_close) {
            self->hide();
        }
    }
    void run() {
        show();
        while (visible()) {
            Fl::wait();
            Fl::flush();
        }
    }
    void update_pref() {
        Fl_Tooltip::font(flw::PREF_FONT);
        Fl_Tooltip::size(flw::PREF_FONTSIZE);
        util::labelfont(this);
        _font_label->copy_label(util::format("%s - %d", flw::PREF_FONTNAME.c_str(), flw::PREF_FONTSIZE).c_str());
        _fixed_label->copy_label(util::format("%s - %d", flw::PREF_FIXED_FONTNAME.c_str(), flw::PREF_FIXED_FONTSIZE).c_str());
        _fixed_label->labelfont(flw::PREF_FIXED_FONT);
        _fixed_label->labelsize(flw::PREF_FIXED_FONTSIZE);
        _theme->textsize(flw::PREF_FONTSIZE);
        size(flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 28);
        size_range(flw::PREF_FONTSIZE * 20, flw::PREF_FONTSIZE * 14);
        _grid->resize(0, 0, w(), h());
        theme::_scrollbar();
        for (int f = 0; f < theme::THEME_NIL; f++) {
            if (flw::PREF_THEME == flw::PREF_THEMES[f]) {
                _theme->value(f + 1);
                break;
            }
        }
        Fl::redraw();
    }
};
void theme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) {
    auto dlg = _DlgTheme(enable_font, enable_fixedfont, parent);
    dlg.run();
}
AbortDialog::AbortDialog(std::string label, double min, double max) :
Fl_Double_Window(0, 0, 0, 0, "Working...") {
    _button   = new Fl_Button(0, 0, 0, 0, "Press to abort");
    _grid     = new GridGroup();
    _progress = new Fl_Hor_Fill_Slider(0, 0, 0, 0);
    _abort    = false;
    _last     = 0;
    _grid->add(_button,     1,   1,   -1,  -6);
    _grid->add(_progress,   1,  -5,   -1,   4);
    add(_grid);
    auto W = flw::PREF_FONTSIZE * 32;
    auto H = flw::PREF_FONTSIZE * 12;
    if (min < max && fabs(max - min) > 0.001) {
        _progress->range(min, max);
        _progress->value(min);
    }
    else {
        _progress->hide();
        _grid->resize(_button, 1, 1, -1, -1);
    }
    _button->callback(AbortDialog::Callback, this);
    _button->labelfont(flw::PREF_FONT);
    _button->labelsize(flw::PREF_FONTSIZE);
    _progress->color(FL_SELECTION_COLOR);
    if (label != "") {
        copy_label(label.c_str());
    }
    resizable(this);
    size(W, H);
    size_range(W, H);
    callback(AbortDialog::Callback, this);
    set_modal();
}
void AbortDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<AbortDialog*>(o);
    if (w == self->_button) {
        self->_abort = true;
    }
}
bool AbortDialog::check(int milliseconds) {
    auto now = util::milliseconds();
    if (now - _last > milliseconds) {
        _last = now;
        Fl::check();
    }
    return _abort;
}
bool AbortDialog::check(double value, double min, double max, int milliseconds) {
    auto now = util::milliseconds();
    if (now - _last > milliseconds) {
        _progress->value(value);
        _progress->range(min, max);
        _last = now;
        Fl::check();
    }
    return _abort;
}
void AbortDialog::range(double min, double max) {
    _progress->range(min, max);
}
void AbortDialog::show(const std::string& label, Fl_Window* parent) {
    _abort = false;
    _last  = 0;
    _button->copy_label(label.c_str());
    util::center_window(this, parent);
    Fl_Double_Window::show();
    Fl::flush();
}
void AbortDialog::value(double value) {
    _progress->value(value);
}
static const std::string _FONTDIALOG_LABEL = R"(
ABCDEFGHIJKLMNOPQRSTUVWXYZ /0123456789
abcdefghijklmnopqrstuvwxyz £©µÀÆÖÞßéöÿ
–—‘“”„†•…‰™œŠŸž€ ΑΒΓΔΩαβγδω АБВГДабвгд
∀∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂӥẄɐː⍎אԱა

japanese: こんにちは世界
korean: 안녕하세요 세계
greek: Γειά σου Κόσμε
ukrainian: Привіт Світ
thai: สวัสดีชาวโลก
amharic: ሰላም ልዑል
braille: ⡌⠁⠧⠑ ⠼⠁⠒  ⡍⠜⠇⠑⠹⠰⠎ ⡣⠕⠌
math: ∮ E⋅da = Q,  n → ∞, ∑ f(i) 2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ

“There is nothing else than now.
There is neither yesterday, certainly,
nor is there any tomorrow.
How old must you be before you know that?
There is only now, and if now is only two days,
then two days is your life and everything in it will be in proportion.
This is how you live a life in two days.
And if you stop complaining and asking for what you never will get,
you will have a good life.
A good life is not measured by any biblical span.”
― Ernest Hemingway, For Whom the Bell Tolls
)";
class _FontDialogLabel : public Fl_Box {
public:
    int font;
    int size;
    _FontDialogLabel(int x, int y, int w, int h) : Fl_Box(x, y, w, h, _FONTDIALOG_LABEL.c_str()) {
        font = FL_HELVETICA;
        size = 14;
        align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        box(FL_BORDER_BOX);
        color(FL_BACKGROUND2_COLOR);
    }
    void draw() override {
        draw_box();
        fl_font((Fl_Font) font, size);
        fl_color(FL_FOREGROUND_COLOR);
        fl_draw(label(), x() + 3, y() + 3, w() - 6, h() - 6, align());
    }
};
FontDialog::FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& label, bool limit_to_default) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 64, flw::PREF_FONTSIZE * 36) {
    _create(font, "", fontsize, label, limit_to_default);
}
FontDialog::FontDialog(std::string font, Fl_Fontsize fontsize, std::string label, bool limit_to_default) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 64, flw::PREF_FONTSIZE * 36) {
    _create(0, font, fontsize, label, limit_to_default);
}
void FontDialog::_activate() {
    if (_fonts->value() == 0 || _sizes->value() == 0 || (_fonts->active() == 0 && _sizes->active() == 0)) {
        _select->deactivate();
    }
    else {
        _select->activate();
    }
}
void FontDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<FontDialog*>(o);
    if (w == self) {
        self->hide();
    }
    else if (w == self->_cancel) {
        self->hide();
    }
    else if (w == self->_fonts) {
        auto row = self->_fonts->value();
        if (row > 0) {
            static_cast<_FontDialogLabel*>(self->_label)->font = row - 1;
        }
        self->_activate();
        Fl::redraw();
    }
    else if (w == self->_select) {
        auto row1 = self->_fonts->value();
        auto row2 = self->_sizes->value();
        if (row1 > 0 && row2 > 0) {
            row1--;
            self->_fontname = util::remove_browser_format(flw::PREF_FONTNAMES[row1]);
            self->_font     = row1;
            self->_fontsize = row2 + 5;
            self->_ret      = true;
            self->hide();
        }
    }
    else if (w == self->_sizes) {
        auto row = self->_sizes->value();
        if (row > 0) {
            static_cast<_FontDialogLabel*>(self->_label)->size = row + 5;
        }
        self->_activate();
        Fl::redraw();
    }
}
void FontDialog::_create(Fl_Font font, std::string fontname, Fl_Fontsize fontsize, std::string label, bool limit_to_default) {
    end();
    _cancel   = new Fl_Button(0, 0, 0, 0, "&Cancel");
    _fonts    = new ScrollBrowser(12);
    _grid     = new GridGroup();
    _label    = new _FontDialogLabel(0, 0, 0, 0);
    _select   = new Fl_Button(0, 0, 0, 0, "&Select");
    _sizes    = new ScrollBrowser(6);
    _font     = -1;
    _fontsize = -1;
    _ret      = false;
    _grid->add(_fonts,    1,   1,  50,  -6);
    _grid->add(_sizes,   52,   1,  12,  -6);
    _grid->add(_label,   65,   1,  -1,  -6);
    _grid->add(_cancel, -34,  -5,  16,   4);
    _grid->add(_select, -17,  -5,  16,   4);
    add(_grid);
    _cancel->callback(FontDialog::Callback, this);
    _cancel->labelfont(flw::PREF_FONT);
    _cancel->labelsize(flw::PREF_FONTSIZE);
    _fonts->box(FL_BORDER_BOX);
    _fonts->callback(FontDialog::Callback, this);
    _fonts->textsize(flw::PREF_FONTSIZE);
    _fonts->when(FL_WHEN_CHANGED);
    static_cast<_FontDialogLabel*>(_label)->font = font;
    static_cast<_FontDialogLabel*>(_label)->size = fontsize;
    _select->callback(FontDialog::Callback, this);
    _select->labelfont(flw::PREF_FONT);
    _select->labelsize(flw::PREF_FONTSIZE);
    _sizes->box(FL_BORDER_BOX);
    _sizes->callback(FontDialog::Callback, this);
    _sizes->textsize(flw::PREF_FONTSIZE);
    _sizes->when(FL_WHEN_CHANGED);
    theme::load_fonts();
    auto count = 0;
    for (auto name : flw::PREF_FONTNAMES) {
        if (limit_to_default == true && count == 12) {
            break;
        }
        _fonts->add(name);
        count++;
    }
    for (auto f = 6; f <= 72; f++) {
        char buf[50];
        snprintf(buf, 50, "@r%2d  ", f);
        _sizes->add(buf);
    }
    if (fontsize >= 6 && fontsize <= 72) {
        _sizes->value(fontsize - 5);
        _sizes->middleline(fontsize - 5);
        static_cast<_FontDialogLabel*>(_label)->font = fontsize;
    }
    else {
        _sizes->value(14 - 5);
        _sizes->middleline(14 - 5);
        static_cast<_FontDialogLabel*>(_label)->font = 14;
    }
    if (fontname != "") {
        _select_name(fontname);
    }
    else if (font >= 0 && font < _fonts->size()) {
        _fonts->value(font + 1);
        _fonts->middleline(font + 1);
        static_cast<_FontDialogLabel*>(_label)->font = font;
    }
    else {
        _fonts->value(1);
        _fonts->middleline(1);
    }
    resizable(this);
    copy_label(label.c_str());
    callback(FontDialog::Callback, this);
    size_range(flw::PREF_FONTSIZE * 38, flw::PREF_FONTSIZE * 12);
    set_modal();
    _fonts->take_focus();
    _grid->do_layout();
}
bool FontDialog::run(Fl_Window* parent) {
    _ret = false;
    _activate();
    util::center_window(this, parent);
    show();
    while (visible() != 0) {
        Fl::wait();
        Fl::flush();
    }
    return _ret;
}
void FontDialog::_select_name(std::string fontname) {
    auto count = 1;
    for (auto f : flw::PREF_FONTNAMES) {
        auto font_without_style = util::remove_browser_format(f);
        if (fontname == font_without_style) {
            _fonts->value(count);
            _fonts->middleline(count);
            static_cast<_FontDialogLabel*>(_label)->font = count - 1;
            return;
        }
        count++;
    }
    _fonts->value(1);
    static_cast<_FontDialogLabel*>(_label)->font = 0;
}
WorkDialog::WorkDialog(const char* title, Fl_Window* parent, bool cancel, bool pause, int W, int H) :
Fl_Double_Window(0, 0, W * flw::PREF_FONTSIZE, H * flw::PREF_FONTSIZE) {
    end();
    _cancel = new Fl_Button(0, 0, 0, 0, "Cancel");
    _grid   = new GridGroup();
    _label  = new Fl_Hold_Browser(0, 0, 0, 0);
    _pause  = new Fl_Toggle_Button(0, 0, 0, 0, "Pause");
    _ret    = true;
    _last   = 0.0;
    _grid->add(_label,    1,   1,  -1,  -6);
    _grid->add(_pause,  -34,  -5,  16,   4);
    _grid->add(_cancel, -17,  -5,  16,   4);
    add(_grid);
    _cancel->callback(WorkDialog::Callback, this);
    _label->box(FL_BORDER_BOX);
    _label->textfont(flw::PREF_FONT);
    _label->textsize(flw::PREF_FONTSIZE);
    _pause->callback(WorkDialog::Callback, this);
    if (cancel == false) {
        _cancel->deactivate();
    }
    if (pause == false) {
        _pause->deactivate();
    }
    util::labelfont(this);
    callback(WorkDialog::Callback, this);
    copy_label(title);
    size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
    set_modal();
    resizable(this);
    util::center_window(this, parent);
    show();
}
void WorkDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<WorkDialog*>(o);
    if (w == self) {
    }
    else if (w == self->_cancel) {
        self->_ret = false;
    }
    else if (w == self->_pause) {
        bool cancel = self->_cancel->active();
        self->_cancel->deactivate();
        self->_pause->label("C&ontinue");
        while (self->_pause->value() != 0) {
            util::sleep(10);
            Fl::check();
        }
        self->_pause->label("&Pause");
        if (cancel) {
            self->_cancel->activate();
        }
    }
}
bool WorkDialog::run(double update_time, const StringVector& messages) {
    auto now = util::clock();
    if ((now - _last) > update_time) {
        _label->clear();
        for (const auto& s : messages) {
            _label->add(s.c_str());
        }
        _last = now;
        Fl::check();
        Fl::flush();
    }
    return _ret;
}
bool WorkDialog::run(double update_time, const std::string& message) {
    auto now = util::clock();
    if ((now - _last) > update_time) {
        _label->clear();
        _label->add(message.c_str());
        _last = now;
        Fl::check();
        Fl::flush();
    }
    return _ret;
}
}
}
namespace flw {
struct _GridGroupChild {
    Fl_Widget*                  widget;
    short                       x;
    short                       y;
    short                       w;
    short                       h;
    short                       l;
    short                       r;
    short                       t;
    short                       b;
    _GridGroupChild(Fl_Widget* WIDGET, int X, int Y, int W, int H) {
        set(WIDGET, X, Y, W, H);
        adjust();
    }
    void adjust(int L = 0, int R = 0, int T = 0, int B = 0) {
        l = L;
        r = R;
        t = T;
        b = B;
    }
    void set(Fl_Widget* WIDGET, int X, int Y, int W, int H) {
        widget = WIDGET;
        x      = X;
        y      = Y;
        w      = W;
        h      = H;
    }
};
GridGroup::GridGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);
    _size = 0;
}
GridGroup::~GridGroup() {
    for (auto v : _widgets) {
        delete static_cast<_GridGroupChild*>(v);
    }
}
void GridGroup::add(Fl_Widget* widget, int X, int Y, int W, int H) {
    _widgets.push_back(new _GridGroupChild(widget, X, Y, W, H));
    Fl_Group::add(widget);
}
void GridGroup::adjust(Fl_Widget* widget, int L, int R, int T, int B) {
    for (auto& v : _widgets) {
        auto child = static_cast<_GridGroupChild*>(v);
        if (child->widget == widget) {
            child->adjust(L, R, T, B);
            return;
        }
    }
    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::adjust() failed to find widget (label=%s)\n", widget->label());
    #endif
}
void GridGroup::clear() {
    _widgets.clear();
    Fl_Group::clear();
}
int GridGroup::handle(int event) {
    if (event == FL_KEYDOWN && Fl::event_key() == FL_Tab) {
        if (children() > 0) {
            Fl_Widget* first   = nullptr;
            Fl_Widget* last    = nullptr;
            Fl_Widget* current = Fl::focus();
            _last_active_widget(&first, &last);
            if (Fl::event_shift() == 0) {
                if (first != nullptr && current != nullptr && current == last) {
                    Fl::focus(first);
                    first->redraw();
                    return 1;
                }
            }
            else {
                if (first != nullptr && current != nullptr && current == first) {
                    Fl::focus(last);
                    last->redraw();
                    return 1;
                }
            }
        }
    }
    return Fl_Group::handle(event);
}
void GridGroup::_last_active_widget(Fl_Widget** first, Fl_Widget** last) {
    for (int f = 0; f < children(); f++) {
        auto c = child(f);
        auto g = c->as_group();
        if (g == nullptr) {
            if (c->active() != 0) {
                *last = c;
            }
            if (*first == nullptr && c->active() != 0) {
                *first = c;
            }
        }
    }
}
Fl_Widget* GridGroup::remove(Fl_Widget* widget) {
    for (auto it = _widgets.begin(); it != _widgets.end(); it++) {
        auto child = static_cast<_GridGroupChild*>(*it);
        if (child->widget == widget) {
            delete child;
            Fl_Group::remove(widget);
            _widgets.erase(it);
            return widget;
        }
    }
    #ifdef DEBUG
        fprintf(stderr, "error: GridGroup::remove can't find widget\n");
    #endif
    return nullptr;
}
void GridGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    if (children() == 0 || W == 0 || H == 0 || visible() == 0) {
        return;
    }
    int size = (_size > 0) ? _size : flw::PREF_FONTSIZE / 2;
    for (const auto& v : _widgets) {
        auto child = static_cast<_GridGroupChild*>(v);
        if (child->widget != nullptr && child->widget->visible() != 0) {
            int widget_x  = 0;
            int widget_x2 = 0;
            int widget_y  = 0;
            int widget_y2 = 0;
            int widget_w  = 0;
            int widget_h  = 0;
            if (child->x >= 0) {
                widget_x = X + child->x * size;
            }
            else {
                widget_x = X + W + child->x * size;
            }
            if (child->y >= 0) {
                widget_y = Y + child->y * size;
            }
            else {
                widget_y = Y + H + child->y * size;
            }
            if (child->w == 0) {
                widget_x2 = X + W;
            }
            else if (child->w > 0) {
                widget_x2 = widget_x + child->w * size;
            }
            else {
                widget_x2 = X + W + child->w * size;
            }
            if (child->h == 0) {
                widget_y2 = Y + H;
            }
            else if (child->h > 0) {
                widget_y2 = widget_y + child->h * size;
            }
            else {
                widget_y2 = Y + H + child->h * size;
            }
            widget_w = widget_x2 - widget_x;
            widget_h = widget_y2 - widget_y;
            if (widget_w >= 0 && widget_h >= 0) {
                child->widget->resize(widget_x + child->l, widget_y + child->t, widget_w + child->r, widget_h + child->b);
            }
            else {
                child->widget->resize(0, 0, 0, 0);
            }
        }
    }
}
void GridGroup::resize(Fl_Widget* widget, int X, int Y, int W, int H) {
    for (auto& v : _widgets) {
        auto child = static_cast<_GridGroupChild*>(v);
        if (child->widget == widget) {
            child->set(widget, X, Y, W, H);
            return;
        }
    }
    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::resize() failed to find widget (label=%s)\n", widget->label());
    #endif
}
}
#include <algorithm>
namespace flw {
static const std::string _INPUTMENU_TOOLTIP = "Use up/down arrows to switch between previous values\nPress ctrl + space to open menu button (if visible)";
class _InputMenu : public Fl_Input {
public:
    bool            show_menu;
    int             index;
    StringVector    history;
    _InputMenu() : Fl_Input(0, 0, 0, 0) {
        tooltip(_INPUTMENU_TOOLTIP.c_str());
        index     = -1;
        show_menu = false;
    }
    int handle(int event) override {
        if (event == FL_KEYBOARD) {
            auto key = Fl::event_key();
            if (Fl::event_ctrl() != 0 && key == ' ') {
                if (history.size() > 0) {
                    show_menu = true;
                    do_callback();
                }
                return 1;
            }
            else if (key == FL_Up && history.size() > 0) {
                if (index <= 0) {
                    value("");
                    index = -1;
                }
                else {
                    index--;
                    show_menu = false;
                    value(history[index].c_str());
                }
                return 1;
            }
            else if (key == FL_Down && history.size() > 0 && index < (int) history.size() - 1) {
                index++;
                value(history[index].c_str());
                show_menu = false;
                return 1;
            }
            else {
                show_menu = false;
            }
        }
        return Fl_Input::handle(event);
    }
};
InputMenu::InputMenu(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    _input = new flw::_InputMenu();
    _menu  = new Fl_Menu_Button(0, 0, 0, 0);
    Fl_Group::add(_input);
    Fl_Group::add(_menu);
    _input->callback(InputMenu::Callback, this);
    _input->when(FL_WHEN_ENTER_KEY_ALWAYS);
    _menu->callback(InputMenu::Callback, this);
    update_pref();
}
void InputMenu::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<InputMenu*>(o);
    if (w == self->_input) {
        if (self->_input->show_menu) {
            if (self->_menu->visible()) {
                self->_menu->popup();
            }
        }
        else {
            self->do_callback();
        }
    }
    else if (w == self->_menu) {
        auto index = self->_menu->find_index(self->_menu->text());
        if (index >= 0 && index < (int) self->_input->history.size()) {
            self->_input->value(self->_input->history[index].c_str());
            self->_input->index = index;
        }
        self->_input->take_focus();
    }
}
void InputMenu::clear() {
    _menu->clear();
    _input->history.clear();
    _input->index = -1;
}
flw::StringVector InputMenu::get_history() const {
    return _input->history;
}
void InputMenu::insert(std::string string, int max_list_len) {
    for (auto it = _input->history.begin(); it != _input->history.end(); ++it) {
        if (*it == string) {
            _input->history.erase(it);
            break;
        }
    }
    _input->history.insert(_input->history.begin(), string);
    while ((int) _input->history.size() > max_list_len) {
        _input->history.pop_back();
    }
    _menu->clear();
    for (const auto& s : _input->history) {
        _menu->add(flw::util::fix_menu_string(s.c_str()).c_str());
    }
    _input->index = -1;
    _input->value(string.c_str());
    _input->insert_position(string.length(), 0);
}
void InputMenu::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    if (_menu->visible() != 0) {
        auto mw = (int) flw::PREF_FONTSIZE;
        _input->resize(X, Y, W - flw::PREF_FONTSIZE - mw, H);
        _menu->resize(X + W - flw::PREF_FONTSIZE - mw, Y, flw::PREF_FONTSIZE + mw, H);
    }
    else {
        _input->resize(X, Y, W, H);
    }
}
void InputMenu::update_pref(Fl_Font text_font, Fl_Fontsize text_size) {
    labelfont(flw::PREF_FONT);
    labelsize(flw::PREF_FONTSIZE);
    _input->labelfont(text_font);
    _input->labelsize(text_font);
    _input->textfont(text_font);
    _input->textsize(text_size);
    _menu->labelfont(text_font);
    _menu->labelsize(text_size);
    _menu->textfont(text_font);
    _menu->textsize(text_size);
}
const char* InputMenu::value() const {
    return _input->value();
}
void InputMenu::value(const char* string) {
    _input->value(string ? string : "");
}
void InputMenu::values(const StringVector& list, bool copy_first_to_input) {
    clear();
    _input->history = list;
    for (const auto& s : _input->history) {
        _menu->add(flw::util::fix_menu_string(s.c_str()).c_str());
    }
    if (list.size() > 0 && copy_first_to_input == true) {
        auto s = list.front();
        _input->value(s.c_str());
        _input->insert_position(s.length(), 0);
    }
}
}
namespace flw {
    RecentMenu::RecentMenu(Fl_Menu_* menu, Fl_Callback* file_callback, void* userdata, std::string base_label, std::string clear_label) {
        _menu     = menu;
        _callback = file_callback;
        _base     = base_label;
        _clear    = clear_label;
        _user     = userdata;
        _max      = 10;
        _menu->add((_base + _clear).c_str(), 0, RecentMenu::CallbackClear, this, FL_MENU_DIVIDER);
    }
    void RecentMenu::_add(std::string file, bool append) {
        if (file == "") {
            _files.clear();
        }
        else if (append == true) {
            _add_string(_files, _max, file);
        }
        else {
            _insert_string(_files, _max, file);
        }
        auto index = _menu->find_index(_base.c_str());
        if (index >= 0) {
            _menu->clear_submenu(index);
            _menu->add((_base + _clear).c_str(), 0, RecentMenu::CallbackClear, this, FL_MENU_DIVIDER);
        }
        for (const auto& f : _files) {
            _menu->add((_base + "/" + flw::util::fix_menu_string(f)).c_str(), 0, _callback, _user);
        }
    }
    size_t RecentMenu::_add_string(StringVector& in, size_t max_size, std::string string) {
        for (auto it = in.begin(); it != in.end(); ++it) {
            if (*it == string) {
                in.erase(it);
                break;
            }
        }
        in.push_back(string);
        while (in.size() > max_size) in.erase(in.begin());
        return in.size();
    }
    void RecentMenu::CallbackClear(Fl_Widget*, void* o) {
        auto self = static_cast<RecentMenu*>(o);
        self->_add("", false);
    }
    size_t RecentMenu::_insert_string(StringVector& in, size_t max_size, std::string string) {
        for (auto it = in.begin(); it != in.end(); ++it) {
            if (*it == string) {
                in.erase(it);
                break;
            }
        }
        in.insert(in.begin(), string);
        while (in.size() > max_size) in.pop_back();
        return (int) in.size();
    }
    void RecentMenu::load_pref(Fl_Preferences& pref, std::string base_name) {
        auto index = 1;
        char buffer[1000];
        while (index <= 100) {
            auto key = flw::util::format("%s%d", base_name.c_str(), index++);
            pref.get(key.c_str(), buffer, "", 1000);
            if (*buffer == 0) {
                break;
            }
            _add(buffer, true);
        }
    }
    void RecentMenu::save_pref(Fl_Preferences& pref, std::string base_name) {
        auto index = 1;
        for (const auto& s : _files) {
            pref.set(flw::util::format("%s%d", base_name.c_str(), index++).c_str(), s.c_str());
        }
        pref.set(flw::util::format("%s%d", base_name.c_str(), index++).c_str(), "");
    }
}
#include <FL/fl_draw.H>
flw::SplitGroup::SplitGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);
    clear();
}
void flw::SplitGroup::add(Fl_Widget* widget, SplitGroup::CHILD child) {
    auto num = child == SplitGroup::CHILD::FIRST ? 0 : 1;
    if (_widgets[num] != nullptr) {
        remove(_widgets[num]);
        delete _widgets[num];
    }
    _widgets[num] = widget;
    if (widget != nullptr) {
        Fl_Group::add(widget);
    }
}
void flw::SplitGroup::clear() {
    Fl_Group::clear();
    _drag       = false;
    _min        = 50;
    _split_pos  = -1;
    _direction  = DIRECTION::VERTICAL;
    _widgets[0] = nullptr;
    _widgets[1] = nullptr;
}
void flw::SplitGroup::direction(SplitGroup::DIRECTION direction) {
    _direction = direction;
    _split_pos = -1;
    Fl::redraw();
}
int flw::SplitGroup::handle(int event) {
    if (event == FL_DRAG) {
        if (_drag == true) {
            auto pos = 0;
            if (_direction == SplitGroup::DIRECTION::VERTICAL) {
                pos = Fl::event_x() - x();
            }
            else {
                pos = Fl::event_y() - y();
            }
            if (pos != _split_pos) {
                _split_pos = pos;
                do_layout();
            }
            return 1;
        }
    }
    else if (event == FL_LEAVE) {
        if (_direction == SplitGroup::DIRECTION::VERTICAL) {
            if (Fl::event_y() < y() || Fl::event_y() > y() + h() || Fl::event_x() < x() || Fl::event_x() > x() + w()) {
                _drag = false;
                fl_cursor(FL_CURSOR_DEFAULT);
            }
        }
        else {
            if (Fl::event_y() < y() || Fl::event_y() > y() + h() || Fl::event_x() < x() || Fl::event_x() > x() + w()) {
                _drag = false;
                fl_cursor(FL_CURSOR_DEFAULT);
            }
        }
    }
    else if (event == FL_MOVE) {
        if (_widgets[0] != nullptr && _widgets[1] != nullptr && _widgets[0]->visible() != 0 && _widgets[1]->visible() != 0) {
            if (_direction == SplitGroup::DIRECTION::VERTICAL) {
                auto mx  = Fl::event_x();
                auto pos = x() + _split_pos;
                if (mx > (pos - 3) && mx <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }
                    return 1;
                }
            }
            else {
                auto my  = Fl::event_y();
                auto pos = y() + _split_pos;
                if (my > (pos - 3) && my <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_NS);
                    }
                    return 1;
                }
            }
        }
        if (_drag == true) {
            _drag = false;
            fl_cursor(FL_CURSOR_DEFAULT);
        }
    }
    else if (event == FL_PUSH) {
        if (_drag == true) {
            return 1;
        }
    }
    return Fl_Group::handle(event);
}
void flw::SplitGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    if (children() == 0 || W == 0 || H == 0 || visible() == 0) {
        return;
    }
    auto currx = X;
    auto curry = Y;
    auto currh = H;
    auto currw = W;
    if (_direction == SplitGroup::DIRECTION::VERTICAL) {
        if (_widgets[0] != nullptr && _widgets[1] != nullptr && _widgets[0]->visible() != 0 && _widgets[1]->visible() != 0) {
            if (_split_pos == -1) {
                _split_pos = W / 2;
            }
            else if (_split_pos >= W - _min) {
                _split_pos = W - _min;
            }
            else if (_split_pos <= _min) {
                _split_pos = _min;
            }
            auto max = (X + W) - 4;
            auto pos = _split_pos + X;
            if (pos < X) {
                pos = X;
            }
            else if (pos > max) {
                pos = max;
            }
            auto w1 = pos - (X + 2);
            auto w2 = (X + W) - (pos + 2);
            _widgets[0]->resize(currx, curry, w1, currh);
            _widgets[1]->resize(currx + w1 + 4, curry, w2, currh);
        }
        else if (_widgets[0] && _widgets[0]->visible()) {
            _widgets[0]->resize(currx, curry, currw, currh);
        }
        else if (_widgets[1] && _widgets[1]->visible()) {
            _widgets[1]->resize(currx, curry, currw, currh);
        }
    }
    else if (_widgets[0] != nullptr && _widgets[1] != nullptr && _widgets[0]->visible() != 0 && _widgets[1]->visible() != 0) {
        if (_split_pos == -1) {
            _split_pos = H / 2;
        }
        else if (_split_pos >= H - _min) {
            _split_pos = H - _min;
        }
        else if (_split_pos <= _min) {
            _split_pos = _min;
        }
        auto max = (Y + H) - 4;
        auto pos = _split_pos + Y;
        if (pos < Y) {
            pos = Y;
        }
        else if (pos > max) {
            pos = max;
        }
        auto h1 = pos - (Y + 2);
        auto h2 = (Y + H) - (pos + 2);
        _widgets[0]->resize(currx, curry, currw, h1);
        _widgets[1]->resize(currx, curry + h1 + 4, currw, h2);
    }
    else if (_widgets[0] != nullptr && _widgets[0]->visible() != 0) {
        _widgets[0]->resize(currx, curry, currw, currh);
    }
    else if (_widgets[1] != nullptr && _widgets[1]->visible() != 0) {
        _widgets[1]->resize(currx, curry, currw, currh);
    }
}
void flw::SplitGroup::toggle(SplitGroup::CHILD child, SplitGroup::DIRECTION direction, int second_size) {
    if (_widgets[0] == nullptr || _widgets[1] == nullptr) {
        return;
    }
    auto num = (child == SplitGroup::CHILD::FIRST) ? 0 : 1;
    if (_widgets[num]->visible() && _direction == direction) {
        _widgets[num]->hide();
    }
    else {
        _widgets[num]->show();
        if (_direction != direction || split_pos() == -1) {
            _direction = direction;
            if (second_size == -1) {
                split_pos(-1);
            }
            else if (_direction == SplitGroup::DIRECTION::VERTICAL) {
                split_pos(w() - second_size);
            }
            else {
                split_pos(h() - second_size);
            }
        }
    }
}
#include <assert.h>
#include <algorithm>
#include <FL/Fl_Toggle_Button.H>
#include <FL/fl_draw.H>
namespace flw {
class _TabsGroupButton : public Fl_Toggle_Button {
public:
    int                         tw;
    Fl_Widget*                  widget;
    explicit _TabsGroupButton(std::string label, Fl_Widget* WIDGET, void* o) : Fl_Toggle_Button(0, 0, 0, 0) {
        tw     = 0;
        widget = WIDGET;
        align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        copy_label(label.c_str());
        tooltip("");
        when(FL_WHEN_CHANGED);
        callback(TabsGroup::Callback, o);
        selection_color(FL_SELECTION_COLOR);
        labelfont(flw::PREF_FONT);
        labelsize(flw::PREF_FONTSIZE);
    }
};
TabsGroup::TabsGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);
    tooltip(TabsGroup::Help());
    _scroll = new Fl_Scroll(X, Y, W, H);
    _pack   = new Fl_Pack(X, Y, W, H);
    _active = -1;
    _n      = 0;
    _s      = 0;
    _w      = 0;
    _e      = 0;
    _pack->end();
    _scroll->box(FL_NO_BOX);
    _scroll->add(_pack);
    Fl_Group::add(_scroll);
    tabs(TABS::NORTH);
    update_pref();
}
void TabsGroup::add(std::string label, Fl_Widget* widget, const Fl_Widget* after) {
    assert(widget);
    auto button = new _TabsGroupButton(label, widget, this);
    auto idx    = (after != nullptr) ? find(after) : (int) _widgets.size();
    if (idx < 0 || idx >= (int) _widgets.size() - 1) {
        Fl_Group::add(widget);
        _pack->add(button);
        _widgets.push_back(button);
    }
    else {
        idx++;
        auto b = static_cast<_TabsGroupButton*>(_widgets[idx]);
        Fl_Group::insert(*widget, b->widget);
        _pack->insert(*button, b);
        _widgets.insert(_widgets.begin() + idx, button);
    }
    TabsGroup::Callback(button, this);
    do_layout();
}
Fl_Widget* TabsGroup::_active_button() {
    return (_active >= 0 && _active < (int) _widgets.size()) ? _widgets[_active] : nullptr;
}
void TabsGroup::Callback(Fl_Widget* sender, void* object) {
    auto self   = static_cast<TabsGroup*>(object);
    auto count  = 0;
    self->_active = -1;
    for (auto widget : self->_widgets) {
        auto b = static_cast<_TabsGroupButton*>(widget);
        if (b == sender) {
            self->_active = count;
            b->value(1);
            b->widget->show();
            b->widget->take_focus();
        }
        else {
            b->value(0);
            b->widget->hide();
        }
        count++;
    }
    self->_resize_widgets();
}
Fl_Widget* TabsGroup::child(int num) const {
    return (num >= 0 && num < (int) _widgets.size()) ? static_cast<_TabsGroupButton*>(_widgets[num])->widget : nullptr;
}
void TabsGroup::clear() {
    _scroll->remove(_pack);
    _scroll->clear();
    _pack->clear();
    Fl_Group::remove(_scroll);
    Fl_Group::clear();
    Fl_Group::add(_scroll);
    _widgets.clear();
    _scroll->add(_pack);
    _active = -1;
    update_pref();
    Fl::redraw();
}
void TabsGroup::debug() const {
#ifdef DEBUG
    printf("TabsGroup ==>\n");
    printf("    _active  = %d\n", _active);
    printf("    _drag    = %d\n", _drag);
    printf("    _pos     = %d\n", _pos);
    printf("    _widgets = %d\n", (int) _widgets.size());
    printf("    tabs     = %s\n", _scroll->visible() ? "visible" : "hidden");
    printf("    children = %d\n", children());
    printf("    scroll   = %d\n", _scroll->children());
    printf("\n");
    auto count = 0;
    for (auto b : _widgets) {
        printf("    widget[%02d] = %s\n", count++, b->label());
    }
    printf("\n");
    flw::debug::print(this);
    printf("TabsGroup <==\n");
    fflush(stdout);
#endif
}
void TabsGroup::draw() {
    _scroll->redraw();
    Fl_Group::draw();
}
int TabsGroup::find(const Fl_Widget* widget) const {
    auto num = 0;
    for (const auto W : _widgets) {
        const auto b = static_cast<_TabsGroupButton*>(W);
        if (b->widget == widget) {
            return num;
        }
        else {
            num++;
        }
    }
    return -1;
}
int TabsGroup::handle(int event) {
    if (_widgets.size() == 0) {
        return Fl_Group::handle(event);
    }
    if (_tabs == TABS::WEST || _tabs == TABS::EAST) {
        if (event == FL_DRAG) {
            if (_drag == true) {
                auto pos = 0;
                if (_tabs == TABS::WEST) {
                    pos = Fl::event_x() - x();
                }
                else {
                    pos = x() + w() - Fl::event_x();
                }
                if (pos != _pos) {
                    _pos = pos;
                    do_layout();
                }
                return 1;
            }
        }
        else if (event == FL_LEAVE) {
            _drag = false;
            fl_cursor(FL_CURSOR_DEFAULT);
        }
        else if (event == FL_MOVE) {
            auto event_x = Fl::event_x();
            if (_tabs == TABS::WEST) {
                auto pos = x() + _pos;
                if (event_x > (pos - 3) && event_x <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }
                    return 1;
                }
            }
            else {
                auto pos = x() + w() - _pos;
                if (event_x > (pos - 3) && event_x <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }
                    return 1;
                }
            }
            if (_drag == true) {
                _drag = false;
                fl_cursor(FL_CURSOR_DEFAULT);
            }
        }
        else if (event == FL_PUSH) {
            if (_drag == true) {
                return 1;
            }
        }
    }
    if (_widgets.size() > 1) {
        if (event == FL_KEYBOARD) {
            auto key   = Fl::event_key();
            auto alt   = Fl::event_alt() != 0;
            auto alt2  = alt;
            auto shift = Fl::event_shift() != 0;
#ifdef __APPLE__
            alt2 = Fl::event_command() != 0;
#endif
            if (alt2 == true && key >= '0' && key <= '9') {
                auto tab = key - '0';
                tab = (tab == 0) ? 9 : tab - 1;
                if (tab < (int) _widgets.size()) {
                    TabsGroup::Callback(_widgets[tab], this);
                }
                return 1;
            }
            else if (alt == true && shift == true && key == FL_Left) {
                swap(_active, _active - 1);
                TabsGroup::Callback(_active_button(), this);
                return 1;
            }
            else if (alt == true && shift == true && key == FL_Right) {
                swap(_active, _active + 1);
                TabsGroup::Callback(_active_button(), this);
                return 1;
            }
            else if (alt == true && key == FL_Left) {
                _active = _active == 0 ? (int) _widgets.size() - 1 : _active - 1;
                TabsGroup::Callback(_active_button(), this);
                return 1;
            }
            else if (alt == true && key == FL_Right) {
                _active = _active == (int) _widgets.size() - 1 ? 0 : _active + 1;
                TabsGroup::Callback(_active_button(), this);
                return 1;
            }
        }
    }
    if (event == FL_FOCUS) {
        auto widget = value();
        if (widget != nullptr && widget != Fl::focus()) {
            widget->take_focus();
            return 1;
        }
    }
    return Fl_Group::handle(event);
}
const char* TabsGroup::Help() {
    static const char* const HELP =
    "Use alt + left/right to move between tabs.\n"
    "Or alt (command key) + [1 - 9] to select tab.\n"
    "And alt + shift + left/right to move tabs.\n"
    "Tabs on the left/right side can have its width changed by dragging the mouse.";
    return HELP;
}
void TabsGroup::hide_tabs() {
    _scroll->hide();
    do_layout();
}
void TabsGroup::insert(std::string label, Fl_Widget* widget, const Fl_Widget* before) {
    auto button = new _TabsGroupButton(label, widget, this);
    auto idx    = (before != nullptr) ? find(before) : 0;
    if (idx >= (int) _widgets.size()) {
        Fl_Group::add(widget);
        _pack->add(button);
        _widgets.push_back(button);
    }
    else {
        auto b = static_cast<_TabsGroupButton*>(_widgets[idx]);
        Fl_Group::insert(*widget, b->widget);
        _pack->insert(*button, b);
        _widgets.insert(_widgets.begin() + idx, button);
    }
    TabsGroup::Callback(button, this);
    do_layout();
}
std::string TabsGroup::label(Fl_Widget* widget) {
    auto num = find(widget);
    if (num == -1) {
        return "";
    }
    return _widgets[num]->label();
}
void TabsGroup::label(std::string label, Fl_Widget* widget) {
    auto num = find(widget);
    if (num == -1) {
        return;
    }
    _widgets[num]->copy_label(label.c_str());
}
Fl_Widget* TabsGroup::remove(int num) {
    if (num < 0 || num >= (int) _widgets.size()) {
        return nullptr;
    }
    auto W = _widgets[num];
    auto b = static_cast<_TabsGroupButton*>(W);
    auto w = b->widget;
    _widgets.erase(_widgets.begin() + num);
    remove(w);
    _scroll->remove(b);
    delete b;
    if (num < _active) {
        _active--;
    }
    else if (_active == (int) _widgets.size()) {
        _active = (int) _widgets.size() - 1;
    }
    do_layout();
    TabsGroup::Callback(_active_button(), this);
    return w;
}
void TabsGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    if (W == 0 || H == 0) {
        return;
    }
    if (_scroll->visible() == 0) {
        _area = Fl_Rect(X + _w, Y + _n, W - _w - _e, H - _n - _s);
    }
    else {
        fl_font(flw::PREF_FONT, flw::PREF_FONTSIZE);
        if (_tabs == TABS::NORTH || _tabs == TABS::SOUTH) {
            _resize_north_south(X, Y, W, H);
        }
        else {
            _resize_east_west(X, Y, W, H);
        }
    }
    _resize_widgets();
}
void TabsGroup::_resize_east_west(int X, int Y, int W, int H) {
    auto height = flw::PREF_FONTSIZE + 8;
    auto pack_h = (height + _space) * (int) _widgets.size() - _space;
    auto scroll = 0;
    if (_pos < flw::PREF_FONTSIZE * _space) {
        _pos = flw::PREF_FONTSIZE * _space;
    }
    else if (_pos > W - flw::PREF_FONTSIZE * 3) {
        _pos = W - flw::PREF_FONTSIZE * 3;
    }
    if (pack_h > H) {
        scroll = (_scroll->scrollbar_size() == 0) ? Fl::scrollbar_size() : _scroll->scrollbar_size();
    }
    for (auto b : _widgets) {
        b->size(0, height);
    }
    if (_tabs == TABS::WEST) {
        _scroll->resize(X, Y, _pos, H);
        _pack->resize(X, Y, _pos - scroll, pack_h);
        _area = Fl_Rect(X + _pos + _w, Y + _n, W - _pos - _w - _e, H - _n - _s);
    }
    else {
        _scroll->resize(X + W - _pos, Y, _pos, H);
        _pack->resize(X + W - _pos, Y, _pos - scroll, pack_h);
        _area = Fl_Rect(X + _w, Y + _n, W - _pos - _w - _e, H - _n - _s);
    }
}
void TabsGroup::_resize_north_south(int X, int Y, int W, int H) {
    auto height = flw::PREF_FONTSIZE + 8;
    auto scroll = 0;
    auto pack_w = 0;
    for (auto widget : _widgets) {
        auto b  = static_cast<_TabsGroupButton*>(widget);
        auto th = 0;
        b->tw = 0;
        fl_measure(b->label(), b->tw, th);
        b->tw += flw::PREF_FONTSIZE;
        pack_w += b->tw + _space;
    }
    if (pack_w - _space > W) {
        scroll = (_scroll->scrollbar_size() == 0) ? Fl::scrollbar_size() : _scroll->scrollbar_size();
    }
    for (auto widget : _widgets) {
        auto b  = static_cast<_TabsGroupButton*>(widget);
        b->size(b->tw, 0);
    }
    if (_tabs == TABS::NORTH) {
        _scroll->resize(X, Y, W, height + scroll);
        _pack->resize(X, Y, pack_w, height);
        _area = Fl_Rect(X + _w, Y + height + scroll + _n, W - _w - _e, H - height - scroll - _n - _s);
    }
    else {
        _scroll->resize(X, Y + H - height - scroll, W, height + scroll);
        _pack->resize(X, Y + H - height - scroll, pack_w, height);
        _area = Fl_Rect(X + _w, Y + _n, W - _w - _e, H - height - scroll - _n - _s);
    }
}
void TabsGroup::_resize_widgets() {
    for (auto w : _widgets) {
        auto b = static_cast<_TabsGroupButton*>(w);
        if (b->widget->visible() != 0) {
            b->widget->resize(_area.x(), _area.y(), _area.w(), _area.h());
        }
    }
    Fl::redraw();
}
void TabsGroup::show_tabs() {
    _scroll->show();
    do_layout();
}
void TabsGroup::sort(bool ascending, bool casecompare) {
    auto pack = const_cast<Fl_Widget**>(_pack->array());
    auto butt = _active_button();
    if (ascending == true && casecompare == true) {
        std::sort(_widgets.begin(), _widgets.end(), [](const Fl_Widget* a, const Fl_Widget* b) { return strcmp(a->label(), b->label()) < 0; });
    }
    else if (ascending == true && casecompare == false) {
        std::sort(_widgets.begin(), _widgets.end(), [](const Fl_Widget* a, const Fl_Widget* b) { return fl_utf_strcasecmp(a->label(), b->label()) < 0; });
    }
    else if (ascending == false && casecompare == true) {
        std::sort(_widgets.begin(), _widgets.end(), [](const Fl_Widget* a, const Fl_Widget* b) { return strcmp(b->label(), a->label()) < 0; });
    }
    else if (ascending == false && casecompare == false) {
        std::sort(_widgets.begin(), _widgets.end(), [](const Fl_Widget* a, const Fl_Widget* b) { return fl_utf_strcasecmp(b->label(), a->label()) < 0; });
    }
    for (int f = 0; f < _pack->children(); f++) {
        pack[f] = _widgets[f];
        if (_widgets[f] == butt) {
            _active = f;
        }
    }
    do_layout();
}
void TabsGroup::swap(int from, int to) {
    auto last = (int) _widgets.size() - 1;
    if (_widgets.size() < 2 || to < -1 || to > (int) _widgets.size()) {
        return;
    }
    auto active = (_active == from);
    auto pack   = const_cast<Fl_Widget**>(_pack->array());
    if (from == 0 && to == -1) {
        auto widget = _widgets[0];
        for (int f = 1; f <= last; f++) {
            _widgets[f - 1] = _widgets[f];
            pack[f - 1]     = pack[f];
        }
        from           = last;
        pack[from]     = widget;
        _widgets[from] = widget;
    }
    else if (from == last && to == (int) _widgets.size()) {
        auto widget = _widgets[last];
        for (int f = last - 1; f >= 0; f--) {
            _widgets[f + 1] = _widgets[f];
            pack[f + 1]     = pack[f];
        }
        from           = 0;
        pack[from]     = widget;
        _widgets[from] = widget;
    }
    else {
        auto widget = _widgets[from];
        pack[from]     = pack[to];
        pack[to]       = widget;
        _widgets[from] = _widgets[to];
        _widgets[to]   = widget;
        from           = to;
    }
    if (active == true) {
        _active = from;
    }
    do_layout();
}
void TabsGroup::tabs(TABS tabs, int space_max_20) {
    _tabs   = tabs;
    _space  = (space_max_20 >= 0 && space_max_20 <= 20) ? space_max_20 : TabsGroup::DEFAULT_SPACE;
    auto al = FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP;
    if (_tabs == TABS::NORTH || _tabs == TABS::SOUTH) {
        al = FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP;
        _scroll->type(Fl_Scroll::HORIZONTAL);
        _pack->type(Fl_Pack::HORIZONTAL);
    }
    else {
        _pack->type(Fl_Pack::VERTICAL);
        _scroll->type(Fl_Scroll::VERTICAL);
    }
    for (auto widget : _widgets) {
        widget->align(al);
    }
    _pack->spacing(_space);
    do_layout();
    auto w = value();
    if (w != nullptr) {
        w->take_focus();
    }
}
void TabsGroup::update_pref(unsigned characters, Fl_Font font, Fl_Fontsize fontsize) {
    _drag = false;
    _pos  = fontsize * characters;
    for (auto widget : _widgets) {
        widget->labelfont(font);
        widget->labelsize(fontsize);
    }
}
Fl_Widget* TabsGroup::value() const {
    return (_active >= 0 && _active < (int) _widgets.size()) ? static_cast<_TabsGroupButton*>(_widgets[_active])->widget : nullptr;
}
void TabsGroup::value(int num) {
    if (num >= 0 && num < (int) _widgets.size()) {
        TabsGroup::Callback(_widgets[num], this);
    }
}
}
namespace flw {
    static const std::string _SCROLLBROWSER_MENU_ALL  = "Copy All Lines";
    static const std::string _SCROLLBROWSER_MENU_LINE = "Copy Current Line";
    static const std::string _SCROLLBROWSER_TOOLTIP   = "Right click to show the menu";
}
flw::ScrollBrowser::ScrollBrowser(int scroll, int X, int Y, int W, int H, const char* l) : Fl_Hold_Browser(X, Y, W, H, l) {
    end();
    _menu      = new Fl_Menu_Button(0, 0, 0, 0);
    _scroll    = (scroll > 0) ? scroll : 9;
    _flag_move = true;
    _flag_menu = true;
    static_cast<Fl_Group*>(this)->add(_menu);
    _menu->add(_SCROLLBROWSER_MENU_LINE.c_str(), 0, ScrollBrowser::Callback, this);
    _menu->add(_SCROLLBROWSER_MENU_ALL.c_str(), 0, ScrollBrowser::Callback, this);
    _menu->type(Fl_Menu_Button::POPUP3);
    tooltip(_SCROLLBROWSER_TOOLTIP.c_str());
    update_pref();
}
void flw::ScrollBrowser::Callback(Fl_Widget*, void* o) {
    auto self  = static_cast<ScrollBrowser*>(o);
    auto txt   = self->_menu->text();
    auto label = std::string((txt != nullptr) ? txt : "");
    auto clip  = std::string();
    clip.reserve(self->size() * 40 + 100);
    if (label == _SCROLLBROWSER_MENU_LINE) {
        if (self->value() != 0) {
            clip = util::remove_browser_format(self->text(self->value()));
        }
    }
    else if (label == _SCROLLBROWSER_MENU_ALL) {
        for (auto f = 1; f <= self->size(); f++) {
            auto s = util::remove_browser_format(self->text(f));
            clip += s;
            clip += "\n";
        }
    }
    if (clip != "") {
        Fl::copy(clip.c_str(), clip.length(), 2);
    }
}
int flw::ScrollBrowser::handle(int event) {
    if (event == FL_MOUSEWHEEL) {
        if (Fl::event_dy() > 0) {
            topline(topline() + _scroll);
        }
        else if (Fl::event_dy() < 0) {
            topline(topline() - _scroll);
        }
        return 1;
    }
    else if (event == FL_KEYBOARD) {
        if (_flag_move == true) {
            auto key = Fl::event_key();
            if (key == FL_Page_Up) {
                auto line = value();
                if (line == 1) {
                }
                else if (line - _scroll < 1) {
                    value(1);
                    do_callback();
                }
                else {
                    value(line - _scroll);
                    topline(line - _scroll);
                    do_callback();
                }
                return 1;
            }
            else if (key == FL_Page_Down) {
                auto line = value();
                if (line == size()) {
                }
                else if (line + _scroll > size()) {
                    value(size());
                    do_callback();
                }
                else {
                    value(line + _scroll);
                    bottomline(line + _scroll);
                    do_callback();
                }
                return 1;
            }
        }
    }
    else if (event == FL_PUSH) {
        if (_flag_menu == true && Fl::event_button() == FL_RIGHT_MOUSE) {
            _menu->popup();
            return 1;
        }
    }
    return Fl_Hold_Browser::handle(event);
}
void flw::ScrollBrowser::update_pref(Fl_Font text_font, Fl_Fontsize text_size) {
    labelfont(flw::PREF_FONT);
    labelsize(flw::PREF_FONTSIZE);
    textfont(text_font);
    textsize(text_size);
}
#include <FL/fl_draw.H>
#include <FL/Fl.H>
flw::WaitCursor* flw::WaitCursor::WAITCURSOR = nullptr;
flw::WaitCursor::WaitCursor() {
    if (WaitCursor::WAITCURSOR == nullptr) {
        WaitCursor::WAITCURSOR = this;
        fl_cursor(FL_CURSOR_WAIT);
        Fl::redraw();
        Fl::flush();
    }
}
flw::WaitCursor::~WaitCursor() {
    if (WaitCursor::WAITCURSOR == this) {
        WaitCursor::WAITCURSOR = nullptr;
        fl_cursor(FL_CURSOR_DEFAULT);
    }
}
#include <algorithm>
namespace fle {
namespace limits {
    const size_t FIND_LIST_MAX               =             30;
    const size_t HEXFILE_DIVIDER             =              5;
    const size_t MOUSE_SCROLL_MAX            =             15;
    const size_t TAB_WIDTH_MAX               =              8;
    const size_t UNDO_WARNING                =  1'000'000'000;
    const size_t WRAP_DEF                    =             80;
    const size_t WRAP_MAX                    =            140;
    const size_t WRAP_MIN                    =             60;
    size_t       AUTOCOMPLETE_FILESIZE_DEF   =     50'000'000;
    size_t       AUTOCOMPLETE_FILESIZE_MAX   =    100'000'000;
    size_t       AUTOCOMPLETE_FILESIZE_MIN   =      1'000'000;
    size_t       AUTOCOMPLETE_FILESIZE_STEP  =      1'000'000;
    size_t       AUTOCOMPLETE_FILESIZE_VAL   =     50'000'000;
    size_t       AUTOCOMPLETE_LINES_DEF      =         50'000;
    size_t       AUTOCOMPLETE_LINES_MAX      =        100'000;
    size_t       AUTOCOMPLETE_LINES_MIN      =          1'000;
    size_t       AUTOCOMPLETE_LINES_STEP     =          1'000;
    size_t       AUTOCOMPLETE_LINES_VAL      =         50'000;
    size_t       AUTOCOMPLETE_WORD_SIZE_DEF  =             40;
    size_t       AUTOCOMPLETE_WORD_SIZE_MAX  =            100;
    size_t       AUTOCOMPLETE_WORD_SIZE_MIN  =             10;
    size_t       AUTOCOMPLETE_WORD_SIZE_STEP =              1;
    size_t       AUTOCOMPLETE_WORD_SIZE_VAL  =             40;
    size_t       COUNT_CHAR_DEF              =          5'000;
    size_t       COUNT_CHAR_MAX              =      1'000'000;
    size_t       COUNT_CHAR_MIN              =              0;
    size_t       COUNT_CHAR_STEP             =          5'000;
    size_t       COUNT_CHAR_VAL              =          5'000;
    size_t       FILE_BACKUP_SIZE_DEF        =     50'000'000;
    size_t       FILE_BACKUP_SIZE_MAX        =    100'000'000;
    size_t       FILE_BACKUP_SIZE_MIN        =      1'000'000;
    size_t       FILE_BACKUP_SIZE_STEP       =      1'000'000;
    size_t       FILE_BACKUP_SIZE_VAL        =     50'000'000;
    size_t       FILE_SIZE_DEF               =  1'000'000'000;
    size_t       FILE_SIZE_MAX               =  2'000'000'000;
    size_t       FILE_SIZE_MIN               =     10'000'000;
    size_t       FILE_SIZE_STEP              =     10'000'000;
    size_t       FILE_SIZE_VAL               =  1'000'000'000;
    size_t       FORCE_RESTYLING             =              0;
    size_t       OUTPUT_LINES_DEF            =        100'000;
    size_t       OUTPUT_LINES_MAX            =        500'000;
    size_t       OUTPUT_LINES_MIN            =         50'000;
    size_t       OUTPUT_LINES_STEP           =         10'000;
    size_t       OUTPUT_LINES_VAL            =        100'000;
    size_t       OUTPUT_LINE_LENGTH_DEF      =            400;
    size_t       OUTPUT_LINE_LENGTH_MAX      =           2000;
    size_t       OUTPUT_LINE_LENGTH_MIN      =             80;
    size_t       OUTPUT_LINE_LENGTH_STEP     =             10;
    size_t       OUTPUT_LINE_LENGTH_VAL      =            400;
    size_t       STYLE_FILESIZE_DEF          =     20'000'000;
    size_t       STYLE_FILESIZE_MAX          =    100'000'000;
    size_t       STYLE_FILESIZE_MIN          =        500'000;
    size_t       STYLE_FILESIZE_STEP         =        500'000;
    size_t       STYLE_FILESIZE_VAL          =     20'000'000;
    size_t       WRAP_LINE_LENGTH_DEF        =          3'000;
    size_t       WRAP_LINE_LENGTH_MAX        =         10'000;
    size_t       WRAP_LINE_LENGTH_MIN        =            100;
    size_t       WRAP_LINE_LENGTH_STEP       =            100;
    size_t       WRAP_LINE_LENGTH_VAL        =          3'000;
}
static const char _FLE_HEX_STRINGS[32 * 16 + 1] =
    "000102030405060708090a0b0c0d0e0f"
    "101112131415161718191a1b1c1d1e1f"
    "202122232425262728292a2b2c2d2e2f"
    "303132333435363738393a3b3c3d3e3f"
    "404142434445464748494a4b4c4d4e4f"
    "505152535455565758595a5b5c5d5e5f"
    "606162636465666768696a6b6c6d6e6f"
    "707172737475767778797a7b7c7d7e7f"
    "808182838485868788898a8b8c8d8e8f"
    "909192939495969798999a9b9c9d9e9f"
    "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf"
    "b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
    "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
    "d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
    "e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
    "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";
KeyConf KEYS[(size_t) fle::FKEY_SIZE] = {
    { .num = FKEY_KOMMAND_START,        .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = 'k',              .key_u = 0, .help = "Start command mode - then press one of the command keys." },
    { .num = FKEY_KOMMAND_STOP,         .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = FL_Escape,        .key_u = 0, .help = "Stop command mode." },
    { .num = FKEY_AUTOCOMPLETE,         .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = ' ',              .key_u = 0, .help = "Show autcomplete dialog." },
    { .num = FKEY_BOOKMARKS_NEXT,       .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_F + 2,         .key_u = 0, .help = "Jump to next bookmark." },
    { .num = FKEY_BOOKMARKS_PREV,       .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_F + 2,         .key_u = 0, .help = "Jump to previous bookmark." },
    { .num = FKEY_BOOKMARKS_TOGGLE,     .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = 'b',              .key_u = 0, .help = "Toggle bookmark." },
    { .num = FKEY_CASE_LOWER,           .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = 'l',              .key_u = 0, .help = "Convert selected text to lower case." },
    { .num = FKEY_CASE_UPPER,           .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = 'l',              .key_u = 0, .help = "Convert selected text to upper case." },
    { .num = FKEY_COMMENT_BLOCK,        .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = 'c',              .key_u = 0, .help = "Toggle block comments for selected text." },
    { .num = FKEY_COMMENT_LINE,         .alt_d = true,  .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = 'c',              .key_u = 0, .help = "Toggle line comments." },
    { .num = FKEY_COPY_TEXT,            .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = 'c',              .key_u = 0, .help = "Copy selected text or current line." },
    { .num = FKEY_CUT_TEXT,             .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = 'x',              .key_u = 0, .help = "Cut selected text or current line." },
    { .num = FKEY_DELETE_LINE_LEFT,     .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_BackSpace,     .key_u = 0, .help = "Delete all characters to the left until start of line." },
    { .num = FKEY_DELETE_LINE_RIGHT,    .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_Delete,        .key_u = 0, .help = "Delete all characters to the right until end of the line." },
    { .num = FKEY_DELETE_WORD_LEFT,     .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_BackSpace,     .key_u = 0, .help = "Delete word to the left." },
    { .num = FKEY_DELETE_WORD_RIGHT,    .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_Delete,        .key_u = 0, .help = "Delete word to the left." },
    { .num = FKEY_DUP_TEXT,             .alt_d = true,  .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = 'd',              .key_u = 0, .help = "Duplicate selection or current line if unselected." },
    { .num = FKEY_ENTER1,               .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_Enter,         .key_u = 0, .help = "Break line and indent." },
    { .num = FKEY_ENTER2,               .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_Enter,         .key_u = 0, .help = "Insert new line and indent." },
    { .num = FKEY_FIND_LINES,           .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = 'j',              .key_u = 0, .help = "Find lines in text and show result in output panel." },
    { .num = FKEY_FIND_LINES_AGAIN,     .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = 'j',              .key_u = 0, .help = "Repeat find lines." },
    { .num = FKEY_GOTO_LINE,            .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = 'g',              .key_u = 0, .help = "Goto line." },
    { .num = FKEY_HELP,                 .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_F + 1,         .key_u = 0, .help = "Show editor help." },
    { .num = FKEY_HOME,                 .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_Home,          .key_u = 0, .help = "Move cursor to first word or first column." },
    { .num = FKEY_MENU,                 .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = 'm',              .key_u = 0, .help = "Show menu (where the mouse cursor is)." },
    { .num = FKEY_MOVE_DOWN,            .alt_d = true,  .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_Down,          .key_u = 0, .help = "Move current line(s) down." },
    { .num = FKEY_MOVE_UP,              .alt_d = true,  .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_Up,            .key_u = 0, .help = "Move current line(s) up." },
    { .num = FKEY_OUTPUT_NEXT,          .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_F + 5,         .key_u = 0, .help = "Jump to next line in output panel." },
    { .num = FKEY_OUTPUT_PREV,          .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_F + 5,         .key_u = 0, .help = "Jump to previous line in output panel." },
    { .num = FKEY_OUTPUT_TOGGLE,        .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = '4',              .key_u = 0, .help = "Toggle output." },
    { .num = FKEY_QUICK_FIND,           .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = 'd',              .key_u = 0, .help = "Select current word (and set search word in find widget),|or jump to next found word.|Search is case sensitive and part of words can be found." },
    { .num = FKEY_REDO,                 .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = 'z',              .key_u = 0, .help = "Redo changes." },
    { .num = FKEY_REPLACE,              .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_F + 4,         .key_u = 0, .help = "Replace selection and search again." },
    { .num = FKEY_SEARCH_BACKWARD,      .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_F + 3,         .key_u = 0, .help = "Search backward." },
    { .num = FKEY_SEARCH_FORWARD,       .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_F + 3,         .key_u = 0, .help = "Search forward." },
    { .num = FKEY_SELECT_LINE,          .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = 'l',              .key_u = 0, .help = "Select current line." },
    { .num = FKEY_SELECT_PAIR1,         .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = 'p',              .key_u = 0, .help = "Select text between matching pairs of {}[]()<>." },
    { .num = FKEY_SELECT_PAIR2,         .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = 'p',              .key_u = 0, .help = "Select text between matching pairs of {}[]()<>.|Cursor will be moved to the end." },
    { .num = FKEY_SELECT_STYLE,         .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = 'd',              .key_u = 0, .help = "Select text with same highlightning code.|Cursor will not be moved." },
    { .num = FKEY_SHIFT_LEFT,           .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_Tab,           .key_u = 0, .help = "Insert/replace tab or shift lines to the left." },
    { .num = FKEY_SHIFT_RIGHT,          .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = FL_Tab,           .key_u = 0, .help = "Insert/replace tab or shift lines to the right." },
    { .num = FKEY_SHOW_FIND_REPLACE,    .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = 'f',              .key_u = 0, .help = "Show find and replace widget (close with escape)." },
    { .num = FKEY_SORT_ASCENDING,       .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = 's',              .key_u = 0, .help = "Sort selected lines in ascending order." },
    { .num = FKEY_SORT_DESCENDING,      .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = true,  .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = 's',              .key_u = 0, .help = "Sort selected lines in descending order." },
    { .num = FKEY_UNDO,                 .alt_d = false, .alt_u = false, .ctrl_d = true,  .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = false, .kommand_u = false, .key_d = 'z',              .key_u = 0, .help = "Undo changes." },
    { .num = FKEY_VIEW_1,               .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = '1',              .key_u = 0, .help = "Select left/top split edit widget." },
    { .num = FKEY_VIEW_2,               .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = '2',              .key_u = 0, .help = "Select right/bottom split edit widget or split editor." },
    { .num = FKEY_VIEW_CLOSE,           .alt_d = false, .alt_u = false, .ctrl_d = false, .ctrl_u = false, .shift_d = false, .shift_u = false, .kommand_d = true,  .kommand_u = false, .key_d = '3',              .key_u = 0, .help = "Close split screen." },
};
Bookmarks::Bookmarks(Editor* editor, std::string bookmarks) {
    _editor = editor;
    for (const auto& bookmark : gnu::str::split(bookmarks, ',')) {
        if (bookmark != "") {
            auto pos = gnu::str::to_int(bookmark, -1);
            if (pos != -1) {
                _vec.push_back(pos);
            }
        }
    }
}
bool Bookmarks::add(int pos) {
    for (auto it = _vec.begin(); it != _vec.end(); ++it) {
        if (pos < *it) {
            _vec.insert(it, pos);
            return true;
        }
        else if (pos == *it) {
            _vec.erase(it);
            return false;
        }
    }
    _vec.push_back(pos);
    return true;
}
void Bookmarks::debug() const {
#ifdef DEBUG
    printf("\nBookmarks:\n");
    int c = 0;
    for (auto pos : _vec) {
        printf("    pos[%2d]            = %9d\n", c++, (int) pos);
    }
    fflush(stdout);
#endif
}
int Bookmarks::find_next(int pos, size_t& index) {
    index = 0;
    for (auto& bookmark : _vec) {
        if (bookmark > pos) {
            return (int) bookmark;
        }
        index++;
    }
    index = 0;
    return (_vec.size() > 0) ? (int) _vec.front() : -1;
}
int Bookmarks::find_prev(int pos, size_t& index) {
    index = _vec.size() - 1;
    for (auto it = _vec.rbegin(); it != _vec.rend(); ++it) {
        if (*it < pos) {
            return (int) *it;
        }
        index--;
    }
    index = _vec.size() - 1;
    return (_vec.size() > 0) ? (int) _vec.back() : -1;
}
void Bookmarks::goto_next() {
    assert(_editor);
    auto index = (size_t) 0;
    auto next  = find_next(_editor->cursor_insert_position(), index);
    if (next >= 0) {
        _editor->cursor_move_to_pos(next, true);
        _editor->statusbar_set_message(gnu::str::format("Bookmark %d of %d", (int) index + 1, (int) _vec.size()));
    }
}
void Bookmarks::goto_prev() {
    assert(_editor);
    auto index = (size_t) 0;
    auto prev = find_prev(_editor->cursor_insert_position(), index);
    if (prev >= 0) {
        _editor->cursor_move_to_pos(prev, true);
        _editor->statusbar_set_message(gnu::str::format("Bookmark %d of %d", (int) index + 1, (int) _vec.size()));
    }
}
void Bookmarks::toggle() {
    assert(_editor);
    auto pos = _editor->cursor_insert_position();
    if (add(pos) == true) {
        _editor->statusbar_set_message(gnu::str::format("Bookmark added at pos %d (total %d)", pos + 1, (int) _vec.size()));
    }
    else {
        _editor->statusbar_set_message(gnu::str::format("Bookmark deleted at pos %d (total %d)", pos + 1, (int) _vec.size()));
    }
}
std::string Bookmarks::tostring() const {
    std::string res;
    for (auto pos : _vec) {
        if (res != "") {
            res += ",";
        }
        res += gnu::str::format("%d", pos);
    }
    return res;
}
size_t Bookmarks::update(int pos, int inserted, int deleted) {
    auto size1 = _vec.size();
    auto it    = _vec.begin();
    while (it != _vec.end()) {
        if (deleted > 0 && *it >= pos && *it <= pos + deleted) {
            it = _vec.erase(it);
        }
        else if (pos <= *it) {
            *it += inserted;
            *it -= deleted;
            it++;
        }
        else {
            it++;
        }
    }
    return size1 - _vec.size();
}
Config::Config() {
    _del               = false;
    _id                = 0;
    active             = nullptr;
    disable_autoreload = false;
    disable_lineending = false;
    disable_style      = false;
    disable_tab        = false;
    pref_autocomplete  = true;
    pref_autoreload    = true;
    pref_backup        = gnu::File();
    pref_binary        = FBINFILE::TEXT;
    pref_cursor        = Fl_Text_Display::NORMAL_CURSOR;
    pref_indentation   = true;
    pref_insert        = true;
    pref_linenumber    = true;
    pref_mouse_scroll  = 3;
    pref_scheme        = style::SCHEME_DEF;
    pref_shrink_status = 0;
    pref_statusbar     = true;
    pref_tmp_fontsize  = 0;
    pref_undo          = FUNDO::FLE;
    pref_wrap          = 80;
}
bool Config::add_find_word(std::string word, bool append) {
    if (word == "" || gnu::str::is_whitespace(word) == true || word.find_first_of("\n\r\t") != std::string::npos || word.length() > 80) {
        return false;
    }
    if (find_list.size() == 0 || find_list.front() != word) {
        if (append == true) {
            gnu::str::list_append(find_list, word, limits::FIND_LIST_MAX);
        }
        else {
            gnu::str::list_insert(find_list, word, limits::FIND_LIST_MAX);
        }
        return true;
    }
    return false;
}
int Config::add_receiver(Message* object) {
    for (const auto o : _list) {
        if (o == object) {
            return object->object_id();
        }
    }
    _list.push_back(object);
    return ++_id;
}
bool Config::add_replace_word(std::string word, bool append) {
    if (word == "" || gnu::str::is_whitespace(word) == true || word.find_first_of("\n\r\t") != std::string::npos || word.length() > 80) {
        return false;
    }
    if (replace_list.size() == 0 || replace_list.front() != word) {
        if (append == true) {
            gnu::str::list_append(replace_list, word, limits::FIND_LIST_MAX);
        }
        else {
            gnu::str::list_insert(replace_list, word, limits::FIND_LIST_MAX);
        }
        return true;
    }
    return false;
}
std::string Config::backup_name(std::string filename) {
    if (pref_backup.is_dir() == false) {
        return "";
    }
    std::string res = filename;
    gnu::str::replace(res, "\\", "_");
    gnu::str::replace(res, "/", "_");
    gnu::str::replace(res, ":", "_");
    return pref_backup.filename + "/" + res;
}
void Config::debug() const {
#ifdef DEBUG
    printf("\nConfig:\n");
    printf("    active             = %llx\n", (long long unsigned int) active);
    printf("    id                 = %2d\n", _id);
    printf("    find_words         = %2d\n", (int) find_list.size());
    printf("    replace_words      = %2d\n", (int) replace_list.size());
    printf("    disable_autoreload = %s\n", disable_autoreload ? "true" : "false");
    printf("    disable_lineending = %s\n", disable_lineending ? "true" : "false");
    printf("    disable_style      = %s\n", disable_style ? "true" : "false");
    printf("    disable_tab        = %s\n", disable_tab ? "true" : "false");
    printf("    pref_autocomplete  = %s\n", pref_autocomplete ? "true" : "false");
    printf("    pref_autoreload    = %s\n", pref_autoreload ? "true" : "false");
    printf("    pref_backup        = %s\n", pref_backup.c_str());
    printf("    pref_binary        = %2d\n", (int) pref_binary);
    printf("    pref_cursor        = %2d\n", pref_cursor);
    printf("    pref_indentation   = %s\n", pref_indentation ? "true" : "false");
    printf("    pref_insert        = %s\n", pref_insert ? "true" : "false");
    printf("    pref_linenumber    = %s\n", pref_linenumber ? "true" : "false");
    printf("    pref_mouse_scroll  = %2u\n", pref_mouse_scroll);
    printf("    pref_scheme        = %s\n", pref_scheme.c_str());
    printf("    pref_shrink_status = %2d\n", pref_shrink_status);
    printf("    pref_statusbar     = %s\n", pref_statusbar ? "true" : "false");
    printf("    pref_tmp_fontsize  = %2d\n", pref_tmp_fontsize);
    printf("    pref_undo          = %s\n", pref_undo == FUNDO::FLE ? "FLE" : pref_undo == FUNDO::FLTK ? "FLTK" : "NONE");
    printf("    pref_wrap          = %2u\n", pref_wrap);
    fflush(stdout);
#endif
}
void Config::load_pref(Fl_Preferences& preferences, FindReplace* findreplace) {
    auto val = 0;
    std::string s;
    {
        preferences.get("fle.autocomplete", val, pref_autocomplete);
        pref_autocomplete = val;
        preferences.get("fle.backup", s, "");
        gnu::File file(s);
        if (file.is_dir() == true) {
            pref_backup = file;
        }
        preferences.get("fle.binary", val, 0);
        pref_binary = FBINFILE::NO;
        if (val == 1) {
            pref_binary = FBINFILE::TEXT;
        }
        else if (val == 2) {
            pref_binary = FBINFILE::HEX;
        }
        preferences.get("fle.cursor", val, Fl_Text_Display::NORMAL_CURSOR);
        pref_cursor = val;
        preferences.get("fle.indent", val, pref_indentation);
        pref_indentation = val;
        preferences.get("fle.insert", val, pref_insert);
        pref_insert = val;
        preferences.get("fle.linenumber", val, pref_linenumber);
        pref_linenumber = val;
        preferences.get("fle.reload", val, 0);
        pref_autoreload = val;
        preferences.get("fle.scheme", s, style::SCHEME_DEF);
        pref_scheme = s;
        preferences.get("fle.scroll", val, 0);
        pref_mouse_scroll = (val > 0 && (unsigned) val <= limits::MOUSE_SCROLL_MAX) ? val : 0;
        preferences.get("fle.statusbar", val, pref_statusbar);
        pref_statusbar = val;
        preferences.get("fle.undo_mode", val, 0);
        pref_undo = (val == (int) FUNDO::FLTK) ? FUNDO::FLTK : val == (int) FUNDO::NONE ? FUNDO::NONE : FUNDO::FLE;
    }
    {
        preferences.get("tweak.autocomplete_filesize", val, 0);
        limits::AUTOCOMPLETE_FILESIZE_VAL = ((unsigned) val >= limits::AUTOCOMPLETE_FILESIZE_MIN && (unsigned) val <= limits::AUTOCOMPLETE_FILESIZE_MAX) ? val : limits::AUTOCOMPLETE_FILESIZE_DEF;
        preferences.get("tweak.autocomplete_lines", val, 0);
        limits::AUTOCOMPLETE_LINES_VAL = ((unsigned) val >= limits::AUTOCOMPLETE_LINES_MIN && (unsigned) val <= limits::AUTOCOMPLETE_LINES_MAX) ? val : limits::AUTOCOMPLETE_LINES_DEF;
        preferences.get("tweak.autocomplete_word_size", val, 0);
        limits::AUTOCOMPLETE_WORD_SIZE_VAL = ((unsigned) val >= limits::AUTOCOMPLETE_WORD_SIZE_MIN && (unsigned) val <= limits::AUTOCOMPLETE_WORD_SIZE_MAX) ? val : limits::AUTOCOMPLETE_WORD_SIZE_DEF;
        preferences.get("tweak.count_char", val, 0);
        limits::COUNT_CHAR_VAL = ((unsigned) val >= limits::COUNT_CHAR_MIN && (unsigned) val <= limits::COUNT_CHAR_MAX) ? val : limits::COUNT_CHAR_DEF;
        preferences.get("tweak.file_backup_size", val, 0);
        limits::FILE_BACKUP_SIZE_VAL = ((unsigned) val >= limits::FILE_BACKUP_SIZE_MIN && (unsigned) val <= limits::FILE_BACKUP_SIZE_MAX) ? val : limits::FILE_BACKUP_SIZE_DEF;
        preferences.get("tweak.file_size", val, 0);
        limits::FILE_SIZE_VAL = ((unsigned) val >= limits::FILE_SIZE_MIN && (unsigned) val <= limits::FILE_SIZE_MAX) ? val : limits::FILE_SIZE_DEF;
        preferences.get("tweak.force_restyling", val, 0);
        limits::FORCE_RESTYLING = (val == 1) ? 1 : 0;
        preferences.get("tweak.output_lines", val, 0);
        limits::OUTPUT_LINES_VAL = ((unsigned) val >= limits::OUTPUT_LINES_MIN && (unsigned) val <= limits::OUTPUT_LINES_MAX) ? val : limits::OUTPUT_LINES_DEF;
        preferences.get("tweak.output_line_length", val, 0);
        limits::OUTPUT_LINE_LENGTH_VAL = ((unsigned) val >= limits::OUTPUT_LINE_LENGTH_MIN && (unsigned) val <= limits::OUTPUT_LINE_LENGTH_MAX) ? val : limits::OUTPUT_LINE_LENGTH_DEF;
        preferences.get("tweak.style_filesize", val, 0);
        limits::STYLE_FILESIZE_VAL = ((unsigned) val >= limits::STYLE_FILESIZE_MIN && (unsigned) val <= limits::STYLE_FILESIZE_MAX) ? val : limits::STYLE_FILESIZE_DEF;
        preferences.get("tweak.wrap_line_length", val, 0);
        limits::WRAP_LINE_LENGTH_VAL = ((unsigned) val >= limits::WRAP_LINE_LENGTH_MIN && (unsigned) val <= limits::WRAP_LINE_LENGTH_MAX) ? val : limits::WRAP_LINE_LENGTH_DEF;
    }
    {
        preferences.get("finddialog.fregex", val, 0);
        FindDialog::REGEX = (val == (int) FREGEX::YES) ? FREGEX::YES : FREGEX::NO;
        preferences.get("finddialog.ftrim", val, 0);
        FindDialog::TRIM = (val == (int) FTRIM::YES) ? FTRIM::YES : FTRIM::NO;
    }
    {
        preferences.get("replacedialog.fcasecompare", val, 0);
        ReplaceDialog::CASECOMPARE = (val == (int) FCASECOMPARE::YES) ? FCASECOMPARE::YES : FCASECOMPARE::NO;
        preferences.get("replacedialog.fnltab", val, 0);
        ReplaceDialog::NLTAB = (val >= (int) FNLTAB::NO && val <= (int) FNLTAB::YES) ? (FNLTAB) val : FNLTAB::NO;
        preferences.get("replacedialog.fregex", val, 0);
        ReplaceDialog::REGEX = (val == (int) FREGEX::YES) ? FREGEX::YES : FREGEX::NO;
        preferences.get("replacedialog.fselection", val, 0);
        ReplaceDialog::SELECTION = (val == (int) FSELECTION::YES) ? FSELECTION::YES : FSELECTION::NO;
        preferences.get("replacedialog.fwordcompare", val, 0);
        ReplaceDialog::WORDCOMPARE = (val == (int) FWORDCOMPARE::YES) ? FWORDCOMPARE::YES : FWORDCOMPARE::NO;
    }
    if (findreplace != nullptr) {
        preferences.get("findreplace.fcasecompare", val, 0);
        findreplace->fcasecompare((val == (int) FCASECOMPARE::YES) ? FCASECOMPARE::YES : FCASECOMPARE::NO);
        preferences.get("findreplace.fnltab", val, 0);
        if (val >= (int) FNLTAB::NO && val <= (int) FNLTAB::YES) {
            findreplace->fnltab((FNLTAB) val);
        }
        preferences.get("findreplace.fselection", val, 0);
        findreplace->fselection((val == (int) FSELECTION::YES) ? FSELECTION::YES : FSELECTION::NO);
        preferences.get("findreplace.fwordcompare", val, 0);
        findreplace->fwordcompare((val == (int) FWORDCOMPARE::YES) ? FWORDCOMPARE::YES : FWORDCOMPARE::NO);
        preferences.get("findreplace.fregex", val, 0);
        findreplace->fregex((val == (int) FREGEX::YES) ? FREGEX::YES : FREGEX::NO);
    }
    {
        preferences.get("fle.wordwrap", val, 80);
        pref_wrap = ((unsigned) val >= limits::WRAP_MIN && (unsigned) val <= limits::WRAP_MAX) ? val : limits::WRAP_DEF;
        preferences.get("fle.syntax.bat.tab", val, (int) style::get_tab_type(style::BAT));
        style::set_tab_type(style::BAT, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.bat.tab_width", val, style::get_tab_width(style::BAT));
        style::set_tab_width(style::BAT, val);
        preferences.get("fle.syntax.cpp.tab", val, (int) style::get_tab_type(style::CPP));
        style::set_tab_type(style::CPP, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.cpp.tab_width", val, style::get_tab_width(style::CPP));
        style::set_tab_width(style::CPP, val);
        preferences.get("fle.syntax.cs.tab", val, (int) style::get_tab_type(style::CS));
        style::set_tab_type(style::CS, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.cs.tab_width", val, style::get_tab_width(style::CS));
        style::set_tab_width(style::CS, val);
        preferences.get("fle.syntax.go.tab", val, (int) style::get_tab_type(style::GO));
        style::set_tab_type(style::GO, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.go.tab_width", val, style::get_tab_width(style::GO));
        style::set_tab_width(style::GO, val);
        preferences.get("fle.syntax.java.tab", val, (int) style::get_tab_type(style::JAVA));
        style::set_tab_type(style::JAVA, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.java.tab_width", val, style::get_tab_width(style::JAVA));
        style::set_tab_width(style::JAVA, val);
        preferences.get("fle.syntax.js.tab", val, (int) style::get_tab_type(style::JS));
        style::set_tab_type(style::JS, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.js.tab_width", val, style::get_tab_width(style::JS));
        style::set_tab_width(style::JS, val);
        preferences.get("fle.syntax.kotlin.tab", val, (int) style::get_tab_type(style::KOTLIN));
        style::set_tab_type(style::KOTLIN, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.kotlin.tab_width", val, style::get_tab_width(style::KOTLIN));
        style::set_tab_width(style::KOTLIN, val);
        preferences.get("fle.syntax.lua.tab", val, (int) style::get_tab_type(style::LUA));
        style::set_tab_type(style::LUA, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.lua.tab_width", val, style::get_tab_width(style::LUA));
        style::set_tab_width(style::LUA, val);
        preferences.get("fle.syntax.makefile.tab", val, (int) style::get_tab_type(style::MAKEFILE));
        style::set_tab_type(style::MAKEFILE, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.makefile.tab_width", val, style::get_tab_width(style::MAKEFILE));
        style::set_tab_width(style::MAKEFILE, val);
        preferences.get("fle.syntax.markup.tab", val, (int) style::get_tab_type(style::MARKUP));
        style::set_tab_type(style::MARKUP, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.markup.tab_width", val, style::get_tab_width(style::MARKUP));
        style::set_tab_width(style::MARKUP, val);
        preferences.get("fle.syntax.python.tab", val, (int) style::get_tab_type(style::PYTHON));
        style::set_tab_type(style::PYTHON, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.python.tab_width", val, style::get_tab_width(style::PYTHON));
        style::set_tab_width(style::PYTHON, val);
        preferences.get("fle.syntax.ruby.tab", val, (int) style::get_tab_type(style::RUBY));
        style::set_tab_type(style::RUBY, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.ruby.tab_width", val, style::get_tab_width(style::RUBY));
        style::set_tab_width(style::RUBY, val);
        preferences.get("fle.syntax.rust.tab", val, (int) style::get_tab_type(style::RUST));
        style::set_tab_type(style::RUST, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.rust.tab_width", val, style::get_tab_width(style::RUST));
        style::set_tab_width(style::RUST, val);
        preferences.get("fle.syntax.shell.tab", val, (int) style::get_tab_type(style::SHELL));
        style::set_tab_type(style::SHELL, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.shell.tab_width", val, style::get_tab_width(style::SHELL));
        style::set_tab_width(style::SHELL, val);
        preferences.get("fle.syntax.text.tab", val, (int) style::get_tab_type(style::TEXT));
        style::set_tab_type(style::TEXT, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.text.tab_width", val, style::get_tab_width(style::TEXT));
        style::set_tab_width(style::TEXT, val);
        preferences.get("fle.syntax.ts.tab", val, (int) style::get_tab_type(style::TS));
        style::set_tab_type(style::TS, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.ts.tab_width", val, style::get_tab_width(style::TS));
        style::set_tab_width(style::TS, val);
        preferences.get("fle.syntax.wren.tab", val, (int) style::get_tab_type(style::WREN));
        style::set_tab_type(style::WREN, (val == 0) ? FTAB::HARD : FTAB::SOFT);
        preferences.get("fle.syntax.wren.tab_width", val, style::get_tab_width(style::WREN));
        style::set_tab_width(style::WREN, val);
    }
    {
        size_t count = 1;
        while (count <= limits::FIND_LIST_MAX) {
            auto key = gnu::str::format("find.%d", (int) count++);
            preferences.get(key.c_str(), s, "");
            if (s == "") {
                break;
            }
            add_find_word(s, true);
        }
        count = 1;
        while (count <= limits::FIND_LIST_MAX) {
            auto key = gnu::str::format("replace.%d", (int) count++);
            preferences.get(key.c_str(), s, "");
            if (s == "") {
                break;
            }
            add_replace_word(s, true);
        }
    }
}
void Config::remove_receiver(const Message* object) {
    for (auto it = _list.begin(); it != _list.end(); ++it) {
        if (object == *it) {
            _list.erase(it);
            _del = true;
            return;
        }
    }
}
void Config::save_pref(Fl_Preferences& preferences, FindReplace* findreplace) {
    preferences.set("fle.autocomplete", pref_autocomplete);
    preferences.set("fle.backup", pref_backup.filename);
    preferences.set("fle.binary", (int) pref_binary);
    preferences.set("fle.cursor", pref_cursor);
    preferences.set("fle.indent", pref_indentation);
    preferences.set("fle.insert", pref_insert);
    preferences.set("fle.linenumber", pref_linenumber);
    preferences.set("fle.reload", pref_autoreload);
    preferences.set("fle.scheme", pref_scheme);
    preferences.set("fle.scroll", (int) pref_mouse_scroll);
    preferences.set("fle.statusbar", pref_statusbar);
    preferences.set("fle.undo_mode", (int) pref_undo);
    preferences.set("fle.wordwrap", (int) pref_wrap);
    preferences.set("tweak.autocomplete_filesize", (int) limits::AUTOCOMPLETE_FILESIZE_VAL);
    preferences.set("tweak.autocomplete_lines", (int) limits::AUTOCOMPLETE_LINES_VAL);
    preferences.set("tweak.autocomplete_word_size", (int) limits::AUTOCOMPLETE_WORD_SIZE_VAL);
    preferences.set("tweak.count_char", (int) limits::COUNT_CHAR_VAL);
    preferences.set("tweak.file_backup_size", (int) limits::FILE_BACKUP_SIZE_VAL);
    preferences.set("tweak.file_size", (int) limits::FILE_SIZE_VAL);
    preferences.set("tweak.output_lines", (int) limits::OUTPUT_LINES_VAL);
    preferences.set("tweak.output_line_length", (int) limits::OUTPUT_LINE_LENGTH_VAL);
    preferences.set("tweak.force_restyling", (int) limits::FORCE_RESTYLING);
    preferences.set("tweak.style_filesize", (int) limits::STYLE_FILESIZE_VAL);
    preferences.set("tweak.wrap_line_length", (int) limits::WRAP_LINE_LENGTH_VAL);
    preferences.set("finddialog.fregex", (int) FindDialog::REGEX);
    preferences.set("finddialog.ftrim", (int) FindDialog::TRIM);
    preferences.set("replacedialog.fcasecompare", (int) ReplaceDialog::CASECOMPARE);
    preferences.set("replacedialog.fnltab", (int) ReplaceDialog::NLTAB);
    preferences.set("replacedialog.fregex", (int) ReplaceDialog::REGEX);
    preferences.set("replacedialog.fselection", (int) ReplaceDialog::SELECTION);
    preferences.set("replacedialog.fwordcompare", (int) ReplaceDialog::WORDCOMPARE);
    if (findreplace != nullptr) {
        preferences.set("findreplace.fcasecompare", (int) findreplace->fcasecompare());
        preferences.set("findreplace.fnltab", (int) findreplace->fnltab());
        preferences.set("findreplace.fregex", (int) findreplace->fregex());
        preferences.set("findreplace.fselection", (int) findreplace->fselection());
        preferences.set("findreplace.fwordcompare", (int) findreplace->fwordcompare());
    }
    preferences.set("fle.syntax.bat.tab", (int) style::get_tab_type(style::BAT));
    preferences.set("fle.syntax.bat.tab_width", (int) style::get_tab_width(style::BAT));
    preferences.set("fle.syntax.cpp.tab", (int) style::get_tab_type(style::CPP));
    preferences.set("fle.syntax.cpp.tab_width", (int) style::get_tab_width(style::CPP));
    preferences.set("fle.syntax.cs.tab", (int) style::get_tab_type(style::CS));
    preferences.set("fle.syntax.cs.tab_width", (int) style::get_tab_width(style::CS));
    preferences.set("fle.syntax.go.tab", (int) style::get_tab_type(style::GO));
    preferences.set("fle.syntax.go.tab_width", (int) style::get_tab_width(style::GO));
    preferences.set("fle.syntax.java.tab", (int) style::get_tab_type(style::JAVA));
    preferences.set("fle.syntax.java.tab_width", (int) style::get_tab_width(style::JAVA));
    preferences.set("fle.syntax.js.tab", (int) style::get_tab_type(style::JS));
    preferences.set("fle.syntax.js.tab_width", (int) style::get_tab_width(style::JS));
    preferences.set("fle.syntax.kotlin.tab", (int) style::get_tab_type(style::KOTLIN));
    preferences.set("fle.syntax.kotlin.tab_width", (int) style::get_tab_width(style::KOTLIN));
    preferences.set("fle.syntax.lua.tab", (int) style::get_tab_type(style::LUA));
    preferences.set("fle.syntax.lua.tab_width", (int) style::get_tab_width(style::LUA));
    preferences.set("fle.syntax.makefile.tab", (int) style::get_tab_type(style::MAKEFILE));
    preferences.set("fle.syntax.makefile.tab_width", (int) style::get_tab_width(style::MAKEFILE));
    preferences.set("fle.syntax.markup.tab", (int) style::get_tab_type(style::MARKUP));
    preferences.set("fle.syntax.markup.tab_width", (int) style::get_tab_width(style::MARKUP));
    preferences.set("fle.syntax.python.tab", (int) style::get_tab_type(style::PYTHON));
    preferences.set("fle.syntax.python.tab_width", (int) style::get_tab_width(style::PYTHON));
    preferences.set("fle.syntax.ruby.tab", (int) style::get_tab_type(style::RUBY));
    preferences.set("fle.syntax.ruby.tab_width", (int) style::get_tab_width(style::RUBY));
    preferences.set("fle.syntax.rust.tab", (int) style::get_tab_type(style::RUST));
    preferences.set("fle.syntax.rust.tab_width", (int) style::get_tab_width(style::RUST));
    preferences.set("fle.syntax.shell.tab", (int) style::get_tab_type(style::SHELL));
    preferences.set("fle.syntax.shell.tab_width", (int) style::get_tab_width(style::SHELL));
    preferences.set("fle.syntax.text.tab", (int) style::get_tab_type(style::TEXT));
    preferences.set("fle.syntax.text.tab_width", (int) style::get_tab_width(style::TEXT));
    preferences.set("fle.syntax.ts.tab", (int) style::get_tab_type(style::TS));
    preferences.set("fle.syntax.ts.tab_width", (int) style::get_tab_width(style::TS));
    preferences.set("fle.syntax.wren.tab", (int) style::get_tab_type(style::WREN));
    preferences.set("fle.syntax.wren.tab_width", (int) style::get_tab_width(style::WREN));
    int count = 1;
    for (const auto& find : find_list) {
        auto key = gnu::str::format("find.%d", count++);
        preferences.set(key.c_str(), find);
    }
    count = 1;
    for (const auto& replace : replace_list) {
        auto key = gnu::str::format("replace.%d", count++);
        preferences.set(key.c_str(), replace);
    }
}
void Config::send_message(std::string message, std::string s, const void* p) {
    static int COUNT = 0;
    auto       list  = _list;
    COUNT++;
    for (auto* o1 : list) {
        auto b = Message::CTRL::CONTINUE;
        if (_del == false) {
            b = o1->message(message, s, p);
        }
        else {
            for (auto* o2 : _list) {
                if (o1 == o2) {
                    b = o1->message(message, s, p);
                    break;
                }
            }
        }
        if (b == Message::CTRL::ABORT) {
            break;
        }
    }
    COUNT--;
    if (COUNT == 0) {
        _del = false;
    }
}
CursorPos::CursorPos() {
    drag  = -1;
    end   = -1;
    pos1  = -1;
    pos2  = -1;
    start = -1;
    swap  = false;
    top1  = -1;
    top2  = -1;
}
CursorPos::CursorPos(int pos1_, int pos2_, int start_, int end_, bool swap_) {
    drag  = 0;
    end   = end_;
    pos1  = pos1_;
    pos2  = pos2_;
    start = start_;
    swap  = swap_;
    top1  = -1;
    top2  = -1;
    if (start >= 0 && end > start) {
        drag = (pos1 > start) ? start : end;
    }
}
CursorPos::CursorPos(int pos1_, int pos2_, int drag_, int start_, int end_, bool swap_) {
    drag  = drag_;
    end   = end_;
    pos1  = pos1_;
    pos2  = pos2_;
    start = start_;
    swap  = swap_;
    top1  = -1;
    top2  = -1;
}
void CursorPos::_convert(bool swapped) {
    if ((swapped == true && swap == false) || (swapped == false && swap == true)) {
        auto tmp = pos1;
        pos1 = pos2;
        pos2 = tmp;
        tmp = top1;
        top1 = top2;
        top2 = tmp;
        swap = swapped;
    }
}
void CursorPos::debug(int line, const char* file) const {
#ifdef DEBUG
    printf("\nCursorPos: (%s -  %d)\n", file, line);
    printf("    pos1               = %9d\n", pos1);
    printf("    pos2               = %9d\n", pos2);
    printf("    top1               = %9d\n", top1);
    printf("    top2               = %9d\n", top2);
    printf("    drag               = %9d\n", drag);
    printf("    start              = %9d\n", start);
    printf("    end                = %9d\n", end);
    printf("    swap               = %9s\n", swap ? "TRUE" : "FALSE");
    fflush(stdout);
#else
    (void) line;
    (void) file;
#endif
}
EditorFlags::EditorFlags() {
    dnd        = false;
    fsearchdir = FSEARCHDIR::FORWARD;
    fsplitview = FSPLITVIEW::VERTICAL;
    fwrap      = FWRAP::NO;
    kommand    = false;
    ro         = false;
    tab_mode   = FTAB::SOFT;
    tab_width  = 8;
    wrap_col   = 0;
}
void EditorFlags::debug() const {
#ifdef DEBUG
    printf("\nEditorFlags:\n");
    printf("    fsearchdir         = %9d\n", (int) fsearchdir);
    printf("    fsplitview         = %9d\n", (int) fsplitview);
    printf("    fwrap              = %9s\n", fwrap == FWRAP::YES ? "YES" : "NO");
    printf("    kommand            = %9s\n", kommand ? "TRUE" : "FALSE");
    printf("    ro                 = %9s\n", ro ? "TRUE" : "FALSE");
    printf("    tab_mode           = %9s\n", tab_mode == FTAB::HARD ? "HARD" : "SOFT");
    printf("    tab_width          = %9u\n", tab_width);
    printf("    wrap_col           = %9u\n", wrap_col);
    fflush(stdout);
#endif
}
void EditorFlags::set_tab_from_string(std::string s) {
    if (s == "FTAB::HARD") {
        tab_mode = FTAB::HARD;
    }
    else if (s == "FTAB::SOFT") {
        tab_mode = FTAB::SOFT;
    }
    else if (s == "1") {
        tab_width = 1;
    }
    else if (s == "2") {
        tab_width = 2;
    }
    else if (s == "3") {
        tab_width = 3;
    }
    else if (s == "4") {
        tab_width = 4;
    }
    else if (s == "5") {
        tab_width = 5;
    }
    else if (s == "6") {
        tab_width = 6;
    }
    else if (s == "7") {
        tab_width = 7;
    }
    else if (s == "8") {
        tab_width = 8;
    }
    else {
        assert(false);
    }
}
FileInfo::FileInfo() {
    binary      = false;
    fletcher64  = 0;
    fi          = gnu::File();
    flineending = FLINEENDING::UNIX;
    reload_time = 0;
}
void FileInfo::debug() const {
#ifdef DEBUG
    printf("\nFileInfo:\n");
    printf("    binary             = %9s\n", binary ? "TRUE" : "FALSE");
    printf("    file               = %s\n", fi.to_string().c_str());
    printf("    fletcher64         = %llx\n", (long long unsigned) fletcher64);
    printf("    flineending        = %s\n", flineending == FLINEENDING::UNIX ? "UNIX" : "WINDOWS");
    printf("    reload_time        = %9lld\n", (long long int) reload_time);
    fflush(stdout);
#endif
}
std::string help::find_lines() {
    std::string res = R"(Find lines in a file.
Enter an search string.
Or an valid regular expression (using pcre syntax).

Find all lines that starts with 'local'.
^local.*

Find all lines that end with 'do' and set found column.
.*(do$)

)";
    return res;
}
std::string help::flags(const Config& config) {
    std::string res;
    int width = 30;
    res += "Settings, flags and other limits:\n";
    res += gnu::str::format("%-*s = %d\n", width, "flw::PREF_FIXED_FONT", flw::PREF_FIXED_FONT);
    res += gnu::str::format("%-*s = %s\n", width, "flw::PREF_FIXED_FONTNAME", flw::PREF_FIXED_FONTNAME.c_str());
    res += gnu::str::format("%-*s = %d\n", width, "flw::PREF_FONT", flw::PREF_FONT);
    res += gnu::str::format("%-*s = %s\n", width, "flw::PREF_FONTNAME", flw::PREF_FONTNAME.c_str());
    res += gnu::str::format("%-*s = %d\n", width, "flw::PREF_FONTSIZE", flw::PREF_FONTSIZE);
    res += gnu::str::format("%-*s = %s\n", width, "pref::autocomplete", (int) config.pref_autocomplete ? "true" : "false");
    res += gnu::str::format("%-*s = %s\n", width, "pref::autoreload", config.pref_autoreload ? "true" : "false");
    res += gnu::str::format("%-*s = '%s'\n", width, "pref::backup", config.pref_backup.c_str());
    res += gnu::str::format("%-*s = %d\n", width, "pref::binary", config.pref_binary);
    res += gnu::str::format("%-*s = %d\n", width, "pref::cursor", (int) config.pref_cursor);
    res += gnu::str::format("%-*s = %s\n", width, "pref::indentation", config.pref_indentation ? "true" : "false");
    res += gnu::str::format("%-*s = %s\n", width, "pref::insert", config.pref_insert ? "true" : "false");
    res += gnu::str::format("%-*s = %s\n", width, "pref::linenumber", (int) config.pref_linenumber ? "true" : "false");
    res += gnu::str::format("%-*s = %d\n", width, "pref::tmp_fontsize", config.pref_tmp_fontsize);
    res += gnu::str::format("%-*s = %d\n", width, "pref::mouse_scroll", config.pref_mouse_scroll + 3);
    res += gnu::str::format("%-*s = %s\n", width, "pref::scheme", config.pref_scheme.c_str());
    res += gnu::str::format("%-*s = %s\n", width, "pref::statusbar", (int) config.pref_statusbar ? "true" : "false");
    res += gnu::str::format("%-*s = %d\n", width, "pref::wrap", config.pref_wrap);
    res += gnu::str::format("%-*s = %14s\n", width, "limits::AUTOCOMPLETE_FILESIZE", flw::util::format_int(limits::AUTOCOMPLETE_FILESIZE_VAL, '\'').c_str());
    res += gnu::str::format("%-*s = %14s\n", width, "limits::AUTOCOMPLETE_LINES", flw::util::format_int(limits::AUTOCOMPLETE_LINES_VAL, '\'').c_str());
    res += gnu::str::format("%-*s = %14s\n", width, "limits::AUTOCOMPLETE_WORD_SIZE", flw::util::format_int(limits::AUTOCOMPLETE_WORD_SIZE_VAL, '\'').c_str());
    res += gnu::str::format("%-*s = %14s\n", width, "limits::COUNT_CHAR", flw::util::format_int(limits::COUNT_CHAR_VAL, '\'').c_str());
    res += gnu::str::format("%-*s = %14s\n", width, "limits::FILE_BACKUP_SIZE", flw::util::format_int(limits::FILE_BACKUP_SIZE_VAL, '\'').c_str());
    res += gnu::str::format("%-*s = %14s\n", width, "limits::FILE_SIZE", flw::util::format_int(limits::FILE_SIZE_VAL, '\'').c_str());
    res += gnu::str::format("%-*s = %14s\n", width, "limits::FIND_LIST_MAX", flw::util::format_int(limits::FIND_LIST_MAX, '\'').c_str());
    res += gnu::str::format("%-*s = %14s\n", width, "limits::FORCE_RESTYLING", limits::FORCE_RESTYLING ? "YES" : "NO");
    res += gnu::str::format("%-*s = %14s\n", width, "limits::HEXFILE_SIZE_MAX", flw::util::format_int(limits::FILE_SIZE_VAL / limits::HEXFILE_DIVIDER, '\'').c_str());
    res += gnu::str::format("%-*s = %14s\n", width, "limits::OUTPUT_LINES", flw::util::format_int(limits::OUTPUT_LINES_VAL, '\'').c_str());
    res += gnu::str::format("%-*s = %14s\n", width, "limits::OUTPUT_LINE_LENGTH", flw::util::format_int(limits::OUTPUT_LINE_LENGTH_VAL, '\'').c_str());
    res += gnu::str::format("%-*s = %14s\n", width, "limits::STYLE_FILESIZE", flw::util::format_int(limits::STYLE_FILESIZE_VAL, '\'').c_str());
    res += gnu::str::format("%-*s = %14s\n", width, "limits::UNDO_WARNING", flw::util::format_int(limits::UNDO_WARNING, '\'').c_str());
    res += gnu::str::format("%-*s = %14s\n", width, "limits::WRAP_LINE_LENGTH", flw::util::format_int(limits::WRAP_LINE_LENGTH_VAL, '\'').c_str());
    res += "\n";
    return res;
}
std::string help::general() {
    std::string res = R"(Font size can be overrided temporarily by using ctrl + mousewheel.
Right click to show the editor menu.
Use alt + mousewheel to scroll 3x faster.
Although the shortcut might be used by the windows manager (like XFCE for zooming).

)";
    res += "Keyboard shortcuts:\n";
    std::string keys1;
    std::string keys2;
    for (size_t f = 0; f < (size_t) fle::FKEY_SIZE; f++) {
        if (KEYS[f].help[0] != 0) {
            if (KEYS[f].kommand_u == false) {
                keys1 += fle::KEYS[f].make_help(26);
            }
            else {
                keys2 += fle::KEYS[f].make_help(26);
            }
        }
    }
    res += keys1;
    res += "\nCommand mode (press command key and release then key combination below):\n";
    res += keys2;
    res += "\n";
    return res;
}
std::string help::pcre() {
    std::string res = R"(Perl Compatible Regular Expressions

Regular expression basics:
.             Any character except newline
a             The character a
ab            The string ab
a|b           a or b
a*            0 or more a's
\             Escapes a special character

Regular expression quantifiers:
*             0 or more
+             1 or more
?             0 or 1
{2}           Exactly 2
{2, 5}        Between 2 and 5
{2,}          2 or more
Default is greedy. Append ? for reluctant.

Regular expression groups:
(...)         Capturing group
(?P<Y>...)    Capturing group named Y
(?:...)       Non-capturing group
(?>...)       Atomic group
(?|...)       Duplicate group numbers
\Y            Match the Y'th captured group
(?P=Y)        Match the named group Y
(?R)          Recurse into entire pattern
(?Y)          Recurse into numbered group Y
(?&Y)         Recurse into named group Y
\g{Y}         Match the named or numbered group Y
\g<Y>         Recurse into named or numbered group Y
(?#...)       Comment

Regular expression character classes:
[ab-d]        One character of: a, b, c, d
[^ab-d]       One character except: a, b, c, d
[\b]          Backspace character
\d            One digit
\D            One non-digit
\s            One whitespace
\S            One non-whitespace
\w            One word character
\W            One non-word character

Regular expression assertions:
^             Start of string
\A            Start of string, ignores m flag
$             End of string
\Z            End of string, ignores m flag
\b            Word boundary
\B            Non-word boundary
\G            Start of match
(?=...)       Positive lookahead
(?!...)       Negative lookahead
(?<=...)      Positive lookbehind
(?<!...)      Negative lookbehind
(?()|)        Conditional

Regular expression escapes:
\Q..\E        Remove special meaning

Regular expression flags:
i             Ignore case
m             ^ and $ match start and end of line
s             . matches newline as well
x             Allow spaces and comments
J             Duplicate group names allowed
U             Ungreedy quantifiers
(?iLmsux)     Set flags within regex

Regular expression special characters:
\n            Newline
\r            Carriage return
\t            Tab
\0            Null character
\YYY          Octal character YYY
\xYY          Hexadecimal character YY
\x{YY}        Hexadecimeal character YY
\cY           Control character Y

Regular expression posix classes:
[:alnum:]     Letters and digits
[:alpha:]     Letters
[:ascii:]     Ascii codes 0 - 127
[:blank:]     Space or tab only
[:cntrl:]     Control characters
[:digit:]     Decimal digits
[:graph:]     Visible characters, except space
[:lower:]     Lowercase letters
[:print:]     Visible characters
[:punct:]     Visible punctuation characters
[:space:]     Whitespace
[:upper:]     Uppercase letters
[:word:]      Word characters
[:xdigit:]    Hexadecimal digits

)";
    return res;
}
std::string help::replace_text() {
    std::string res = R"(Replace text in all files.
Enter an search string.
Or an valid regular expression (using pcre syntax).

)";
    return res;
}
std::string KeyConf::KeyDescr(int key) {
    if (key > 0x20 && key < 0x7F) {
        char b[20];
        snprintf(b, 20, "'%c'", key);
        return b;
    }
    switch (key) {
        case 0x20:
            return "space";
        case FL_BackSpace:
            return "backspace";
        case FL_Tab:
            return "tab";
        case FL_Enter:
            return "enter";
        case FL_Pause:
            return "pause";
        case FL_Scroll_Lock:
            return "scroll";
        case FL_Escape:
            return "escape";
        case FL_Home:
            return "home";
        case FL_Left:
            return "left";
        case FL_Up:
            return "up";
        case FL_Right:
            return "right";
        case FL_Down:
            return "down";
        case FL_Page_Up:
            return "page-up";
        case FL_Page_Down:
            return "page-down";
        case FL_End:
            return "end";
        case FL_Print:
            return "print";
        case FL_Insert:
            return "insert";
        case FL_Menu:
            return "menu";
        case FL_Help:
            return "help";
        case FL_Num_Lock:
            return "num";
        case FL_KP + 0:
            return "Kp 0";
        case FL_KP + 1:
            return "Kp 1";
        case FL_KP + 2:
            return "Kp 2";
        case FL_KP + 3:
            return "Kp 3";
        case FL_KP + 4:
            return "Kp 4";
        case FL_KP + 5:
            return "Kp 5";
        case FL_KP + 6:
            return "Kp 6";
        case FL_KP + 7:
            return "Kp 7";
        case FL_KP + 8:
            return "Kp 8";
        case FL_KP + 9:
            return "Kp 9";
        case FL_KP_Enter:
            return "kp enter";
        case FL_F + 1:
            return "F1";
        case FL_F + 2:
            return "F2";
        case FL_F + 3:
            return "F3";
        case FL_F + 4:
            return "F4";
        case FL_F + 5:
            return "F5";
        case FL_F + 6:
            return "F6";
        case FL_F + 7:
            return "F7";
        case FL_F + 8:
            return "F8";
        case FL_F + 9:
            return "F9";
        case FL_F + 10:
            return "F10";
        case FL_F + 11:
            return "F11";
        case FL_F + 12:
            return "F12";
        case FL_Shift_L:
            return "left shift";
        case FL_Shift_R:
            return "right shift";
        case FL_Control_L:
            return "left control";
        case FL_Control_R:
            return "right control";
        case FL_Caps_Lock:
            return "caps lock key";
        case FL_Meta_L:
            return "left meta";
        case FL_Meta_R:
            return "right meta";
        case FL_Alt_L:
            return "left alt";
        case FL_Alt_R:
            return "right alt";
        case FL_Delete:
            return "delete";
        default:
            return "?";
        }
}
bool KeyConf::has_custom_key() const {
    return alt_d != alt_u || ctrl_d != ctrl_u || shift_d != shift_u || kommand_d != kommand_u || key_d != key_u;
}
void KeyConf::LoadPref(Fl_Preferences& preferences) {
    for (int f = 0; f < fle::FKEY_SIZE; f++) {
        fle::KEYS[f].alt_u     = fle::KEYS[f].alt_d;
        fle::KEYS[f].ctrl_u    = fle::KEYS[f].ctrl_d;
        fle::KEYS[f].shift_u   = fle::KEYS[f].shift_d;
        fle::KEYS[f].kommand_u = fle::KEYS[f].kommand_d;
        fle::KEYS[f].key_u     = fle::KEYS[f].key_d;
    }
    auto keyboard = 0;
    preferences.get("keyboard", keyboard, 0);
    if (keyboard == KeyConf::VERSION) {
        for (int f = 0; f < fle::FKEY_SIZE; f++) {
            auto value = 0;
            preferences.get(gnu::str::format("alt_%02d", f).c_str(), value, fle::KEYS[f].alt_d);
            fle::KEYS[f].alt_u = value;
            preferences.get(gnu::str::format("ctrl_%02d", f).c_str(), value, fle::KEYS[f].ctrl_d);
            fle::KEYS[f].ctrl_u = value;
            preferences.get(gnu::str::format("shift_%02d", f).c_str(), value, fle::KEYS[f].shift_d);
            fle::KEYS[f].shift_u = value;
            preferences.get(gnu::str::format("kommand_%02d", f).c_str(), value, fle::KEYS[f].kommand_d);
            fle::KEYS[f].kommand_u = value;
            preferences.get(gnu::str::format("key_%02d", f).c_str(), value, fle::KEYS[f].key_d);
            fle::KEYS[f].key_u = value;
        }
    }
}
std::string KeyConf::make_help(int w) const {
    auto        lines = gnu::str::split(help, '|');
    std::string res;
    for (size_t f = 0; f < lines.size(); f++) {
        if (f == 0) {
            res += gnu::str::format("%-*s%s\n", w, to_string().c_str(), lines[f].c_str());
        }
        else {
            res += gnu::str::format("%-*s%s\n", w, "", lines[f].c_str());
        }
    }
    return res;
}
void KeyConf::SavePref(Fl_Preferences& preferences) {
    for (int f = 0; f < fle::FKEY_SIZE; f++) {
        preferences.set("keyboard", KeyConf::VERSION);
        if (fle::KEYS[f].has_custom_key() == true) {
            preferences.set(gnu::str::format("alt_%02d", f).c_str(), fle::KEYS[f].alt_u);
            preferences.set(gnu::str::format("ctrl_%02d", f).c_str(), fle::KEYS[f].ctrl_u);
            preferences.set(gnu::str::format("shift_%02d", f).c_str(), fle::KEYS[f].shift_u);
            preferences.set(gnu::str::format("kommand_%02d", f).c_str(), fle::KEYS[f].kommand_u);
            preferences.set(gnu::str::format("key_%02d", f).c_str(), fle::KEYS[f].key_u);
        }
    }
}
int KeyConf::to_int() const {
    if (kommand_u == true) {
        return 0;
    }
    auto res = key_u;
    if (alt_u == true) {
        res += FL_ALT;
    }
    if (ctrl_u == true) {
        res += FL_CTRL;
    }
    if (shift_u == true) {
        res += FL_SHIFT;
    }
    return res;
}
std::string KeyConf::to_string() const {
    std::string res;
    if (alt_u == true) {
        res = "alt";
    }
    if (ctrl_u == true) {
        if (res != "") {
            res += " + ";
        }
        res += "ctrl";
    }
    if (shift_u == true) {
        if (res != "") {
            res += " + ";
        }
        res += "shift";
    }
    if (res != "") {
        res += " + ";
    }
    res += KeyConf::KeyDescr(key_u);
    return res;
}
Message::Message(Config& config) : _config(config) {
    _id = 0;
    _id = _config.add_receiver(this);
}
Message::~Message() {
    _config.remove_receiver(this);
}
StatusBarInfo::StatusBarInfo() {
    col   = 0;
    end   = 0;
    pos   = 0;
    row   = 0;
    rows  = 0;
    start = 0;
}
void StatusBarInfo::debug() const {
#ifdef DEBUG
    printf("\nStatusBarInfo:\n");
    printf("    start              = %9d\n", start);
    printf("    end                = %9d\n", end);
    printf("    pos                = %9d\n", pos);
    printf("    selected           = %9d\n", end - start);
    printf("    rows               = %9d\n", rows);
    printf("    row                = %9d\n", row);
    printf("    column             = %9d\n", col);
#endif
}
gnu::FileBuf string::binary_to_hex(const char* in, size_t in_size) {
    assert(in);
    auto size = (size_t) (in_size / 16.0 * 79.0) + 200;
    auto fbuf = gnu::FileBuf(size);
    auto pos  = (size_t) 0;
#ifdef DEBUG_EDITOR
    auto start = gnu::Time::Milli();
#endif
    for (size_t f = 0; f < in_size;) {
        sprintf(fbuf.p + pos, "%08x:", (unsigned int) f);
        pos += 8;
        for (int e = 0; e < 16; e++) {
            if (f + e < in_size) {
                auto c = (unsigned char) in[f + e];
                auto h = 2 * c;
                fbuf.p[pos++] = ' ';
                fbuf.p[pos++] = _FLE_HEX_STRINGS[h];
                fbuf.p[pos++] = _FLE_HEX_STRINGS[h + 1];
            }
            else {
                fbuf.p[pos++] = ' ';
                fbuf.p[pos++] = ' ';
                fbuf.p[pos++] = ' ';
            }
            if (e == 7) {
                fbuf.p[pos++] = ' ';
                fbuf.p[pos++] = '|';
            }
        }
        fbuf.p[pos++] = ' ';
        fbuf.p[pos++] = ' ';
        for (int e = 0; e < 16; e++) {
            if (f + e < in_size) {
                auto c = (unsigned char) in[f + e];
                if (e % 8 == 0) {
                    fbuf.p[pos++] = ' ';
                }
                if (c < 32 || c > 126) {
                    fbuf.p[pos++] = '.';
                }
                else {
                    fbuf.p[pos++] = c;
                }
            }
            else {
                fbuf.p[pos++] = ' ';
            }
        }
        fbuf.p[pos++] = '\n';
        fbuf.p[pos] = 0;
        fbuf.s = pos;
        f += 16;
    }
    return fbuf;
}
gnu::FileBuf string::binary_to_text(const char* in, size_t in_size) {
    assert(in);
    auto text_size = (size_t) (in_size + (in_size / 80) + 100);
    auto text_pos  = 0;
    auto fbuf      = gnu::FileBuf(text_size);
    auto c0        = (unsigned char) 0;
    auto c1        = (unsigned char) 0;
    auto c2        = (unsigned char) 0;
    auto c3        = (unsigned char) 0;
    auto nl        = 0;
#ifdef DEBUG_EDITOR
    auto start = gnu::Time::Milli();
#endif
    for (size_t f = 0; f < in_size; f++) {
        auto c = (unsigned char) in[f];
        if (c >= 0xC2 && c <= 0xDF && f < in_size - 1) {
            c0 = c;
            f++;
            c1 = (unsigned char) in[f];
            nl++;
            if (c1 >= 0x80 && c1 <= 0xBF) {
                fbuf.p[text_pos++] = c0;
                fbuf.p[text_pos++] = c1;
            }
            else {
                nl++;
                fbuf.p[text_pos++] = '.';
                fbuf.p[text_pos++] = '.';
            }
        }
        else if (c >= 0xE0 && c <= 0xEF && f < in_size - 2) {
            c0 = c;
            f++;
            c1 = (unsigned char) in[f];
            f++;
            c2 = (unsigned char) in[f];
            nl++;
            if (c1 >= 0x80 && c1 <= 0xBF && c2 >= 0x80 && c2 <= 0xBF) {
                fbuf.p[text_pos++] = c0;
                fbuf.p[text_pos++] = c1;
                fbuf.p[text_pos++] = c2;
            }
            else {
                nl++;
                nl++;
                fbuf.p[text_pos++] = '.';
                fbuf.p[text_pos++] = '.';
                fbuf.p[text_pos++] = '.';
            }
        }
        else if (c >= 0xF0 && c <= 0xF3 && f < in_size - 3) {
            c0 = c;
            f++;
            c1 = (unsigned char) in[f];
            f++;
            c2 = (unsigned char) in[f];
            f++;
            c3 = (unsigned char) in[f];
            nl++;
            if (c1 >= 0x80 && c1 <= 0xBF && c2 >= 0x80 && c2 <= 0xBF && c3 >= 0x80 && c3 <= 0xBF) {
                fbuf.p[text_pos++] = c0;
                fbuf.p[text_pos++] = c1;
                fbuf.p[text_pos++] = c2;
                fbuf.p[text_pos++] = c3;
            }
            else {
                nl++;
                nl++;
                nl++;
                nl++;
                fbuf.p[text_pos++] = '.';
                fbuf.p[text_pos++] = '.';
                fbuf.p[text_pos++] = '.';
                fbuf.p[text_pos++] = '.';
            }
        }
        else if (c >= 0x20 && c < 0x7F) {
            fbuf.p[text_pos++] = c;
            nl++;
        }
        else if (c == '\t') {
            fbuf.p[text_pos++] = 'T';
            nl++;
        }
        else if (c == '\n') {
            fbuf.p[text_pos++] = 'N';
            nl++;
        }
        else if (c == '\r') {
            fbuf.p[text_pos++] = 'R';
            nl++;
        }
        else {
            fbuf.p[text_pos++] = '.';
            nl++;
        }
        if (nl >= 80 && text_pos) {
            fbuf.p[text_pos++] = '\n';
            nl = 0;
        }
        fbuf.s = text_pos;
    }
    return fbuf;
}
std::string string::fnltab(std::string text) {
    gnu::str::replace(text, "\\t", "\t");
    gnu::str::replace(text, "\\n", "\n");
    gnu::str::replace(text, "\\r", "\r");
    return text;
}
bool string::is_one_char(const char* in) {
    assert(in);
    if (*in == 0) {
        return false;
    }
    auto ustring = reinterpret_cast<const unsigned char*>(in);
    auto count   = 0;
    while (*ustring != 0) {
        count += (*ustring & 0xc0) != 0x80;
        ustring++;
        if (count > 1) {
            return false;
        }
    }
    return true;
}
void string::replace_char(char* in, char find, char replace) {
    assert(in);
    while (*in != 0) {
        if (*in == find) {
            *in = replace;
        }
        in++;
    }
}
int string::toints(const std::string& string, int numbers[], int size, int def) {
    auto in  = string.c_str();
    auto end = (char*) nullptr;
    auto res = 0;
    errno = 0;
    for (int f = 0; f < size; f++) {
        numbers[f] = def;
    }
    for (int f = 0; f < size; f++) {
        auto n = strtol(in, &end, 10);
        if (errno != 0) {
            break;
        }
        else if (in == end) {
            break;
        }
        else {
            numbers[res++] = n;
        }
        in = end;
    }
    return res;
}
std::string string::tolower(std::string in) {
    std::transform(in.begin(), in.end(), in.begin(), [](unsigned char c) { return std::tolower(c); });
    return in;
}
int string::wordlist(const char* text, StringSet& words) {
    assert(text);
    auto time     = gnu::Time::Milli();
    auto tokens   = Token::MakeWord();
    auto len      = strlen(text);
    auto word_len = (size_t) 0;
    auto tmp      = StringHash();
    char word[limits::AUTOCOMPLETE_WORD_SIZE_MAX + 10];
    tmp.max_load_factor(0.8);
    if (len > 1'000'000) {
        tmp.reserve(50'000);
    }
    while (*text != 0) {
        auto c = (unsigned char) *text;
        auto t = tokens.get(c);
        if (t & Token::LETTER) {
            if (word_len < limits::AUTOCOMPLETE_WORD_SIZE_VAL) {
                word[word_len] = c;
                word_len++;
            }
        }
        else if (word_len > 1 && word_len < limits::AUTOCOMPLETE_WORD_SIZE_VAL) {
            word[word_len] = 0;
            tmp.insert(word);
            word_len = 0;
        }
        else {
            word_len = 0;
        }
        text++;
        if (tmp.size() > limits::AUTOCOMPLETE_LINES_VAL) {
            break;
        }
    }
    for (const auto& w : tmp) {
        words.insert(w);
    }
    time = gnu::Time::Milli() - time;
#ifdef DEBUG_EDITOR
    printf("string::wordlist(%d words from %d bytes) in %u mS\n", (int) words.size(), (int) len, (unsigned) time);
    fflush(stdout);
#endif
    return time;
}
Token::Token() {
    _char[(unsigned char) 0]    = Token::NIL;
    _char[(unsigned char) '\t'] = Token::SPACE;
    _char[(unsigned char) '\n'] = Token::NEWLINE;
    _char[(unsigned char) '\r'] = Token::NEWLINE;
    _char[(unsigned char) ' ']  = Token::SPACE;
    _char[(unsigned char) '!']  = Token::PUNCTUATOR;
    _char[(unsigned char) '"']  = Token::PUNCTUATOR;
    _char[(unsigned char) '#']  = Token::PUNCTUATOR;
    _char[(unsigned char) '$']  = Token::PUNCTUATOR;
    _char[(unsigned char) '%']  = Token::PUNCTUATOR;
    _char[(unsigned char) '&']  = Token::PUNCTUATOR;
    _char[(unsigned char) '\''] = Token::PUNCTUATOR;
    _char[(unsigned char) '(']  = Token::PUNCTUATOR;
    _char[(unsigned char) ')']  = Token::PUNCTUATOR;
    _char[(unsigned char) '*']  = Token::PUNCTUATOR;
    _char[(unsigned char) '+']  = Token::PUNCTUATOR;
    _char[(unsigned char) ',']  = Token::PUNCTUATOR;
    _char[(unsigned char) '-']  = Token::PUNCTUATOR;
    _char[(unsigned char) '.']  = Token::PUNCTUATOR;
    _char[(unsigned char) '/']  = Token::PUNCTUATOR;
    _char[(unsigned char) ':']  = Token::PUNCTUATOR;
    _char[(unsigned char) ';']  = Token::PUNCTUATOR;
    _char[(unsigned char) '<']  = Token::PUNCTUATOR;
    _char[(unsigned char) '=']  = Token::PUNCTUATOR;
    _char[(unsigned char) '>']  = Token::PUNCTUATOR;
    _char[(unsigned char) '?']  = Token::PUNCTUATOR;
    _char[(unsigned char) '@']  = Token::PUNCTUATOR;
    _char[(unsigned char) '[']  = Token::PUNCTUATOR;
    _char[(unsigned char) '\\'] = Token::PUNCTUATOR;
    _char[(unsigned char) ']']  = Token::PUNCTUATOR;
    _char[(unsigned char) '^']  = Token::PUNCTUATOR;
    _char[(unsigned char) '_']  = Token::PUNCTUATOR;
    _char[(unsigned char) '`']  = Token::PUNCTUATOR;
    _char[(unsigned char) '{']  = Token::PUNCTUATOR;
    _char[(unsigned char) '|']  = Token::PUNCTUATOR;
    _char[(unsigned char) '}']  = Token::PUNCTUATOR;
    _char[(unsigned char) '~']  = Token::PUNCTUATOR;
    _char[127]                  = Token::CTRL;
    _char[192]                  = Token::CTRL;
    _char[193]                  = Token::CTRL;
    set(1, 8, Token::CTRL);
    set(11, 12, Token::CTRL);
    set(14, 31, Token::CTRL);
    set('0', '9', Token::DECIMAL);
    set('A', 'Z', Token::LETTER);
    set('a', 'z', Token::LETTER);
    set(128, 191, Token::UTF2);
    set(194, 244, Token::UTF1);
    set(245, 255, Token::CTRL);
}
Token::Token(const Token& other) {
    for (int f = 0; f < 256; f++) {
        _char[f] = other._char[f];
    }
}
Token::Token(Token&& other) {
    for (int f = 0; f < 256; f++) {
        _char[f] = other._char[f];
    }
}
Token& Token::operator=(const Token& other) {
    for (int f = 0; f < 256; f++) {
        _char[f] = other._char[f];
    }
    return *this;
}
Token& Token::operator=(Token&& other) {
    for (int f = 0; f < 256; f++) {
        _char[f] = other._char[f];
    }
    return *this;
}
void Token::Debug(unsigned t) {
#ifdef DEBUG
    printf("Token(%u): ", (unsigned) t);
    if (t & Token::NIL)           printf("NIL, ");
    if (t & Token::CTRL)          printf("CTRL, ");
    if (t & Token::NEWLINE)       printf("NEWLINE, ");
    if (t & Token::SPACE)         printf("SPACE, ");
    if (t & Token::PUNCTUATOR)    printf("PUNCTUATOR, ");
    if (t & Token::DECIMAL)       printf("DECIMAL, ");
    if (t & Token::LETTER)        printf("LETTER, ");
    if (t & Token::IDENT1)        printf("IDENT1, ");
    if (t & Token::IDENT2)        printf("IDENT2, ");
    if (t & Token::HEX)           printf("HEX, ");
    if (t & Token::NUM_SEPARATOR) printf("NUM_SEPARATOR, ");
    if (t & Token::VALUE)         printf("VALUE, ");
    if (t & Token::PRAGMA)        printf("PRAGMA, ");
    if (t & Token::UTF1)          printf("UTF1, ");
    if (t & Token::UTF2)          printf("UTF2, ");
    printf("\n");
    fflush(stdout);
#else
    (void) t;
#endif
}
Token Token::MakeWord() {
    auto res = Token();
    res.set('_', Token::LETTER);
    res.set('0', '9', Token::LETTER);
    res.set(128, 191, Token::LETTER);
    res.set(194, 244, Token::LETTER);
    return res;
}
Token& Token::set(uint8_t index, uint16_t value) {
    _char[index] = value;
    return *this;
}
Token& Token::set(uint8_t index_from, uint8_t index_to, uint16_t value) {
    for (int f = index_from; f <= index_to; f++) {
        _char[f] = value;
    }
    return *this;
}
}
#define _STYLE_START_TAG(X) ((X >= 'a' && X <= 'z') || (X >= 'A' && X <= 'Z') || X == '_' || X > 128)
#define _STYLE_VALID_TAG(X) ((X >= 'a' && X <= 'z') || (X >= 'A' && X <= 'Z') || (X >= '0' && X <= '9') || X == '_' || X == '-' || X == '.' || X == ':' || X > 128)
#define _STYLE_BACKSLASH()\
_style->poke(start, style::STYLE_FG);\
c = _text->peek(++start);\
\
if (c != 0) {\
    _style->poke(start, style::STYLE_FG);\
}
#define _STYLE_CHAR()\
_style->poke(start, style::STYLE_STRING);\
if (_text->peek(start + 1) == ascii::BACKSLASH && _text->peek(start + 3) == ascii::SINGLE_QUOTE) {\
    _style->poke(start + 1, style::STYLE_STRING);\
    _style->poke(start + 2, style::STYLE_STRING);\
    _style->poke(start + 3, style::STYLE_STRING);\
    start += 3;\
}\
else if (_text->peek(start + 2) == ascii::SINGLE_QUOTE) {\
    _style->poke(start + 1, style::STYLE_STRING);\
    _style->poke(start + 2, style::STYLE_STRING);\
    start += 2;\
}
#define _STYLE_COMMENT_LINE()\
e = start + _line_comment_size;\
\
for (; e < last; e++) {\
    if (_text->peek(e) == ascii::NEWLINE) {\
        break;\
    }\
}\
\
_style->poke(start, e, style::STYLE_COMMENT);\
start = e;
#define _STYLE_COMMENT_BLOCK()\
e = start + _block_start_size;\
for (; e < last; e++) {\
    c = _text->peek(e);\
    if (c == block_end_c && _text->compare(e, _block_end, _block_end_size)) {\
        e += _block_end_size;\
        break;\
    }\
}\
_style->poke(start, e, style::STYLE_BLOCK_COMMENT);\
start = e - 1;
#define _STYLE_IDENT()\
auto e = start;\
auto l = 0;\
w[0] = c;\
l++;\
e++;\
for (; e < last; e++) {\
    c = _text->peek(e);\
    t = _tokens.get(c);\
\
    if ((t & Token::IDENT2) == 0) {\
        break;\
    }\
    else if (l > MAX_WORD) {\
        break;\
    }\
    else {\
        w[l] = c;\
        l++;\
    }\
}\
w[l] = 0;\
l = style::WORD_GROUP0;\
auto it = _lookup.find(w);\
\
if (it != _lookup.end()) {\
    l = it->second;\
}\
if (l & style::WORD_GROUP1) {\
    if (_pragma == true) {\
        _style->poke(start, e, style::STYLE_PRAGMA);\
        _pragma = false;\
    }\
    else {\
        _style->poke(start, e, style::STYLE_KEYWORD);\
    }\
}\
else if (l & style::WORD_GROUP2) {\
    _style->poke(start, e, style::STYLE_TYPE);\
}\
else if (l & style::WORD_GROUP4) {\
    _style->poke(start, e, style::STYLE_PRAGMA);\
    _pragma = false;\
}\
else if (l & style::WORD_GROUP8) {\
    _style->poke(start, e, style::STYLE_VAR);\
}\
else {\
    while (c <= ascii::SPACE && e < last) {\
        e++;\
        c = _text->peek(e);\
    }\
\
    if (c == ascii::OPEN_PARENT) {\
        _style->poke(start, e, style::STYLE_FUNCTION);\
    }\
    else {\
        _style->poke(start, e, style::STYLE_FG);\
    }\
}\
start = e - 1;
#define _STYLE_NUM()\
cn  = _text->peek(start + 1);\
hex = (cn == 'X' || cn == 'x');\
bin = (cn == 'b' || cn == 'B');\
oct = (cn == 'o' || cn == 'O');\
e   = start + 1 + hex + bin + oct;\
E = p = 0;\
_style->poke(start, style::STYLE_NUMBER);\
if ((hex && !_hex) || (bin && !_bin) || (oct && !_oct)) {\
}\
else {\
    st = style::STYLE_NUMBER;\
    _style->poke(start + hex + bin + oct, st);\
    \
    if (hex) {\
        for (; e < last; e++) {\
            c = _text->peek(e);\
            t = _tokens.get(c);\
            if ((t & Token::DECIMAL) == 0 && (t & Token::HEX) == 0 && (t & Token::NUM_SEPARATOR) == 0) break;\
            _style->poke(e, st);\
        }\
    }\
    else if (bin) {\
        for (; e < last; e++) {\
            c = _text->peek(e);\
            t = _tokens.get(c);\
            if ((t & Token::DECIMAL) == 0 && (t & Token::NUM_SEPARATOR) == 0) break;\
            else if (c != '0' && c != '1' && (t & Token::NUM_SEPARATOR) == 0) st = style::STYLE_FG;\
            _style->poke(e, st);\
        }\
    }\
    else if (oct) {\
        for (; e < last; e++) {\
            c = _text->peek(e);\
            t = _tokens.get(c);\
            if ((t & Token::DECIMAL) == 0 && (t & Token::NUM_SEPARATOR) == 0) break;\
            else if ((c < '0' || c > '7') && (t & Token::NUM_SEPARATOR) == 0) st = style::STYLE_FG;\
            _style->poke(e, st);\
        }\
    }\
    else {\
        for (; e < last; e++) {\
            c = _text->peek(e);\
            t = _tokens.get(c);\
            if (p && (c == 'E' || c == 'e')) st = style::STYLE_FG;\
            else if (c == 'E' || c == 'e') E++;\
            else if (c == '+' && E == 0) break;\
            else if (c == '+') p++;\
            else if ((t & Token::DECIMAL) == 0 && (t & Token::NUM_SEPARATOR) == 0) break;\
            if (p > 1 || E > 1) st = style::STYLE_FG;\
            _style->poke(e, st);\
        }\
    }\
    start = e - 1;\
}
#define _STYLE_PRAGMA()\
_style->poke(start, style::STYLE_PRAGMA);\
_pragma = true;
#define _STYLE_STRING()\
stop = c;\
e    = start + 1;\
p    = 0;\
st   = style::STYLE_STRING;\
for (; e < last; e++) {\
    c = _text->peek(e);\
\
    if (p == ascii::BACKSLASH && c == ascii::NEWLINE) {\
        st = style::STYLE_RAW_STRING;\
    }\
    else if (c == ascii::NEWLINE) {\
        break;\
    }\
    else if (c == stop && p != ascii::BACKSLASH) {\
        break;\
    }\
\
    if (c == ascii::BACKSLASH && p == ascii::BACKSLASH) {\
        p = 0;\
    }\
    else if (c == stop && p == ascii::BACKSLASH) {\
        p = 0;\
    }\
    else {\
        p = c;\
    }\
}\
_style->poke(start, e + 1, st);\
start = e;
#define _STYLE_STRING_RAW(RAW_START_SIZE, RAW_END_C, RAW_END, RAW_END_SIZE, RAW_ESCAPE)\
e = start + RAW_START_SIZE;\
for (; e < last; e++) {\
    p = c;\
    c = _text->peek(e);\
\
    if (p == ascii::BACKSLASH && c == RAW_END_C && RAW_ESCAPE == true) {\
    }\
    else if (c == RAW_END_C && _text->compare(e, RAW_END, RAW_END_SIZE)) {\
        e += RAW_END_SIZE;\
        break;\
    }\
}\
_style->poke(start, e, style::STYLE_RAW_STRING);\
start = e - 1;
#define _STYLE_VALUE()\
if (_text->peek(start + 1) == ascii::OPEN_PARENT) {\
    l = 1;\
    e = start + 2;\
    for (; e < last; e++) {\
        c = _text->peek(e);\
\
        if (c == ascii::OPEN_PARENT) {\
            l++;\
        }\
        else if (c == ascii::CLOSE_PARENT) {\
            l--;\
        }\
        else if (c == ascii::NEWLINE) {\
            e++;\
            break;\
        }\
\
        if (l == 0) {\
            e++;\
            break;\
        }\
    }\
    _style->poke(start, style::STYLE_FG);\
    _style->poke(start + 1, style::STYLE_FG);\
    _style->poke(start + 2, e - 1, style::STYLE_VAR);\
    _style->poke(e - 1, style::STYLE_FG);\
    start = e;\
}\
else {\
    e = start + 1;\
    for (; e < last; e++) {\
        c = _text->peek(e);\
        t = _tokens.get(c);\
\
        if ((t & Token::IDENT2) == 0) {\
            break;\
        }\
    }\
\
    _style->poke(start, e, style::STYLE_VAR);\
    start = e;\
}
namespace fle {
namespace style {
const char*     FILE_FILTER             = "All files (*)\t"
                                              "C/C++ files (*.{c,cpp,cxx,cc,h,hpp})\t"
                                              "Lua files (*.{lua,p8})\t"
                                              "Python files (*.{py})\t"
                                              "Ruby files (*.{rb})\t"
                                              "Java files (*.{java})\t"
                                              "Kotlin files (*.{kt})\t"
                                              "Go Files (*.{go})\t"
                                              "Rust files (*.{rs})\t"
                                              "C# files (*.{cs})\t"
                                              "Wren files (*.{wren})\t"
                                              "JavaScript files (*.{js,jsx,json})\t"
                                              "TypeScript files (*.{ts,tsx})\t"
                                              "PHP files (*{php})\t"
                                              "Markup files (*{xml,html})\t"
                                              "Shell scripts (*{sh,bash})\t"
                                              "Makefiles (*{makeinclude,Makefile,makefile,GNUmakefile,make})\t"
                                              "Batch scripts (*{bat,cmd})";
const char*     SCHEME_BLUE             = "Blue";
const char*     SCHEME_DARK             = "Dark";
const char*     SCHEME_DEF              = "Default";
const char*     SCHEME_LIGHT            = "Light";
const char*     SCHEME_NEON             = "Neon";
const char*     SCHEME_TAN              = "Tan";
const char*     BAT                     = "Batch script";
FTAB            BAT_TAB                 = FTAB::HARD;
unsigned        BAT_TAB_WIDTH           = 4;
const char*     CPP                     = "C && C++";
FTAB            CPP_TAB                 = FTAB::SOFT;
unsigned        CPP_TAB_WIDTH           = 4;
const char*     CS                      = "C#";
FTAB            CS_TAB                  = FTAB::SOFT;
unsigned        CS_TAB_WIDTH            = 4;
const char*     GO                      = "Go";
FTAB            GO_TAB                  = FTAB::SOFT;
unsigned        GO_TAB_WIDTH            = 4;
const char*     JAVA                    = "Java";
FTAB            JAVA_TAB                = FTAB::SOFT;
unsigned        JAVA_TAB_WIDTH          = 4;
const char*     JS                      = "JavaScript";
FTAB            JS_TAB                  = FTAB::SOFT;
unsigned        JS_TAB_WIDTH            = 4;
const char*     KOTLIN                  = "Kotlin";
FTAB            KOTLIN_TAB              = FTAB::SOFT;
unsigned        KOTLIN_TAB_WIDTH        = 4;
const char*     LUA                     = "Lua";
FTAB            LUA_TAB                 = FTAB::SOFT;
unsigned        LUA_TAB_WIDTH           = 4;
const char*     MAKEFILE                = "Makefile";
FTAB            MAKEFILE_TAB            = FTAB::HARD;
unsigned        MAKEFILE_TAB_WIDTH      = 4;
const char*     MARKUP                  = "Markup";
FTAB            MARKUP_TAB              = FTAB::SOFT;
unsigned        MARKUP_TAB_WIDTH        = 4;
const char*     PHP                     = "PHP";
FTAB            PHP_TAB                 = FTAB::SOFT;
unsigned        PHP_TAB_WIDTH           = 4;
const char*     PYTHON                  = "Python";
FTAB            PYTHON_TAB              = FTAB::SOFT;
unsigned        PYTHON_TAB_WIDTH        = 4;
const char*     RUBY                    = "Ruby";
FTAB            RUBY_TAB                = FTAB::SOFT;
unsigned        RUBY_TAB_WIDTH          = 4;
const char*     RUST                    = "Rust";
FTAB            RUST_TAB                = FTAB::SOFT;
unsigned        RUST_TAB_WIDTH          = 4;
const char*     SHELL                   = "Shell script";
FTAB            SHELL_TAB               = FTAB::SOFT;
unsigned        SHELL_TAB_WIDTH         = 4;
const char*     TEXT                    = "Text";
FTAB            TEXT_TAB                = FTAB::HARD;
unsigned        TEXT_TAB_WIDTH          = 4;
const char*     TS                      = "TypeScript";
FTAB            TS_TAB                  = FTAB::SOFT;
unsigned        TS_TAB_WIDTH            = 4;
const char*     WREN                    = "Wren";
FTAB            WREN_TAB                = FTAB::SOFT;
unsigned        WREN_TAB_WIDTH          = 4;
static StyleProperties _BLUE = {
    StyleProp(fl_rgb_color(0xa3, 0xa3, 0xa3)),
    StyleProp(fl_rgb_color(0x20, 0x28, 0x31)),
    StyleProp(fl_rgb_color(0xd6, 0xd6, 0xd6)),
    StyleProp(fl_rgb_color(0x6a, 0x7b, 0x83)),
    StyleProp(fl_rgb_color(0x30, 0x38, 0x41)),
    StyleProp(fl_rgb_color(0xff, 0x00, 0x00)),
    StyleProp(fl_rgb_color(0x58, 0x58, 0xbc)),
    StyleProp(fl_rgb_color(0x8a, 0x7e, 0x9f)),
    StyleProp(fl_rgb_color(0x95, 0x46, 0x46)),
    StyleProp(fl_rgb_color(0x8e, 0x8e, 0x00)),
    StyleProp(fl_rgb_color(0x28, 0x75, 0x83)),
    StyleProp(fl_rgb_color(0x88, 0x55, 0x00)),
    StyleProp(fl_rgb_color(0xa3, 0xa3, 0xa3)),
    StyleProp(fl_rgb_color(0x29, 0x63, 0x38)),
    StyleProp(fl_rgb_color(0x29, 0x63, 0x38)),
    StyleProp(fl_rgb_color(0x60, 0x60, 0x60)),
    StyleProp(fl_rgb_color(0x60, 0x60, 0x60)),
    StyleProp(fl_rgb_color(0xa3, 0xa3, 0xa3)),
};
static StyleProperties _DARK = {
    StyleProp(fl_rgb_color(0xa3, 0xa3, 0xa3)),
    StyleProp(fl_rgb_color(0x20, 0x20, 0x20)),
    StyleProp(fl_rgb_color(0xd6, 0xd6, 0xd6)),
    StyleProp(fl_rgb_color(0x68, 0x68, 0x68)),
    StyleProp(fl_rgb_color(0x30, 0x30, 0x30)),
    StyleProp(fl_rgb_color(0xff, 0x00, 0x00)),
    StyleProp(fl_rgb_color(0x58, 0x58, 0xbc)),
    StyleProp(fl_rgb_color(0x8a, 0x7e, 0x9f)),
    StyleProp(fl_rgb_color(0x95, 0x46, 0x46)),
    StyleProp(fl_rgb_color(0x8e, 0x8e, 0x00)),
    StyleProp(fl_rgb_color(0x28, 0x75, 0x83)),
    StyleProp(fl_rgb_color(0x88, 0x55, 0x00)),
    StyleProp(fl_rgb_color(0xa3, 0xa3, 0xa3)),
    StyleProp(fl_rgb_color(0x29, 0x63, 0x38)),
    StyleProp(fl_rgb_color(0x29, 0x63, 0x38)),
    StyleProp(fl_rgb_color(0x60, 0x60, 0x60)),
    StyleProp(fl_rgb_color(0x60, 0x60, 0x60)),
    StyleProp(fl_rgb_color(0xa3, 0xa3, 0xa3)),
};
static StyleProperties _DEF = {
    StyleProp(FL_FOREGROUND_COLOR),
    StyleProp(FL_BACKGROUND2_COLOR),
    StyleProp(FL_SELECTION_COLOR),
    StyleProp(FL_INACTIVE_COLOR),
    StyleProp(49),
    StyleProp(FL_FOREGROUND_COLOR),
    StyleProp(FL_FOREGROUND_COLOR),
    StyleProp(FL_FOREGROUND_COLOR),
    StyleProp(FL_FOREGROUND_COLOR),
    StyleProp(FL_FOREGROUND_COLOR),
    StyleProp(FL_FOREGROUND_COLOR),
    StyleProp(FL_FOREGROUND_COLOR),
    StyleProp(FL_FOREGROUND_COLOR),
    StyleProp(FL_FOREGROUND_COLOR),
    StyleProp(FL_FOREGROUND_COLOR),
    StyleProp(FL_FOREGROUND_COLOR),
    StyleProp(FL_FOREGROUND_COLOR),
    StyleProp(FL_FOREGROUND_COLOR),
};
static StyleProperties _LIGHT = {
    StyleProp(fl_rgb_color(0x38, 0x3a, 0x42)),
    StyleProp(fl_rgb_color(0xe6, 0xe6, 0xe6)),
    StyleProp(fl_rgb_color(0xc8, 0xc8, 0xc8)),
    StyleProp(fl_rgb_color(0x8e, 0x8e, 0x8e)),
    StyleProp(fl_rgb_color(0xd6, 0xd6, 0xd6)),
    StyleProp(fl_rgb_color(0xff, 0x00, 0x00)),
    StyleProp(fl_rgb_color(0x16, 0x16, 0xff)),
    StyleProp(fl_rgb_color(0xa2, 0x54, 0xd6)),
    StyleProp(fl_rgb_color(0xa0, 0x00, 0x00)),
    StyleProp(fl_rgb_color(0x7e, 0x7e, 0x00)),
    StyleProp(fl_rgb_color(0x37, 0x7d, 0xaa)),
    StyleProp(fl_rgb_color(0xed, 0xa0, 0x5c)),
    StyleProp(fl_rgb_color(0x38, 0x3a, 0x42)),
    StyleProp(fl_rgb_color(0x16, 0x8e, 0x16)),
    StyleProp(fl_rgb_color(0x00, 0x60, 0x00)),
    StyleProp(fl_rgb_color(0xb0, 0xb0, 0xb0)),
    StyleProp(fl_rgb_color(0xb0, 0xb0, 0xb0)),
    StyleProp(fl_rgb_color(0x38, 0x3a, 0x42)),
};
static StyleProperties _NEON = {
    StyleProp(fl_rgb_color(0xff, 0xff, 0xff)),
    StyleProp(fl_rgb_color(0x00, 0x00, 0x00)),
    StyleProp(fl_rgb_color(0xe6, 0xe6, 0x00)),
    StyleProp(fl_rgb_color(0x95, 0x95, 0x95)),
    StyleProp(fl_rgb_color(0x15, 0x15, 0x15)),
    StyleProp(fl_rgb_color(0xff, 0x00, 0x00)),
    StyleProp(fl_rgb_color(0x40, 0x40, 0xff)),
    StyleProp(fl_rgb_color(0x9f, 0x00, 0x9f)),
    StyleProp(fl_rgb_color(0xaa, 0x00, 0x00)),
    StyleProp(fl_rgb_color(0xaa, 0xaa, 0x00)),
    StyleProp(fl_rgb_color(0x42, 0xc1, 0xd8)),
    StyleProp(fl_rgb_color(0xff, 0x9f, 0x00)),
    StyleProp(fl_rgb_color(0xff, 0xff, 0xff)),
    StyleProp(fl_rgb_color(0x00, 0xaa, 0x00)),
    StyleProp(fl_rgb_color(0x00, 0xaa, 0x00)),
    StyleProp(fl_rgb_color(0x85, 0x85, 0x85)),
    StyleProp(fl_rgb_color(0x85, 0x85, 0x85)),
    StyleProp(fl_rgb_color(0xff, 0xff, 0xff)),
};
static StyleProperties _TAN = {
    StyleProp(fl_rgb_color(0x58, 0x6e, 0x75)),
    StyleProp(fl_rgb_color(0xfd, 0xf7, 0xe2)),
    StyleProp(fl_rgb_color(0xc8, 0xc8, 0xc8)),
    StyleProp(fl_rgb_color(0x83, 0x94, 0x96)),
    StyleProp(fl_rgb_color(0xed, 0xe7, 0xd2)),
    StyleProp(fl_rgb_color(0xff, 0x00, 0x00)),
    StyleProp(fl_rgb_color(0x66, 0x79, 0xff)),
    StyleProp(fl_rgb_color(0xba, 0x8f, 0x73)),
    StyleProp(fl_rgb_color(0x81, 0x25, 0x25)),
    StyleProp(fl_rgb_color(0x18, 0x7a, 0x76)),
    StyleProp(fl_rgb_color(0x66, 0x66, 0x00)),
    StyleProp(fl_rgb_color(0xb2, 0x47, 0xb8)),
    StyleProp(fl_rgb_color(0x58, 0x6e, 0x75)),
    StyleProp(fl_rgb_color(0x16, 0x8e, 0x16)),
    StyleProp(fl_rgb_color(0x0a, 0x65, 0x2a)),
    StyleProp(fl_rgb_color(0xa0, 0xa0, 0xa0)),
    StyleProp(fl_rgb_color(0xa0, 0xa0, 0xa0)),
    StyleProp(fl_rgb_color(0x58, 0x6e, 0x75)),
};
static Style_Table_Entry _SCHEME[(int) style::STYLE_SIZE] = {
};
StyleProp* get_style_prop(std::string scheme, style::STYLE style) {
    assert(style < style::STYLE_SIZE);
    if (scheme == style::SCHEME_BLUE) {
        return &style::_BLUE[style];
    }
    else if (scheme == style::SCHEME_DARK) {
        return &style::_DARK[style];
    }
    else if (scheme == style::SCHEME_LIGHT) {
        return &style::_LIGHT[style];
    }
    else if (scheme == style::SCHEME_NEON) {
        return &style::_NEON[style];
    }
    else if (scheme == style::SCHEME_TAN) {
        return &style::_TAN[style];
    }
    else {
        return &style::_DEF[style];
    }
}
FTAB get_tab_type(std::string name) {
    if (name == style::BAT) {
        return style::BAT_TAB;
    }
    else if (name == style::CPP) {
        return style::CPP_TAB;
    }
    else if (name == style::CS) {
        return style::CS_TAB;
    }
    else if (name == style::GO) {
        return style::GO_TAB;
    }
    else if (name == style::JAVA) {
        return style::JAVA_TAB;
    }
    else if (name == style::JS) {
        return style::JS_TAB;
    }
    else if (name == style::KOTLIN) {
        return style::KOTLIN_TAB;
    }
    else if (name == style::LUA) {
        return style::LUA_TAB;
    }
    else if (name == style::MAKEFILE) {
        return style::MAKEFILE_TAB;
    }
    else if (name == style::MARKUP) {
        return style::MARKUP_TAB;
    }
    else if (name == style::PHP) {
        return style::PHP_TAB;
    }
    else if (name == style::PYTHON) {
        return style::PYTHON_TAB;
    }
    else if (name == style::RUBY) {
        return style::RUBY_TAB;
    }
    else if (name == style::RUST) {
        return style::RUST_TAB;
    }
    else if (name == style::SHELL) {
        return style::SHELL_TAB;
    }
    else if (name == style::TS) {
        return style::TS_TAB;
    }
    else if (name == style::WREN) {
        return style::WREN_TAB;
    }
    else {
        return style::TEXT_TAB;
    }
}
unsigned get_tab_width(std::string name) {
    unsigned res = 4;
    if (name == style::BAT) {
        res = style::BAT_TAB_WIDTH;
    }
    else if (name == style::CPP) {
        res = style::CPP_TAB_WIDTH;
    }
    else if (name == style::CS) {
        res = style::CS_TAB_WIDTH;
    }
    else if (name == style::GO) {
        res = style::GO_TAB_WIDTH;
    }
    else if (name == style::JAVA) {
        res = style::JAVA_TAB_WIDTH;
    }
    else if (name == style::JS) {
        res = style::JS_TAB_WIDTH;
    }
    else if (name == style::KOTLIN) {
        res = style::KOTLIN_TAB_WIDTH;
    }
    else if (name == style::LUA) {
        res = style::LUA_TAB_WIDTH;
    }
    else if (name == style::MAKEFILE) {
        res = style::MAKEFILE_TAB_WIDTH;
    }
    else if (name == style::MARKUP) {
        res = style::MARKUP_TAB_WIDTH;
    }
    else if (name == style::PHP) {
        res = style::PHP_TAB_WIDTH;
    }
    else if (name == style::PYTHON) {
        res = style::PYTHON_TAB_WIDTH;
    }
    else if (name == style::RUBY) {
        res = style::RUBY_TAB_WIDTH;
    }
    else if (name == style::RUST) {
        res = style::RUST_TAB_WIDTH;
    }
    else if (name == style::SHELL) {
        res = style::SHELL_TAB_WIDTH;
    }
    else if (name == style::TS) {
        res = style::TS_TAB_WIDTH;
    }
    else if (name == style::WREN) {
        res = style::WREN_TAB_WIDTH;
    }
    else {
        res = style::TEXT_TAB_WIDTH;
    }
    if (res > limits::TAB_WIDTH_MAX) {
        res = 4;
    }
    return res;
}
const Fl_Text_Display::Style_Table_Entry* get_table(std::string scheme, Fl_Fontsize fs) {
    StyleProp* text;
    for (int f = 0; f < (int) style::STYLE_SIZE; f++) {
        auto prop = style::get_style_prop(scheme, static_cast<style::STYLE>(f));
        if (f == 0) {
            text = prop;
        }
        if (prop->attr() == Fl_Text_Display::ATTR_BGCOLOR || prop->attr() == Fl_Text_Display::ATTR_BGCOLOR_EXT) {
            _SCHEME[f].color   = text->color();
            _SCHEME[f].font    = flw::PREF_FIXED_FONT + prop->font();
            _SCHEME[f].size    = fs;
            _SCHEME[f].attr    = prop->attr();
            _SCHEME[f].bgcolor = prop->color();
        }
        else {
            _SCHEME[f].color   = prop->color();
            _SCHEME[f].font    = flw::PREF_FIXED_FONT + prop->font();
            _SCHEME[f].size    = fs;
            _SCHEME[f].attr    = prop->attr();
            _SCHEME[f].bgcolor = 0;
        }
    }
    return _SCHEME;
}
static void _load_pref(Fl_Preferences& preferences, std::string name, StyleProperties& props) {
    for (int f = 0; f <= style::STYLE_LAST; f++) {
        auto prop = &props[f];
        auto val  = 0;
        if (preferences.get(gnu::str::format("%s_%02d_color", name.c_str(), f).c_str(), val, 0) == 1) {
            prop->color_u = val;
        }
        if (preferences.get(gnu::str::format("%s_%02d_italic", name.c_str(), f).c_str(), val, 0) == 1) {
            prop->italic_u = val;
        }
        if (preferences.get(gnu::str::format("%s_%02d_bold", name.c_str(), f).c_str(), val, 0) == 1) {
            prop->bold_u = val;
        }
        if (preferences.get(gnu::str::format("%s_%02d_attr", name.c_str(), f).c_str(), val, 0) == 1) {
            prop->attr_u = val;
        }
    }
}
void load_pref(Fl_Preferences& preferences) {
    _load_pref(preferences, "blue", style::_BLUE);
    _load_pref(preferences, "dark", style::_DARK);
    _load_pref(preferences, "def", style::_DEF);
    _load_pref(preferences, "light", style::_LIGHT);
    _load_pref(preferences, "neon", style::_NEON);
    _load_pref(preferences, "tan", style::_TAN);
}
Style* make_from_file(const gnu::File& file) {
    auto ext = string::tolower(file.ext);
    if (ext == "bat" || ext == "cmd") {
        return new StyleBat();
    }
    else if (ext == "cs") {
        return new StyleCS();
    }
    else if (ext == "c" || ext == "cpp" || ext == "cc" || ext == "cxx" || ext == "h" || ext == "H" || ext == "hpp") {
        return new StyleCpp();
    }
    else if (ext == "go") {
        return new StyleGo();
    }
    else if (ext == "java") {
        return new StyleJava();
    }
    else if (ext == "js" || ext == "jsx" || ext == "json") {
        return new StyleJS();
    }
    else if (ext == "kt") {
        return new StyleKotlin();
    }
    else if (ext == "lua" || ext == "p8") {
        return new StyleLua();
    }
    else if (file.name == "Makefile" || file.name == "makefile" || file.name == "GNUmakefile" || file.name == "makeinclude" || file.name.find(".makefile") != std::string::npos) {
        return new StyleMakefile();
    }
    else if (ext == "html" || ext == "htm" || ext == "xml") {
        return new StyleMarkup();
    }
    else if (ext == "php") {
        return new StylePHP();
    }
    else if (ext == "py") {
        return new StylePython();
    }
    else if (ext == "rb") {
        return new StyleRuby();
    }
    else if (ext == "rs") {
        return new StyleRust();
    }
    else if (ext == "sh" || ext == "bash") {
        return new StyleShell();
    }
    else if (ext == "ts" || ext == "tsx") {
        return new StyleTS();
    }
    else if (ext == "wren") {
        return new StyleWren();
    }
    else {
        return new Style();
    }
}
Style* make_from_name(std::string name) {
    if (name == style::BAT) {
        return new StyleBat();
    }
    else if (name == style::CPP) {
        return new StyleCpp();
    }
    else if (name == style::CS) {
        return new StyleCS();
    }
    else if (name == style::GO) {
        return new StyleGo();
    }
    else if (name == style::JAVA) {
        return new StyleJava();
    }
    else if (name == style::JS) {
        return new StyleJS();
    }
    else if (name == style::KOTLIN) {
        return new StyleKotlin();
    }
    else if (name == style::LUA) {
        return new StyleLua();
    }
    else if (name == style::MAKEFILE) {
        return new StyleMakefile();
    }
    else if (name == style::MARKUP) {
        return new StyleMarkup();
    }
    else if (name == style::PYTHON) {
        return new StylePython();
    }
    else if (name == style::PHP) {
        return new StylePHP();
    }
    else if (name == style::RUBY) {
        return new StyleRuby();
    }
    else if (name == style::RUST) {
        return new StyleRust();
    }
    else if (name == style::SHELL) {
        return new StyleShell();
    }
    else if (name == style::TS) {
        return new StyleTS();
    }
    else if (name == style::WREN) {
        return new StyleWren();
    }
    else {
        return new Style();
    }
}
void reset_all_styles() {
    style::reset_style(style::SCHEME_BLUE);
    style::reset_style(style::SCHEME_DARK);
    style::reset_style(style::SCHEME_DEF);
    style::reset_style(style::SCHEME_LIGHT);
    style::reset_style(style::SCHEME_NEON);
    style::reset_style(style::SCHEME_TAN);
}
void reset_style(std::string scheme) {
    for (int f = 0; f <= style::STYLE_LAST; f++) {
        auto prop = get_style_prop(scheme, static_cast<style::STYLE>(f));
        prop->reset();
    }
}
static void _save_pref(Fl_Preferences& preferences, std::string name, const StyleProperties& props) {
    for (int f = 0; f <= style::STYLE_LAST; f++) {
        auto prop = props[f];
        if (prop.color_u != prop.color_d) {
            preferences.set(gnu::str::format("%s_%02d_color", name.c_str(), f).c_str(), (int) prop.color_u);
        }
        if (prop.italic_u != prop.italic_d) {
            preferences.set(gnu::str::format("%s_%02d_italic", name.c_str(), f).c_str(), (int) prop.italic_u);
        }
        if (prop.bold_u != prop.bold_d) {
            preferences.set(gnu::str::format("%s_%02d_bold", name.c_str(), f).c_str(), (int) prop.bold_u);
        }
        if (prop.attr_u != prop.attr_d) {
            preferences.set(gnu::str::format("%s_%02d_attr", name.c_str(), f).c_str(), (int) prop.attr_u);
        }
    }
}
void save_pref(Fl_Preferences& preferences) {
    _save_pref(preferences, "blue", style::_BLUE);
    _save_pref(preferences, "dark", style::_DARK);
    _save_pref(preferences, "def", style::_DEF);
    _save_pref(preferences, "light", style::_LIGHT);
    _save_pref(preferences, "neon", style::_NEON);
    _save_pref(preferences, "tan", style::_TAN);
}
void set_tab_type(std::string name, FTAB tab) {
    if (name == style::BAT) {
        style::BAT_TAB = tab;
    }
    else if (name == style::CPP) {
        style::CPP_TAB = tab;
    }
    else if (name == style::CS) {
        style::CS_TAB = tab;
    }
    else if (name == style::GO) {
        style::GO_TAB = tab;
    }
    else if (name == style::JAVA) {
        style::JAVA_TAB = tab;
    }
    else if (name == style::JS) {
        style::JS_TAB = tab;
    }
    else if (name == style::KOTLIN) {
        style::KOTLIN_TAB = tab;
    }
    else if (name == style::LUA) {
        style::LUA_TAB = tab;
    }
    else if (name == style::MAKEFILE) {
        style::MAKEFILE_TAB = tab;
    }
    else if (name == style::MARKUP) {
        style::MARKUP_TAB = tab;
    }
    else if (name == style::PYTHON) {
        style::PYTHON_TAB = tab;
    }
    else if (name == style::RUBY) {
        style::RUBY_TAB = tab;
    }
    else if (name == style::RUST) {
        style::RUST_TAB = tab;
    }
    else if (name == style::SHELL) {
        style::SHELL_TAB = tab;
    }
    else if (name == style::TS) {
        style::TS_TAB = tab;
    }
    else if (name == style::WREN) {
        style::WREN_TAB = tab;
    }
    else {
        style::TEXT_TAB = tab;
    }
}
void set_tab_width(std::string name, unsigned width) {
    if (width == 0 || width > limits::TAB_WIDTH_MAX) {
        return;
    }
    else if (name == style::BAT) {
        style::BAT_TAB_WIDTH = width;
    }
    else if (name == style::CPP) {
        style::CPP_TAB_WIDTH = width;
    }
    else if (name == style::CS) {
        style::CS_TAB_WIDTH = width;
    }
    else if (name == style::GO) {
        style::GO_TAB_WIDTH = width;
    }
    else if (name == style::JAVA) {
        style::JAVA_TAB_WIDTH = width;
    }
    else if (name == style::JS) {
        style::JS_TAB_WIDTH = width;
    }
    else if (name == style::KOTLIN) {
        style::KOTLIN_TAB_WIDTH = width;
    }
    else if (name == style::LUA) {
        style::LUA_TAB_WIDTH = width;
    }
    else if (name == style::MAKEFILE) {
        style::MAKEFILE_TAB_WIDTH = width;
    }
    else if (name == style::MARKUP) {
        style::MARKUP_TAB_WIDTH = width;
    }
    else if (name == style::PYTHON) {
        style::PYTHON_TAB_WIDTH = width;
    }
    else if (name == style::RUBY) {
        style::RUBY_TAB_WIDTH = width;
    }
    else if (name == style::RUST) {
        style::RUST_TAB_WIDTH = width;
    }
    else if (name == style::SHELL) {
        style::SHELL_TAB_WIDTH = width;
    }
    else if (name == style::TS) {
        style::TS_TAB_WIDTH = width;
    }
    else if (name == style::WREN) {
        style::WREN_TAB_WIDTH = width;
    }
    else {
        style::TEXT_TAB_WIDTH = width;
    }
}
}
Style::Style(std::string name) : _name(name) {
    _bin               = false;
    _block_end         = "";
    _block_end_size    = 0;
    _block_start       = "";
    _block_start_size  = 0;
    _hex               = false;
    _line_comment      = "";
    _line_comment_size = 0;
    _oct               = false;
    _pause             = false;
    _pragma            = false;
    _single_quote_str  = 0;
    _style             = nullptr;
    _text              = nullptr;
    for (size_t f = 0; f < Style::MAX_RAW; f++) {
        _raw_start[f]      = "";
        _raw_start_size[f] = 0;
        _raw_end[f]        = "";
        _raw_end_size[f]   = 0;
        _raw_escape[f]     = false;
    }
}
Style::~Style() {
}
void Style::debug() const {
    printf("\nStyle(%s)\n", _name.c_str());
    printf("    _lookup             = %d\n", (int) _lookup.size());
    printf("    _words              = %d\n", (int) _words.size());
    printf("    _custom             = %d\n", (int) _custom.size());
    printf("    _block_start        = %s\n", _block_start);
    printf("    _block_start_size   = %d\n", _block_start_size);
    printf("    _block_end          = %s\n", _block_end);
    printf("    _block_end_size     = %d\n", _block_end_size);
    printf("    _line_comment       = %s\n", _line_comment);
    printf("    _line_comment_size  = %d\n", _line_comment_size);
    printf("    _bin                = %s\n", _bin ? "TRUE" : "FALSE");
    printf("    _hex                = %s\n", _hex ? "TRUE" : "FALSE");
    printf("    _oct                = %s\n", _oct ? "TRUE" : "FALSE");
    printf("    _pragma             = %s\n", _pragma ? "TRUE" : "FALSE");
    printf("\n");
    for (int f = 0; f < (int) Style::MAX_RAW; f++) {
        if (*_raw_start[f] == 0) {
            break;
        }
        printf("    raw(%d)\n", f);
        printf("        _raw_start      = %s\n", _raw_start[f]);
        printf("        _raw_start_size = %d\n", _raw_start_size[f]);
        printf("        _raw_end        = %s\n", _raw_end[f]);
        printf("        _raw_end_size   = %d\n", _raw_end_size[f]);
        printf("        _raw_escape     = %d\n", _raw_escape[f]);
        printf("\n");
    }
    for (int f = 0; f < 256; f++) {
        if (_tokens[f] & Token::NUM_SEPARATOR) printf("    NUM_SEPARATOR[%03d]  = %c\n", f, f);
        if (_tokens[f] & Token::PRAGMA)        printf("    PRAGMA[%03d]         = %c\n", f, f);
    }
    fflush(stdout);
}
bool Style::insert_word(std::string word, int word_type) {
    auto res = _lookup.insert({word, word_type});
#ifdef DEBUG
    if (res.second == false) {
        auto it = _lookup.find(word);
        printf("error: inserting %s with type %d failed (found type %d)\n", word.c_str(), word_type, it->second);
        fflush(stdout);
    }
#endif
    return res.second;
}
void Style::make_words() {
    _words.clear();
    for (const auto& it : _lookup) {
        if (it.first.length() > 1) {
            _words.insert(it.first);
        }
    }
    for (const auto& it : _custom) {
        _words.insert(it);
    }
}
FTAB Style::tab_mode() const {
    return style::get_tab_type(_name);
}
void Style::tab_mode(FTAB tab) {
    style::set_tab_type(_name, tab);
}
unsigned Style::tab_width() const {
    return style::get_tab_width(_name);
}
void Style::tab_width(unsigned width) {
    style::set_tab_width(_name, width);
}
int Style::update() {
    Fl::redraw();
    return update(0, _text->length(), 0, nullptr, nullptr, nullptr);
}
int Style::update(int, int, int, const char*, const char*, Editor*) {
    return 0;
}
StyleDef::StyleDef(std::string name) : Style(name) {
    _tokens.set('0', '9', Token::DECIMAL | Token::IDENT2);
    _tokens.set('A', 'F', Token::LETTER | Token::IDENT2 | Token::IDENT1 | Token::HEX);
    _tokens.set('G', 'Z', Token::LETTER | Token::IDENT2 | Token::IDENT1);
    _tokens.set('_', Token::IDENT2 | Token::IDENT1);
    _tokens.set('.', Token::PUNCTUATOR | Token::NUM_SEPARATOR);
    _tokens.set('a', 'f', Token::LETTER | Token::IDENT2 | Token::IDENT1 | Token::HEX);
    _tokens.set('g', 'z', Token::LETTER | Token::IDENT2 | Token::IDENT1);
}
int StyleDef::_expand_left(int& start) {
    auto time = gnu::Time::Milli();
    auto s    = (unsigned char) 0;
    auto f    = _text->line_start(start);
    while (f >= 0) {
        s = _style->peek(f);
        if (s != style::STYLE_INIT) {
            start = f + 1;
            break;
        }
        else {
            f--;
        }
    }
    if (s == style::STYLE_BLOCK_COMMENT || s == style::STYLE_RAW_STRING) {
        while (f >= 0) {
            s = _style->peek(f);
            if (s != style::STYLE_BLOCK_COMMENT && s != style::STYLE_RAW_STRING) {
                start = f + 1;
                break;
            }
            else if (f == 1) {
                start = 0;
                break;
            }
            else {
                start = f;
                f--;
            }
        }
    }
    else {
        while (f >= 0) {
            s = _style->peek(f);
            if (s == style::STYLE_BLOCK_COMMENT || s == style::STYLE_RAW_STRING) {
                start = f + 1;
                break;
            }
            else if (_text->peek(f) == '\n') {
                start = f + 1;
                break;
            }
            else if (f == 1) {
                start = 0;
                break;
            }
            else {
                start = f;
                f--;
            }
        }
    }
    time = gnu::Time::Milli() - time;
    return time;
}
int StyleDef::_expand_right(int& end) {
    auto time = gnu::Time::Milli();
    auto f    = _text->line_end(end);
    auto s    = (unsigned char) 0;
    auto last = _text->length();
    while (f < last) {
        s = _style->peek(f);
        if (s != style::STYLE_INIT) {
            end = f - 1;
            break;
        }
        else {
            f++;
        }
    }
    if (s == style::STYLE_BLOCK_COMMENT || s == style::STYLE_RAW_STRING) {
        while (f < last) {
            s = _style->peek(f);
            if (s != style::STYLE_BLOCK_COMMENT && s != style::STYLE_RAW_STRING) {
                end = f;
                break;
            }
            else if (f == last - 1) {
                end = last;
                break;
            }
            else {
                end = f;
                f++;
            }
        }
    }
    else {
        while (f < last) {
            s = _style->peek(f);
            if (s == style::STYLE_BLOCK_COMMENT || s == style::STYLE_RAW_STRING) {
                while (f < last) {
                    f++;
                    s = _style->peek(f);
                    if (s != style::STYLE_BLOCK_COMMENT && s != style::STYLE_RAW_STRING) {
                        break;
                    }
                }
                end = f;
                break;
            }
            else if (_text->peek(f) == '\n') {
                end = f;
                break;
            }
            else if (f == last - 1) {
                end = last;
                break;
            }
            else {
                end = f;
                f++;
            }
        }
    }
    time = gnu::Time::Milli() - time;
    return time;
}
int StyleDef::update(int pos, int inserted_size, int deleted_size, const char*, const char*, Editor* editor) {
    assert(pos >= 0);
    if (_pause == true || pos < 0) {
        return 0;
    }
    auto time  = gnu::Time::Milli();
    auto start = (pos > 0) ? pos - 1 : 0;
    auto end   = pos + ((inserted_size > deleted_size) ? inserted_size : deleted_size);
    if (limits::FORCE_RESTYLING != 0) {
        start = 0;
        end   = _text->length();
    }
#ifdef DEBUG_STYLE
    printf("\nSTYLE_START: pos=%4d, end=%4d, length=%4d\n", pos, end, _text->length());
#endif
    if (start > 0 || end != _text->length()) {
#ifdef DEBUG_STYLE
        auto left_ms  = _expand_left(start);
        auto right_ms = _expand_right(end);
        printf("    start = %7d, time = %4d mS\n", start, left_ms);
        printf("    end   = %7d, time = %4d mS\n", end, right_ms);
#else
        _expand_left(start);
        _expand_right(end);
#endif
    }
    end = _update(start, end);
    if (end < _text->length() && _text->peek(end) != '\n') {
    #ifdef DEBUG_STYLE
        printf("    start= %7d, end = %7d, char = %c, new_end = %4d\n", start, end, _text->peek(end), _text->line_end(end));
    #endif
        auto start2 = end;
        end         = _text->line_end(end);
        end         = _update(start2, end);
    }
    time = gnu::Time::Milli() - time;
#ifdef DEBUG_STYLE
    printf("STYLE_END: %s(%d => %d) in %d mS\n\n", _name.c_str(), start, end, (int) time);
    fflush(stdout);
#endif
    if (editor != nullptr) {
        editor->view1()->redisplay_range(start, end + 100);
        if (editor->view2() != nullptr) {
            editor->view2()->redisplay_range(start, end + 100);
        }
    }
    return (int) time;
}
int StyleDef::_update(int start, int end) {
    auto bin            = false;
    auto block_end_c    = (unsigned) *_block_end;
    auto block_start_c  = (unsigned) *_block_start;
    auto c              = (unsigned) 0;
    auto cn             = 0;
    auto e              = 0;
    auto E              = 0;
    auto hex            = false;
    auto l              = 0;
    auto last           = _text->length();
    auto line_comment_c = (unsigned) *_line_comment;
    auto oct            = false;
    auto p              = (unsigned) 0;
    auto raw_end_c      = (unsigned) *_raw_end[0];
    auto raw_end_c1     = (unsigned) *_raw_end[1];
    auto raw_end_c2     = (unsigned) *_raw_end[2];
    auto raw_end_c3     = (unsigned) *_raw_end[3];
    auto raw_start_c    = (unsigned) *_raw_start[0];
    auto raw_start_c1   = (unsigned) *_raw_start[1];
    auto raw_start_c2   = (unsigned) *_raw_start[2];
    auto raw_start_c3   = (unsigned) *_raw_start[3];
    auto st             = style::STYLE_FG;
    auto stop           = (unsigned) 0;
    auto t              = (unsigned) 0;
    auto MAX_WORD       = 100;
    char w[110];
    while (start < end) {
        p = c;
        c = static_cast<unsigned>(_text->peek(start));
        t = static_cast<unsigned>(_tokens.get(c));
        if (c == '\n') {
            _pragma = false;
        }
        if (c == 0) {
            break;
        }
        else if (c <= ' ') {
            _style->poke(start, style::STYLE_FG);
        }
        else if (c == raw_start_c && _text->compare(start, _raw_start[0], _raw_start_size[0])) {
            _STYLE_STRING_RAW(_raw_start_size[0], raw_end_c, _raw_end[0], _raw_end_size[0], _raw_escape[0])
        }
        else if (c == raw_start_c1 && _text->compare(start, _raw_start[1], _raw_start_size[1])) {
            _STYLE_STRING_RAW(_raw_start_size[1], raw_end_c1, _raw_end[1], _raw_end_size[1], _raw_escape[1])
        }
        else if (c == raw_start_c2 && _text->compare(start, _raw_start[2], _raw_start_size[2])) {
            _STYLE_STRING_RAW(_raw_start_size[2], raw_end_c2, _raw_end[2], _raw_end_size[2], _raw_escape[2])
        }
        else if (c == raw_start_c3 && _text->compare(start, _raw_start[3], _raw_start_size[3])) {
            _STYLE_STRING_RAW(_raw_start_size[3], raw_end_c3, _raw_end[3], _raw_end_size[3], _raw_escape[3])
        }
        else if (c == block_start_c && _text->compare(start, _block_start, _block_start_size)) {
            _STYLE_COMMENT_BLOCK()
        }
        else if (c == '\\') {
            _STYLE_BACKSLASH()
        }
        else if (c == '\"') {
            _STYLE_STRING()
        }
        else if (c == '\'' && c == _single_quote_str) {
            _STYLE_STRING()
        }
        else if (c == '\'') {
            _STYLE_CHAR()
        }
        else if (c == line_comment_c && _text->compare(start, _line_comment, _line_comment_size)) {
            _STYLE_COMMENT_LINE()
        }
        else if (t & Token::PRAGMA) {
            _STYLE_PRAGMA()
        }
        else if (t & Token::IDENT1) {
            _STYLE_IDENT()
        }
        else if (t & Token::DECIMAL) {
            _STYLE_NUM()
        }
        else if (t & Token::VALUE) {
            _STYLE_VALUE()
        }
        else if (t & Token::PUNCTUATOR) {
            _style->poke(start, style::STYLE_PUNCTUATOR);
        }
        else {
            _style->poke(start, style::STYLE_FG);
        }
        start++;
    }
    return start;
}
StyleProp::StyleProp() {
    attr_d   = attr_u   = Fl_Text_Display::ATTR_UNDERLINE;
    color_d  = color_u  = 0;
    bold_d   = bold_u   = false;
    italic_d = italic_u = false;
}
StyleProp::StyleProp(Fl_Color color, unsigned attr, bool bold, bool italic) {
    attr_d   = attr_u   = attr;
    color_d  = color_u  = color;
    bold_d   = bold_u   = bold;
    italic_d = italic_u = italic;
}
int StyleProp::font() const {
    if (bold_u != bold_d && italic_u != italic_d) {
        return 3;
    }
    else if (italic_u != italic_d) {
        return 2;
    }
    else if (bold_u != bold_d) {
        return 1;
    }
    return 0;
}
void StyleProp::reset() {
    attr_u   = attr_d;
    bold_u   = bold_d;
    color_u  = color_d;
    italic_u = italic_d;
}
}
namespace fle {
StyleBat::StyleBat() : StyleDef(style::BAT) {
    _line_comment      = "rem ";
    _line_comment_size = 4;
    insert_word("AUX", style::WORD_GROUP1);
    insert_word("BREAK", style::WORD_GROUP1);
    insert_word("CALL", style::WORD_GROUP1);
    insert_word("CASE", style::WORD_GROUP1);
    insert_word("CLS", style::WORD_GROUP1);
    insert_word("COM1", style::WORD_GROUP1);
    insert_word("COM2", style::WORD_GROUP1);
    insert_word("COM3", style::WORD_GROUP1);
    insert_word("COM4", style::WORD_GROUP1);
    insert_word("CON", style::WORD_GROUP1);
    insert_word("COPY", style::WORD_GROUP1);
    insert_word("DEFINED", style::WORD_GROUP1);
    insert_word("DEL", style::WORD_GROUP1);
    insert_word("DO", style::WORD_GROUP1);
    insert_word("DONE", style::WORD_GROUP1);
    insert_word("ECHO", style::WORD_GROUP1);
    insert_word("ELIF", style::WORD_GROUP1);
    insert_word("ELSE", style::WORD_GROUP1);
    insert_word("EQU", style::WORD_GROUP1);
    insert_word("ERRORLEVEL", style::WORD_GROUP1);
    insert_word("ESAC", style::WORD_GROUP1);
    insert_word("EXIST", style::WORD_GROUP1);
    insert_word("EXIT", style::WORD_GROUP1);
    insert_word("FI", style::WORD_GROUP1);
    insert_word("FOR", style::WORD_GROUP1);
    insert_word("FUNCTION", style::WORD_GROUP1);
    insert_word("GEQ", style::WORD_GROUP1);
    insert_word("GOTO", style::WORD_GROUP1);
    insert_word("GTR", style::WORD_GROUP1);
    insert_word("IF", style::WORD_GROUP1);
    insert_word("IN", style::WORD_GROUP1);
    insert_word("LEQ", style::WORD_GROUP1);
    insert_word("LOCAL", style::WORD_GROUP1);
    insert_word("LPT1", style::WORD_GROUP1);
    insert_word("LPT2", style::WORD_GROUP1);
    insert_word("LPT3", style::WORD_GROUP1);
    insert_word("LSS", style::WORD_GROUP1);
    insert_word("MD", style::WORD_GROUP1);
    insert_word("MOVE", style::WORD_GROUP1);
    insert_word("NEQ", style::WORD_GROUP1);
    insert_word("NOT", style::WORD_GROUP1);
    insert_word("NUL", style::WORD_GROUP1);
    insert_word("OFF", style::WORD_GROUP1);
    insert_word("ON", style::WORD_GROUP1);
    insert_word("PATH", style::WORD_GROUP1);
    insert_word("PAUSE", style::WORD_GROUP1);
    insert_word("POPD", style::WORD_GROUP1);
    insert_word("PRN", style::WORD_GROUP1);
    insert_word("PROMPT", style::WORD_GROUP1);
    insert_word("PUSHD", style::WORD_GROUP1);
    insert_word("READ", style::WORD_GROUP1);
    insert_word("REN", style::WORD_GROUP1);
    insert_word("RETURN", style::WORD_GROUP1);
    insert_word("SELECT", style::WORD_GROUP1);
    insert_word("SET", style::WORD_GROUP1);
    insert_word("SHIFT", style::WORD_GROUP1);
    insert_word("START", style::WORD_GROUP1);
    insert_word("THEN", style::WORD_GROUP1);
    insert_word("TITLE", style::WORD_GROUP1);
    insert_word("TYPE", style::WORD_GROUP1);
    insert_word("UNTIL", style::WORD_GROUP1);
    insert_word("VER", style::WORD_GROUP1);
    insert_word("VERIFY", style::WORD_GROUP1);
    insert_word("VOL", style::WORD_GROUP1);
    insert_word("WHILE", style::WORD_GROUP1);
    insert_word("aux", style::WORD_GROUP1);
    insert_word("break", style::WORD_GROUP1);
    insert_word("call", style::WORD_GROUP1);
    insert_word("case", style::WORD_GROUP1);
    insert_word("cls", style::WORD_GROUP1);
    insert_word("com1", style::WORD_GROUP1);
    insert_word("com2", style::WORD_GROUP1);
    insert_word("com3", style::WORD_GROUP1);
    insert_word("com4", style::WORD_GROUP1);
    insert_word("con", style::WORD_GROUP1);
    insert_word("copy", style::WORD_GROUP1);
    insert_word("defined", style::WORD_GROUP1);
    insert_word("del", style::WORD_GROUP1);
    insert_word("do", style::WORD_GROUP1);
    insert_word("done", style::WORD_GROUP1);
    insert_word("echo", style::WORD_GROUP1);
    insert_word("elif", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("equ", style::WORD_GROUP1);
    insert_word("errorlevel", style::WORD_GROUP1);
    insert_word("esac", style::WORD_GROUP1);
    insert_word("exist", style::WORD_GROUP1);
    insert_word("exit", style::WORD_GROUP1);
    insert_word("fi", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("function", style::WORD_GROUP1);
    insert_word("geq", style::WORD_GROUP1);
    insert_word("goto", style::WORD_GROUP1);
    insert_word("gtr", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("in", style::WORD_GROUP1);
    insert_word("leq", style::WORD_GROUP1);
    insert_word("local", style::WORD_GROUP1);
    insert_word("lpt1", style::WORD_GROUP1);
    insert_word("lpt2", style::WORD_GROUP1);
    insert_word("lpt3", style::WORD_GROUP1);
    insert_word("lss", style::WORD_GROUP1);
    insert_word("md", style::WORD_GROUP1);
    insert_word("move", style::WORD_GROUP1);
    insert_word("neq", style::WORD_GROUP1);
    insert_word("not", style::WORD_GROUP1);
    insert_word("nul", style::WORD_GROUP1);
    insert_word("off", style::WORD_GROUP1);
    insert_word("on", style::WORD_GROUP1);
    insert_word("path", style::WORD_GROUP1);
    insert_word("pause", style::WORD_GROUP1);
    insert_word("popd", style::WORD_GROUP1);
    insert_word("prn", style::WORD_GROUP1);
    insert_word("prompt", style::WORD_GROUP1);
    insert_word("pushd", style::WORD_GROUP1);
    insert_word("read", style::WORD_GROUP1);
    insert_word("ren", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("select", style::WORD_GROUP1);
    insert_word("set", style::WORD_GROUP1);
    insert_word("shift", style::WORD_GROUP1);
    insert_word("start", style::WORD_GROUP1);
    insert_word("then", style::WORD_GROUP1);
    insert_word("title", style::WORD_GROUP1);
    insert_word("type", style::WORD_GROUP1);
    insert_word("until", style::WORD_GROUP1);
    insert_word("ver", style::WORD_GROUP1);
    insert_word("verify", style::WORD_GROUP1);
    insert_word("vol", style::WORD_GROUP1);
    insert_word("while", style::WORD_GROUP1);
    make_words();
}
StyleCpp::StyleCpp() : StyleDef(style::CPP) {
    _line_comment      = "//";
    _line_comment_size = 2;
    _block_start       = "/*";
    _block_start_size  = 2;
    _block_end         = "*/";
    _block_end_size    = 2;
    _raw_start[0]      = "R\"(";
    _raw_start_size[0] = 3;
    _raw_end[0]        = ")\"";
    _raw_end_size[0]   = 2;
    _hex               = true;
    _bin               = true;
    _tokens.set('\'', Token::NUM_SEPARATOR | Token::PUNCTUATOR);
    _tokens.set('#', Token::PRAGMA);
    insert_word("and", style::WORD_GROUP1);
    insert_word("and_eq", style::WORD_GROUP1);
    insert_word("asm", style::WORD_GROUP1);
    insert_word("auto", style::WORD_GROUP1);
    insert_word("bitand", style::WORD_GROUP1);
    insert_word("bitor", style::WORD_GROUP1);
    insert_word("break", style::WORD_GROUP1);
    insert_word("case", style::WORD_GROUP1);
    insert_word("catch", style::WORD_GROUP1);
    insert_word("class", style::WORD_GROUP1);
    insert_word("compl", style::WORD_GROUP1);
    insert_word("concept", style::WORD_GROUP1);
    insert_word("continue", style::WORD_GROUP1);
    insert_word("default", style::WORD_GROUP1);
    insert_word("delete", style::WORD_GROUP1);
    insert_word("do", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("enum", style::WORD_GROUP1);
    insert_word("explicit", style::WORD_GROUP1);
    insert_word("final", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("friend", style::WORD_GROUP1);
    insert_word("goto", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("inline", style::WORD_GROUP1);
    insert_word("mutable", style::WORD_GROUP1);
    insert_word("namespace", style::WORD_GROUP1);
    insert_word("new", style::WORD_GROUP1);
    insert_word("noexcept", style::WORD_GROUP1);
    insert_word("not", style::WORD_GROUP1);
    insert_word("not_eq", style::WORD_GROUP1);
    insert_word("operator", style::WORD_GROUP1);
    insert_word("or", style::WORD_GROUP1);
    insert_word("or_eq", style::WORD_GROUP1);
    insert_word("override", style::WORD_GROUP1);
    insert_word("private", style::WORD_GROUP1);
    insert_word("protected", style::WORD_GROUP1);
    insert_word("public", style::WORD_GROUP1);
    insert_word("register", style::WORD_GROUP1);
    insert_word("requires", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("sizeof", style::WORD_GROUP1);
    insert_word("struct", style::WORD_GROUP1);
    insert_word("switch", style::WORD_GROUP1);
    insert_word("template", style::WORD_GROUP1);
    insert_word("this", style::WORD_GROUP1);
    insert_word("throw", style::WORD_GROUP1);
    insert_word("try", style::WORD_GROUP1);
    insert_word("union", style::WORD_GROUP1);
    insert_word("using", style::WORD_GROUP1);
    insert_word("virtual", style::WORD_GROUP1);
    insert_word("volatile", style::WORD_GROUP1);
    insert_word("while", style::WORD_GROUP1);
    insert_word("xor", style::WORD_GROUP1);
    insert_word("xor_eq", style::WORD_GROUP1);
    insert_word("FILE", style::WORD_GROUP2);
    insert_word("NULL", style::WORD_GROUP2);
    insert_word("bool", style::WORD_GROUP2);
    insert_word("char", style::WORD_GROUP2);
    insert_word("char16_t", style::WORD_GROUP2);
    insert_word("char32_t", style::WORD_GROUP2);
    insert_word("char8_t", style::WORD_GROUP2);
    insert_word("const", style::WORD_GROUP2);
    insert_word("double", style::WORD_GROUP2);
    insert_word("extern", style::WORD_GROUP2);
    insert_word("false", style::WORD_GROUP2);
    insert_word("float", style::WORD_GROUP2);
    insert_word("fpos_t", style::WORD_GROUP2);
    insert_word("int", style::WORD_GROUP2);
    insert_word("int16_t", style::WORD_GROUP2);
    insert_word("int32_t", style::WORD_GROUP2);
    insert_word("int64_t", style::WORD_GROUP2);
    insert_word("int8_t", style::WORD_GROUP2);
    insert_word("int_fast16_t", style::WORD_GROUP2);
    insert_word("int_fast32_t", style::WORD_GROUP2);
    insert_word("int_fast64_t", style::WORD_GROUP2);
    insert_word("int_fast8_t", style::WORD_GROUP2);
    insert_word("int_least16_t", style::WORD_GROUP2);
    insert_word("int_least32_t", style::WORD_GROUP2);
    insert_word("int_least64_t", style::WORD_GROUP2);
    insert_word("int_least8_t", style::WORD_GROUP2);
    insert_word("intmax_t", style::WORD_GROUP2);
    insert_word("intptr_t", style::WORD_GROUP2);
    insert_word("long", style::WORD_GROUP2);
    insert_word("nullptr", style::WORD_GROUP2);
    insert_word("off_t", style::WORD_GROUP2);
    insert_word("pid_t", style::WORD_GROUP2);
    insert_word("ptrdiff_t", style::WORD_GROUP2);
    insert_word("short", style::WORD_GROUP2);
    insert_word("sig_atomic_t", style::WORD_GROUP2);
    insert_word("signed", style::WORD_GROUP2);
    insert_word("size_t", style::WORD_GROUP2);
    insert_word("ssize_t", style::WORD_GROUP2);
    insert_word("static", style::WORD_GROUP2);
    insert_word("time_t", style::WORD_GROUP2);
    insert_word("true", style::WORD_GROUP2);
    insert_word("uint16_t", style::WORD_GROUP2);
    insert_word("uint32_t", style::WORD_GROUP2);
    insert_word("uint64_t", style::WORD_GROUP2);
    insert_word("uint8_t", style::WORD_GROUP2);
    insert_word("uint_fast16_t", style::WORD_GROUP2);
    insert_word("uint_fast32_t", style::WORD_GROUP2);
    insert_word("uint_fast64_t", style::WORD_GROUP2);
    insert_word("uint_fast8_t", style::WORD_GROUP2);
    insert_word("uint_least16_t", style::WORD_GROUP2);
    insert_word("uint_least32_t", style::WORD_GROUP2);
    insert_word("uint_least64_t", style::WORD_GROUP2);
    insert_word("uint_least8_t", style::WORD_GROUP2);
    insert_word("uintmax_t", style::WORD_GROUP2);
    insert_word("uintptr_t", style::WORD_GROUP2);
    insert_word("unsigned", style::WORD_GROUP2);
    insert_word("va_list", style::WORD_GROUP2);
    insert_word("void", style::WORD_GROUP2);
    insert_word("wchar_t", style::WORD_GROUP2);
    insert_word("wint_t", style::WORD_GROUP2);
    insert_word("_Pragma", style::WORD_GROUP4);
    insert_word("__DATE__", style::WORD_GROUP4);
    insert_word("__FILE__", style::WORD_GROUP4);
    insert_word("__LINE__", style::WORD_GROUP4);
    insert_word("__TIMESTAMP__", style::WORD_GROUP4);
    insert_word("__TIME__", style::WORD_GROUP4);
    insert_word("__cplusplus", style::WORD_GROUP4);
    insert_word("__func__", style::WORD_GROUP4);
    insert_word("__has_cpp_attribute", style::WORD_GROUP4);
    insert_word("__has_include", style::WORD_GROUP4);
    insert_word("define", style::WORD_GROUP4);
    insert_word("defined", style::WORD_GROUP4);
    insert_word("elif", style::WORD_GROUP4);
    insert_word("elifdef", style::WORD_GROUP4);
    insert_word("elifndef", style::WORD_GROUP4);
    insert_word("endif", style::WORD_GROUP4);
    insert_word("error", style::WORD_GROUP4);
    insert_word("export", style::WORD_GROUP4);
    insert_word("ifdef", style::WORD_GROUP4);
    insert_word("ifndef", style::WORD_GROUP4);
    insert_word("import", style::WORD_GROUP4);
    insert_word("include", style::WORD_GROUP4);
    insert_word("line", style::WORD_GROUP4);
    insert_word("module", style::WORD_GROUP4);
    insert_word("pragma", style::WORD_GROUP4);
    insert_word("typedef", style::WORD_GROUP4);
    insert_word("typeid", style::WORD_GROUP4);
    insert_word("typename", style::WORD_GROUP4);
    insert_word("undef", style::WORD_GROUP4);
    insert_word("alignas", style::WORD_GROUP8);
    insert_word("alignof", style::WORD_GROUP8);
    insert_word("co_await", style::WORD_GROUP8);
    insert_word("co_return", style::WORD_GROUP8);
    insert_word("co_yield", style::WORD_GROUP8);
    insert_word("const_cast", style::WORD_GROUP8);
    insert_word("consteval", style::WORD_GROUP8);
    insert_word("constexpr", style::WORD_GROUP8);
    insert_word("constinit", style::WORD_GROUP8);
    insert_word("decltype", style::WORD_GROUP8);
    insert_word("dynamic_cast", style::WORD_GROUP8);
    insert_word("reinterpret_cast", style::WORD_GROUP8);
    insert_word("static_assert", style::WORD_GROUP8);
    insert_word("static_cast", style::WORD_GROUP8);
    insert_word("thread_local", style::WORD_GROUP8);
    _custom.insert("for (int f = 0; f < 100; f++) {|}");
    make_words();
}
StyleCS::StyleCS() : StyleDef(style::CS) {
    _line_comment      = "//";
    _line_comment_size = 2;
    _block_start       = "/*";
    _block_start_size  = 2;
    _block_end         = "*/";
    _block_end_size    = 2;
    _raw_start[0]      = "\"\"\"";
    _raw_start_size[0] = 3;
    _raw_end[0]        = "\"\"\"";
    _raw_end_size[0]   = 3;
    _hex               = true;
    _bin               = true;
    _tokens.set('_', Token::NUM_SEPARATOR);
    _tokens.set('#', Token::PRAGMA);
    insert_word("abstract", style::WORD_GROUP1);
    insert_word("and", style::WORD_GROUP1);
    insert_word("as", style::WORD_GROUP1);
    insert_word("async", style::WORD_GROUP1);
    insert_word("await", style::WORD_GROUP1);
    insert_word("base", style::WORD_GROUP1);
    insert_word("break", style::WORD_GROUP1);
    insert_word("case", style::WORD_GROUP1);
    insert_word("catch", style::WORD_GROUP1);
    insert_word("checked", style::WORD_GROUP1);
    insert_word("class", style::WORD_GROUP1);
    insert_word("continue", style::WORD_GROUP1);
    insert_word("default", style::WORD_GROUP1);
    insert_word("delegate", style::WORD_GROUP1);
    insert_word("do", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("enum", style::WORD_GROUP1);
    insert_word("event", style::WORD_GROUP1);
    insert_word("explicit", style::WORD_GROUP1);
    insert_word("extern", style::WORD_GROUP1);
    insert_word("finally", style::WORD_GROUP1);
    insert_word("fixed", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("foreach", style::WORD_GROUP1);
    insert_word("goto", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("implicit", style::WORD_GROUP1);
    insert_word("in", style::WORD_GROUP1);
    insert_word("int", style::WORD_GROUP1);
    insert_word("interface", style::WORD_GROUP1);
    insert_word("internal", style::WORD_GROUP1);
    insert_word("is", style::WORD_GROUP1);
    insert_word("lock", style::WORD_GROUP1);
    insert_word("long", style::WORD_GROUP1);
    insert_word("nameof", style::WORD_GROUP1);
    insert_word("namespace", style::WORD_GROUP1);
    insert_word("new", style::WORD_GROUP1);
    insert_word("not", style::WORD_GROUP1);
    insert_word("object", style::WORD_GROUP1);
    insert_word("operator", style::WORD_GROUP1);
    insert_word("or", style::WORD_GROUP1);
    insert_word("out", style::WORD_GROUP1);
    insert_word("override", style::WORD_GROUP1);
    insert_word("params", style::WORD_GROUP1);
    insert_word("private", style::WORD_GROUP1);
    insert_word("protected", style::WORD_GROUP1);
    insert_word("public", style::WORD_GROUP1);
    insert_word("readonly", style::WORD_GROUP1);
    insert_word("ref", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("sealed", style::WORD_GROUP1);
    insert_word("sizeof", style::WORD_GROUP1);
    insert_word("stackalloc", style::WORD_GROUP1);
    insert_word("static", style::WORD_GROUP1);
    insert_word("string", style::WORD_GROUP1);
    insert_word("struct", style::WORD_GROUP1);
    insert_word("switch", style::WORD_GROUP1);
    insert_word("this", style::WORD_GROUP1);
    insert_word("throw", style::WORD_GROUP1);
    insert_word("try", style::WORD_GROUP1);
    insert_word("typeof", style::WORD_GROUP1);
    insert_word("unchecked", style::WORD_GROUP1);
    insert_word("unsafe", style::WORD_GROUP1);
    insert_word("using", style::WORD_GROUP1);
    insert_word("var", style::WORD_GROUP1);
    insert_word("virtual", style::WORD_GROUP1);
    insert_word("void", style::WORD_GROUP1);
    insert_word("volatile", style::WORD_GROUP1);
    insert_word("while", style::WORD_GROUP1);
    insert_word("with", style::WORD_GROUP1);
    insert_word("yield", style::WORD_GROUP1);
    insert_word("bool", style::WORD_GROUP2);
    insert_word("byte", style::WORD_GROUP2);
    insert_word("char", style::WORD_GROUP2);
    insert_word("const", style::WORD_GROUP2);
    insert_word("decimal", style::WORD_GROUP2);
    insert_word("double", style::WORD_GROUP2);
    insert_word("false", style::WORD_GROUP2);
    insert_word("float", style::WORD_GROUP2);
    insert_word("nint", style::WORD_GROUP2);
    insert_word("nuint", style::WORD_GROUP2);
    insert_word("null", style::WORD_GROUP2);
    insert_word("sbyte", style::WORD_GROUP2);
    insert_word("short", style::WORD_GROUP2);
    insert_word("string", style::WORD_GROUP2);
    insert_word("true", style::WORD_GROUP2);
    insert_word("uint", style::WORD_GROUP2);
    insert_word("ulong", style::WORD_GROUP2);
    insert_word("ushort", style::WORD_GROUP2);
    insert_word("define", style::WORD_GROUP4);
    insert_word("elif", style::WORD_GROUP4);
    insert_word("endif", style::WORD_GROUP4);
    insert_word("endregion", style::WORD_GROUP4);
    insert_word("error", style::WORD_GROUP4);
    insert_word("line", style::WORD_GROUP4);
    insert_word("nullable", style::WORD_GROUP4);
    insert_word("pragma", style::WORD_GROUP4);
    insert_word("region", style::WORD_GROUP4);
    insert_word("warning", style::WORD_GROUP4);
    make_words();
}
StyleGo::StyleGo() : StyleDef(style::GO) {
    _line_comment      = "//";
    _line_comment_size = 2;
    _block_start       = "/*";
    _block_start_size  = 2;
    _block_end         = "*/";
    _block_end_size    = 2;
    _raw_start[0]      = "`";
    _raw_start_size[0] = 1;
    _raw_end[0]        = "`";
    _raw_end_size[0]   = 1;
    _hex               = true;
    _oct               = true;
    _bin               = true;
    _tokens.set('_', Token::NUM_SEPARATOR | Token::PUNCTUATOR);
    insert_word("break", style::WORD_GROUP1);
    insert_word("case", style::WORD_GROUP1);
    insert_word("chan", style::WORD_GROUP1);
    insert_word("continue", style::WORD_GROUP1);
    insert_word("default", style::WORD_GROUP1);
    insert_word("defer", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("fallthrough", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("go", style::WORD_GROUP1);
    insert_word("goto", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("interface", style::WORD_GROUP1);
    insert_word("map", style::WORD_GROUP1);
    insert_word("range", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("select", style::WORD_GROUP1);
    insert_word("struct", style::WORD_GROUP1);
    insert_word("switch", style::WORD_GROUP1);
    insert_word("bool", style::WORD_GROUP2);
    insert_word("byte", style::WORD_GROUP2);
    insert_word("complex128", style::WORD_GROUP2);
    insert_word("complex64", style::WORD_GROUP2);
    insert_word("false", style::WORD_GROUP2);
    insert_word("float32", style::WORD_GROUP2);
    insert_word("float64", style::WORD_GROUP2);
    insert_word("int", style::WORD_GROUP2);
    insert_word("int16", style::WORD_GROUP2);
    insert_word("int32", style::WORD_GROUP2);
    insert_word("int64", style::WORD_GROUP2);
    insert_word("int8", style::WORD_GROUP2);
    insert_word("iota", style::WORD_GROUP2);
    insert_word("nil", style::WORD_GROUP2);
    insert_word("rune", style::WORD_GROUP2);
    insert_word("string", style::WORD_GROUP2);
    insert_word("true", style::WORD_GROUP2);
    insert_word("uint", style::WORD_GROUP2);
    insert_word("uint16", style::WORD_GROUP2);
    insert_word("uint32", style::WORD_GROUP2);
    insert_word("uint64", style::WORD_GROUP2);
    insert_word("uint8", style::WORD_GROUP2);
    insert_word("uintptr", style::WORD_GROUP2);
    insert_word("const", style::WORD_GROUP4);
    insert_word("func", style::WORD_GROUP4);
    insert_word("import", style::WORD_GROUP4);
    insert_word("package", style::WORD_GROUP4);
    insert_word("type", style::WORD_GROUP4);
    insert_word("var", style::WORD_GROUP4);
    insert_word("append", style::WORD_GROUP8);
    insert_word("cap", style::WORD_GROUP8);
    insert_word("clear", style::WORD_GROUP8);
    insert_word("close", style::WORD_GROUP8);
    insert_word("complex", style::WORD_GROUP8);
    insert_word("copy", style::WORD_GROUP8);
    insert_word("delete", style::WORD_GROUP8);
    insert_word("imag", style::WORD_GROUP8);
    insert_word("len", style::WORD_GROUP8);
    insert_word("make", style::WORD_GROUP8);
    insert_word("max", style::WORD_GROUP8);
    insert_word("min", style::WORD_GROUP8);
    insert_word("new", style::WORD_GROUP8);
    insert_word("panic", style::WORD_GROUP8);
    insert_word("print", style::WORD_GROUP8);
    insert_word("println", style::WORD_GROUP8);
    insert_word("real", style::WORD_GROUP8);
    insert_word("recover", style::WORD_GROUP8);
    _custom.insert("err := something()|if err != nil {|    log.Fatal(err)|}");
    make_words();
}
StyleJava::StyleJava() : StyleDef(style::JAVA) {
    _line_comment      = "//";
    _line_comment_size = 2;
    _block_start       = "/*";
    _block_start_size  = 2;
    _block_end         = "*/";
    _block_end_size    = 2;
    _raw_start[0]      = "\"\"\"";
    _raw_start_size[0] = 3;
    _raw_end[0]        = "\"\"\"";
    _raw_end_size[0]   = 3;
    _hex               = true;
    _bin               = true;
    _tokens.set('@', Token::PRAGMA);
    insert_word("abstract", style::WORD_GROUP1);
    insert_word("assert", style::WORD_GROUP1);
    insert_word("break", style::WORD_GROUP1);
    insert_word("case", style::WORD_GROUP1);
    insert_word("catch", style::WORD_GROUP1);
    insert_word("class", style::WORD_GROUP1);
    insert_word("continue", style::WORD_GROUP1);
    insert_word("default", style::WORD_GROUP1);
    insert_word("do", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("enum", style::WORD_GROUP1);
    insert_word("extends", style::WORD_GROUP1);
    insert_word("false", style::WORD_GROUP1);
    insert_word("finally", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("goto", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("implements", style::WORD_GROUP1);
    insert_word("instanceof", style::WORD_GROUP1);
    insert_word("interface", style::WORD_GROUP1);
    insert_word("native", style::WORD_GROUP1);
    insert_word("new", style::WORD_GROUP1);
    insert_word("non-sealed", style::WORD_GROUP1);
    insert_word("null", style::WORD_GROUP1);
    insert_word("permits", style::WORD_GROUP1);
    insert_word("private", style::WORD_GROUP1);
    insert_word("protected", style::WORD_GROUP1);
    insert_word("public", style::WORD_GROUP1);
    insert_word("record", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("sealed", style::WORD_GROUP1);
    insert_word("strictfp", style::WORD_GROUP1);
    insert_word("super", style::WORD_GROUP1);
    insert_word("switch", style::WORD_GROUP1);
    insert_word("synchronized", style::WORD_GROUP1);
    insert_word("this", style::WORD_GROUP1);
    insert_word("throw", style::WORD_GROUP1);
    insert_word("throws", style::WORD_GROUP1);
    insert_word("transient", style::WORD_GROUP1);
    insert_word("true", style::WORD_GROUP1);
    insert_word("try", style::WORD_GROUP1);
    insert_word("volatile", style::WORD_GROUP1);
    insert_word("while", style::WORD_GROUP1);
    insert_word("yield", style::WORD_GROUP1);
    insert_word("Iterator", style::WORD_GROUP2);
    insert_word("ArrayList", style::WORD_GROUP2);
    insert_word("AtomicInteger", style::WORD_GROUP2);
    insert_word("AtomicLong", style::WORD_GROUP2);
    insert_word("BigDecimal", style::WORD_GROUP2);
    insert_word("BigInteger", style::WORD_GROUP2);
    insert_word("Byte", style::WORD_GROUP2);
    insert_word("Double", style::WORD_GROUP2);
    insert_word("DoubleAccumulator", style::WORD_GROUP2);
    insert_word("DoubleAdder", style::WORD_GROUP2);
    insert_word("Float", style::WORD_GROUP2);
    insert_word("HashMap", style::WORD_GROUP2);
    insert_word("HashSet", style::WORD_GROUP2);
    insert_word("Integer", style::WORD_GROUP2);
    insert_word("LinkedList", style::WORD_GROUP2);
    insert_word("LocalDate", style::WORD_GROUP2);
    insert_word("Long", style::WORD_GROUP2);
    insert_word("LongAccumulator", style::WORD_GROUP2);
    insert_word("LongAdder", style::WORD_GROUP2);
    insert_word("Matcher", style::WORD_GROUP2);
    insert_word("Pattern", style::WORD_GROUP2);
    insert_word("Short", style::WORD_GROUP2);
    insert_word("String", style::WORD_GROUP2);
    insert_word("Thread", style::WORD_GROUP2);
    insert_word("boolean", style::WORD_GROUP2);
    insert_word("byte", style::WORD_GROUP2);
    insert_word("char", style::WORD_GROUP2);
    insert_word("const", style::WORD_GROUP2);
    insert_word("double", style::WORD_GROUP2);
    insert_word("final", style::WORD_GROUP2);
    insert_word("float", style::WORD_GROUP2);
    insert_word("int", style::WORD_GROUP2);
    insert_word("long", style::WORD_GROUP2);
    insert_word("short", style::WORD_GROUP2);
    insert_word("static", style::WORD_GROUP2);
    insert_word("var", style::WORD_GROUP2);
    insert_word("void", style::WORD_GROUP2);
    insert_word("Deprecated", style::WORD_GROUP4);
    insert_word("Documented", style::WORD_GROUP4);
    insert_word("FunctionalInterface", style::WORD_GROUP4);
    insert_word("Inherited", style::WORD_GROUP4);
    insert_word("Override", style::WORD_GROUP4);
    insert_word("Repeatable", style::WORD_GROUP4);
    insert_word("Retention", style::WORD_GROUP4);
    insert_word("SafeVarargs", style::WORD_GROUP4);
    insert_word("SuppressWarnings", style::WORD_GROUP4);
    insert_word("Target", style::WORD_GROUP4);
    insert_word("import", style::WORD_GROUP4);
    insert_word("package", style::WORD_GROUP4);
    make_words();
}
StyleJS::StyleJS(std::string name) : StyleDef(name) {
    _single_quote_str  = '\'';
    _line_comment      = "//";
    _line_comment_size = 2;
    _block_start       = "/*";
    _block_start_size  = 2;
    _block_end         = "*/";
    _block_end_size    = 2;
    _raw_start[0]      = "`";
    _raw_start_size[0] = 1;
    _raw_end[0]        = "`";
    _raw_end_size[0]   = 1;
    _raw_escape[0]     = true;
    _hex               = true;
    _bin               = true;
    insert_word("arguments", style::WORD_GROUP1);
    insert_word("async", style::WORD_GROUP1);
    insert_word("await", style::WORD_GROUP1);
    insert_word("break", style::WORD_GROUP1);
    insert_word("case", style::WORD_GROUP1);
    insert_word("catch", style::WORD_GROUP1);
    insert_word("class", style::WORD_GROUP1);
    insert_word("const", style::WORD_GROUP1);
    insert_word("continue", style::WORD_GROUP1);
    insert_word("debugger", style::WORD_GROUP1);
    insert_word("delete", style::WORD_GROUP1);
    insert_word("do", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("extends", style::WORD_GROUP1);
    insert_word("finally", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("function", style::WORD_GROUP1);
    insert_word("get", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("implements", style::WORD_GROUP1);
    insert_word("in", style::WORD_GROUP1);
    insert_word("instanceof", style::WORD_GROUP1);
    insert_word("interface", style::WORD_GROUP1);
    insert_word("let", style::WORD_GROUP1);
    insert_word("new", style::WORD_GROUP1);
    insert_word("of", style::WORD_GROUP1);
    insert_word("private", style::WORD_GROUP1);
    insert_word("protected", style::WORD_GROUP1);
    insert_word("public", style::WORD_GROUP1);
    insert_word("reject", style::WORD_GROUP1);
    insert_word("resolve", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("set", style::WORD_GROUP1);
    insert_word("static", style::WORD_GROUP1);
    insert_word("super", style::WORD_GROUP1);
    insert_word("switch", style::WORD_GROUP1);
    insert_word("this", style::WORD_GROUP1);
    insert_word("throw", style::WORD_GROUP1);
    insert_word("try", style::WORD_GROUP1);
    insert_word("typeof", style::WORD_GROUP1);
    insert_word("var", style::WORD_GROUP1);
    insert_word("void", style::WORD_GROUP1);
    insert_word("while", style::WORD_GROUP1);
    insert_word("with", style::WORD_GROUP1);
    insert_word("yield", style::WORD_GROUP1);
    insert_word("Array", style::WORD_GROUP2);
    insert_word("BigInt64Array", style::WORD_GROUP2);
    insert_word("BigUint64Array", style::WORD_GROUP2);
    insert_word("Boolean", style::WORD_GROUP2);
    insert_word("Date", style::WORD_GROUP2);
    insert_word("Enumerator", style::WORD_GROUP2);
    insert_word("Error", style::WORD_GROUP2);
    insert_word("Float32Array", style::WORD_GROUP2);
    insert_word("Float64Array", style::WORD_GROUP2);
    insert_word("Function", style::WORD_GROUP2);
    insert_word("Generator", style::WORD_GROUP2);
    insert_word("Infinity", style::WORD_GROUP2);
    insert_word("Int16Array", style::WORD_GROUP2);
    insert_word("Map", style::WORD_GROUP2);
    insert_word("Math", style::WORD_GROUP2);
    insert_word("NaN", style::WORD_GROUP2);
    insert_word("Number", style::WORD_GROUP2);
    insert_word("Object", style::WORD_GROUP2);
    insert_word("Promise", style::WORD_GROUP2);
    insert_word("Proxy", style::WORD_GROUP2);
    insert_word("Reflect", style::WORD_GROUP2);
    insert_word("RegExp", style::WORD_GROUP2);
    insert_word("Set", style::WORD_GROUP2);
    insert_word("String", style::WORD_GROUP2);
    insert_word("Symbol", style::WORD_GROUP2);
    insert_word("WeakMap", style::WORD_GROUP2);
    insert_word("WeakSet", style::WORD_GROUP2);
    insert_word("enum", style::WORD_GROUP2);
    insert_word("false", style::WORD_GROUP2);
    insert_word("globalThis", style::WORD_GROUP2);
    insert_word("null", style::WORD_GROUP2);
    insert_word("true", style::WORD_GROUP2);
    insert_word("undefined", style::WORD_GROUP2);
    insert_word("as", style::WORD_GROUP4);
    insert_word("default", style::WORD_GROUP4);
    insert_word("export", style::WORD_GROUP4);
    insert_word("from", style::WORD_GROUP4);
    insert_word("import", style::WORD_GROUP4);
    insert_word("package", style::WORD_GROUP4);
    make_words();
}
StyleKotlin::StyleKotlin() : StyleDef(style::KOTLIN) {
    _line_comment      = "//";
    _line_comment_size = 2;
    _block_start       = "/*";
    _block_start_size  = 2;
    _block_end         = "*/";
    _block_end_size    = 2;
    _raw_start[0]      = "\"\"\"";
    _raw_start_size[0] = 3;
    _raw_end[0]        = "\"\"\"";
    _raw_end_size[0]   = 3;
    _hex               = true;
    _bin               = true;
    _tokens.set('@', Token::PRAGMA);
    insert_word("abstract", style::WORD_GROUP1);
    insert_word("actual", style::WORD_GROUP1);
    insert_word("annotation", style::WORD_GROUP1);
    insert_word("as", style::WORD_GROUP1);
    insert_word("as?", style::WORD_GROUP1);
    insert_word("break", style::WORD_GROUP1);
    insert_word("by", style::WORD_GROUP1);
    insert_word("catch", style::WORD_GROUP1);
    insert_word("class", style::WORD_GROUP1);
    insert_word("companion", style::WORD_GROUP1);
    insert_word("const", style::WORD_GROUP1);
    insert_word("constructor", style::WORD_GROUP1);
    insert_word("continue", style::WORD_GROUP1);
    insert_word("crossinline", style::WORD_GROUP1);
    insert_word("data", style::WORD_GROUP1);
    insert_word("delegate", style::WORD_GROUP1);
    insert_word("do", style::WORD_GROUP1);
    insert_word("dynamic", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("enum", style::WORD_GROUP1);
    insert_word("expect", style::WORD_GROUP1);
    insert_word("external", style::WORD_GROUP1);
    insert_word("field", style::WORD_GROUP1);
    insert_word("file", style::WORD_GROUP1);
    insert_word("final", style::WORD_GROUP1);
    insert_word("finally", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("fun", style::WORD_GROUP1);
    insert_word("get", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("in", style::WORD_GROUP1);
    insert_word("infix", style::WORD_GROUP1);
    insert_word("init", style::WORD_GROUP1);
    insert_word("inline", style::WORD_GROUP1);
    insert_word("inner", style::WORD_GROUP1);
    insert_word("interface", style::WORD_GROUP1);
    insert_word("internal", style::WORD_GROUP1);
    insert_word("is", style::WORD_GROUP1);
    insert_word("lateinit", style::WORD_GROUP1);
    insert_word("noinline", style::WORD_GROUP1);
    insert_word("object", style::WORD_GROUP1);
    insert_word("open", style::WORD_GROUP1);
    insert_word("operator", style::WORD_GROUP1);
    insert_word("out", style::WORD_GROUP1);
    insert_word("override", style::WORD_GROUP1);
    insert_word("param", style::WORD_GROUP1);
    insert_word("private", style::WORD_GROUP1);
    insert_word("property", style::WORD_GROUP1);
    insert_word("protected", style::WORD_GROUP1);
    insert_word("public", style::WORD_GROUP1);
    insert_word("receiver", style::WORD_GROUP1);
    insert_word("reified", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("sealed", style::WORD_GROUP1);
    insert_word("set", style::WORD_GROUP1);
    insert_word("setparam", style::WORD_GROUP1);
    insert_word("super", style::WORD_GROUP1);
    insert_word("suspend", style::WORD_GROUP1);
    insert_word("tailrec", style::WORD_GROUP1);
    insert_word("this", style::WORD_GROUP1);
    insert_word("throw", style::WORD_GROUP1);
    insert_word("try", style::WORD_GROUP1);
    insert_word("typealias", style::WORD_GROUP1);
    insert_word("typeof", style::WORD_GROUP1);
    insert_word("val", style::WORD_GROUP1);
    insert_word("value", style::WORD_GROUP1);
    insert_word("var", style::WORD_GROUP1);
    insert_word("vararg", style::WORD_GROUP1);
    insert_word("when", style::WORD_GROUP1);
    insert_word("where", style::WORD_GROUP1);
    insert_word("while", style::WORD_GROUP1);
    insert_word("Any", style::WORD_GROUP2);
    insert_word("Array", style::WORD_GROUP2);
    insert_word("ArrayList", style::WORD_GROUP2);
    insert_word("Boolean", style::WORD_GROUP2);
    insert_word("BooleanArrray", style::WORD_GROUP2);
    insert_word("Byte", style::WORD_GROUP2);
    insert_word("ByteArray", style::WORD_GROUP2);
    insert_word("Char", style::WORD_GROUP2);
    insert_word("CharArray", style::WORD_GROUP2);
    insert_word("Double", style::WORD_GROUP2);
    insert_word("DoubleArray", style::WORD_GROUP2);
    insert_word("Enum", style::WORD_GROUP2);
    insert_word("Float", style::WORD_GROUP2);
    insert_word("FloatArray", style::WORD_GROUP2);
    insert_word("Function", style::WORD_GROUP2);
    insert_word("HashMap", style::WORD_GROUP2);
    insert_word("HashSet", style::WORD_GROUP2);
    insert_word("Int", style::WORD_GROUP2);
    insert_word("IntArray", style::WORD_GROUP2);
    insert_word("LinkedList", style::WORD_GROUP2);
    insert_word("List", style::WORD_GROUP2);
    insert_word("LocalDate", style::WORD_GROUP2);
    insert_word("Long", style::WORD_GROUP2);
    insert_word("LongArray", style::WORD_GROUP2);
    insert_word("Map", style::WORD_GROUP2);
    insert_word("Matcher", style::WORD_GROUP2);
    insert_word("Nothing", style::WORD_GROUP2);
    insert_word("Pattern", style::WORD_GROUP2);
    insert_word("Set", style::WORD_GROUP2);
    insert_word("Short", style::WORD_GROUP2);
    insert_word("ShortArray", style::WORD_GROUP2);
    insert_word("String", style::WORD_GROUP2);
    insert_word("Thread", style::WORD_GROUP2);
    insert_word("Unit", style::WORD_GROUP2);
    insert_word("false", style::WORD_GROUP2);
    insert_word("null", style::WORD_GROUP2);
    insert_word("true", style::WORD_GROUP2);
    insert_word("Deprecated", style::WORD_GROUP4);
    insert_word("Documented", style::WORD_GROUP4);
    insert_word("FunctionalInterface", style::WORD_GROUP4);
    insert_word("Inherited", style::WORD_GROUP4);
    insert_word("Override", style::WORD_GROUP4);
    insert_word("Repeatable", style::WORD_GROUP4);
    insert_word("Retention", style::WORD_GROUP4);
    insert_word("SafeVarargs", style::WORD_GROUP4);
    insert_word("Strictfp", style::WORD_GROUP4);
    insert_word("SuppressWarnings", style::WORD_GROUP4);
    insert_word("Synchronized", style::WORD_GROUP4);
    insert_word("Target", style::WORD_GROUP4);
    insert_word("Throwable", style::WORD_GROUP4);
    insert_word("Throws", style::WORD_GROUP4);
    insert_word("Transient", style::WORD_GROUP4);
    insert_word("Volatile", style::WORD_GROUP4);
    insert_word("import", style::WORD_GROUP4);
    insert_word("package", style::WORD_GROUP4);
    insert_word("arrayListOf", style::WORD_GROUP8);
    insert_word("compareTo", style::WORD_GROUP8);
    insert_word("equals", style::WORD_GROUP8);
    insert_word("hashCode", style::WORD_GROUP8);
    insert_word("hashMapOf", style::WORD_GROUP8);
    insert_word("hashSetOf", style::WORD_GROUP8);
    insert_word("listOf", style::WORD_GROUP8);
    insert_word("mapOf", style::WORD_GROUP8);
    insert_word("mutableListOf", style::WORD_GROUP8);
    insert_word("mutableMapOf", style::WORD_GROUP8);
    insert_word("mutableSetOf", style::WORD_GROUP8);
    insert_word("println", style::WORD_GROUP8);
    insert_word("setOf", style::WORD_GROUP8);
    insert_word("toString", style::WORD_GROUP8);
}
StyleLua::StyleLua() : StyleDef(style::LUA) {
    _single_quote_str  = '\'';
    _line_comment      = "--";
    _line_comment_size = 2;
    _block_start       = "--[[";
    _block_start_size  = 4;
    _block_end         = "]]";
    _block_end_size    = 2;
    _raw_start[0]      = "[[";
    _raw_start_size[0] = 2;
    _raw_end[0]        = "]]";
    _raw_end_size[0]   = 2;
    _hex               = true;
    insert_word("and", style::WORD_GROUP1);
    insert_word("assert", style::WORD_GROUP1);
    insert_word("break", style::WORD_GROUP1);
    insert_word("collectgarbage", style::WORD_GROUP1);
    insert_word("defer", style::WORD_GROUP1);
    insert_word("do", style::WORD_GROUP1);
    insert_word("dofile", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("elseif", style::WORD_GROUP1);
    insert_word("end", style::WORD_GROUP1);
    insert_word("error", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("function", style::WORD_GROUP1);
    insert_word("getmetatable", style::WORD_GROUP1);
    insert_word("goto", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("in", style::WORD_GROUP1);
    insert_word("ipairs", style::WORD_GROUP1);
    insert_word("load", style::WORD_GROUP1);
    insert_word("loadfile", style::WORD_GROUP1);
    insert_word("local", style::WORD_GROUP1);
    insert_word("next", style::WORD_GROUP1);
    insert_word("not", style::WORD_GROUP1);
    insert_word("or", style::WORD_GROUP1);
    insert_word("pairs", style::WORD_GROUP1);
    insert_word("pcall", style::WORD_GROUP1);
    insert_word("print", style::WORD_GROUP1);
    insert_word("rawequal", style::WORD_GROUP1);
    insert_word("rawget", style::WORD_GROUP1);
    insert_word("rawlen", style::WORD_GROUP1);
    insert_word("rawset", style::WORD_GROUP1);
    insert_word("repeat", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("select", style::WORD_GROUP1);
    insert_word("setmetatable", style::WORD_GROUP1);
    insert_word("then", style::WORD_GROUP1);
    insert_word("tonumber", style::WORD_GROUP1);
    insert_word("tostring", style::WORD_GROUP1);
    insert_word("type", style::WORD_GROUP1);
    insert_word("until", style::WORD_GROUP1);
    insert_word("while", style::WORD_GROUP1);
    insert_word("xpcall", style::WORD_GROUP1);
    insert_word("coroutine", style::WORD_GROUP2);
    insert_word("debug", style::WORD_GROUP2);
    insert_word("false", style::WORD_GROUP2);
    insert_word("file", style::WORD_GROUP2);
    insert_word("io", style::WORD_GROUP2);
    insert_word("math", style::WORD_GROUP2);
    insert_word("nil", style::WORD_GROUP2);
    insert_word("os", style::WORD_GROUP2);
    insert_word("string", style::WORD_GROUP2);
    insert_word("table", style::WORD_GROUP2);
    insert_word("true", style::WORD_GROUP2);
    insert_word("require", style::WORD_GROUP4);
    _custom.insert("for f = 1, 100 do|end");
    _custom.insert("for index, value in ipairs(t) do|end");
    _custom.insert("for key, value in pairs(t) do|end");
    make_words();
}
StyleMakefile::StyleMakefile() : StyleDef(style::MAKEFILE) {
    _single_quote_str  = '\'';
    _line_comment      = "#";
    _line_comment_size = 1;
    _raw_start[0]      = "";
    _raw_start_size[0] = 0;
    _raw_end[0]        = "";
    _raw_end_size[0]   = 0;
    _tokens.set('$', Token::VALUE | Token::PUNCTUATOR);
    insert_word("cd", style::WORD_GROUP1);
    insert_word("chmod", style::WORD_GROUP1);
    insert_word("define", style::WORD_GROUP1);
    insert_word("done", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("endef", style::WORD_GROUP1);
    insert_word("endif", style::WORD_GROUP1);
    insert_word("fi", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("ifdef", style::WORD_GROUP1);
    insert_word("ifeq", style::WORD_GROUP1);
    insert_word("ifndef", style::WORD_GROUP1);
    insert_word("ifneq", style::WORD_GROUP1);
    insert_word("touch", style::WORD_GROUP1);
    insert_word("undefine", style::WORD_GROUP1);
    insert_word("CURDIR", style::WORD_GROUP2);
    insert_word("DEFAULT", style::WORD_GROUP2);
    insert_word("DELETE_ON_ERROR", style::WORD_GROUP2);
    insert_word("EXPORT_ALL_VARIABLES", style::WORD_GROUP2);
    insert_word("GNUMAKEFLAGS", style::WORD_GROUP2);
    insert_word("IGNORE", style::WORD_GROUP2);
    insert_word("INTERMEDIATE", style::WORD_GROUP2);
    insert_word("LOWORD_RESOLUTION_TIME", style::WORD_GROUP2);
    insert_word("MAKE", style::WORD_GROUP2);
    insert_word("MAKECMDGOALS", style::WORD_GROUP2);
    insert_word("MAKEFILES", style::WORD_GROUP2);
    insert_word("MAKEFLAGS", style::WORD_GROUP2);
    insert_word("MAKELEVEL", style::WORD_GROUP2);
    insert_word("MAKE_HOST", style::WORD_GROUP2);
    insert_word("MAKE_VERSION", style::WORD_GROUP2);
    insert_word("NOTPARALLEL", style::WORD_GROUP2);
    insert_word("ONESHELL", style::WORD_GROUP2);
    insert_word("PHONY", style::WORD_GROUP2);
    insert_word("POSIX", style::WORD_GROUP2);
    insert_word("PRECIOUS", style::WORD_GROUP2);
    insert_word("SECONDARY", style::WORD_GROUP2);
    insert_word("SECONDEXPANSION", style::WORD_GROUP2);
    insert_word("SHELL", style::WORD_GROUP2);
    insert_word("SILENT", style::WORD_GROUP2);
    insert_word("SUFFIXES", style::WORD_GROUP2);
    insert_word("VPATH", style::WORD_GROUP2);
    insert_word("all", style::WORD_GROUP4);
    insert_word("clean", style::WORD_GROUP4);
    insert_word("depend", style::WORD_GROUP4);
    insert_word("distclean", style::WORD_GROUP4);
    insert_word("export", style::WORD_GROUP4);
    insert_word("include", style::WORD_GROUP4);
    insert_word("install", style::WORD_GROUP4);
    insert_word("makeinclude", style::WORD_GROUP4);
    insert_word("private", style::WORD_GROUP4);
    insert_word("unexport", style::WORD_GROUP4);
    insert_word("uninstall", style::WORD_GROUP4);
    insert_word("vpath", style::WORD_GROUP4);
    make_words();
}
StyleMarkup::StyleMarkup() : StyleDef(style::MARKUP) {
    _block_start       = "<!--";
    _block_start_size  = 4;
    _block_end         = "-->";
    _block_end_size    = 3;
    _raw_start[0]      = "<![CDATA[";
    _raw_start_size[0] = 9;
    _raw_end[0]        = "]]>";
    _raw_end_size[0]   = 3;
}
void StyleMarkup::_tag(int& start, int& end, int last, bool term) {
    auto e           = 0;
    auto c           = (unsigned) 0;
    auto p           = (unsigned) 0;
    auto s           = style::STYLE_FG;
    auto glob        = false;
    auto raw_end_c   = (unsigned) *_raw_end[0];
    auto raw_start_c = (unsigned) *_raw_start[0];
    _style->poke(start, style::STYLE_FG);
    if (_text->peek(start + 1) == '/') {
        _style->poke(++start, style::STYLE_FG);
    }
    while (start < end) {
        c = _text->peek(++start);
        _style->poke(start, style::STYLE_FG);
        if (_STYLE_START_TAG(c) == true) {
            while (start < end) {
                if (s == style::STYLE_FG) {
                    s = style::STYLE_KEYWORD;
                    glob = (term == false) ? _text->compare_lowercase(start, "script", 6) || _text->compare_lowercase(start, "style", 5) : false;
                }
                _style->poke(start, s);
                c = _text->peek(++start);
                if (_STYLE_VALID_TAG(c) == false) {
                    start--;
                    break;
                }
            }
            s = style::STYLE_TYPE;
        }
        else if (c == '"' || c == '\'') {
            auto term = c;
            _style->poke(start, style::STYLE_STRING);
            while (start < end) {
                c = _text->peek(++start);
                _style->poke(start, style::STYLE_STRING);
                if (c == term) {
                    break;
                }
            }
        }
        else if (c == '>') {
            _style->poke(start, style::STYLE_FG);
            if (glob == true) {
                while (start < end) {
                    c = _text->peek(++start);
                    if (c == raw_start_c && _text->compare(start, _raw_start[0], _raw_start_size[0])) {
                        _STYLE_STRING_RAW(_raw_start_size[0], raw_end_c, _raw_end[0], _raw_end_size[0], _raw_escape[0])
                        start++;
                    }
                    if (_text->peek(start) == '<' && _text->peek(start + 1) == '/') {
                        start--;
                        break;
                    }
                    _style->poke(start, style::STYLE_FG);
                }
            }
            break;
        }
        else {
            _style->poke(start, style::STYLE_FG);
        }
    }
}
int StyleMarkup::update(int, int, int, const char*, const char*, Editor*) {
    auto len = _text->length();
#ifdef DEBUG_STYLE
    printf("\nSTYLE_START: length=%4d\n", len);
#endif
    auto time  = gnu::Time::Milli();
    _update(0, len);
    time = gnu::Time::Milli() - time;
#ifdef DEBUG_STYLE
    printf("STYLE_END: %s in %d mS\n\n", _name.c_str(), (int) time);
    fflush(stdout);
#endif
    return time;
}
int StyleMarkup::_update(int start, int end) {
    auto block_end_c   = (unsigned) *_block_end;
    auto block_start_c = (unsigned) *_block_start;
    auto c             = (unsigned) 0;
    auto e             = 0;
    auto last          = _text->length();
    auto raw_end_c     = (unsigned) *_raw_end[0];
    auto raw_start_c   = (unsigned) *_raw_start[0];
    auto p             = (unsigned) 0;
    while (start < end) {
        c = _text->peek(start);
        if (c == raw_start_c && _text->compare(start, _raw_start[0], _raw_start_size[0])) {
            _STYLE_STRING_RAW(_raw_start_size[0], raw_end_c, _raw_end[0], _raw_end_size[0], _raw_escape[0])
        }
        else if (c == block_start_c && _text->compare(start, _block_start, _block_start_size)) {
            _STYLE_COMMENT_BLOCK()
        }
        else if (c == '<' && _STYLE_START_TAG(_text->peek(start + 1)) == true) {
            _tag(start, end, last, false);
        }
        else if (c == '<' && _text->peek(start + 1) == '/') {
            _tag(start, end, last, true);
        }
        else {
            _style->poke(start, style::STYLE_FG);
        }
        start++;
    }
    return start;
}
StylePHP::StylePHP() : StyleDef(style::PHP) {
    _single_quote_str  = '\'';
    _line_comment      = "//";
    _line_comment_size = 2;
    _block_start       = "/*";
    _block_start_size  = 2;
    _block_end         = "*/";
    _block_end_size    = 2;
    _raw_start[0]      = "<<HEREDOC";
    _raw_start_size[0] = 9;
    _raw_end[0]        = "HEREDOC";
    _raw_end_size[0]   = 7;
    _raw_start[0]      = "<<END";
    _raw_start_size[0] = 5;
    _raw_end[0]        = "END";
    _raw_end_size[0]   = 3;
    _hex               = true;
    _bin               = true;
    _oct               = true;
    _tokens.set('$', Token::VALUE);
    insert_word("__halt_compiler", style::WORD_GROUP1);
    insert_word("abstract", style::WORD_GROUP1);
    insert_word("and", style::WORD_GROUP1);
    insert_word("array", style::WORD_GROUP1);
    insert_word("as", style::WORD_GROUP1);
    insert_word("break", style::WORD_GROUP1);
    insert_word("callable", style::WORD_GROUP1);
    insert_word("case", style::WORD_GROUP1);
    insert_word("catch", style::WORD_GROUP1);
    insert_word("class", style::WORD_GROUP1);
    insert_word("clone", style::WORD_GROUP1);
    insert_word("const", style::WORD_GROUP1);
    insert_word("continue", style::WORD_GROUP1);
    insert_word("declare", style::WORD_GROUP1);
    insert_word("default", style::WORD_GROUP1);
    insert_word("die", style::WORD_GROUP1);
    insert_word("do", style::WORD_GROUP1);
    insert_word("echo", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("elseif", style::WORD_GROUP1);
    insert_word("empty", style::WORD_GROUP1);
    insert_word("enddeclare", style::WORD_GROUP1);
    insert_word("endfor", style::WORD_GROUP1);
    insert_word("endforeach", style::WORD_GROUP1);
    insert_word("endif", style::WORD_GROUP1);
    insert_word("endswitch", style::WORD_GROUP1);
    insert_word("endwhile", style::WORD_GROUP1);
    insert_word("eval", style::WORD_GROUP1);
    insert_word("exit", style::WORD_GROUP1);
    insert_word("extends", style::WORD_GROUP1);
    insert_word("final", style::WORD_GROUP1);
    insert_word("finally", style::WORD_GROUP1);
    insert_word("fn", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("foreach", style::WORD_GROUP1);
    insert_word("from", style::WORD_GROUP1);
    insert_word("function", style::WORD_GROUP1);
    insert_word("global", style::WORD_GROUP1);
    insert_word("goto", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("implements", style::WORD_GROUP1);
    insert_word("include", style::WORD_GROUP1);
    insert_word("include_once", style::WORD_GROUP1);
    insert_word("instanceof", style::WORD_GROUP1);
    insert_word("insteadof", style::WORD_GROUP1);
    insert_word("interface", style::WORD_GROUP1);
    insert_word("isset", style::WORD_GROUP1);
    insert_word("list", style::WORD_GROUP1);
    insert_word("match", style::WORD_GROUP1);
    insert_word("namespace", style::WORD_GROUP1);
    insert_word("new", style::WORD_GROUP1);
    insert_word("or", style::WORD_GROUP1);
    insert_word("print", style::WORD_GROUP1);
    insert_word("private", style::WORD_GROUP1);
    insert_word("protected", style::WORD_GROUP1);
    insert_word("public", style::WORD_GROUP1);
    insert_word("readonly", style::WORD_GROUP1);
    insert_word("require", style::WORD_GROUP1);
    insert_word("require_once", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("static", style::WORD_GROUP1);
    insert_word("switch", style::WORD_GROUP1);
    insert_word("throw", style::WORD_GROUP1);
    insert_word("trait", style::WORD_GROUP1);
    insert_word("try", style::WORD_GROUP1);
    insert_word("unset", style::WORD_GROUP1);
    insert_word("use", style::WORD_GROUP1);
    insert_word("var", style::WORD_GROUP1);
    insert_word("while", style::WORD_GROUP1);
    insert_word("xor", style::WORD_GROUP1);
    insert_word("yield", style::WORD_GROUP1);
    insert_word("FALSE", style::WORD_GROUP2);
    insert_word("NAN", style::WORD_GROUP2);
    insert_word("NULL", style::WORD_GROUP2);
    insert_word("PHP_FLOAT_MAX", style::WORD_GROUP2);
    insert_word("PHP_INT_MAX", style::WORD_GROUP2);
    insert_word("PHP_INT_MIN", style::WORD_GROUP2);
    insert_word("PHP_INT_SIZE", style::WORD_GROUP2);
    insert_word("TRUE", style::WORD_GROUP2);
    insert_word("bool", style::WORD_GROUP2);
    insert_word("false", style::WORD_GROUP2);
    insert_word("float", style::WORD_GROUP2);
    insert_word("int", style::WORD_GROUP2);
    insert_word("null", style::WORD_GROUP2);
    insert_word("object", style::WORD_GROUP2);
    insert_word("string", style::WORD_GROUP2);
    insert_word("true", style::WORD_GROUP2);
    insert_word("__CLASS__", style::WORD_GROUP4);
    insert_word("__DIR__", style::WORD_GROUP4);
    insert_word("__FILE__", style::WORD_GROUP4);
    insert_word("__FUNCTION__", style::WORD_GROUP4);
    insert_word("__LINE__", style::WORD_GROUP4);
    insert_word("__METHOD__", style::WORD_GROUP4);
    insert_word("__NAMESPACE__", style::WORD_GROUP4);
    insert_word("__TRAIT__", style::WORD_GROUP4);
    make_words();
}
StylePython::StylePython() : StyleDef(style::PYTHON) {
    _single_quote_str  = '\'';
    _line_comment      = "#";
    _line_comment_size = 1;
    _block_start       = "'''";
    _block_start_size  = 3;
    _block_end         = "'''";
    _block_end_size    = 3;
    _raw_start[0]      = "\"\"\"";
    _raw_start_size[0] = 3;
    _raw_end[0]        = "\"\"\"";
    _raw_end_size[0]   = 3;
    _raw_start[1]      = "'''";
    _raw_start_size[1] = 3;
    _raw_end[1]        = "'''";
    _raw_end_size[1]   = 3;
    insert_word("and", style::WORD_GROUP1);
    insert_word("as", style::WORD_GROUP1);
    insert_word("assert", style::WORD_GROUP1);
    insert_word("async", style::WORD_GROUP1);
    insert_word("await", style::WORD_GROUP1);
    insert_word("break", style::WORD_GROUP1);
    insert_word("class", style::WORD_GROUP1);
    insert_word("continue", style::WORD_GROUP1);
    insert_word("def", style::WORD_GROUP1);
    insert_word("del", style::WORD_GROUP1);
    insert_word("elif", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("except", style::WORD_GROUP1);
    insert_word("finally", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("from", style::WORD_GROUP1);
    insert_word("global", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("import", style::WORD_GROUP1);
    insert_word("in", style::WORD_GROUP1);
    insert_word("is", style::WORD_GROUP1);
    insert_word("lambda", style::WORD_GROUP1);
    insert_word("nonlocal", style::WORD_GROUP1);
    insert_word("not", style::WORD_GROUP1);
    insert_word("or", style::WORD_GROUP1);
    insert_word("pass", style::WORD_GROUP1);
    insert_word("raise", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("try", style::WORD_GROUP1);
    insert_word("while", style::WORD_GROUP1);
    insert_word("with", style::WORD_GROUP1);
    insert_word("yield", style::WORD_GROUP1);
    insert_word("Ellipsis", style::WORD_GROUP2);
    insert_word("False", style::WORD_GROUP2);
    insert_word("None", style::WORD_GROUP2);
    insert_word("True", style::WORD_GROUP2);
    insert_word("bool", style::WORD_GROUP2);
    insert_word("bytearray", style::WORD_GROUP2);
    insert_word("bytes", style::WORD_GROUP2);
    insert_word("classmethod", style::WORD_GROUP2);
    insert_word("cls", style::WORD_GROUP2);
    insert_word("complex", style::WORD_GROUP2);
    insert_word("dict", style::WORD_GROUP2);
    insert_word("enumerate", style::WORD_GROUP2);
    insert_word("filter", style::WORD_GROUP2);
    insert_word("float", style::WORD_GROUP2);
    insert_word("frozenset", style::WORD_GROUP2);
    insert_word("int", style::WORD_GROUP2);
    insert_word("list", style::WORD_GROUP2);
    insert_word("map", style::WORD_GROUP2);
    insert_word("memoryview", style::WORD_GROUP2);
    insert_word("object", style::WORD_GROUP2);
    insert_word("property", style::WORD_GROUP2);
    insert_word("range", style::WORD_GROUP2);
    insert_word("reversed", style::WORD_GROUP2);
    insert_word("self", style::WORD_GROUP2);
    insert_word("set", style::WORD_GROUP2);
    insert_word("slice", style::WORD_GROUP2);
    insert_word("staticmethod", style::WORD_GROUP2);
    insert_word("str", style::WORD_GROUP2);
    insert_word("super", style::WORD_GROUP2);
    insert_word("tuple", style::WORD_GROUP2);
    insert_word("type", style::WORD_GROUP2);
    insert_word("zip", style::WORD_GROUP2);
    insert_word("__bases__", style::WORD_GROUP4);
    insert_word("__builtin__", style::WORD_GROUP4);
    insert_word("__class__", style::WORD_GROUP4);
    insert_word("__debug__", style::WORD_GROUP4);
    insert_word("__dict__", style::WORD_GROUP4);
    insert_word("__doc__", style::WORD_GROUP4);
    insert_word("__file__", style::WORD_GROUP4);
    insert_word("__import__", style::WORD_GROUP4);
    insert_word("__members__", style::WORD_GROUP4);
    insert_word("__methods__", style::WORD_GROUP4);
    insert_word("__name__", style::WORD_GROUP4);
    insert_word("__self__", style::WORD_GROUP4);
    make_words();
}
StyleRuby::StyleRuby() : StyleDef(style::RUBY) {
    _single_quote_str  = '\'';
    _line_comment      = "#";
    _line_comment_size = 1;
    _block_start       = "=begin";
    _block_start_size  = 6;
    _block_end         = "=end";
    _block_end_size    = 4;
    _raw_start[0]      = "%q(";
    _raw_start_size[0] = 3;
    _raw_end[0]        = ")";
    _raw_end_size[0]   = 1;
    _raw_start[1]      = "%Q(";
    _raw_start_size[1] = 3;
    _raw_end[1]        = ")";
    _raw_end_size[1]   = 1;
    _raw_start[2]      = "<<HEREDOC";
    _raw_start_size[2] = 9;
    _raw_end[2]        = "HEREDOC";
    _raw_end_size[2]   = 7;
    _hex               = true;
    _bin               = true;
    _oct               = true;
    _tokens.set('@', Token::VALUE);
    _tokens.set('_', Token::IDENT2 | Token::IDENT1 | Token::NUM_SEPARATOR);
    insert_word("alias", style::WORD_GROUP1);
    insert_word("and", style::WORD_GROUP1);
    insert_word("begin", style::WORD_GROUP1);
    insert_word("break", style::WORD_GROUP1);
    insert_word("case", style::WORD_GROUP1);
    insert_word("class", style::WORD_GROUP1);
    insert_word("def", style::WORD_GROUP1);
    insert_word("defined?", style::WORD_GROUP1);
    insert_word("do", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("elsif", style::WORD_GROUP1);
    insert_word("end", style::WORD_GROUP1);
    insert_word("ensure", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("in", style::WORD_GROUP1);
    insert_word("module", style::WORD_GROUP1);
    insert_word("next", style::WORD_GROUP1);
    insert_word("not", style::WORD_GROUP1);
    insert_word("or", style::WORD_GROUP1);
    insert_word("private", style::WORD_GROUP1);
    insert_word("protected", style::WORD_GROUP1);
    insert_word("public", style::WORD_GROUP1);
    insert_word("raise", style::WORD_GROUP1);
    insert_word("redo", style::WORD_GROUP1);
    insert_word("rescue", style::WORD_GROUP1);
    insert_word("retry", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("super", style::WORD_GROUP1);
    insert_word("then", style::WORD_GROUP1);
    insert_word("undef", style::WORD_GROUP1);
    insert_word("unless", style::WORD_GROUP1);
    insert_word("until", style::WORD_GROUP1);
    insert_word("when", style::WORD_GROUP1);
    insert_word("while", style::WORD_GROUP1);
    insert_word("yield", style::WORD_GROUP1);
    insert_word("ARGV", style::WORD_GROUP2);
    insert_word("ArgumentError", style::WORD_GROUP2);
    insert_word("Array", style::WORD_GROUP2);
    insert_word("Bignum", style::WORD_GROUP2);
    insert_word("Class", style::WORD_GROUP2);
    insert_word("Comparable", style::WORD_GROUP2);
    insert_word("EOFError", style::WORD_GROUP2);
    insert_word("Encoding", style::WORD_GROUP2);
    insert_word("Enumerable", style::WORD_GROUP2);
    insert_word("Enumerator", style::WORD_GROUP2);
    insert_word("Errno", style::WORD_GROUP2);
    insert_word("Exception", style::WORD_GROUP2);
    insert_word("FALSE", style::WORD_GROUP2);
    insert_word("FalseClass", style::WORD_GROUP2);
    insert_word("Fiber", style::WORD_GROUP2);
    insert_word("File", style::WORD_GROUP2);
    insert_word("Fixnum", style::WORD_GROUP2);
    insert_word("Float", style::WORD_GROUP2);
    insert_word("FloatDomainError", style::WORD_GROUP2);
    insert_word("GC", style::WORD_GROUP2);
    insert_word("IO", style::WORD_GROUP2);
    insert_word("IOError", style::WORD_GROUP2);
    insert_word("IndexError", style::WORD_GROUP2);
    insert_word("Integer", style::WORD_GROUP2);
    insert_word("LoadError", style::WORD_GROUP2);
    insert_word("LocalJumpError", style::WORD_GROUP2);
    insert_word("Marshal", style::WORD_GROUP2);
    insert_word("Math", style::WORD_GROUP2);
    insert_word("Method", style::WORD_GROUP2);
    insert_word("Module", style::WORD_GROUP2);
    insert_word("NameError", style::WORD_GROUP2);
    insert_word("NilClass", style::WORD_GROUP2);
    insert_word("NoMemoryError", style::WORD_GROUP2);
    insert_word("NoMethodError", style::WORD_GROUP2);
    insert_word("NotImplementedError", style::WORD_GROUP2);
    insert_word("Numeric", style::WORD_GROUP2);
    insert_word("Object", style::WORD_GROUP2);
    insert_word("Range", style::WORD_GROUP2);
    insert_word("RangeError", style::WORD_GROUP2);
    insert_word("Regexp", style::WORD_GROUP2);
    insert_word("RegexpError", style::WORD_GROUP2);
    insert_word("RubyVM", style::WORD_GROUP2);
    insert_word("RuntimeError", style::WORD_GROUP2);
    insert_word("STDERR", style::WORD_GROUP2);
    insert_word("STDIN", style::WORD_GROUP2);
    insert_word("STDOUT", style::WORD_GROUP2);
    insert_word("ScriptError", style::WORD_GROUP2);
    insert_word("SecurityError", style::WORD_GROUP2);
    insert_word("StandardError", style::WORD_GROUP2);
    insert_word("String", style::WORD_GROUP2);
    insert_word("Struct", style::WORD_GROUP2);
    insert_word("Symbol", style::WORD_GROUP2);
    insert_word("SyntaxError", style::WORD_GROUP2);
    insert_word("SystemCallError", style::WORD_GROUP2);
    insert_word("SystemStackError", style::WORD_GROUP2);
    insert_word("TRUE", style::WORD_GROUP2);
    insert_word("Thread", style::WORD_GROUP2);
    insert_word("ThreadError", style::WORD_GROUP2);
    insert_word("Time", style::WORD_GROUP2);
    insert_word("TrueClass", style::WORD_GROUP2);
    insert_word("TypeError", style::WORD_GROUP2);
    insert_word("ZeroDivisionError", style::WORD_GROUP2);
    insert_word("false", style::WORD_GROUP2);
    insert_word("nil", style::WORD_GROUP2);
    insert_word("self", style::WORD_GROUP2);
    insert_word("true", style::WORD_GROUP2);
    insert_word("BEGIN", style::WORD_GROUP4);
    insert_word("END", style::WORD_GROUP4);
    insert_word("PLATFORM", style::WORD_GROUP4);
    insert_word("__ENCODING__", style::WORD_GROUP4);
    insert_word("__FILE__", style::WORD_GROUP4);
    insert_word("__LINE__", style::WORD_GROUP4);
    make_words();
}
StyleRust::StyleRust() : StyleDef(style::RUST) {
    _line_comment      = "//";
    _line_comment_size = 2;
    _block_start       = "/*";
    _block_start_size  = 2;
    _block_end         = "*/";
    _block_end_size    = 2;
    _raw_start[0]      = "r#\"";
    _raw_start_size[0] = 3;
    _raw_end[0]        = "\"#";
    _raw_end_size[0]   = 2;
    _raw_start[1]      = "r\"";
    _raw_start_size[1] = 2;
    _raw_end[1]        = "\"";
    _raw_end_size[1]   = 1;
    _raw_start[2]      = "b\"";
    _raw_start_size[2] = 2;
    _raw_end[2]        = "\"";
    _raw_end_size[2]   = 1;
    _raw_start[3]      = "br\"";
    _raw_start_size[3] = 3;
    _raw_end[3]        = "\"";
    _raw_end_size[3]   = 1;
    _tokens.set('_', Token::IDENT2 | Token::IDENT1 | Token::NUM_SEPARATOR);
    _tokens.set('#', Token::PRAGMA);
    insert_word("abstract", style::WORD_GROUP1);
    insert_word("alignof", style::WORD_GROUP1);
    insert_word("as", style::WORD_GROUP1);
    insert_word("async", style::WORD_GROUP1);
    insert_word("await", style::WORD_GROUP1);
    insert_word("become", style::WORD_GROUP1);
    insert_word("box", style::WORD_GROUP1);
    insert_word("break", style::WORD_GROUP1);
    insert_word("const", style::WORD_GROUP1);
    insert_word("continue", style::WORD_GROUP1);
    insert_word("crate", style::WORD_GROUP1);
    insert_word("default", style::WORD_GROUP1);
    insert_word("do", style::WORD_GROUP1);
    insert_word("dyn", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("enum", style::WORD_GROUP1);
    insert_word("extern", style::WORD_GROUP1);
    insert_word("final", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("impl", style::WORD_GROUP1);
    insert_word("in", style::WORD_GROUP1);
    insert_word("let", style::WORD_GROUP1);
    insert_word("loop", style::WORD_GROUP1);
    insert_word("macro", style::WORD_GROUP1);
    insert_word("match", style::WORD_GROUP1);
    insert_word("mod", style::WORD_GROUP1);
    insert_word("move", style::WORD_GROUP1);
    insert_word("mut", style::WORD_GROUP1);
    insert_word("offsetof", style::WORD_GROUP1);
    insert_word("override", style::WORD_GROUP1);
    insert_word("priv", style::WORD_GROUP1);
    insert_word("proc", style::WORD_GROUP1);
    insert_word("pub", style::WORD_GROUP1);
    insert_word("pure", style::WORD_GROUP1);
    insert_word("ref", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("sizeof", style::WORD_GROUP1);
    insert_word("static", style::WORD_GROUP1);
    insert_word("struct", style::WORD_GROUP1);
    insert_word("super", style::WORD_GROUP1);
    insert_word("trait", style::WORD_GROUP1);
    insert_word("try", style::WORD_GROUP1);
    insert_word("typeof", style::WORD_GROUP1);
    insert_word("union", style::WORD_GROUP1);
    insert_word("unsafe", style::WORD_GROUP1);
    insert_word("unsized", style::WORD_GROUP1);
    insert_word("use", style::WORD_GROUP1);
    insert_word("virtual", style::WORD_GROUP1);
    insert_word("where", style::WORD_GROUP1);
    insert_word("while", style::WORD_GROUP1);
    insert_word("yield", style::WORD_GROUP1);
    insert_word("BUFSIZ", style::WORD_GROUP2);
    insert_word("Box", style::WORD_GROUP2);
    insert_word("Cons", style::WORD_GROUP2);
    insert_word("DIGITS", style::WORD_GROUP2);
    insert_word("DIR", style::WORD_GROUP2);
    insert_word("EOF", style::WORD_GROUP2);
    insert_word("EPSILON", style::WORD_GROUP2);
    insert_word("EXIT_FAILURE", style::WORD_GROUP2);
    insert_word("EXIT_SUCCESS", style::WORD_GROUP2);
    insert_word("Err", style::WORD_GROUP2);
    insert_word("FILE", style::WORD_GROUP2);
    insert_word("FILENAME_MAX", style::WORD_GROUP2);
    insert_word("FOPEN_MAX", style::WORD_GROUP2);
    insert_word("F_OK", style::WORD_GROUP2);
    insert_word("Failure", style::WORD_GROUP2);
    insert_word("INFINITY", style::WORD_GROUP2);
    insert_word("L_tmpnam", style::WORD_GROUP2);
    insert_word("MAIN_SEPARATOR", style::WORD_GROUP2);
    insert_word("MANTISSA_DIGITS", style::WORD_GROUP2);
    insert_word("MAX", style::WORD_GROUP2);
    insert_word("MAX_10_EXP", style::WORD_GROUP2);
    insert_word("MAX_EXP", style::WORD_GROUP2);
    insert_word("MIN", style::WORD_GROUP2);
    insert_word("MIN_10_EXP", style::WORD_GROUP2);
    insert_word("MIN_EXP", style::WORD_GROUP2);
    insert_word("MIN_POSITIVE", style::WORD_GROUP2);
    insert_word("NAN", style::WORD_GROUP2);
    insert_word("NEG_INFINITY", style::WORD_GROUP2);
    insert_word("Nil", style::WORD_GROUP2);
    insert_word("None", style::WORD_GROUP2);
    insert_word("ONCE_INIT", style::WORD_GROUP2);
    insert_word("O_APPEND", style::WORD_GROUP2);
    insert_word("O_CREAT", style::WORD_GROUP2);
    insert_word("O_EXCL", style::WORD_GROUP2);
    insert_word("O_RDONLY", style::WORD_GROUP2);
    insert_word("O_RDWR", style::WORD_GROUP2);
    insert_word("O_TRUNC", style::WORD_GROUP2);
    insert_word("O_WRONLY", style::WORD_GROUP2);
    insert_word("Ok", style::WORD_GROUP2);
    insert_word("Option", style::WORD_GROUP2);
    insert_word("Path", style::WORD_GROUP2);
    insert_word("PathBuf", style::WORD_GROUP2);
    insert_word("RADIX", style::WORD_GROUP2);
    insert_word("RAND_MAX", style::WORD_GROUP2);
    insert_word("REPLACEMENT_CHARACTER", style::WORD_GROUP2);
    insert_word("R_OK", style::WORD_GROUP2);
    insert_word("Result", style::WORD_GROUP2);
    insert_word("SEEK_CUR", style::WORD_GROUP2);
    insert_word("SEEK_END", style::WORD_GROUP2);
    insert_word("SEEK_SET", style::WORD_GROUP2);
    insert_word("STDERR_FILENO", style::WORD_GROUP2);
    insert_word("STDIN_FILENO", style::WORD_GROUP2);
    insert_word("STDOUT_FILENO", style::WORD_GROUP2);
    insert_word("S_IEXEC", style::WORD_GROUP2);
    insert_word("S_IFBLK", style::WORD_GROUP2);
    insert_word("S_IFCHR", style::WORD_GROUP2);
    insert_word("S_IFDIR", style::WORD_GROUP2);
    insert_word("S_IFIFO", style::WORD_GROUP2);
    insert_word("S_IFMT", style::WORD_GROUP2);
    insert_word("S_IFREG", style::WORD_GROUP2);
    insert_word("S_IREAD", style::WORD_GROUP2);
    insert_word("S_IRUSR", style::WORD_GROUP2);
    insert_word("S_IRWXU", style::WORD_GROUP2);
    insert_word("S_IWRITE", style::WORD_GROUP2);
    insert_word("S_IWUSR", style::WORD_GROUP2);
    insert_word("S_IXUSR", style::WORD_GROUP2);
    insert_word("Self", style::WORD_GROUP2);
    insert_word("Some", style::WORD_GROUP2);
    insert_word("String", style::WORD_GROUP2);
    insert_word("Success", style::WORD_GROUP2);
    insert_word("TMP_MAX", style::WORD_GROUP2);
    insert_word("UNICODE_VERSION", style::WORD_GROUP2);
    insert_word("UNIX_EPOCH", style::WORD_GROUP2);
    insert_word("Vec", style::WORD_GROUP2);
    insert_word("W_OK", style::WORD_GROUP2);
    insert_word("X_OK", style::WORD_GROUP2);
    insert_word("_IOFBF", style::WORD_GROUP2);
    insert_word("_IOLBF", style::WORD_GROUP2);
    insert_word("_IONBF", style::WORD_GROUP2);
    insert_word("bool", style::WORD_GROUP2);
    insert_word("c_char", style::WORD_GROUP2);
    insert_word("c_double", style::WORD_GROUP2);
    insert_word("c_float", style::WORD_GROUP2);
    insert_word("c_int", style::WORD_GROUP2);
    insert_word("c_long", style::WORD_GROUP2);
    insert_word("c_longlong", style::WORD_GROUP2);
    insert_word("c_schar", style::WORD_GROUP2);
    insert_word("c_short", style::WORD_GROUP2);
    insert_word("c_uchar", style::WORD_GROUP2);
    insert_word("c_uint", style::WORD_GROUP2);
    insert_word("c_ulong", style::WORD_GROUP2);
    insert_word("c_ulonglong", style::WORD_GROUP2);
    insert_word("c_ushort", style::WORD_GROUP2);
    insert_word("c_void", style::WORD_GROUP2);
    insert_word("char", style::WORD_GROUP2);
    insert_word("clock_t", style::WORD_GROUP2);
    insert_word("dev_t", style::WORD_GROUP2);
    insert_word("dirent", style::WORD_GROUP2);
    insert_word("f32", style::WORD_GROUP2);
    insert_word("f64", style::WORD_GROUP2);
    insert_word("false", style::WORD_GROUP2);
    insert_word("fpos_t", style::WORD_GROUP2);
    insert_word("i128", style::WORD_GROUP2);
    insert_word("i16", style::WORD_GROUP2);
    insert_word("i32", style::WORD_GROUP2);
    insert_word("i64", style::WORD_GROUP2);
    insert_word("i8", style::WORD_GROUP2);
    insert_word("ino_t", style::WORD_GROUP2);
    insert_word("intptr_t", style::WORD_GROUP2);
    insert_word("isize", style::WORD_GROUP2);
    insert_word("mode_t", style::WORD_GROUP2);
    insert_word("off_t", style::WORD_GROUP2);
    insert_word("pid_t", style::WORD_GROUP2);
    insert_word("ptrdiff_t", style::WORD_GROUP2);
    insert_word("self", style::WORD_GROUP2);
    insert_word("size_t", style::WORD_GROUP2);
    insert_word("ssize_t", style::WORD_GROUP2);
    insert_word("str", style::WORD_GROUP2);
    insert_word("time_t", style::WORD_GROUP2);
    insert_word("true", style::WORD_GROUP2);
    insert_word("u128", style::WORD_GROUP2);
    insert_word("u16", style::WORD_GROUP2);
    insert_word("u32", style::WORD_GROUP2);
    insert_word("u64", style::WORD_GROUP2);
    insert_word("u8", style::WORD_GROUP2);
    insert_word("uintptr_t", style::WORD_GROUP2);
    insert_word("usize", style::WORD_GROUP2);
    insert_word("Add", style::WORD_GROUP8);
    insert_word("AddAssign", style::WORD_GROUP8);
    insert_word("Alloc", style::WORD_GROUP8);
    insert_word("Any", style::WORD_GROUP8);
    insert_word("AsMut", style::WORD_GROUP8);
    insert_word("AsRef", style::WORD_GROUP8);
    insert_word("AsSlice", style::WORD_GROUP8);
    insert_word("Binary", style::WORD_GROUP8);
    insert_word("BitAnd", style::WORD_GROUP8);
    insert_word("BitAndAssign", style::WORD_GROUP8);
    insert_word("BitOr", style::WORD_GROUP8);
    insert_word("BitOrAssign", style::WORD_GROUP8);
    insert_word("BitXor", style::WORD_GROUP8);
    insert_word("BitXorAssign", style::WORD_GROUP8);
    insert_word("Borrow", style::WORD_GROUP8);
    insert_word("BorrowMut", style::WORD_GROUP8);
    insert_word("BufRead", style::WORD_GROUP8);
    insert_word("BuildHasher", style::WORD_GROUP8);
    insert_word("CharExt", style::WORD_GROUP8);
    insert_word("Clone", style::WORD_GROUP8);
    insert_word("CoerceUnsized", style::WORD_GROUP8);
    insert_word("Copy", style::WORD_GROUP8);
    insert_word("Debug", style::WORD_GROUP8);
    insert_word("Decodable", style::WORD_GROUP8);
    insert_word("Default", style::WORD_GROUP8);
    insert_word("Deref", style::WORD_GROUP8);
    insert_word("DerefMut", style::WORD_GROUP8);
    insert_word("DispatchFromDyn", style::WORD_GROUP8);
    insert_word("Display", style::WORD_GROUP8);
    insert_word("Div", style::WORD_GROUP8);
    insert_word("DivAssign", style::WORD_GROUP8);
    insert_word("DoubleEndedIterator", style::WORD_GROUP8);
    insert_word("Drop", style::WORD_GROUP8);
    insert_word("Encodable", style::WORD_GROUP8);
    insert_word("Eq", style::WORD_GROUP8);
    insert_word("Error", style::WORD_GROUP8);
    insert_word("ExactSizeIterator", style::WORD_GROUP8);
    insert_word("Extend", style::WORD_GROUP8);
    insert_word("FixedSizeArray", style::WORD_GROUP8);
    insert_word("Fn", style::WORD_GROUP8);
    insert_word("FnBox", style::WORD_GROUP8);
    insert_word("FnMut", style::WORD_GROUP8);
    insert_word("FnOnce", style::WORD_GROUP8);
    insert_word("From", style::WORD_GROUP8);
    insert_word("FromIterator", style::WORD_GROUP8);
    insert_word("FromPrimitive", style::WORD_GROUP8);
    insert_word("FromStr", style::WORD_GROUP8);
    insert_word("FusedIterator", style::WORD_GROUP8);
    insert_word("Future", style::WORD_GROUP8);
    insert_word("Generator", style::WORD_GROUP8);
    insert_word("GlobalAlloc", style::WORD_GROUP8);
    insert_word("Hash", style::WORD_GROUP8);
    insert_word("Hasher", style::WORD_GROUP8);
    insert_word("Index", style::WORD_GROUP8);
    insert_word("IndexMut", style::WORD_GROUP8);
    insert_word("Into", style::WORD_GROUP8);
    insert_word("IntoIterator", style::WORD_GROUP8);
    insert_word("Iterator", style::WORD_GROUP8);
    insert_word("IteratorExt", style::WORD_GROUP8);
    insert_word("LowerExp", style::WORD_GROUP8);
    insert_word("LowerHex", style::WORD_GROUP8);
    insert_word("Mul", style::WORD_GROUP8);
    insert_word("MulAssign", style::WORD_GROUP8);
    insert_word("MultiSpan", style::WORD_GROUP8);
    insert_word("MutPtrExt", style::WORD_GROUP8);
    insert_word("Neg", style::WORD_GROUP8);
    insert_word("Not", style::WORD_GROUP8);
    insert_word("Octal", style::WORD_GROUP8);
    insert_word("Ord", style::WORD_GROUP8);
    insert_word("PartialEq", style::WORD_GROUP8);
    insert_word("PartialOrd", style::WORD_GROUP8);
    insert_word("Pattern", style::WORD_GROUP8);
    insert_word("Pointer", style::WORD_GROUP8);
    insert_word("Product", style::WORD_GROUP8);
    insert_word("PtrExt", style::WORD_GROUP8);
    insert_word("Rand", style::WORD_GROUP8);
    insert_word("RangeBounds", style::WORD_GROUP8);
    insert_word("Read", style::WORD_GROUP8);
    insert_word("RefUnwindSafe", style::WORD_GROUP8);
    insert_word("Rem", style::WORD_GROUP8);
    insert_word("RemAssign", style::WORD_GROUP8);
    insert_word("Seek", style::WORD_GROUP8);
    insert_word("Send", style::WORD_GROUP8);
    insert_word("Shl", style::WORD_GROUP8);
    insert_word("ShlAssign", style::WORD_GROUP8);
    insert_word("Shr", style::WORD_GROUP8);
    insert_word("ShrAssign", style::WORD_GROUP8);
    insert_word("Sized", style::WORD_GROUP8);
    insert_word("SliceConcatExt", style::WORD_GROUP8);
    insert_word("SliceExt", style::WORD_GROUP8);
    insert_word("SliceIndex", style::WORD_GROUP8);
    insert_word("Step", style::WORD_GROUP8);
    insert_word("Str", style::WORD_GROUP8);
    insert_word("StrExt", style::WORD_GROUP8);
    insert_word("Sub", style::WORD_GROUP8);
    insert_word("SubAssign", style::WORD_GROUP8);
    insert_word("Sum", style::WORD_GROUP8);
    insert_word("Sync", style::WORD_GROUP8);
    insert_word("TDynBenchFn", style::WORD_GROUP8);
    insert_word("Termination", style::WORD_GROUP8);
    insert_word("ToOwned", style::WORD_GROUP8);
    insert_word("ToSocketAddrs", style::WORD_GROUP8);
    insert_word("ToString", style::WORD_GROUP8);
    insert_word("TrustedLen", style::WORD_GROUP8);
    insert_word("Try", style::WORD_GROUP8);
    insert_word("TryFrom", style::WORD_GROUP8);
    insert_word("TryInto", style::WORD_GROUP8);
    insert_word("Unpin", style::WORD_GROUP8);
    insert_word("Unsize", style::WORD_GROUP8);
    insert_word("UnwindSafe", style::WORD_GROUP8);
    insert_word("UpperExp", style::WORD_GROUP8);
    insert_word("UpperHex", style::WORD_GROUP8);
    insert_word("Write", style::WORD_GROUP8);
    make_words();
}
StyleShell::StyleShell() : StyleDef(style::SHELL) {
    _single_quote_str  = '\'';
    _line_comment      = "#";
    _line_comment_size = 1;
    _raw_start[0]      = "'";
    _raw_start_size[0] = 1;
    _raw_end[0]        = "'";
    _raw_end_size[0]   = 1;
    insert_word("case", style::WORD_GROUP1);
    insert_word("do", style::WORD_GROUP1);
    insert_word("done", style::WORD_GROUP1);
    insert_word("elif", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("esac", style::WORD_GROUP1);
    insert_word("eval", style::WORD_GROUP1);
    insert_word("exit", style::WORD_GROUP1);
    insert_word("fi", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("function", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("in", style::WORD_GROUP1);
    insert_word("local", style::WORD_GROUP1);
    insert_word("read", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("select", style::WORD_GROUP1);
    insert_word("shift", style::WORD_GROUP1);
    insert_word("then", style::WORD_GROUP1);
    insert_word("until", style::WORD_GROUP1);
    insert_word("while", style::WORD_GROUP1);
    insert_word("awk", style::WORD_GROUP2);
    insert_word("base64", style::WORD_GROUP4);
    insert_word("basename", style::WORD_GROUP4);
    insert_word("bash", style::WORD_GROUP4);
    insert_word("cat", style::WORD_GROUP4);
    insert_word("cd", style::WORD_GROUP4);
    insert_word("chcon", style::WORD_GROUP4);
    insert_word("chgrp", style::WORD_GROUP4);
    insert_word("chmod", style::WORD_GROUP4);
    insert_word("chown", style::WORD_GROUP4);
    insert_word("chroot", style::WORD_GROUP4);
    insert_word("cksum", style::WORD_GROUP4);
    insert_word("comm", style::WORD_GROUP4);
    insert_word("cp", style::WORD_GROUP4);
    insert_word("csplit", style::WORD_GROUP4);
    insert_word("cut", style::WORD_GROUP4);
    insert_word("dash", style::WORD_GROUP4);
    insert_word("date", style::WORD_GROUP4);
    insert_word("dd", style::WORD_GROUP4);
    insert_word("df", style::WORD_GROUP4);
    insert_word("dir", style::WORD_GROUP4);
    insert_word("dircolors", style::WORD_GROUP4);
    insert_word("dirname", style::WORD_GROUP4);
    insert_word("du", style::WORD_GROUP4);
    insert_word("echo", style::WORD_GROUP4);
    insert_word("env", style::WORD_GROUP4);
    insert_word("expand", style::WORD_GROUP4);
    insert_word("export", style::WORD_GROUP4);
    insert_word("expr", style::WORD_GROUP4);
    insert_word("factor", style::WORD_GROUP4);
    insert_word("false", style::WORD_GROUP4);
    insert_word("find", style::WORD_GROUP4);
    insert_word("fmt", style::WORD_GROUP4);
    insert_word("fold", style::WORD_GROUP4);
    insert_word("grep", style::WORD_GROUP4);
    insert_word("head", style::WORD_GROUP4);
    insert_word("hostid", style::WORD_GROUP4);
    insert_word("id", style::WORD_GROUP4);
    insert_word("install", style::WORD_GROUP4);
    insert_word("join", style::WORD_GROUP4);
    insert_word("kill", style::WORD_GROUP4);
    insert_word("killall", style::WORD_GROUP4);
    insert_word("less", style::WORD_GROUP4);
    insert_word("let", style::WORD_GROUP4);
    insert_word("link", style::WORD_GROUP4);
    insert_word("ln", style::WORD_GROUP4);
    insert_word("logname", style::WORD_GROUP4);
    insert_word("ls", style::WORD_GROUP4);
    insert_word("make", style::WORD_GROUP4);
    insert_word("md5sum", style::WORD_GROUP4);
    insert_word("mkdir", style::WORD_GROUP4);
    insert_word("mkfifo", style::WORD_GROUP4);
    insert_word("mknod", style::WORD_GROUP4);
    insert_word("mktemp", style::WORD_GROUP4);
    insert_word("mv", style::WORD_GROUP4);
    insert_word("nice", style::WORD_GROUP4);
    insert_word("nl", style::WORD_GROUP4);
    insert_word("nohup", style::WORD_GROUP4);
    insert_word("nproc", style::WORD_GROUP4);
    insert_word("numfmt", style::WORD_GROUP4);
    insert_word("od", style::WORD_GROUP4);
    insert_word("paste", style::WORD_GROUP4);
    insert_word("pathchk", style::WORD_GROUP4);
    insert_word("pinky", style::WORD_GROUP4);
    insert_word("pkill", style::WORD_GROUP4);
    insert_word("pr", style::WORD_GROUP4);
    insert_word("printenv", style::WORD_GROUP4);
    insert_word("printf", style::WORD_GROUP4);
    insert_word("ptx", style::WORD_GROUP4);
    insert_word("pwd", style::WORD_GROUP4);
    insert_word("readlink", style::WORD_GROUP4);
    insert_word("realpath", style::WORD_GROUP4);
    insert_word("rm", style::WORD_GROUP4);
    insert_word("rmdir", style::WORD_GROUP4);
    insert_word("runcon", style::WORD_GROUP4);
    insert_word("sed", style::WORD_GROUP4);
    insert_word("seq", style::WORD_GROUP4);
    insert_word("set", style::WORD_GROUP4);
    insert_word("sh", style::WORD_GROUP4);
    insert_word("sha1", style::WORD_GROUP4);
    insert_word("sha224", style::WORD_GROUP4);
    insert_word("sha256", style::WORD_GROUP4);
    insert_word("sha384", style::WORD_GROUP4);
    insert_word("sha512", style::WORD_GROUP4);
    insert_word("shred", style::WORD_GROUP4);
    insert_word("shuf", style::WORD_GROUP4);
    insert_word("sleep", style::WORD_GROUP4);
    insert_word("sort", style::WORD_GROUP4);
    insert_word("split", style::WORD_GROUP4);
    insert_word("stat", style::WORD_GROUP4);
    insert_word("stdbuf", style::WORD_GROUP4);
    insert_word("stty", style::WORD_GROUP4);
    insert_word("sum", style::WORD_GROUP4);
    insert_word("sync", style::WORD_GROUP4);
    insert_word("tac", style::WORD_GROUP4);
    insert_word("tail", style::WORD_GROUP4);
    insert_word("tar", style::WORD_GROUP4);
    insert_word("tee", style::WORD_GROUP4);
    insert_word("test", style::WORD_GROUP4);
    insert_word("time", style::WORD_GROUP4);
    insert_word("timeout", style::WORD_GROUP4);
    insert_word("touch", style::WORD_GROUP4);
    insert_word("tr", style::WORD_GROUP4);
    insert_word("true", style::WORD_GROUP4);
    insert_word("truncate", style::WORD_GROUP4);
    insert_word("tsort", style::WORD_GROUP4);
    insert_word("tty", style::WORD_GROUP4);
    insert_word("umask", style::WORD_GROUP4);
    insert_word("uname", style::WORD_GROUP4);
    insert_word("unexpand", style::WORD_GROUP4);
    insert_word("uniq", style::WORD_GROUP4);
    insert_word("unlink", style::WORD_GROUP4);
    insert_word("unset", style::WORD_GROUP4);
    insert_word("users", style::WORD_GROUP4);
    insert_word("vdir", style::WORD_GROUP4);
    insert_word("wc", style::WORD_GROUP4);
    insert_word("who", style::WORD_GROUP4);
    insert_word("whoami", style::WORD_GROUP4);
    insert_word("yes", style::WORD_GROUP4);
    make_words();
}
StyleTS::StyleTS() : StyleJS(style::TS) {
    _tokens.set('_', Token::IDENT2 | Token::IDENT1 | Token::NUM_SEPARATOR);
    insert_word("abstract", style::WORD_GROUP1);
    insert_word("constructor", style::WORD_GROUP1);
    insert_word("declare", style::WORD_GROUP1);
    insert_word("goto", style::WORD_GROUP1);
    insert_word("readonly", style::WORD_GROUP1);
    insert_word("type", style::WORD_GROUP1);
    insert_word("any", style::WORD_GROUP2);
    insert_word("bigint", style::WORD_GROUP2);
    insert_word("boolean", style::WORD_GROUP2);
    insert_word("never", style::WORD_GROUP2);
    insert_word("number", style::WORD_GROUP2);
    insert_word("object", style::WORD_GROUP2);
    insert_word("string", style::WORD_GROUP2);
    insert_word("symbol", style::WORD_GROUP2);
    insert_word("unknown", style::WORD_GROUP2);
    insert_word("module", style::WORD_GROUP4);
    insert_word("namespace", style::WORD_GROUP4);
    make_words();
}
StyleWren::StyleWren() : StyleDef(style::WREN) {
    _line_comment      = "//";
    _line_comment_size = 2;
    _block_start       = "/*";
    _block_start_size  = 2;
    _block_end         = "*/";
    _block_end_size    = 2;
    _hex               = true;
    _raw_start[0]      = "\"\"\"";
    _raw_start_size[0] = 3;
    _raw_end[0]        = "\"\"\"";
    _raw_end_size[0]   = 3;
    insert_word("as", style::WORD_GROUP1);
    insert_word("break", style::WORD_GROUP1);
    insert_word("class", style::WORD_GROUP1);
    insert_word("construct", style::WORD_GROUP1);
    insert_word("continue", style::WORD_GROUP1);
    insert_word("else", style::WORD_GROUP1);
    insert_word("for", style::WORD_GROUP1);
    insert_word("foreign", style::WORD_GROUP1);
    insert_word("if", style::WORD_GROUP1);
    insert_word("in", style::WORD_GROUP1);
    insert_word("is", style::WORD_GROUP1);
    insert_word("return", style::WORD_GROUP1);
    insert_word("static", style::WORD_GROUP1);
    insert_word("super", style::WORD_GROUP1);
    insert_word("this", style::WORD_GROUP1);
    insert_word("var", style::WORD_GROUP1);
    insert_word("while", style::WORD_GROUP1);
    insert_word("Directory", style::WORD_GROUP2);
    insert_word("Fiber", style::WORD_GROUP2);
    insert_word("File", style::WORD_GROUP2);
    insert_word("FileFlags", style::WORD_GROUP2);
    insert_word("Fn", style::WORD_GROUP2);
    insert_word("List", style::WORD_GROUP2);
    insert_word("Map", style::WORD_GROUP2);
    insert_word("Meta", style::WORD_GROUP2);
    insert_word("Null", style::WORD_GROUP2);
    insert_word("Num", style::WORD_GROUP2);
    insert_word("Random", style::WORD_GROUP2);
    insert_word("Stat", style::WORD_GROUP2);
    insert_word("Stdin", style::WORD_GROUP2);
    insert_word("Stdout", style::WORD_GROUP2);
    insert_word("System", style::WORD_GROUP2);
    insert_word("false", style::WORD_GROUP2);
    insert_word("null", style::WORD_GROUP2);
    insert_word("true", style::WORD_GROUP2);
    insert_word("import", style::WORD_GROUP4);
    make_words();
}
}
#include <algorithm>
#define FLE_CLEAR_CHANGED_FLAG
namespace fle {
static bool _textbuffer_pair(char c, char& e, bool& forward) {
    switch(c) {
    case '[':
        e = ']';
        break;
    case '{':
        e = '}';
        break;
    case '(':
        e = ')';
        break;
    case '<':
        e = '>';
        break;
    case ']':
        e = '[';
        forward = false;
        break;
    case '}':
        e = '{';
        forward = false;
        break;
    case ')':
        e = '(';
        forward = false;
        break;
    case '>':
        e = '<';
        forward = false;
        break;
    default:
        return false;
    }
    return true;
}
BufferController::BufferController(TextBuffer* buffer, int timeout, bool start_group_lock) {
    _buffer  = buffer;
    _running = false;
    _stopped = false;
    _time    = gnu::Time::Milli();
    _timeout = timeout;
    _wc      = nullptr;
    if (_buffer->has_fle_undo() == true && start_group_lock == true) {
        _buffer->_undo->group_lock();
    }
}
void BufferController::check_timeout() {
    auto ed = _buffer->_editor;
    if (_running == true) {
        return;
    }
    else if (_timeout == 0) {
    }
    else if (_timeout > 0 && gnu::Time::Milli() - _time > (int64_t) _timeout) {
    }
    else {
        return;
    }
    _wc      = new flw::WaitCursor();
    _running = true;
    if (ed != nullptr) {
        ed->callback_disconnect();
        ed->view1()->buffer(nullptr);
        if (ed->view2() != nullptr) {
            ed->view2()->buffer(nullptr);
        }
        _buffer->callback_connect();
    }
#ifdef DEBUG_
    ::printf("BufferController::check_timeout: (ON)\n");
    ::printf("    timeout            = %d mS\n", _timeout);
    ::printf("    time               = %d mS\n", (int) (gnu::Time::Milli() - _time));
    ::printf("    _count_changes     = %d\n", (int) _buffer.count_changes());
    fflush(stdout);
#endif
}
void BufferController::stop() {
    auto ed = _buffer->_editor;
    if (_stopped == false) {
        _stopped = true;
        if (_running == true) {
            _running = false;
            if (ed != nullptr) {
                _buffer->callback_disconnect();
                ed->view1()->buffer(_buffer);
                if (ed->view2() != nullptr) {
                    ed->view2()->buffer(_buffer);
                }
                ed->callback_connect();
                ed->style_resize_buffer();
                ed->style().update();
            }
        }
        if (_buffer->has_fle_undo() == true && _buffer->_undo->is_group_locked() == true) {
            if (_buffer->count_changes() > 0) {
                _buffer->_undo->group_unlock_and_add();
            }
            else {
                _buffer->_undo->group_unlock();
            }
        }
        _time = gnu::Time::Milli() - _time;
    }
    delete _wc;
    _wc = nullptr;
}
int TextBuffer::TIMEOUT_LONG  = 200;
int TextBuffer::TIMEOUT_SHORT =  50;
TextBuffer::TextBuffer(Editor* editor, Config& config) : Fl_Text_Buffer(4'096, 8'192), _config(config) {
    _count_changes = 0;
    _dirty         = false;
    _editor        = editor;
    _fdelkey       = FDELKEY::NIL;
    _fletcher64    = 0;
    _pause_undo    = false;
    _style_text    = false;
    _undo          = nullptr;
    _word          = Token::MakeWord();
    undo_set_mode_using_config();
}
TextBuffer::~TextBuffer() {
    delete _undo;
}
uint32_t TextBuffer::adler32() const {
    auto a   = (uint32_t) 1;
    auto b   = (uint32_t) 0;
    for (int f = 0; f < mLength; f++) {
        a += peek(f);
        b += a;
    }
    a %= 65521;
    b %= 65521;
    return (b << 16) | a;
}
uint64_t TextBuffer::calc_fletcher64() const {
    int       dwords = mLength / 4;
    uint64_t  sum1   = 0;
    uint64_t  sum2   = 0;
    uint32_t  num    = 0;
    char*     nump   = reinterpret_cast<char*>(&num);
    for (int f = 0, pos = 0; f < dwords; ++f) {
        nump[0] = peek(pos++);
        nump[1] = peek(pos++);
        nump[2] = peek(pos++);
        nump[3] = peek(pos++);
        sum1     = (sum1 + num) % UINT32_MAX;
        sum2     = (sum2 + sum1) % UINT32_MAX;
    }
    auto left = mLength - dwords * 4;
    if (left > 0 && left < 4) {
        for (int f = 0; f < 4; ++f) {
            nump[f] = (f < left) ? peek(dwords * 4 + f) : 0;
        }
        sum1 = (sum1 + num) % UINT32_MAX;
        sum2 = (sum2 + sum1) % UINT32_MAX;
    }
    return (sum2 << 32) | sum1;
}
CursorPos TextBuffer::case_for_selection(FCASE fcase) {
    _count_changes = 0;
    auto cursor = _editor->cursor(false);
    if (cursor.text_has_selection() == false) {
        return CursorPos();
    }
    auto text = text_range(cursor.start, cursor.end);
    auto len  = strlen(text);
    auto buf  = gnu::File::Allocate(nullptr, len * 4 + 1);
    if (fcase == FCASE::LOWER) {
        fl_utf_tolower(reinterpret_cast<const unsigned char*>(text), len, buf);
    }
    else {
        fl_utf_toupper(reinterpret_cast<const unsigned char*>(text), len, buf);
    }
    if (strcmp(text, buf) == 0) {
        free(text);
        free(buf);
        return CursorPos();
    }
    replace_selection(buf);
    free(text);
    free(buf);
    cursor.set_drag();
    return cursor;
}
void TextBuffer::CallbackUndo(const int pos, const int inserted_size, const int deleted_size, const int restyled_size, const char* deleted_text, void* o) {
    (void) restyled_size;
    assert(pos >= 0);
    auto buffer = static_cast<TextBuffer*>(o);
    auto editor = buffer->_editor;
    buffer->_style_text = false;
    if (inserted_size == 0 && deleted_size == 0) {
        return;
    }
    char* inserted_text = nullptr;
    if (inserted_size > 0) {
        inserted_text = buffer->text_range(pos, pos + inserted_size);
    }
    if (inserted_size == deleted_size && memcmp(inserted_text, deleted_text, inserted_size) == 0) {
        free(inserted_text);
        return;
    }
    buffer->_style_text = true;
    if (buffer->undo_mode() == FUNDO::FLE) {
        auto undo = buffer->_undo;
        if (buffer->_pause_undo == false) {
            if (editor != nullptr) {
                undo->add(buffer->_fdelkey, editor->text_has_selection(), pos, inserted_text, inserted_size, deleted_text, deleted_size);
            }
            else {
                undo->add(buffer->_fdelkey, buffer->has_selection(), pos, inserted_text, inserted_size, deleted_text, deleted_size);
            }
        }
    }
    buffer->_has_selection = false;
    buffer->clear_key();
    buffer->_count_changes++;
    if (editor != nullptr) {
        if (inserted_size != deleted_size) {
            editor->bookmarks().update(pos, inserted_size, deleted_size);
        }
        buffer->set_dirty(true);
    }
    free(inserted_text);
}
CursorPos TextBuffer::comment_block(std::string comment_start, std::string comment_end) {
    _count_changes = 0;
    if (comment_start == "" || comment_end == "") {
        return CursorPos();
    }
    auto cursor = _editor->cursor(false);
    if (cursor.text_has_selection() == false) {
        return CursorPos();
    }
    auto len    = cursor.len();
    auto adjust = 0;
    if (has_fle_undo() == true) {
        _undo->group_lock();
        _undo->add_custom1(gnu::str::format("%d %d %d", cursor.pos1, cursor.start, cursor.end));
    }
    _editor->style().pause(true);
    if (len >= (int) comment_start.length() + (int) comment_end.length() &&
        compare(cursor.start, comment_start.c_str(), comment_start.length()) == true &&
        compare(cursor.end - comment_end.length(), comment_end.c_str(), comment_end.length()) == true) {
        remove(cursor.end - comment_end.length(), cursor.end);
        remove(cursor.start, cursor.start + comment_start.length());
        cursor.pos1  -= (cursor.pos1 == cursor.end) ? (comment_start.length() + comment_end.length()) : 0;
        adjust = -1;
    }
    else if (cursor.start >= (int) comment_start.length() &&
        cursor.end <= length() - (int) comment_end.length() &&
        compare(cursor.start - comment_start.length(), comment_start.c_str(), comment_start.length()) == true &&
        compare(cursor.end, comment_end.c_str(), comment_end.length()) == true) {
        remove(cursor.end, cursor.end + comment_end.length());
        remove(cursor.start - comment_start.length(), cursor.start);
        cursor.pos1  -= comment_start.length();
        adjust = -1;
    }
    else {
        insert(cursor.start, comment_start.c_str());
        insert(cursor.end + comment_start.length(), comment_end.c_str());
        cursor.pos1  += comment_start.length();
        adjust = 1;
    }
    selection_position(&cursor.start, &cursor.end);
    if (has_fle_undo() == true) {
        _undo->add_custom1(gnu::str::format("%d %d %d", cursor.pos1, cursor.start, cursor.end));
        _undo->group_unlock_and_add();
    }
    _editor->style().pause(false);
    _editor->style().update();
    if (cursor.pos2 > cursor.start) {
        cursor.pos2 += (adjust < 0) ? -comment_start.length() : comment_start.length();
    }
    if (cursor.pos2 > cursor.end) {
        cursor.pos2 += (adjust < 0) ? -comment_end.length() : comment_end.length();
    }
    cursor.set_drag();
    return cursor;
}
CursorPos TextBuffer::comment_line(std::string comment) {
    _count_changes = 0;
    auto start2    = 0;
    auto end2      = 0;
    auto cursor    = _editor->cursor(false);
    if (cursor.text_has_selection() == true) {
        get_selection(start2, end2, true);
    }
    else {
        get_line_pos(cursor.pos1, start2, end2);
    }
    auto text   = line_text(start2);
    auto remove = gnu::PCRE(gnu::str::format("^\\s*(%s)", comment.c_str()));
    remove.exec(text);
    free(text);
    if (remove.matches() == 2) {
        return _find_replace_regex(&remove, "", start2, end2, FREGEXTYPE::REPLACE, false);
    }
    else {
        auto insert = gnu::PCRE("^(.*)$");
        return _find_replace_regex(&insert, comment, start2, end2, FREGEXTYPE::INSERT, false);
    }
}
bool TextBuffer::cut_or_copy_line(int pos, FCOPY action) {
    if (selected() != 0) {
        return false;
    }
    auto text  = (char*) nullptr;
    auto start = 0;
    auto end   = 0;
    get_line_pos_with_nl(pos, start, end);
    if (start == end) {
        end++;
    }
    text = text_range(start, end);
    if (action == FCOPY::CUT_LINE) {
        remove(start, end);
    }
    if (text != nullptr) {
        Fl::copy(text, strlen(text), 2);
        free(text);
    }
    return true;
}
void TextBuffer::debug() const {
#ifdef DEBUG
    auto end   = 0;
    auto start = 0;
    auto sel   = const_cast<TextBuffer*>(this)->selection_position(&start, &end);
    ::printf("\nTextBuffer:\n");
    ::printf("    selected           = %9d\n", sel);
    ::printf("    start              = %9d\n", start);
    ::printf("    end                = %9d\n", end);
    ::printf("    size               = %9d\n", mLength);
    ::printf("    mGapStart          = %9d\n", mGapStart);
    ::printf("    mGapEnd            = %9d\n", mGapEnd);
    ::printf("    count_changes      = %9d\n", (int) _count_changes);
    ::printf("    fdelkey            = %9d\n", (int) _fdelkey);
    ::printf("    has_selection      = %9d\n", (int) _has_selection);
    ::printf("    dirty              = %9s\n", _dirty ? "TRUE" : "FALSE");
    ::printf("    pause_undo         = %9s\n", _pause_undo ? "TRUE" : "FALSE");
    ::printf("    saved_fletcher64   = %llx\n", (long long unsigned) _fletcher64);
    ::printf("    fletcher64         = %llx\n", (long long unsigned) calc_fletcher64());
    fflush(stdout);
#endif
}
int TextBuffer::delete_indent(int pos, FTAB ftab, unsigned tab_width) {
    _count_changes = 0;
    if (ftab == FTAB::SOFT && selected() == 0) {
        int start = 0;
        int end   = 0;
        int align = 0;
        get_line_pos(pos, start, end);
        if (pos == start) {
            return 0;
        }
        for (int f = pos - 1; f >= start; f--) {
            auto c = peek(f);
            if (c > ' ' || (f == pos - 1 && c == '\t')) {
                return 0;
            }
        }
        align = (pos - start) % tab_width;
        if (align == 1) {
            return 0;
        }
        align = tab_width - align;
        remove(pos - align, pos);
        return 1;
    }
    return 0;
}
int TextBuffer::delete_text_left(int pos, FDELTEXT del) {
    _count_changes = 0;
    if (del == FDELTEXT::LINE) {
        auto start = line_start(pos);
        remove(start, pos);
        return 1;
    }
    else if (del == FDELTEXT::WORD && selected() == 0) {
        auto start = get_word_start(pos, true);
        if (start != -1 && start < pos) {
            remove(start, pos);
            return 1;
        }
    }
    return 0;
}
int TextBuffer::delete_text_right(int pos, FDELTEXT del) {
    _count_changes = 0;
    if (del == FDELTEXT::LINE) {
        auto end = line_end(pos);
        remove(pos, end);
        return 1;
    }
    else if (del == FDELTEXT::WORD && selected() == 0) {
        auto end = get_word_end(pos);
        if (end != -1 && pos < end) {
            remove(pos, end);
            return 1;
        }
    }
    return 0;
}
CursorPos TextBuffer::duplicate_text() {
    _count_changes = 0;
    auto cursor = _editor->cursor(false);
    auto pos1   = cursor.pos1;
    auto tmp    = cursor;
    if (cursor.text_has_selection() == false) {
        get_line_pos_with_nl(cursor.pos1, tmp.start, tmp.end);
        if (tmp.len() == 0) {
            insert(tmp.end, "\n");
            cursor.pos1++;
            return cursor;
        }
        auto text = text_range(tmp.start, tmp.end);
        if (text == nullptr) {
            return cursor;
        }
        auto len = tmp.len();
        if (text[len - 1] != '\n') {
            insert(tmp.end, "\n");
            tmp.end++;
        }
        if (has_fle_undo() == true) {
            snprintf(_buf, 256, "%d", pos1);
            _undo->prepare_custom2(_buf);
        }
        insert(tmp.end, text);
        free(text);
        cursor.pos1 += len;
        return cursor;
    }
    else {
        auto text = text_range(tmp.start, tmp.end);
        if (text == nullptr) {
            return cursor;
        }
        auto len = tmp.len();
        insert(tmp.end, text);
        free(text);
        if (tmp.pos1 > tmp.start) {
            tmp.pos1 = tmp.end + len;
        }
        else {
            tmp.pos1 = tmp.end;
        }
        tmp.start  = tmp.end;
        tmp.end += len;
        tmp.set_drag();
        return tmp;
    }
}
size_t TextBuffer::find_lines(std::string filename, std::string find, gnu::PCRE* re, FTRIM ftrim, std::vector<std::string>& out) {
    if (find == "") {
        return 0;
    }
    auto count = (size_t) 0;
    auto start = 0;
    auto stop  = 0;
    for (auto f = 0; f < length();) {
        auto line = line_text(f);
        auto len  = strlen(line);
        auto col  = -1;
        if (re == nullptr) {
            auto found = strstr(line, find.c_str());
            col = (found != nullptr) ? (int) ((size_t) found - (size_t) line) : -1;
        }
        else {
            auto matches = re->exec(line);
            if (matches == 1) {
                col = 0;
            }
            else if (re->offset(1, start, stop) == true) {
                col = start;
            }
        }
        if (col >= 0) {
            if (out.size() < limits::OUTPUT_LINES_VAL) {
                std::string s;
                if (ftrim == FTRIM::YES) {
                    auto trimmed = std::string(line);
                    gnu::str::trim(trimmed);
                    if (filename != "") {
                        s = gnu::str::format("%s: %6d - %4d| %s", filename.c_str(), count + 1, col + 1, trimmed.c_str());
                    }
                    else {
                        s = gnu::str::format("%6d - %4d| %s", count + 1, col + 1, trimmed.c_str());
                    }
                }
                else if (filename != "") {
                    s = gnu::str::format("%s: %6d - %4d| %s", filename.c_str(), count + 1, col + 1, line);
                }
                else  {
                    s = gnu::str::format("%6d - %4d| %s", count + 1, col + 1, line);
                }
                if (s.length() > fle::limits::OUTPUT_LINE_LENGTH_VAL) {
                    s.resize(fle::limits::OUTPUT_LINE_LENGTH_VAL);
                    s += "|";
                }
                out.push_back(s);
            }
            else {
                out.push_back("error: max number of lines has been reached!");
                break;
            }
        }
        count++;
        f += len;
        f++;
        free(line);
    }
    return count;
}
CursorPos TextBuffer::find_replace(
    std::string find,
    const char* replace,
    FSEARCHDIR fsearchdir,
    FCASECOMPARE fcasecompare,
    FWORDCOMPARE fwordcompare,
    FNLTAB fnltab) {
    auto replace2 = gnu::str::to_string(replace);
    find     = (fnltab == FNLTAB::YES || fnltab == FNLTAB::FIND) ? string::fnltab(find) : find;
    replace2 = (fnltab == FNLTAB::YES || fnltab == FNLTAB::REPLACE) ? string::fnltab(replace2) : replace2;
    auto cursor      = _editor->cursor(false);
    auto find_pos    = 0;
    auto find_len    = (int) find.length();
    auto replace_len = (int) replace2.length();
    auto found       = false;
    auto start       = 0;
    auto end         = 0;
    auto type        = (fwordcompare == FWORDCOMPARE::YES) ? token(find.c_str()) : Token::NIL;
    auto loop        = 0;
    auto sel         = selection_position(&start, &end) != 0;
    _count_changes = 0;
    if (find == "" || find_len > length() || (fwordcompare == FWORDCOMPARE::YES && type != Token::LETTER)) {
        return CursorPos();
    }
    if (replace != nullptr && sel == true) {
        if (fwordcompare == FWORDCOMPARE::NO || is_word(start, start + find_len, type) == true) {
            auto text = selection_text();
            if ((fcasecompare == FCASECOMPARE::YES && fl_utf_strcasecmp(text, find.c_str()) == 0) ||
                (fcasecompare == FCASECOMPARE::NO && fl_utf_strncasecmp(text, find.c_str(), find.length()) == 0)) {
                _has_selection = true;
                replace_selection(replace2.c_str());
                cursor.pos1 = start + ((fsearchdir == FSEARCHDIR::FORWARD) ? replace_len : 0);
                sel         = false;
                if (start < cursor.pos2) {
                    cursor.pos2 += replace_len - (end - start);
                }
            }
            free(text);
        }
    }
    while (found == false && loop < 2) {
        if (loop == 0) {
            if (fsearchdir == FSEARCHDIR::FORWARD && sel == true && cursor.pos1 < end) {
                cursor.pos1 = end;
            }
            else if (fsearchdir == FSEARCHDIR::BACKWARD && sel == true && cursor.pos1 == end) {
                cursor.pos1 = start - find_len;
            }
            else if (fsearchdir == FSEARCHDIR::BACKWARD && sel == false) {
                cursor.pos1 -= find_len;
            }
        }
        else if (loop == 1) {
            cursor.pos1 = (fsearchdir == FSEARCHDIR::FORWARD) ? 0 : length() - 1;
        }
        if (fsearchdir == FSEARCHDIR::FORWARD) {
            while (found == false && search_forward(cursor.pos1, find.c_str(), &find_pos, fcasecompare == FCASECOMPARE::YES) != 0) {
                found = (fwordcompare == FWORDCOMPARE::NO || is_word(find_pos, find_pos + find_len, type) == true);
                if (found == false) {
                    cursor.pos1 = find_pos + find_len;
                }
            }
        }
        else {
            while (found == false && cursor.pos1 >= 0 && search_backward(cursor.pos1, find.c_str(), &find_pos, fcasecompare == FCASECOMPARE::YES) != 0) {
                found = (fwordcompare == FWORDCOMPARE::NO || is_word(find_pos, find_pos + find_len, type) == true);
                if (found == false) {
                    cursor.pos1 = find_pos - find_len;
                }
            }
        }
        loop++;
    }
    if (found == false) {
        return CursorPos();
    }
    cursor.pos1  = find_pos + find.length();
    cursor.start = find_pos;
    cursor.end   = find_pos + find.length();
    cursor.set_drag();
    return cursor;
}
CursorPos TextBuffer::find_replace_all(
std::string find,
std::string replace,
FSELECTION fselection,
FCASECOMPARE fcase,
FWORDCOMPARE fword,
FNLTAB fnltab) {
    _count_changes = 0;
    find    = (fnltab == FNLTAB::YES || fnltab == FNLTAB::FIND) ? string::fnltab(find) : find;
    replace = (fnltab == FNLTAB::YES || fnltab == FNLTAB::REPLACE) ? string::fnltab(replace) : replace;
    auto       cursor     = _editor->cursor(true);
    auto       ctrl       = BufferController(this, TextBuffer::TIMEOUT_LONG, true);
    const auto type       = (fword == FWORDCOMPARE::YES) ? token(find.c_str()) : Token::NIL;
    auto       inside_sel = false;
    auto       pos        = 0;
    if (find == "" || (int) find.length() > length() || (fword == FWORDCOMPARE::YES && type != Token::LETTER)) {
        return CursorPos();
    }
    if (fselection == FSELECTION::YES) {
        if (cursor.text_has_selection() == false) {
            return CursorPos();
        }
        pos        = cursor.start;
        inside_sel = true;
    }
    if (has_fle_undo() == true) {
        if (cursor.text_has_selection() == true) {
            _undo->prepare_custom1(gnu::str::format("%d %d %d", cursor.pos1, cursor.start, cursor.end));
        }
        else {
            _undo->prepare_custom1(gnu::str::format("%d -1 -1", cursor.pos1));
        }
    }
    while (search_forward(pos, find.c_str(), &pos, fcase == FCASECOMPARE::YES) != 0) {
        auto do_replace = true;
        if (inside_sel == true && (pos + (int) find.length()) > cursor.end) {
            break;
        }
        if (fword == FWORDCOMPARE::YES && type != Token::NIL) {
            auto word_end = pos + (int) find.length();
            if (inside_sel == false || word_end <= cursor.end) {
                auto pt = peek_token(pos - 1);
                auto nt = peek_token(pos + find.length());
                if (pt == type || nt == type) {
                    do_replace = false;
                }
            }
        }
        if (do_replace == true) {
            auto move = replace.length() - find.length();
            if (pos < cursor.pos1) {
                cursor.pos1 += move;
            }
            if (pos < cursor.pos2) {
                cursor.pos2 += move;
            }
            if (cursor.text_has_selection() == true) {
                if (pos < cursor.start) {
                    cursor.start += move;
                }
                if (pos < cursor.end) {
                    cursor.end += move;
                }
            }
            assert(pos <= length() && pos + (int) find.length() <= length());
            this->replace(pos, pos + find.length(), replace.c_str());
            pos += replace.length();
        }
        else {
            pos += find.length();
        }
        ctrl.check_timeout();
    }
    if (has_fle_undo() == true) {
        if (_count_changes == 0) {
            _undo->clear_custom1();
            return cursor;
        }
        else if (cursor.text_has_selection() == true) {
            _undo->add_custom1(gnu::str::format("%d %d %d", cursor.pos1, cursor.start, cursor.end));
        }
        else {
            _undo->add_custom1(gnu::str::format("%d -1 -1", cursor.pos1));
        }
    }
    else if (_count_changes == 0) {
        return cursor;
    }
    cursor.set_drag();
    return cursor;
}
CursorPos TextBuffer::_find_replace_regex(
    gnu::PCRE* regex,
    const std::string replace,
    int from, int to,
    FREGEXTYPE fregextype,
    bool selection) {
    _count_changes = 0;
    if (to == 0) {
        to = length();
    }
    auto cursor = _editor->cursor(true);
    auto ctrl   = BufferController(this, TextBuffer::TIMEOUT_LONG, true);
    auto pos1   = from;
    auto start  = 0;
    auto end    = 0;
    if (selection == true) {
        from = cursor.start;
        to   = cursor.end;
        pos1 = from;
    }
    if (from == -1 || to == -1 || regex->is_compiled() == false || (fregextype != FREGEXTYPE::REPLACE && replace == "")) {
        return CursorPos();
    }
    if (has_fle_undo() == true) {
        if (selection == true) {
            _undo->prepare_custom1(gnu::str::format("%d %d %d", cursor.pos1, cursor.start, cursor.end));
        }
        else {
            _undo->prepare_custom1(gnu::str::format("%d -1 -1", cursor.pos1));
        }
    }
    while (pos1 < to) {
        auto notbol = false;
        auto noteol = false;
        get_line_pos(pos1, start, end);
        if (selection == true) {
            if (start < cursor.start) {
                notbol = true;
                start  = cursor.start;
            }
            if (end > cursor.end) {
                end    = cursor.end;
                noteol = true;
            }
        }
        auto line = text_range(start, end);
        auto org  = line;
        while (regex->exec(line, notbol, noteol) > 0) {
            auto rs    = 0;
            auto re    = 0;
            auto add   = 0;
            auto pos2  = 0;
            auto match = regex->matches() - 1;
            notbol = true;
            if (fregextype == FREGEXTYPE::REPLACE) {
                auto ma = (regex->matches() > 1) ? 1 : 0;
                if (regex->offset(ma, rs, re) == false) {
                    break;
                }
                add = (int) replace.length() - (re - rs);
                assert(start + rs <= length() && start + re <= length());
                this->replace(start + rs, start + re, replace.c_str());
                pos2 = start + rs;
            }
            else {
                regex->offset(match, rs, re);
                add = (int) replace.length();
                if (fregextype == FREGEXTYPE::APPEND) {
                    assert(start + re <= length());
                    this->insert(start + re, replace.c_str());
                    pos2 = start + rs;
                }
                else if (fregextype == FREGEXTYPE::INSERT) {
                    assert(start + rs <= length());
                    this->insert(start + rs, replace.c_str());
                    pos2 = start + rs - 1;
                }
            }
            auto adjust = 0;
            if (fregextype == FREGEXTYPE::APPEND) {
                adjust = re - rs;
            }
            if (pos2 + adjust < cursor.pos1) {
                cursor.pos1 += add;
            }
            if (pos2 + adjust < cursor.pos2) {
                cursor.pos2 += add;
            }
            if (cursor.text_has_selection() == true) {
                if (pos2 < cursor.start) {
                    cursor.start += add;
                }
                if (pos2 < cursor.end) {
                    cursor.end += add;
                }
            }
            to    += add;
            start += re + add;
            end   += add;
            line   = line + re;
        }
        free(org);
        pos1 = end + 1;
        ctrl.check_timeout();
        if (noteol == true) {
            break;
        }
    }
    if (has_fle_undo() == true) {
        if (_count_changes == 0) {
            _undo->clear_custom1();
            return cursor;
        }
        else if (selection == true) {
            _undo->add_custom1(gnu::str::format("%d %d %d", cursor.pos1, cursor.start, cursor.end));
        }
        else {
            _undo->add_custom1(gnu::str::format("%d -1 -1", cursor.pos1));
        }
    }
    else if (_count_changes == 0) {
        return cursor;
    }
    cursor.set_drag();
    return cursor;
}
CursorPos TextBuffer::find_replace_regex(std::string find, const char* replace, FNLTAB fnltab) {
    auto rx = gnu::PCRE(find, true);
    if (rx.is_compiled() == false) {
        _editor->statusbar_set_message(rx.error());
        return CursorPos();
    }
    auto replace2 = gnu::str::to_string(replace);
    replace2 = (fnltab == FNLTAB::YES || fnltab == FNLTAB::REPLACE) ? string::fnltab(replace2) : replace2;
    _count_changes = 0;
    auto pos    = _editor->cursor_insert_position();
    auto first  = pos;
    auto second = pos;
    auto start  = 0;
    auto end    = 0;
    auto iter   = 0;
    auto part   = 0;
    auto sel    = false;
    if (selection_position(&start, &end) != 0) {
        first  = start;
        second = start;
        sel    = true;
    }
    get_line_pos(pos, start, end);
    if (sel == false) {
        part = pos - start;
    }
    while (iter < 2) {
        auto line   = text_range(start + part, end);
        auto org    = line;
        auto notbol = false;
        start += part;
        part = 0;
        while (rx.exec(line, notbol) > 0) {
            auto rs = 0;
            auto re = 0;
            auto ad = 0;
            auto ma = (rx.matches() > 1) ? 1 : 0;
            notbol = true;
            if (rx.offset(ma, rs, re) == false) {
                return _editor->cursor(false);
            }
            else if (sel == true && replace != nullptr && start + rs == second) {
                replace_selection(replace2.c_str());
                ad  = (int) replace2.length() - (re - rs);
                sel = false;
            }
            else if (sel == false || start + rs > second) {
                auto res = _editor->cursor(false);
                res.pos1 = start + re;
                res.start = start + rs;
                res.end = start + re;
                res.set_drag();
                free(org);
                return res;
            }
            start += re + ad;
            line   = line + re;
        }
        pos = end + 1;
        get_line_pos(pos, start, end);
        free(org);
        if (pos >= length() || (iter == 1 && pos > first)) {
            iter++;
            if (first == 0 || iter == 2) {
                break;
            }
            else if (iter == 1) {
                pos    = 0;
                second = -1;
                get_line_pos(pos, start, end);
            }
        }
    }
    return CursorPos();
}
CursorPos TextBuffer::find_replace_regex_all(gnu::PCRE* regex, std::string replace, FSELECTION fselection, FNLTAB fnltab) {
    assert(regex);
    _count_changes = 0;
    replace = (fnltab == FNLTAB::YES || fnltab == FNLTAB::REPLACE) ? string::fnltab(replace) : replace;
    auto cursor = _editor->cursor(false);
    if (fselection == FSELECTION::YES) {
        if (cursor.text_has_selection() == false) {
            return cursor;
        }
        return _find_replace_regex(regex, replace, cursor.start, cursor.end, FREGEXTYPE::REPLACE, true);
    }
    else {
        return _find_replace_regex(regex, replace, 0, length(), FREGEXTYPE::REPLACE, false);
    }
}
gnu::FileBuf TextBuffer::get(FLINEENDING flineending, bool trim_whitespace) const {
    auto text1 = gnu::FileBuf::Grab(text());
    auto text2 = text1.insert_cr(flineending == FLINEENDING::WINDOWS, trim_whitespace);
    if (text2.p == nullptr) {
        return text1;
    }
    return text2;
}
std::string TextBuffer::get_first(int pos) const {
    auto start = 0;
    auto end   = 0;
    get_line_pos(pos, start, end);
    return get_text_range_string(start, pos);
}
std::string TextBuffer::get_indent(int pos) const {
    std::string res;
    for (auto c : get_first(pos)) {
        if (c == ' ' || c == '\t') {
            res += c;
        }
        else {
            break;
        }
    }
    return res;
}
std::string TextBuffer::get_line(int pos) const {
    auto start = 0;
    auto end   = 0;
    get_line_pos(pos, start, end);
    return get_text_range_string(start, end);
}
void TextBuffer::get_line_pos(int pos, int& start, int& end) const {
    start = line_start(pos);
    end   = line_end(pos);
}
void TextBuffer::get_line_pos_with_nl(int pos, int& start, int& end) const {
    start = line_start(pos);
    end   = line_end(pos);
    if (peek(end - 1) != '\n') {
        end++;
    }
}
bool TextBuffer::get_selection(int& start, int& end, bool expand) {
    if (selection_position(&start, &end) != 0) {
        if (expand == true) {
            start = line_start(start);
            if (peek(end - 1) != '\n') {
                end = line_end(end) + 1;
            }
        }
        return true;
    }
    return false;
}
std::string TextBuffer::get_text_range_string(int start, int end) const {
    return gnu::str::grab_string(text_range(start, end));
}
int TextBuffer::get_word_end(int pos) const {
    auto type = peek_token(pos);
    if (type !=  Token::NIL) {
        auto stop = pos;
        for (auto f = pos + 1; f <= length(); f++) {
            auto t = peek_token(f);
            stop = f;
            if ((type & t) == 0) {
                break;
            }
        }
        return stop;
    }
    return -1;
}
char* TextBuffer::get_word_left(int pos) const {
    auto f = pos;
    auto t = 0;
    do {
        f--;
        t = _word.get(peek(f));
    } while (t &  Token::LETTER);
    if (f < pos - 1) {
        return text_range(f + 1, pos);
    }
    return strdup("");
}
int TextBuffer::get_word_start(int pos, bool move_left) const {
    auto type = peek_token(pos - move_left);
    if (type !=  Token::NIL) {
        auto start = pos - move_left;
        for (auto f = pos - 1; f >= 0; f--) {
            auto t = peek_token(f);
            if ((type & t) == 0) {
                break;
            }
            start = f;
        }
        return start;
    }
    return -1;
}
bool TextBuffer::has_multiline_selection() {
    int s, e;
    return selection_position(&s, &e) != 0 && (line_end(s) < e || line_start(e) > s);
}
int TextBuffer::home(int pos) {
    auto line = line_text(pos);
    if (*line == 0) {
        free(line);
        return -1;
    }
    auto start = line_start(pos);
    auto re    = gnu::PCRE("^(\\s*)");
    auto end   = 0;
    if (re.exec(line) == 2) {
        end = re.end(1);
    }
    free(line);
    if (end < 0) {
        return start;
    }
    else if (pos == start || pos != start + end) {
        return start + end;
    }
    else {
        return start;
    }
}
CursorPos TextBuffer::indent(FINDENT findent) {
    _count_changes = 0;
    if (_config.pref_indentation == true && selected() == 0) {
        auto cursor = _editor->cursor(false);
        auto re     = gnu::PCRE("^(\\s+)");
        auto line   = line_text(cursor.pos1);
        auto start  = line_start(cursor.pos1);
        auto end    = line_end(cursor.pos1);
        if (re.exec(line) == 2) {
            auto text = re.substr(1);
            if (cursor.pos1 == end) {
                text = "\n" + text;
                insert(cursor.pos1, text.c_str());
                cursor.pos1 += text.length();
            }
            else if (findent == FINDENT::ADDLINE) {
                if (has_fle_undo() == true) {
                    _undo->group_lock();
                    _undo->prepare_custom1(gnu::str::format("%d -1 -1", cursor.pos1));
                }
                text += "\n";
                insert(end + 1, text.c_str());
                cursor.pos1 = end + text.length();
                if (has_fle_undo() == true) {
                    _undo->add_custom1(gnu::str::format("%d -1 -1", cursor.pos1));
                    _undo->group_unlock();
                }
            }
            else {
                auto len = cursor.pos1 - start;
                if (len < (int) text.length()) {
                    text = text.substr(0, len);
                }
                text = "\n" + text;
                insert(cursor.pos1, text.c_str());
                cursor.pos1 += text.length();
            }
        }
        else if (findent == FINDENT::ADDLINE) {
            if (has_fle_undo() == true) {
                _undo->group_lock();
                _undo->prepare_custom1(gnu::str::format("%d -1 -1", cursor.pos1));
            }
            insert(end + 1, "\n");
            cursor.pos1 = end + 1;
            if (has_fle_undo() == true) {
                _undo->add_custom1(gnu::str::format("%d -1 -1", cursor.pos1));
                _undo->group_unlock();
            }
        }
        else {
            cursor.pos1 = -1;
        }
        free(line);
        cursor.pos2 = -1;
        return cursor;
    }
    return CursorPos();
}
CursorPos TextBuffer::insert_tab(CursorPos cursor, FTAB ftab, unsigned tab_width) {
    if (tab_width > limits::TAB_WIDTH_MAX) {
        return cursor;
    }
    if (cursor.text_has_selection() == true) {
        remove_selection();
        cursor.debug();
        cursor.pos1 -= ((cursor.pos1 == cursor.end) ? cursor.end - cursor.start : 0);
        cursor.clear_selection();
        cursor.debug();
    }
    _count_changes = 0;
    if (ftab == FTAB::SOFT) {
        auto line  = line_text(cursor.pos1);
        auto start = line_start(cursor.pos1);
        line[cursor.pos1 - start] = 0;
        auto ulen = strlen(line) - gnu::str::utf_len(line);
        auto size = tab_width - ((cursor.pos1 - start - ulen) % tab_width);
        free(line);
        insert(cursor.pos1, strings::SOFT_TABS[size]);
        cursor.pos1 = cursor.pos1 + size;
        cursor.pos2 = -1;
        return cursor;
    }
    else {
        insert(cursor.pos1, "\t");
        cursor.pos1 += 1;
        cursor.pos2 = -1;
        return cursor;
    }
}
CursorPos TextBuffer::insert_tab_multiline(CursorPos cursor, FMOVEH fmoveh, FTAB ftab, unsigned tab_width) {
    if (tab_width > limits::TAB_WIDTH_MAX) {
        return cursor;
    }
    auto ctrl  = BufferController(this, TextBuffer::TIMEOUT_SHORT, true);
    auto tabs  = (ftab == FTAB::HARD) ? "\t" : strings::SOFT_TABS[tab_width];
    auto tabl  = (int) strlen(tabs);
    auto re    = gnu::PCRE(gnu::str::format("^( {1,%u}|\t)", tab_width));
    auto start = 0;
    auto end   = 0;
    _count_changes = 0;
    get_selection(start, end, true);
    if (has_fle_undo() == true) {
        _undo->prepare_custom1(gnu::str::format("%d %d %d", cursor.pos1, cursor.start, cursor.end));
    }
    for (auto f = start; f < end; ) {
        auto line = line_text(f);
        auto len  = strlen(line);
        if (len > 0) {
            auto add = 0;
            if (fmoveh == FMOVEH::LEFT) {
                if (re.exec(line) > 1) {
                    auto s = 0;
                    auto e = 0;
                    if (re.offset(1, s, e) == false) {
                        break;
                    }
                    remove(f + s, f + e);
                    add = -(e - s);
                }
            }
            else {
                insert(f, tabs);
                add = tabl;
            }
            if (cursor.pos1 + add >= f) {
                cursor.pos1 += add;
            }
            if (cursor.pos2 + add >= f) {
                cursor.pos2 += add;
            }
            end += add;
            f   += add;
        }
        f += len;
        f += 1;
        free(line);
        ctrl.check_timeout();
    }
    selection_position(&start, &end);
    if (has_fle_undo() == true) {
        _undo->add_custom1(gnu::str::format("%d %d %d", cursor.pos1, start, end));
    }
    cursor.start = start;
    cursor.end = end;
    cursor.set_drag();
    return cursor;
}
bool TextBuffer::is_word(int start, int end, int word_type) {
    if (word_type != Token::NIL) {
        auto pt = peek_token(start - 1);
        auto nt = peek_token(end);
        if (pt == word_type || nt == word_type) {
            return false;
        }
    }
    return true;
}
CursorPos TextBuffer::move_line(FMOVEV move) {
    _count_changes = 0;
    auto cursor  = _editor->cursor(false);
    auto start    = -1;
    auto end      = -1;
    auto start2   = 0;
    auto end2     = 0;
    auto start3   = 0;
    auto end3     = 0;
    auto len3     = 0;
    auto text     = std::string();
    auto selected = (bool) selection_position(&start, &end);
    if (selected == true) {
        get_selection(start2, end2, true);
    }
    else {
        get_line_pos(cursor.pos1, start2, end2);
    }
    if (move == FMOVEV::UP && start2 < 1) {
        return CursorPos();
    }
    else if (move == FMOVEV::DOWN && end2 >= length() - 1) {
        return CursorPos();
    }
    if (has_fle_undo() == true) {
        _undo->group_lock();
        _undo->prepare_custom1(gnu::str::format("%d %d %d", cursor.pos1, cursor.start, cursor.end));
    }
    if (selected == true) {
        text = get_text_range_string(start2, end2);
        if (move == FMOVEV::UP) {
            get_line_pos(start2 - 1, start3, end3);
            remove(start2, end2);
            if (text.empty() == false && text.back() != '\n') {
                text += "\n";
            }
            insert(start3, text.c_str());
            len3 = -(end3 - start3 + 1);
        }
        else {
            remove(start2, end2);
            get_line_pos(start2, start3, end3);
            if (peek(end3) != 10) {
                insert(end3 + 1, "\n");
            }
            insert(end3 + 1, text.c_str());
            len3 = end3 - start3 + 1;
        }
        cursor.pos1 += len3;
        start       += len3;
        end         += len3;
    }
    else {
        text = get_text_range_string(start2, end2 + 1);
        if (move == FMOVEV::UP) {
            get_line_pos(start2 - 1, start3, end3);
            remove(start2, end2 + 1);
            if (text.empty() == false && text.back() != '\n') {
                text += "\n";
            }
            insert(start3, text.c_str());
            len3  = -(end3 - start3 + 1);
        }
        else {
            remove(start2, end2 + 1);
            get_line_pos(start2, start3, end3);
            if (peek(end3) != 10) {
                insert(end3 + 1, "\n");
            }
            insert(end3 + 1, text.c_str());
            len3 = end3 - start3 + 1;
        }
        cursor.pos1 += len3;
    }
    if (has_fle_undo() == true) {
        _undo->add_custom1(gnu::str::format("%d %d %d", cursor.pos1, start, end));
        _undo->group_unlock_and_add();
    }
    cursor.start = start;
    cursor.end = end;
    cursor.set_drag();
    return cursor;
}
int TextBuffer::peek_token(int pos) const {
    return _word.get(peek(pos));
}
CursorPos TextBuffer::select_color() {
    auto  cursor  = _editor->cursor(false);
    auto& style   = _editor->style_buffer();
    auto  color   = style.peek(cursor.pos1);
    auto  start   = -1;
    auto  end     = -1;
    if (color == 0) {
        return cursor;
    }
    for (auto f = cursor.pos1; f < length(); f++) {
        auto s = style.peek(f);
        if (s != color) {
            break;
        }
        end = f;
    }
    for (auto f = cursor.pos1; f > 0; f--) {
        auto s = style.peek(f);
        if (s != color) {
            break;
        }
        start = f;
    }
    if (start >= 0 && end > start) {
        cursor.start = start;
        cursor.end   = end + 1;
        cursor.set_drag();
    }
    return cursor;
}
CursorPos TextBuffer::select_line(bool exclude_newline) {
    auto cursor = _editor->cursor(false);
    auto start  = 0;
    auto end    = 0;
    get_line_pos_with_nl(cursor.pos1, start, end);
    if (exclude_newline == true && end > start && peek(end - 1) == '\n') {
        end--;
    }
    if (start > end) {
        return CursorPos();
    }
    cursor.start = start;
    if (start == end) {
        cursor.pos1 = start + 1;
        cursor.end  = start + 1;
    }
    else {
        cursor.end = end;
    }
    cursor.set_drag();
    return cursor;
}
CursorPos TextBuffer::select_pair(bool move_cursor) {
    auto  cursor  = _editor->cursor(false);
    auto  c       = peek(cursor.pos1);
    auto  count   = 0;
    auto  e       = (char) 0;
    auto  end     = 0;
    auto  forward = true;
    auto& style   = _editor->style_buffer();
    auto  s       = style.peek(cursor.pos1);
    if (s >= style::STYLE_STRING && s <= style::STYLE_BLOCK_COMMENT) {
        return cursor;
    }
    _count_changes = 0;
    _textbuffer_pair(c, e, forward);
    if (e == 0) {
        c = peek(cursor.pos1 - 1);
        _textbuffer_pair(c, e, forward);
        if (forward == true) {
            count++;
        }
    }
    else {
        end = 1;
    }
    if (e == 0) {
        return cursor;
    }
    if (forward == true) {
        for (auto f = cursor.pos1; f < length(); f++) {
            auto c2 = peek(f);
            s = style.peek(f);
            if (s >= style::STYLE_STRING && s <= style::STYLE_BLOCK_COMMENT) {
            }
            else if (c2 == c) {
                count++;
            }
            else if (c2 == e) {
                count--;
                if (count == 0) {
                    cursor.start = cursor.pos1;
                    cursor.end = f + end;
                    if (move_cursor == true) {
                        cursor.pos1 = cursor.end;
                    }
                    cursor.set_drag();
                    return cursor;
                }
            }
        }
    }
    else {
        for (auto f = cursor.pos1; f > 0; f--) {
            auto c2 = peek(f);
            s = style.peek(f);
            if (s >= style::STYLE_STRING && s <= style::STYLE_BLOCK_COMMENT) {
            }
            else if (c2 == c) {
                count++;
            }
            else if (c2 == e) {
                count--;
                if (count == 0) {
                    cursor.start = f + end;
                    cursor.end = cursor.pos1;
                    if (move_cursor == true) {
                        cursor.pos1 = cursor.start;
                    }
                    cursor.set_drag();
                    return cursor;
                }
            }
        }
    }
    return cursor;
}
CursorPos TextBuffer::select_word() {
    auto cursor = _editor->cursor(false);
    auto start  = get_word_start(cursor.pos1, false);
    auto end    = get_word_end(cursor.pos1);
    if (start == -1 || end == -1 || peek(start) == '\n' || start >= end) {
        return CursorPos();
    }
    cursor.pos1  = end;
    cursor.start = start;
    cursor.end   = end;
    cursor.set_drag();
    return cursor;
}
void TextBuffer::set(const char* TEXT, uint64_t fletcher64) {
    _pause_undo = true;
    if (undo_mode() == FUNDO::FLE) {
        _undo->clear();
        text(TEXT);
    }
    else {
        text(TEXT);
    }
    _pause_undo = false;
    _dirty      = false;
    _fletcher64 = fletcher64;
}
void TextBuffer::set_dirty(bool value, bool force_send) {
    if (value != _dirty || force_send == true) {
        _dirty = value;
        _config.send_message(message::TEXT_CHANGED, "", _editor);
    }
}
CursorPos TextBuffer::sort(FSORT order) {
    _count_changes = 0;
    auto cursor = _editor->cursor(false);
    auto start2 = 0;
    auto end2   = 0;
    if (cursor.text_has_selection() == false) {
        return CursorPos();
    }
    get_selection(start2, end2, true);
    auto text  = text_range(start2, end2);
    auto len   = strlen(text);
    auto lines = gnu::str::split(text, '\n');
    if (text[len - 1] == '\n') {
        lines.pop_back();
    }
    if (lines.size() < 2) {
        free(text);
        return CursorPos();
    }
    std::sort(lines.begin(), lines.end());
    if (order == FSORT::DESCENDING) {
        std::reverse(lines.begin(), lines.end());
    }
    auto sorted = std::string();
    sorted.reserve(len);
    for (const auto& s : lines) {
        sorted += s;
        sorted += "\n";
    }
    if (text[len - 1] != '\n' && sorted != "") {
        sorted.pop_back();
    }
    if (sorted == text) {
        free(text);
        return CursorPos();
    }
    if (has_fle_undo() == true) {
        _undo->group_lock();
        _undo->add_custom1(gnu::str::format("%d %d %d", cursor.pos1, cursor.start, cursor.end));
    }
    replace(start2, end2, sorted.c_str());
    if (has_fle_undo() == true) {
        _undo->prepare_custom1(gnu::str::format("%d %d %d", start2, start2, end2));
        _undo->group_unlock_and_add();
    }
    free(text);
    cursor.pos1  = (cursor.pos1 == cursor.start) ? start2 : end2;
    cursor.start = start2;
    cursor.end   = end2;
    cursor.set_drag();
    return cursor;
}
int TextBuffer::token(const char* string) const {
    auto type = Token::NIL;
    while (*string != 0) {
        auto t = (Token::TYPE) _word.get(*string);
        if (type == Token::NIL) {
            type = t;
        }
        else if (t != type) {
            type = Token::NIL;
            break;
        }
        string++;
    }
    return type;
}
CursorPos TextBuffer::undo_back(bool all, CursorPos cursor) {
    if (undo_mode() != FUNDO::FLE) {
        return cursor;
    }
    auto node  = _undo->undo();
    auto c     = 0;
    _count_changes = 0;
    if (node.is_null() == true) {
        return cursor;
    }
    auto ctrl = BufferController(this, 100, false);
    auto last = UndoEvent();
    _pause_undo = true;
    while (node.is_null() == false) {
        last     = node;
        int pos  = node.pos();
        int len1 = node.len1();
        int len2 = node.len2();
        if (node.is_insert() == true) {
            c++;
            remove(pos, pos + len1);
            if (pos < cursor.pos1) {
                cursor.pos1 -= len1;
            }
            if (pos < cursor.pos2) {
                cursor.pos2 -= len1;
            }
        }
        else if (node.is_delete() == true) {
            c++;
            insert(node.pos(), node.c_str1());
            if (pos < cursor.pos1) {
                cursor.pos1 += len1;
            }
            if (pos < cursor.pos2) {
                cursor.pos2 += len1;
            }
        }
        else if (node.is_replace() == true) {
            c++;
            remove(node.pos(), node.pos() + len2);
            insert(node.pos(), node.c_str1());
            if (pos < cursor.pos1) {
                cursor.pos1 -= len2;
                cursor.pos1 += len1;
            }
            if (pos < cursor.pos2) {
                cursor.pos2 -= len2;
                cursor.pos2 += len1;
            }
        }
        auto p = _undo->peek_undo();
        if (p.is_null() == false && (p.group() == node.group() || all == true)) {
            node = _undo->undo();
        }
        else {
            node = UndoEvent();
        }
        ctrl.check_timeout();
    }
    ctrl.stop();
    undo_cursor_move_to_statusbar_row(cursor, last, true);
    _pause_undo = false;
    _count_changes = c;
    return cursor;
}
bool TextBuffer::undo_check_save_point() {
    if (_undo != nullptr && _undo->is_at_save_point() == true) {
        auto current = calc_fletcher64();
        if (current == _fletcher64) {
            set_dirty(false);
            return true;
        }
    }
    return false;
}
void TextBuffer::undo_cursor_move_to_statusbar_row(CursorPos& cursor, const UndoEvent& node, bool undo) {
    if (node.is_null() == true) {
        return;
    }
    int pos[3]  = { -1, -1, -1 };
    int pos2[3] = { -1, -1, -1 };
    if (node.is_custom1() == true) {
        node.str1_toint(pos);
    }
    else if (undo == true) {
        if (node.is_backspace() == true) {
            pos[0] = node.pos() + node.len1();
            if (node.is_selected() == true) {
                pos[1] = node.pos();
                pos[2] = pos[0];
            }
        }
        else if (node.is_insert() == true) {
            pos[0] = node.pos();
            if (node.is_selected() == true) {
                pos[2] = node.pos();
                pos[1] = node.pos() - node.len1();
            }
            else if (node.is_custom2() == true) {
                if (node.str2_toint(pos2) > 0) {
                    pos[0] = pos2[0];
                }
            }
        }
        else {
            pos[0] = node.pos();
            if (node.is_selected() == true) {
                pos[1] = node.pos();
                pos[2] = node.pos() + node.len1();
            }
        }
    }
    else {
        if (node.is_delete() == true) {
            pos[0] = node.pos();
            if (node.is_selected() == true) {
                pos[1] = node.pos();
                pos[2] = pos[0];
            }
        }
        else if (node.is_replace() == true) {
            pos[0] = node.pos() + node.len2();
            if (node.is_selected() == true) {
                pos[1] = node.pos();
                pos[2] = node.pos() + node.len2();
            }
        }
        else {
            pos[0] = node.pos() + node.len1();
            if (node.is_selected() == true) {
                pos[1] = node.pos();
                pos[2] = node.pos() + node.len1();
            }
            else if (node.is_custom2() == true) {
                if (node.str2_toint(pos2) > 0) {
                    pos[0] = pos2[0] + node.len1();
                }
            }
        }
    }
    if (pos[2] > pos[1]) {
        cursor.pos1  = pos[2];
        cursor.start = pos[1];
        cursor.end   = pos[2];
        cursor.drag  = pos[1];
    }
    else {
        cursor.pos1  = pos[0];
        cursor.start = -1;
        cursor.end   = -1;
        cursor.drag  = 0;
    }
}
CursorPos TextBuffer::undo_forward(bool all, CursorPos cursor) {
    if (undo_mode() != FUNDO::FLE) {
        return cursor;
    }
    auto node = _undo->redo();
    auto c    = 0;
    _count_changes = 0;
    if (node.is_null() == true) {
        return cursor;
    }
    auto ctrl = BufferController(this, 100, false);
    auto last = UndoEvent();
    _pause_undo = true;
    while (node.is_null() == false) {
        last     = node;
        int pos  = node.pos();
        int len1 = node.len1();
        int len2 = node.len2();
        if (node.is_insert() == true) {
            c++;
            insert(node.pos(), node.c_str1());
            if (pos < cursor.pos1) {
                cursor.pos1 += len1;
            }
            if (pos < cursor.pos2) {
                cursor.pos2 += len1;
            }
        }
        else if (node.is_delete() == true) {
            c++;
            remove(node.pos(), node.pos() + len1);
            if (pos < cursor.pos1) {
                cursor.pos1 -= len1;
            }
            if (pos < cursor.pos2) {
                cursor.pos2 -= len1;
            }
        }
        else if (node.is_replace() == true) {
            c++;
            remove(node.pos(), node.pos() + len1);
            insert(node.pos(), node.c_str2());
            if (pos < cursor.pos1) {
                cursor.pos1 -= len1;
                cursor.pos1 += len2;
            }
            if (pos < cursor.pos2) {
                cursor.pos2 -= len1;
                cursor.pos2 += len2;
            }
        }
        auto p = _undo->peek_redo();
        if (p.is_null() == false && (p.group() == node.group() || all == true)) {
            node = _undo->redo();
        }
        else {
            node = UndoEvent();
        }
        ctrl.check_timeout();
    }
    ctrl.stop();
    undo_cursor_move_to_statusbar_row(cursor, last, false);
    _pause_undo = false;
    _count_changes = c;
    return cursor;
}
void TextBuffer::undo_set_mode_using_config() {
    if (_config.pref_undo == FUNDO::NONE) {
        delete _undo;
        _undo = nullptr;
        canUndo(0);
    }
    else if (_config.pref_undo == FUNDO::FLE) {
        if (_undo == nullptr) {
            _undo = new Undo();
            canUndo(0);
        }
    }
    else if (_config.pref_undo == FUNDO::FLTK) {
        delete _undo;
        _undo = nullptr;
        canUndo(1);
    }
    else {
        assert(false);
    }
}
}
#include <assert.h>
namespace fle {
void UndoEvent::debug(int buffer_pos, int count) const {
#ifdef DEBUG
    char flags[50];
    char Str1[50];
    char Str2[50];
    sprintf(flags, "|%c%c%c%c%c%c%c| %04x", is_insert() ? 'I' : '_', is_delete() ? 'D' : '_',
        is_replace() ? 'R' : '_', is_backspace() ? 'B' : '_', is_selected() ? 'S' : '_',
        is_custom1() ? 'C' : '_', is_custom2() ? 'c' : '_', _group);
    strncpy(Str1, _str1.c_str(), 25);
    strncpy(Str2, _str2.c_str(), 25);
    Str1[25] = 0;
    Str2[25] = 0;
    auto t = reinterpret_cast<unsigned char*>(Str1);
    while (*t) {
        if (*t < 32 || *t > 126) *t = '.';
        t++;
    }
    t = reinterpret_cast<unsigned char*>(Str2);
    while (*t) {
        if (*t < 32 || *t > 126) *t = '.';
        t++;
    }
    if ((count - 1) % 40 == 0) {
        printf("----------------------------------------------------------------------\n");
        printf("%8s %6s %15s %9s %9s %s\n", "BUFPOS", "COUNT", "FLAGS      GRP ", "POS", "LEN1", "STR1");
        printf("----------------------------------------------------------------------\n");
    }
    if (len2() > 0) {
        int l = (int) (25 - strlen(Str1));
        printf("%8d %6d %15s %9d %9d |%s| %*s %9d |%s|\n", buffer_pos, count, _flag == 0 ? "|________| XXXX" : flags, _pos, len1(), Str1, l, "", len2(), Str2);
    }
    else {
        printf("%8d %6d %15s %9d %9d |%s|\n", buffer_pos, count, _flag == 0 ? "|________| XXXX" : flags, _pos, len1(), Str1);
    }
    fflush(stdout);
#else
    (void) buffer_pos;
    (void) count;
#endif
}
bool UndoBuffer::buffer_decrease() {
    auto new_bcap = _bcap;
    if (_bend > 2'097'152) {
        while (new_bcap > _bend + 2'097'152) {
            new_bcap -= 2'097'152;
        }
    }
    else {
        while (new_bcap > _bend * 2) {
            new_bcap *= 0.5;
        }
    }
    if (new_bcap < 4'096) {
        new_bcap = 4'096;
    }
    if (new_bcap >= _bcap) {
        return false;
    }
    assert(new_bcap > _bend);
    _bcap = new_bcap;
    _buf  = gnu::File::Allocate(_buf, _bcap + 1);
    return true;
}
bool UndoBuffer::buffer_increase(int64_t requested_bcap) {
    if (requested_bcap <= _bcap) {
        return false;
    }
    auto new_bcap = _bcap;
    if (requested_bcap > 2'097'152) {
        while (requested_bcap > new_bcap) {
            new_bcap += 2'097'152;
        }
    }
    else {
        while (requested_bcap > new_bcap) {
            new_bcap *= 2;
        }
    }
    if (new_bcap == _bcap) {
        return false;
    }
    _bcap = new_bcap;
    _buf  = gnu::File::Allocate(_buf, _bcap + 1);
    return true;
}
void UndoBuffer::clear() {
    free(_buf);
    _bcap = 4'096;
    _buf  = gnu::File::Allocate(nullptr, _bcap + 1, 1);
    _bcur = -1;
    _bend = -1;
    _move = UndoBuffer::MOVING_END;
}
int64_t UndoBuffer::debug(int all) const {
#ifdef DEBUG
    int64_t cursor1 = 0;
    int64_t count   = 0;
    if (all == 1) {
        printf("\nUndoEvent:\n");
    }
    while (cursor1 < _bend) {
        count++;
        if (all == 1) {
            get_node(cursor1).debug(cursor1, count);
        }
        go_right(cursor1);
    }
    if (all >= 0) {
        printf("\nUndoBuffer:\n");
        printf("    count_all          = %8d\n", (int) count);
        printf("    bcap               = %8lld\n", (long long int) _bcap);
        printf("    bcur               = %8lld\n", (long long int) _bcur);
        printf("    bend               = %8lld\n", (long long int) _bend);
        printf("    move               = %8d\n", _move);
        fflush(stdout);
    }
    return count;
#else
    (void) all;
    return 0;
#endif
}
const UndoEvent UndoBuffer::get_node(int64_t cursor) const {
    if (cursor < 0 || cursor >= _bend) {
        return UndoEvent();
    }
    auto b     = static_cast<const char*>(_buf + cursor);
    auto flag  = (uint8_t) 0;
    auto group = (uint16_t) 0;
    auto pos   = (int32_t) 0;
    auto str1  = (const char*) nullptr;
    auto str2  = (const char*) nullptr;
    flag = *b;
    b++;
    memcpy(&group, b, sizeof(group));
    b += sizeof(group);
    memcpy(&pos, b, sizeof(pos));
    b += sizeof(pos);
    str1 = b;
    b    = b + strlen(str1) + 1;
    str2 = b;
    b    = b + strlen(str2) + 1;
    return UndoEvent(flag, group, pos, str1, str2);
}
bool UndoBuffer::go_left() {
    if (_move == UndoBuffer::MOVING_RIGHT) {
        _move  = UndoBuffer::MOVING_LEFT;
        return true;
    }
    else if (go_left(_bcur) == true) {
        _move  = UndoBuffer::MOVING_LEFT;
        return true;
    }
    else {
        _move = UndoBuffer::MOVING_END;
        return false;
    }
}
bool UndoBuffer::go_left(int64_t& cursor) const {
    if (cursor > 8) {
        auto b    = _buf;
        auto size = (uint32_t) 0;
        b += (cursor - sizeof(size));
        memcpy(&size, b, sizeof(size));
        cursor -= size;
        if (cursor >= 0) {
            return true;
        }
    }
    cursor = -1;
    return false;
}
bool UndoBuffer::go_right() {
    if (_move == UndoBuffer::MOVING_LEFT) {
        _move  = UndoBuffer::MOVING_RIGHT;
        return true;
    }
    else if (go_right(_bcur) == true && _bcur < _bend) {
        _move  = UndoBuffer::MOVING_RIGHT;
        return true;
    }
    else {
        _move = UndoBuffer::MOVING_END;
        _bcur = _bend;
        return false;
    }
}
bool UndoBuffer::go_right(int64_t& cursor) const {
    if (cursor < 0 && _bend > 0) {
        cursor = 0;
        return true;
    }
    else if (cursor >= 0 && cursor < _bend) {
        auto b = _buf + cursor;
        b += UndoEvent::NumberSize();
        b += strlen(b);
        b++;
        b += strlen(b);
        b++;
        b += sizeof(uint32_t);
        cursor = (int64_t) (b - _buf);
        return true;
    }
    else {
        return false;
    }
}
void UndoBuffer::go_right_before_cut() {
    if (_move == UndoBuffer::MOVING_RIGHT) {
        go_right(_bcur);
        _move = UndoBuffer::MOVING_END;
    }
}
const UndoEvent UndoBuffer::peek_left() const {
    auto cursor1 = _bcur;
    if (go_left(cursor1) == true) {
        return get_node(cursor1);
    }
    else {
        return UndoEvent();
    }
}
const UndoEvent UndoBuffer::peek_right() const {
    auto cursor1 = _bcur;
    if (go_right(cursor1) == true) {
        return get_node(cursor1);
    }
    else {
        return UndoEvent();
    }
}
void UndoBuffer::pop() {
    if (go_left(_bcur) == true) {
        _bend  = _bcur;
    }
    _move = UndoBuffer::MOVING_END;
}
void UndoBuffer::set_node(const UndoEvent& node) {
    if (_bcur < 0) {
        _bcur = 0;
    }
    auto size = (uint32_t) (node.size() + sizeof(uint32_t));
    auto inc  = buffer_increase(_bcur + size);
    auto b    = static_cast<char*>(_buf + _bcur);
    *b = node.flag();
    b++;
    auto group = node.group();
    memcpy(b, &group, sizeof(group));
    b += sizeof(group);
    auto pos = node.pos();
    memcpy(b, &pos, sizeof(pos));
    b += sizeof(pos);
    auto l = node.len1() + 1;
    memcpy(b, node.c_str1(), l);
    b += l;
    l = node.len2() + 1;
    memcpy(b, node.c_str2(), l);
    b += l;
    auto old_end = _bend;
    memcpy(b, &size, sizeof(size));
    _bcur += size;
    _bend  = _bcur;
    _move  = UndoBuffer::MOVING_END;
    if (inc == false && _bcur < old_end) {
        buffer_decrease();
    }
}
Undo::Undo() {
    _tokens.set('_', Token::LETTER);
    _tokens.set('0', '9', Token::LETTER);
    _tokens.set(128, 191, Token::LETTER);
    _tokens.set(194, 244, Token::LETTER);
    clear();
}
void Undo::add(const FDELKEY delkey, const bool selection, const int pos, const char* inserted_text, const int inserted_size, const char* deleted_text, const int deleted_size) {
    auto flag         = (uint8_t) 0;
    auto type         = 0;
    auto inserted_one = (inserted_text != nullptr) ? string::is_one_char(inserted_text) : false;
    auto deleted_one  = (deleted_text != nullptr) ? string::is_one_char(deleted_text) : false;
    if (_buf.cursor() < _save_point) {
        _save_point = -1;
    }
    if (inserted_text != nullptr && deleted_text == nullptr && inserted_one == true) {
        type = _tokens.get(*inserted_text);
    }
    else if (deleted_text != nullptr && inserted_text == nullptr && deleted_one == true) {
        type = _tokens.get(*deleted_text);
    }
    if (selection == true) {
        flag |= UndoEvent::FLAG_SELECTED;
    }
    if (deleted_size > 0 && inserted_size > 0) {
        flag |= UndoEvent::FLAG_REPLACE;
        _add(nullptr, flag, pos, deleted_text, inserted_text);
    }
    else if (deleted_size > 0) {
        auto last = peek_undo();
        flag |= UndoEvent::FLAG_DELETE;
        if (delkey == FDELKEY::BACKSPACE) {
            flag |= UndoEvent::FLAG_BACKSPACE;
        }
        if (last.is_null() == false && type != Token::NIL && (type & _prev_type)) {
            if (delkey == FDELKEY::DEL && pos == last.pos()) {
                _add(&last, flag, pos, deleted_text);
                goto EXIT;
            }
            else if (delkey == FDELKEY::BACKSPACE && pos + deleted_size == last.pos()) {
                _add(&last, flag, pos, deleted_text);
                goto EXIT;
            }
        }
        _add(nullptr, flag, pos, deleted_text);
    }
    else if (inserted_size > 0) {
        auto last = peek_undo();
        flag |= UndoEvent::FLAG_INSERT;
        if (last.is_null() == false) {
            if (last.is_insert() == true && type != Token::NIL && (type & _prev_type) && pos == last.pos() + last.len1()) {
                _add(&last, flag, pos, inserted_text);
                goto EXIT;
            }
            else if (last.is_delete() == true && pos == last.pos() && selection == true) {
                _add(&last, flag, pos, inserted_text);
                goto EXIT;
            }
        }
        if (_custom2 != "") {
            flag |= UndoEvent::FLAG_CUSTOM2;
            _add(nullptr, flag, pos, inserted_text, _custom2.c_str());
        }
        else {
            _add(nullptr, flag, pos, inserted_text);
        }
    }
    group_add();
EXIT:
    _custom2   = "";
    _prev_type = type;
}
void Undo::_add(UndoEvent* last, uint8_t flag, int pos, const char* str1, const char* str2) {
    if (_is_cursor_at_end() == true && last != nullptr && _append_to_node(last, flag, pos, str1) == true) {
        return;
    }
    auto node = UndoEvent(flag, _group, pos, str1, str2);
    _push_node_to_buf(node);
}
bool Undo::_append_to_node(UndoEvent* last, uint8_t flag, int pos, const char* str1) {
    if (flag & UndoEvent::FLAG_INSERT) {
        if (last->is_insert() == true) {
            last->append_str1(str1);
            _buf.go_left();
            _push_node_to_buf(*last);
            return true;
        }
        else if (last->is_delete() == true) {
            assert(last->len2() == 0);
            last->flag(UndoEvent::FLAG_REPLACE | UndoEvent::FLAG_SELECTED);
            last->str2(str1);
            if (last->str1() == last->str2()) {
                _buf.pop();
            }
            else {
                _buf.go_left();
                _push_node_to_buf(*last);
            }
            return true;
        }
    }
    else if (flag & UndoEvent::FLAG_DELETE && last->is_delete() == true) {
        if (flag & UndoEvent::FLAG_BACKSPACE) {
            last->pos(pos);
            last->insert_str1(str1);
            _buf.go_left();
            _push_node_to_buf(*last);
            return true;
        }
        else {
            last->append_str1(str1);
            _buf.go_left();
            _push_node_to_buf(*last);
            return true;
        }
    }
    return false;
}
void Undo::clear() {
    _buf.clear();
    _custom1    = UndoEvent();
    _custom2    = "";
    _group      = 0;
    _group_lock = false;
    _prev_type  = 0;
    _save_point = -1;
}
int64_t Undo::debug(int all) {
#ifdef DEBUG
    auto res = _buf.debug(all);
    if (all >= 0) {
        printf("\nUndo:\n");
        printf("    custom1            = %s\n", _custom1.is_null() ? "    NULL" : "     SET");
        printf("    custom2            = %s\n", _custom2.c_str());
        printf("    group              = %8d\n", _group);
        printf("    group_lock         = %8d\n", _group_lock);
        printf("    prev_type          = %8d\n", _prev_type);
        printf("    save_point         = %8lld\n", (long long int) _save_point);
        fflush(stdout);
    }
    return res;
#else
    (void) all;
    return 0;
#endif
}
void Undo::_push_node_to_buf(UndoEvent node) {
    if (_custom1.is_null() == false) {
        _buf.set_node(_custom1);
        clear_custom1();
    }
    if (_is_cursor_at_end() == true) {
        _buf.set_node(node);
    }
    else {
        _buf.go_right_before_cut();
        _buf.set_node(node);
    }
}
}
#include <FL/fl_ask.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Scroll.H>
namespace fle {
namespace widgets {
    constexpr static const char*    BUTTON_NL                   = "NL";
    constexpr static const char*    ERROR_EMPTY_STRING          = "error: empty string!";
    constexpr static const char*    ERROR_PCRE                  = "error: invalid PCRE expression!\n%s";
    constexpr static const char*    SCHEME_ATTR_BGCOLOR         = "Background color";
    constexpr static const char*    SCHEME_ATTR_BGCOLOR_EXT     = "Background color ext";
    constexpr static const char*    SCHEME_ATTR_GRAMMAR         = "Grammar";
    constexpr static const char*    SCHEME_ATTR_NONE            = "No attribute";
    constexpr static const char*    SCHEME_ATTR_STRIKE          = "Strike through";
    constexpr static const char*    SCHEME_ATTR_UNDERLINE       = "Underline";
    constexpr static const char*    SCHEME_FONT_BOLD            = "Bold";
    constexpr static const char*    SCHEME_FONT_BOLD_ITALIC     = "Bold && Italic";
    constexpr static const char*    SCHEME_FONT_ITALIC          = "Italic";
    constexpr static const char*    SCHEME_FONT_REGULAR         = "Regular";
    constexpr static const char*    SETTINGS_BINARY_HEX         = "Load binary as hex";
    constexpr static const char*    SETTINGS_BINARY_NO          = "Don't load binary files";
    constexpr static const char*    SETTINGS_BINARY_TEXT        = "Load binary as text";
    constexpr static const char*    SETTINGS_BLOCK_CURSOR       = "Block cursor";
    constexpr static const char*    SETTINGS_CARET_CURSOR       = "Caret cursor";
    constexpr static const char*    SETTINGS_DIM_CURSOR         = "Dim cursor";
    constexpr static const char*    SETTINGS_HEAVY_CURSOR       = "Heavy cursor";
    constexpr static const char*    SETTINGS_NORMAL_CURSOR      = "Normal cursor";
    constexpr static const char*    SETTINGS_SCROLL12           = "12 lines";
    constexpr static const char*    SETTINGS_SCROLL15           = "15 lines";
    constexpr static const char*    SETTINGS_SCROLL18           = "18 lines";
    constexpr static const char*    SETTINGS_SCROLL3            = "3 lines";
    constexpr static const char*    SETTINGS_SCROLL6            = "6 lines";
    constexpr static const char*    SETTINGS_SCROLL9            = "9 lines";
    constexpr static const char*    SETTINGS_SIMPLE_CURSOR      = "Simple cursor";
    constexpr static const char*    SETTINGS_UNDO_FLE           = "FLE undo";
    constexpr static const char*    SETTINGS_UNDO_FLTK          = "FLTK undo";
    constexpr static const char*    SETTINGS_UNDO_NONE          = "None";
    constexpr static const char*    SETTINGS_WRAP100            = "100";
    constexpr static const char*    SETTINGS_WRAP120            = "120";
    constexpr static const char*    SETTINGS_WRAP140            = "140";
    constexpr static const char*    SETTINGS_WRAP60             = "60";
    constexpr static const char*    SETTINGS_WRAP72             = "72";
    constexpr static const char*    SETTINGS_WRAP80             = "80";
    constexpr static const char*    SETTINGS_WRAPWINDOW         = "Wrap window";
    constexpr static const char*    STATUSBAR_LINE_UNIX         = "Unix";
    constexpr static const char*    STATUSBAR_LINE_WIN          = "Windows";
    constexpr static const char*    STATUSBAR_SPACES_TO_TABS    = "Convert to tabs";
    constexpr static const char*    STATUSBAR_TAB_DEF_HARD      = "Default/Use tab";
    constexpr static const char*    STATUSBAR_TAB_DEF_SOFT      = "Default/Use spaces";
    constexpr static const char*    STATUSBAR_TAB_DEF_WIDTH1    = "Default/Tab width: 1";
    constexpr static const char*    STATUSBAR_TAB_DEF_WIDTH2    = "Default/Tab width: 2";
    constexpr static const char*    STATUSBAR_TAB_DEF_WIDTH3    = "Default/Tab width: 3";
    constexpr static const char*    STATUSBAR_TAB_DEF_WIDTH4    = "Default/Tab width: 4";
    constexpr static const char*    STATUSBAR_TAB_DEF_WIDTH5    = "Default/Tab width: 5";
    constexpr static const char*    STATUSBAR_TAB_DEF_WIDTH6    = "Default/Tab width: 6";
    constexpr static const char*    STATUSBAR_TAB_DEF_WIDTH7    = "Default/Tab width: 7";
    constexpr static const char*    STATUSBAR_TAB_DEF_WIDTH8    = "Default/Tab width: 8";
    constexpr static const char*    STATUSBAR_TAB_HARD          = "Use tab";
    constexpr static const char*    STATUSBAR_TAB_SOFT          = "Use spaces";
    constexpr static const char*    STATUSBAR_TAB_WIDTH1        = "Tab width: 1";
    constexpr static const char*    STATUSBAR_TAB_WIDTH2        = "Tab width: 2";
    constexpr static const char*    STATUSBAR_TAB_WIDTH3        = "Tab width: 3";
    constexpr static const char*    STATUSBAR_TAB_WIDTH4        = "Tab width: 4";
    constexpr static const char*    STATUSBAR_TAB_WIDTH5        = "Tab width: 5";
    constexpr static const char*    STATUSBAR_TAB_WIDTH6        = "Tab width: 6";
    constexpr static const char*    STATUSBAR_TAB_WIDTH7        = "Tab width: 7";
    constexpr static const char*    STATUSBAR_TAB_WIDTH8        = "Tab width: 8";
    constexpr static const char*    STATUSBAR_TABS_TO_SPACES    = "Convert to spaces";
    constexpr static const char*    TOOLTIP_AUTOCOMPLETE        = "Turn autocomplete on or off.\nAutocomplete list are generated when file is loaded and every time it is saved.";
    constexpr static const char*    TOOLTIP_AUTORELOAD          = "File is reloaded when it has been updated outside editor.\nBut only when the editor has received focus again.\nIf text has been changed in editor you will be asked.";
    constexpr static const char*    TOOLTIP_BINARY_FILE         = "You can load binary files with contents converted to hexadecimal.\nDue to increased memory usage max file size is 425MB.\nOr converted to some kind of text.\nIt will be opened as a unsaved unnamed document.";
    constexpr static const char*    TOOLTIP_FIND_CASE           = "Use case sensitive search (not for regex).";
    constexpr static const char*    TOOLTIP_FIND_JUMP           = "Jump between find and replace input field with alt+1/2.";
    constexpr static const char*    TOOLTIP_FIND_NL             = "Replace all \\n\\r\\t with actual ascii value in find string.";
    constexpr static const char*    TOOLTIP_FIND_REGEX          = "Use regular expression (PCRE) for search string.";
    constexpr static const char*    TOOLTIP_FIND_SELECTION      = "Replace only in selection.\nOnly valid for 'Replace all'!";
    constexpr static const char*    TOOLTIP_FIND_WORD           = "Find whole words only when searching (not for regex).";
    constexpr static const char*    TOOLTIP_FONT                = "Select editor font.";
    constexpr static const char*    TOOLTIP_INDENT              = "Enable or disable automatic indentation.";
    constexpr static const char*    TOOLTIP_INSERT              = "Enable or disable insert/overwrite mode.";
    constexpr static const char*    TOOLTIP_LINENUMBER          = "Show or hide linenumbers.";
    constexpr static const char*    TOOLTIP_REPLACE_NL          = "Replace all \\n\\r\\t with actual ascii value in replace string.";
    constexpr static const char*    TOOLTIP_STATUSBAR           = "Show or hide statusbar.";
    constexpr static const char*    TOOLTIP_TEST_REGEX          = "Test if string can be compiled by pcre engine.";
    constexpr static const char*    TOOLTIP_UNDO                = "FLE undo is a replacement for the built in undo.\nIt has some additional features like undo batch replacements in one go.\nFLTK is the native undo which undo texts in small to large chunks.\nAnd you can also use no undo.";
}
class _AutoCompleteDialogBrowser : public flw::ScrollBrowser {
    Token                       _tokens;
    std::string                 _selected;
    std::string                 _word;
public:
    _AutoCompleteDialogBrowser() : flw::ScrollBrowser(), _tokens(Token::MakeWord()) {
        callback(_AutoCompleteDialogBrowser::Callback2, this);
    }
    static void Callback2(Fl_Widget*, void* o) {
        auto self = static_cast<_AutoCompleteDialogBrowser*>(o);
        if (Fl::event_clicks() > 0) {
            Fl::event_clicks(0);
            if (self->value() > 0) {
                self->_selected = self->text(self->value());
                self->parent()->hide();
            }
        }
    }
    int handle(int event) override {
        if (event == FL_KEYBOARD) {
            auto key = Fl::event_key();
            if (key == FL_BackSpace) {
                if (_word.size() > 0) {
                    _word.pop_back();
                    find_word();
                }
                return 1;
            }
            else if (key == FL_Enter) {
                if (value() > 0) {
                    _selected = text(value());
                }
                parent()->hide();
            }
            else if (key == FL_Escape) {
                parent()->hide();
            }
            else {
                auto text = Fl::event_text();
                auto type = _tokens.get(*text);
                if (type & Token::LETTER) {
                    auto old = _word;
                    _word += text;
                    if (find_word() == false) {
                        _word = old;
                    }
                }
            }
        }
        return flw::ScrollBrowser::handle(event);
    }
    bool find_word() {
        for (auto f = 1; f <= size(); f++) {
            auto w = std::string(text(f));
            if (w.find(_word) == 0) {
                value(f);
                topline((f > 1) ? f - 1 : 1);
                return true;
            }
        }
        return false;
    }
    int populate(const StringSet& words, std::string word) {
        auto row = 0;
        clear();
        for (const auto& w : words) {
            if (word.size() == 0) {
                add(w.c_str());
            }
            else if (w.find(word) == 0) {
                add(w.c_str());
                if (row == 0) {
                    _word = word;
                    row   = size();
                }
            }
        }
        if (row > 0) {
            value(row);
            topline((row > 2) ? row - 2 : 1);
        }
        return size();
    }
    std::string selected() {
        return _selected;
    }
};
AutoCompleteDialog::AutoCompleteDialog(Fl_Fontsize fontsize) : Fl_Double_Window(0, 0, 0, 0) {
    end();
    _browser = new _AutoCompleteDialogBrowser();
    _browser->textfont(flw::PREF_FIXED_FONT);
    _browser->textsize(fontsize);
    _browser->tooltip("Enter search string");
    add(_browser);
    callback(AutoCompleteDialog::Callback, this);
}
void AutoCompleteDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<AutoCompleteDialog*>(o);
    if (w == self) {
        self->hide();
    }
}
int AutoCompleteDialog::handle(int event) {
    return Fl_Double_Window::handle(event);
}
int AutoCompleteDialog::populate(const std::set<std::string>& words, std::string word) {
    return static_cast<_AutoCompleteDialogBrowser*>(_browser)->populate(words, word);
}
void AutoCompleteDialog::resize(int X, int Y, int W, int H) {
    Fl_Double_Window::resize(X, Y, W, H);
    _browser->resize(0, 0, W, H);
}
std::string AutoCompleteDialog::run(int x, int y, int w, int h) {
    set_modal();
    border(0);
    resize(x, y, w, h);
    show();
    while (visible() != 0) {
        Fl::wait();
        Fl::flush();
    }
    return static_cast<_AutoCompleteDialogBrowser*>(_browser)->selected();
}
class _ConfigDialog : public Fl_Double_Window {
public:
    Config&                     _config;
    Fl_Box*                     _fixed_label;
    Fl_Button*                  _close;
    Fl_Button*                  _fixed;
    Fl_Check_Button*            _autocomplete;
    Fl_Check_Button*            _autoreload;
    Fl_Check_Button*            _indent;
    Fl_Check_Button*            _insert;
    Fl_Check_Button*            _linenumber;
    Fl_Check_Button*            _statusbar;
    Fl_Menu_Button*             _binary;
    Fl_Menu_Button*             _cursor;
    Fl_Menu_Button*             _scroll;
    Fl_Menu_Button*             _undo;
    Fl_Menu_Button*             _wrap;
    flw::GridGroup*             _grid;
    _ConfigDialog(Config& config) :
    Fl_Double_Window(0, 0, 10, 10, "Editor Settings"),
    _config(config) {
        end();
        _autocomplete = new Fl_Check_Button(0, 0, 0, 0, "Autocomplete");
        _autoreload   = new Fl_Check_Button(0, 0, 0, 0, "Autoreload changed file");
        _binary       = new Fl_Menu_Button(0, 0, 0, 0, "Binary files");
        _close        = new Fl_Button(0, 0, 0, 0, "&Close");
        _cursor       = new Fl_Menu_Button(0, 0, 0, 0, "Cursor");
        _fixed        = new Fl_Button(0, 0, 0, 0, "Editor font");
        _fixed_label  = new Fl_Box(0, 0, 0, 0);
        _grid         = new flw::GridGroup(0, 0, w(), h());
        _indent       = new Fl_Check_Button(0, 0, 0, 0, "Automatic indentation");
        _insert       = new Fl_Check_Button(0, 0, 0, 0, "Enable insert/overwrite mode");
        _linenumber   = new Fl_Check_Button(0, 0, 0, 0, "Linenumber");
        _scroll       = new Fl_Menu_Button(0, 0, 0, 0, "Mouse scroll");
        _statusbar    = new Fl_Check_Button(0, 0, 0, 0, "Statusbar");
        _undo         = new Fl_Menu_Button(0, 0, 0, 0, "Undo mode");
        _wrap         = new Fl_Menu_Button(0, 0, 0, 0, "Word wrap");
        _grid->add(_linenumber,     1,   1,  -1,   4);
        _grid->add(_statusbar,      1,   6,  -1,   4);
        _grid->add(_autocomplete,   1,  11,  -1,   4);
        _grid->add(_autoreload,     1,  16,  -1,   4);
        _grid->add(_indent,         1,  21,  -1,   4);
        _grid->add(_insert,         1,  26,  -1,   4);
        _grid->add(_cursor,         1,  31,  -1,   4);
        _grid->add(_scroll,         1,  36,  -1,   4);
        _grid->add(_wrap,           1,  41,  -1,   4);
        _grid->add(_undo,           1,  46,  -1,   4);
        _grid->add(_binary,         1,  51,  -1,   4);
        _grid->add(_fixed_label,    1,  56,  -1,   4);
        _grid->add(_fixed,        -34,  -5,  16,   4);
        _grid->add(_close,        -17,  -5,  16,   4);
        add(_grid);
        _autocomplete->tooltip(widgets::TOOLTIP_AUTOCOMPLETE);
        _autoreload->tooltip(widgets::TOOLTIP_AUTORELOAD);
        _binary->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _binary->tooltip(widgets::TOOLTIP_BINARY_FILE);
        _close->callback(Callback, this);
        _cursor->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _fixed->callback(Callback, this);
        _fixed->tooltip(widgets::TOOLTIP_FONT);
        _fixed_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _fixed_label->box(FL_BORDER_BOX);
        _fixed_label->color(FL_BACKGROUND2_COLOR);
        _fixed_label->tooltip(widgets::TOOLTIP_FONT);
        _indent->tooltip(widgets::TOOLTIP_INDENT);
        _insert->tooltip(widgets::TOOLTIP_INSERT);
        _linenumber->tooltip(widgets::TOOLTIP_LINENUMBER);
        _scroll->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _statusbar->tooltip(widgets::TOOLTIP_STATUSBAR);
        _undo->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _undo->tooltip(widgets::TOOLTIP_UNDO);
        _wrap->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        if (config.disable_autoreload == true) {
            _autoreload->deactivate();
        }
        callback(Callback, this);
        set_modal();
        init();
        resizable(this);
        update_pref();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_ConfigDialog*>(o);
        if (w == self || w == self->_close) {
            self->save();
            self->hide();
        }
        else if (w == self->_fixed) {
            auto dialog = flw::dlg::FontDialog(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE, "Select Editor Font");
            if (dialog.run(Fl::first_window()) == true) {
                flw::PREF_FIXED_FONT            = dialog.font();
                flw::PREF_FIXED_FONTSIZE        = dialog.fontsize();
                flw::PREF_FIXED_FONTNAME        = dialog.fontname();
                self->_config.pref_tmp_fontsize = 0;
                self->update_pref();
            }
        }
    }
    void init() {
        std::string text;
        if (_autoreload->active() != 0) {
            _autoreload->value(_config.pref_autoreload);
        }
        _autocomplete->value(_config.pref_autocomplete);
        _indent->value(_config.pref_indentation);
        _insert->value(_config.pref_insert);
        _linenumber->value(_config.pref_linenumber);
        _statusbar->value(_config.pref_statusbar);
        {
            _cursor->add(widgets::SETTINGS_NORMAL_CURSOR, 0, nullptr, nullptr, FL_MENU_RADIO);
            _cursor->add(widgets::SETTINGS_CARET_CURSOR, 0, nullptr, nullptr, FL_MENU_RADIO);
            _cursor->add(widgets::SETTINGS_DIM_CURSOR, 0, nullptr, nullptr, FL_MENU_RADIO);
            _cursor->add(widgets::SETTINGS_BLOCK_CURSOR, 0, nullptr, nullptr, FL_MENU_RADIO);
            _cursor->add(widgets::SETTINGS_HEAVY_CURSOR, 0, nullptr, nullptr, FL_MENU_RADIO);
            _cursor->add(widgets::SETTINGS_SIMPLE_CURSOR, 0, nullptr, nullptr, FL_MENU_RADIO);
            if (_config.pref_cursor == Fl_Text_Display::CARET_CURSOR) {
                text = widgets::SETTINGS_CARET_CURSOR;
            }
            else if (_config.pref_cursor == Fl_Text_Display::DIM_CURSOR) {
                text = widgets::SETTINGS_DIM_CURSOR;
            }
            else if (_config.pref_cursor == Fl_Text_Display::BLOCK_CURSOR) {
                text = widgets::SETTINGS_BLOCK_CURSOR;
            }
            else if (_config.pref_cursor == Fl_Text_Display::HEAVY_CURSOR) {
                text = widgets::SETTINGS_HEAVY_CURSOR;
            }
            else if (_config.pref_cursor == Fl_Text_Display::SIMPLE_CURSOR) {
                text = widgets::SETTINGS_SIMPLE_CURSOR;
            }
            else {
                text = widgets::SETTINGS_NORMAL_CURSOR;
            }
            flw::menu::setonly_item(_cursor, text.c_str());
        }
        {
            _scroll->add(widgets::SETTINGS_SCROLL3, 0, nullptr, nullptr, FL_MENU_RADIO);
            _scroll->add(widgets::SETTINGS_SCROLL6, 0, nullptr, nullptr, FL_MENU_RADIO);
            _scroll->add(widgets::SETTINGS_SCROLL9, 0, nullptr, nullptr, FL_MENU_RADIO);
            _scroll->add(widgets::SETTINGS_SCROLL12, 0, nullptr, nullptr, FL_MENU_RADIO);
            _scroll->add(widgets::SETTINGS_SCROLL15, 0, nullptr, nullptr, FL_MENU_RADIO);
            _scroll->add(widgets::SETTINGS_SCROLL18, 0, nullptr, nullptr, FL_MENU_RADIO);
            if (_config.pref_mouse_scroll == 3) {
                text = widgets::SETTINGS_SCROLL6;
            }
            else if (_config.pref_mouse_scroll == 6) {
                text = widgets::SETTINGS_SCROLL9;
            }
            else if (_config.pref_mouse_scroll == 9) {
                text = widgets::SETTINGS_SCROLL12;
            }
            else if (_config.pref_mouse_scroll == 12) {
                text = widgets::SETTINGS_SCROLL15;
            }
            else if (_config.pref_mouse_scroll == 15) {
                text = widgets::SETTINGS_SCROLL18;
            }
            else {
                text = widgets::SETTINGS_SCROLL3;
            }
            flw::menu::setonly_item(_scroll, text.c_str());
        }
        {
            _wrap->add(widgets::SETTINGS_WRAP60, 0, nullptr, nullptr, FL_MENU_RADIO);
            _wrap->add(widgets::SETTINGS_WRAP72, 0, nullptr, nullptr, FL_MENU_RADIO);
            _wrap->add(widgets::SETTINGS_WRAP80, 0, nullptr, nullptr, FL_MENU_RADIO);
            _wrap->add(widgets::SETTINGS_WRAP100, 0, nullptr, nullptr, FL_MENU_RADIO);
            _wrap->add(widgets::SETTINGS_WRAP120, 0, nullptr, nullptr, FL_MENU_RADIO);
            _wrap->add(widgets::SETTINGS_WRAP140, 0, nullptr, nullptr, FL_MENU_RADIO);
            _wrap->add(widgets::SETTINGS_WRAPWINDOW, 0, nullptr, nullptr, FL_MENU_RADIO);
            if (_config.pref_wrap == 60) {
                text = widgets::SETTINGS_WRAP60;
            }
            else if (_config.pref_wrap == 72) {
                text = widgets::SETTINGS_WRAP72;
            }
            else if (_config.pref_wrap == 100) {
                text = widgets::SETTINGS_WRAP100;
            }
            else if (_config.pref_wrap == 120) {
                text = widgets::SETTINGS_WRAP120;
            }
            else if (_config.pref_wrap == 140) {
                text = widgets::SETTINGS_WRAP140;
            }
            else if (_config.pref_wrap == 66) {
                text = widgets::SETTINGS_WRAPWINDOW;
            }
            else {
                text = widgets::SETTINGS_WRAP80;
            }
            flw::menu::setonly_item(_wrap, text.c_str());
        }
        {
            _undo->add(widgets::SETTINGS_UNDO_FLE, 0, nullptr, nullptr, FL_MENU_RADIO);
            _undo->add(widgets::SETTINGS_UNDO_FLTK, 0, nullptr, nullptr, FL_MENU_RADIO);
            _undo->add(widgets::SETTINGS_UNDO_NONE, 0, nullptr, nullptr, FL_MENU_RADIO);
            if (_config.pref_undo == FUNDO::FLE) {
                text = widgets::SETTINGS_UNDO_FLE;
            }
            else if (_config.pref_undo == FUNDO::FLTK) {
                text = widgets::SETTINGS_UNDO_FLTK;
            }
            else {
                text = widgets::SETTINGS_UNDO_NONE;
            }
            flw::menu::setonly_item(_undo, text.c_str());
        }
        {
            _binary->add(widgets::SETTINGS_BINARY_NO, 0, nullptr, nullptr, FL_MENU_RADIO);
            _binary->add(widgets::SETTINGS_BINARY_TEXT, 0, nullptr, nullptr, FL_MENU_RADIO);
            _binary->add(widgets::SETTINGS_BINARY_HEX, 0, nullptr, nullptr, FL_MENU_RADIO);
            if (_config.pref_binary == FBINFILE::HEX) {
                text = widgets::SETTINGS_BINARY_HEX;
            }
            else if (_config.pref_binary == FBINFILE::TEXT) {
                text = widgets::SETTINGS_BINARY_TEXT;
            }
            else {
                text = widgets::SETTINGS_BINARY_NO;
            }
            flw::menu::setonly_item(_binary, text.c_str());
        }
    }
    void run() {
        flw::util::center_window(this, Fl::first_window());
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
    }
    void save() {
        if (_insert->value() != _config.pref_insert) {
            _config.send_message(message::RESET_INSERT_MODE);
        }
        _config.pref_autocomplete = _autocomplete->value();
        _config.pref_autoreload   = _autoreload->value();
        _config.pref_indentation  = _indent->value();
        _config.pref_insert       = _insert->value();
        _config.pref_linenumber   = _linenumber->value();
        _config.pref_statusbar    = _statusbar->value();
        auto label = gnu::str::to_string(_cursor->text());
        if (label == widgets::SETTINGS_NORMAL_CURSOR) {
            _config.pref_cursor = Fl_Text_Display::NORMAL_CURSOR;
        }
        else if (label == widgets::SETTINGS_CARET_CURSOR) {
            _config.pref_cursor = Fl_Text_Display::CARET_CURSOR;
        }
        else if (label == widgets::SETTINGS_DIM_CURSOR) {
            _config.pref_cursor = Fl_Text_Display::DIM_CURSOR;
        }
        else if (label == widgets::SETTINGS_BLOCK_CURSOR) {
            _config.pref_cursor = Fl_Text_Display::BLOCK_CURSOR;
        }
        else if (label == widgets::SETTINGS_HEAVY_CURSOR) {
            _config.pref_cursor = Fl_Text_Display::HEAVY_CURSOR;
        }
        else if (label == widgets::SETTINGS_SIMPLE_CURSOR) {
            _config.pref_cursor = Fl_Text_Display::SIMPLE_CURSOR;
        }
        label = gnu::str::to_string(_scroll->text());
        if (label == widgets::SETTINGS_SCROLL3) {
            _config.pref_mouse_scroll = 0;
        }
        else if (label == widgets::SETTINGS_SCROLL6) {
            _config.pref_mouse_scroll = 3;
        }
        else if (label == widgets::SETTINGS_SCROLL9) {
            _config.pref_mouse_scroll = 6;
        }
        else if (label == widgets::SETTINGS_SCROLL12) {
            _config.pref_mouse_scroll = 9;
        }
        else if (label == widgets::SETTINGS_SCROLL15) {
            _config.pref_mouse_scroll = 12;
        }
        else if (label == widgets::SETTINGS_SCROLL18) {
            _config.pref_mouse_scroll = 15;
        }
        label = gnu::str::to_string(_wrap->text());
        if (label == widgets::SETTINGS_WRAP60) {
            _config.pref_wrap = 60;
        }
        else if (label == widgets::SETTINGS_WRAP72) {
            _config.pref_wrap = 72;
        }
        else if (label == widgets::SETTINGS_WRAP100) {
            _config.pref_wrap = 100;
        }
        else if (label == widgets::SETTINGS_WRAP120) {
            _config.pref_wrap = 120;
        }
        else if (label == widgets::SETTINGS_WRAP140) {
            _config.pref_wrap = 140;
        }
        else if (label == widgets::SETTINGS_WRAPWINDOW) {
            _config.pref_wrap = 66;
        }
        else if (label == widgets::SETTINGS_WRAP80) {
            _config.pref_wrap = 80;
        }
        label = gnu::str::to_string(_undo->text());
        if (label == widgets::SETTINGS_UNDO_FLE) {
            _config.pref_undo = FUNDO::FLE;
        }
        else if (label == widgets::SETTINGS_UNDO_FLTK) {
            _config.pref_undo = FUNDO::FLTK;
        }
        else if (label == widgets::SETTINGS_UNDO_NONE) {
            _config.pref_undo = FUNDO::NONE;
        }
        label = gnu::str::to_string(_binary->text());
        if (label == widgets::SETTINGS_BINARY_HEX) {
            _config.pref_binary = FBINFILE::HEX;
        }
        else if (label == widgets::SETTINGS_BINARY_TEXT) {
            _config.pref_binary = FBINFILE::TEXT;
        }
        else if (label == widgets::SETTINGS_BINARY_NO) {
            _config.pref_binary = FBINFILE::NO;
        }
    }
    void update_pref() {
        flw::util::labelfont(this);
        _binary->textfont(flw::PREF_FONT);
        _binary->textsize(flw::PREF_FONTSIZE);
        _cursor->textfont(flw::PREF_FONT);
        _cursor->textsize(flw::PREF_FONTSIZE);
        _fixed_label->copy_label(gnu::str::format("%s - %d", flw::PREF_FIXED_FONTNAME.c_str(), flw::PREF_FIXED_FONTSIZE).c_str());
        _fixed_label->labelfont(flw::PREF_FIXED_FONT);
        _fixed_label->labelsize(flw::PREF_FIXED_FONTSIZE);
        _scroll->textfont(flw::PREF_FONT);
        _scroll->textsize(flw::PREF_FONTSIZE);
        _undo->textfont(flw::PREF_FONT);
        _undo->textsize(flw::PREF_FONTSIZE);
        _wrap->textfont(flw::PREF_FONT);
        _wrap->textsize(flw::PREF_FONTSIZE);
        resize(x(), y(), flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 33);
        size_range(flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 33);
    }
};
void dlg::config(Config& config) {
    auto dlg = _ConfigDialog(config);
    dlg.run();
    config.send_message(message::PREF_CHANGED);
}
class _DlgEditor : public Fl_Double_Window, Message {
    Config&                     _config;
    Editor*                     _editor;
    FindBar*                    _findbar;
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    bool                        _edit;
    bool                        _res;
    std::string                 _org;
public:
    _DlgEditor(Config& config, std::string title, int W, int H) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W, flw::PREF_FONTSIZE * H),
    Message(config),
    _config(config) {
        end();
        _cancel  = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _close   = new Fl_Button(0, 0, 0, 0, "&Close");
        _findbar = new FindBar(config);
        _editor  = new Editor(config, _findbar);
        _res     = false;
        _edit    = false;
        add(_editor);
        add(_cancel);
        add(_close);
        add(_findbar);
        _cancel->callback(_DlgEditor::Callback, this);
        _close->callback(_DlgEditor::Callback, this);
        _findbar->findreplace().update_lists();
        copy_label(title.c_str());
        callback(_DlgEditor::Callback, this);
        size_range(flw::PREF_FONTSIZE * 16, flw::PREF_FONTSIZE * 8);
        set_modal();
        resizable(this);
        update_pref();
        flw::util::center_window(this, Fl::first_window());
        _editor->take_focus();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgEditor*>(o);
        if (w == self) {
        }
        else if (w == self->_cancel) {
            self->_res = false;
            self->hide();
        }
        else if (w == self->_close) {
            self->_res = true;
            self->hide();
        }
    }
    bool load_file(std::string file, std::string style) {
        auto err = _editor->file_load(file);
        if (err != "") {
            fl_message_position(this);
            fl_alert("%s", err.c_str());
            return false;
        }
        else if (style == "") {
            _editor->style_from_filename();
        }
        else {
            _editor->style_from_language(style);
        }
        _editor->update_autocomplete();
        _org = text();
        return true;
    }
    Message::CTRL message(const std::string& message, const std::string&, const void* p) override {
        if (message == message::DND_EVENT) {
            auto discard = reinterpret_cast<bool*>(const_cast<void*>(p));
            *discard = false;
        }
        else if (message == message::PREF_CHANGED) {
            update_pref();
            resize(x(), y(), w(), h());
        }
        else if (message == message::HIDE_FIND) {
            _editor->findbar().hide();
            _editor->take_focus();
            size(w(), h());
            Fl::redraw();
            _config.active->take_focus();
        }
        else if (message == message::TEXT_CHANGED) {
        }
        else if (message == message::SHOW_FIND) {
            _editor->findbar().show();
            size(w(), h());
            Fl::redraw();
        }
        return Message::CTRL::CONTINUE;
    }
    void resize(int X, int Y, int W, int H) override {
        auto fs = flw::PREF_FONTSIZE / 2;
        auto h  = _findbar->height();
        Fl_Double_Window::resize(X, Y, W, H);
        _editor->resize  (0,            0,               W,        H - fs * 6 - h);
        _findbar->resize (0,            H - fs * 6 - h,  W,        h);
        _cancel->resize  (W - fs * 34,  H - fs * 5,      fs * 16,  fs * 4);
        _close->resize   (W - fs * 17,  H - fs * 5,      fs * 16,  fs * 4);
    }
    bool run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        return _edit == true && _res == true && _org != _editor->text_get_buffer_with_trim_and_line().p;
    }
    bool file_save() {
        auto err = _editor->file_save();
        if (err != "") {
            fl_message_position(this);
            fl_alert("%s", err.c_str());
            return false;
        }
        return true;
    }
    void set_text(const char* text, std::string style) {
        _editor->text_set(text);
        _editor->style_from_language(style);
        _editor->update_autocomplete();
        _org = text;
    }
    std::string text() const {
        return _editor->text_get_buffer_with_trim_and_line().p;
    }
    void update_buttons(bool edit, bool file) {
        _edit = edit;
        if (file == true) {
            if (_edit == false) {
                _cancel->hide();
                _close->tooltip("Close dialog.\nText is NOT saved.");
            }
            else {
                _cancel->tooltip("Close dialog.\nText is NOT saved.");
                _close->label("&Save");
                _close->tooltip("Close dialog.\nSave text to file.");
            }
        }
        else {
            if (_edit == false) {
                _cancel->hide();
                _close->tooltip("Close dialog.\nText is NOT updated.");
            }
            else {
                _cancel->tooltip("Close dialog.\nText is NOT updated.");
                _close->label("&Update");
                _close->tooltip("Close dialog.\nUpdate text.");
            }
        }
    }
    void update_pref() {
        flw::util::labelfont(this);
        _editor->findbar().update_pref();
    }
};
bool dlg::edit(Config& config, std::string title, std::string& text, std::string style, int W, int H) {
    auto editor = _DlgEditor(config, title, W, H);
    editor.set_text(text.c_str(), style);
    editor.update_buttons(true, false);
    if (editor.run() == false) {
        return false;
    }
    text = editor.text();
    return true;
}
bool dlg::edit_file(Config& config, std::string title, std::string file, std::string style, int W, int H) {
    auto editor = _DlgEditor(config, title, W, H);
    auto run    = true;
    if (editor.load_file(file, style) == false) {
        return false;
    }
    editor.update_buttons(true, true);
    while (run == true) {
        if (editor.run() == false) {
            return false;
        }
        if (editor.file_save() == true) {
            run = false;
        }
    }
    return true;
}
bool dlg::view(Config& config, std::string title, std::string& text, std::string style, int W, int H) {
    auto editor = _DlgEditor(config, title, W, H);
    editor.set_text(text.c_str(), style);
    editor.update_buttons(false, false);
    if (editor.run() == false) {
        return false;
    }
    text = editor.text();
    return true;
}
bool dlg::view_file(Config& config, std::string title, std::string file, std::string style, int W, int H) {
    auto editor = _DlgEditor(config, title, W, H);
    auto run    = true;
    if (editor.load_file(file, style) == false) {
        return false;
    }
    editor.update_buttons(false, true);
    while (run == true) {
        if (editor.run() == false) {
            return false;
        }
        if (editor.file_save() == true) {
            run = false;
        }
    }
    return true;
}
FindBar::FindBar(Config& config) :
Fl_Group(0, 0, 0, 0, ""),
_config(config) {
    end();
    clip_children(1);
    box(FL_FLAT_BOX);
    _findreplace = new FindReplace(_config);
    _statusbar   = new StatusBar(_config);
    add(_findreplace);
    add(_statusbar);
    FindBar::hide();
}
int FindBar::height() const {
    int res = 0;
    if (_config.pref_statusbar == true) {
        res += _statusbar->height();
    }
    if (_findreplace->visible() != 0) {
        res += FindReplace::Height();
        res += 1;
    }
    return res;
}
void FindBar::hide() {
    if (_config.pref_statusbar == false) {
        _statusbar->hide();
    }
    _findreplace->hide();
}
void FindBar::resize(int X, int Y, int W, int H) {
    Fl_Group::resize(X, Y, W, H);
    if (_findreplace->visible() != 0) {
        _findreplace->resize(X + 2, Y + 2, W - 4, FindReplace::Height());
    }
    if (_statusbar->visible() != 0) {
        _statusbar->resize(X + 2, Y + H - _statusbar->height(), W - 4, _statusbar->height());
    }
}
void FindBar::show() {
    if (_config.pref_statusbar == true) {
        _statusbar->show();
    }
    _findreplace->show();
}
void FindBar::update_pref() {
    _findreplace->update_pref();
    _statusbar->update_pref();
    if (_config.pref_statusbar == true) {
        _statusbar->show();
    }
    else  {
        _statusbar->hide();
    }
}
bool FindBar::visible() const {
    return _statusbar->visible() != 0 || _findreplace->visible() != 0;
}
FREGEX FindDialog::REGEX = FREGEX::NO;
FTRIM  FindDialog::TRIM  = FTRIM::NO;
FindDialog::FindDialog(std::string label, const std::vector<std::string>& find_list) :
Fl_Double_Window(0, 0, 10, 10) {
    end();
    _cancel  = new Fl_Button(0, 0, 0, 0, "&Cancel");
    _find    = new flw::InputMenu(0, 0, 0, 0, "Find");
    _grid    = new flw::GridGroup(0, 0, w(), h());
    _help    = new Fl_Button(0, 0, 0, 0, "&Help");
    _ok      = new Fl_Return_Button(0, 0, 0, 0, "&Find");
    _regex   = new Fl_Check_Button(0, 0, 0, 0, "Rege&x");
    _test    = new Fl_Button(0, 0, 0, 0, "&Test");
    _trim    = new Fl_Check_Button(0, 0, 0, 0, "&Trim");
    _ret     = false;
    _grid->add(_find,     10,   1,  -1,   4);
    _grid->add(_regex,    10,   6,  15,   4);
    _grid->add(_trim,     25,   6,  15,   4);
    _grid->add(_help,    -68,  -5,  16,   4);
    _grid->add(_test,    -51,  -5,  16,   4);
    _grid->add(_cancel,  -34,  -5,  16,   4);
    _grid->add(_ok,      -17,  -5,  16,   4);
    add(_grid);
    _cancel->callback(FindDialog::Callback, this);
    _find->align(FL_ALIGN_LEFT);
    _find->callback(FindDialog::Callback, this);
    _find->values(find_list);
    _find->take_focus();
    _find->update_pref(flw::PREF_FIXED_FONT, flw::PREF_FONTSIZE);
    _help->callback(FindDialog::Callback, this);
    _ok->callback(FindDialog::Callback, this);
    _ok->tooltip("Search for text from all lines in current text.");
    _regex->callback(FindDialog::Callback, this);
    _regex->tooltip(widgets::TOOLTIP_FIND_REGEX);
    _regex->value(FindDialog::REGEX == FREGEX::YES);
    _test->tooltip(widgets::TOOLTIP_TEST_REGEX);
    _test->callback(FindDialog::Callback, this);
    _trim->tooltip("Remove all whitespace from each end of found lines.");
    _trim->value(FindDialog::TRIM == FTRIM::YES);
    FindDialog::Callback(_regex, this);
    flw::util::labelfont(this);
    copy_label(label.c_str());
    callback(FindDialog::Callback, this);
    set_modal();
    resizable(this);
    resize(0, 0, flw::PREF_FONTSIZE * 40, flw::PREF_FONTSIZE * 8);
    size_range(flw::PREF_FONTSIZE * 40, flw::PREF_FONTSIZE * 8);
}
void FindDialog::Callback(Fl_Widget* w, void* o) {
    auto* self = static_cast<FindDialog*>(o);
    if (w == self) {
        self->hide();
    }
    else if (w == self->_cancel) {
        self->hide();
    }
    else if (w == self->_help) {
        flw::dlg::list("Find Lines Help", help::find_lines() + help::pcre(), self, true, 60, 40);
        self->_find->take_focus();
    }
    else if (w == self->_ok || w == self->_find) {
        if (self->_regex->value() != 0 && self->test_pcre() == false) {
            self->_find->take_focus();
        }
        else {
            FindDialog::REGEX = (self->_regex->value() != 0) ? FREGEX::YES : FREGEX::NO;
            FindDialog::TRIM  = (self->_trim->value() != 0) ? FTRIM::YES : FTRIM::NO;
            self->_ret        = true;
            self->hide();
        }
    }
    else if (w == self->_regex) {
        if (self->_regex->value() != 0) {
            self->_test->activate();
        }
        else {
            self->_test->deactivate();
        }
    }
    else if (w == self->_test) {
        self->test_pcre();
        self->_find->take_focus();
    }
}
std::string FindDialog::run() {
    flw::util::center_window(this, Fl::first_window());
    show();
    while (visible() != 0) {
        Fl::wait();
        Fl::flush();
    }
    return (_ret == true) ? _find->value() : "";
}
bool FindDialog::test_pcre() {
    std::string string = _find->value();
    fl_message_position(this);
    if (string == "") {
        fl_alert("%s", widgets::ERROR_EMPTY_STRING);
        return false;
    }
    auto re = gnu::PCRE(string);
    if (re.is_compiled() == false) {
        fl_alert(widgets::ERROR_PCRE, re.error().c_str());
        return false;
    }
    return true;
}
FindReplace::FindReplace(Config& config) :
flw::GridGroup(),
_config(config) {
    end();
    clip_children(1);
    _case          = new Fl_Check_Button(0, 0, 0, 0, "&Case");
    _find_input    = new flw::InputMenu(0, 0, 0, 0, "Find:");
    _find_next     = new Fl_Repeat_Button(0, 0, 0, 0, "&Find");
    _find_nl       = new Fl_Check_Button(0, 0, 0, 0, widgets::BUTTON_NL);
    _find_prev     = new Fl_Repeat_Button(0, 0, 0, 0, "Find &prev");
    _regex         = new Fl_Check_Button(0, 0, 0, 0, "Rege&x");
    _replace       = new Fl_Repeat_Button(0, 0, 0, 0, "&Replace");
    _replace_all   = new Fl_Button(0, 0, 0, 0, "Replace &all");
    _replace_input = new flw::InputMenu(0, 0, 0, 0, "Replace:");
    _replace_nl    = new Fl_Check_Button(0, 0, 0, 0, widgets::BUTTON_NL);
    _selection     = new Fl_Check_Button(0, 0, 0, 0, "&Selection");
    _word          = new Fl_Check_Button(0, 0, 0, 0, "&Word");
    add(_find_input,     10, -10, -67,   4);
    add(_replace_input,  10,  -5, -67,   4);
    add(_find_nl,       -66, -10,   8,   4);
    add(_replace_nl,    -66,  -5,   8,   4);
    add(_case,          -58, -10,  10,   4);
    add(_word,          -58,  -5,  10,   4);
    add(_selection,     -48, -10,  13,   4);
    add(_regex,         -48,  -5,  13,   4);
    add(_find_next,     -34, -10,  16,   4);
    add(_find_prev,     -34,  -5,  16,   4);
    add(_replace,       -17, -10,  16,   4);
    add(_replace_all,   -17,  -5,  16,   4);
    _case->tooltip(widgets::TOOLTIP_FIND_CASE);
    _find_input->align(FL_ALIGN_LEFT);
    _find_next->tooltip("Find and select text after cursor.");
    _find_nl->tooltip(widgets::TOOLTIP_FIND_NL);
    _find_prev->tooltip("Find and select text before cursor.");
    _regex->callback(FindReplace::CallbackCheckButton, this);
    _regex->tooltip(widgets::TOOLTIP_FIND_REGEX);
    _replace->tooltip("Find text before replacing text.\nOnly find text can have regex expressions.");
    _replace_all->tooltip("Replace all found search strings.\nIf text is selected replacements are only done inside the selection.\nOnly find text can have regex expressions.");
    _replace_input->align(FL_ALIGN_LEFT);
    _replace_nl->tooltip(widgets::TOOLTIP_REPLACE_NL);
    _selection->callback(FindReplace::CallbackCheckButton, this);
    _selection->tooltip(widgets::TOOLTIP_FIND_SELECTION);
    _word->tooltip(widgets::TOOLTIP_FIND_WORD);
    box(FL_BORDER_BOX);
    color(FL_BACKGROUND_COLOR);
    tooltip(widgets::TOOLTIP_FIND_JUMP);
}
void FindReplace::add_find_word(std::string word) {
    if (_config.add_find_word(word) == true) {
        update_lists(true, false);
    }
}
void FindReplace::add_replace_word(std::string word) {
    if (_config.add_replace_word(word) == true) {
        update_lists(false, true);
    }
}
void FindReplace::CallbackCheckButton(Fl_Widget*, void* o) {
    auto self = static_cast<FindReplace*>(o);
    self->enable_buttons();
}
void FindReplace::callback(Fl_Callback* cb, void* obj) {
    _find_input->callback(cb, obj);
    _find_next->callback(cb, obj);
    _find_prev->callback(cb, obj);
    _replace->callback(cb, obj);
    _replace_all->callback(cb, obj);
    _replace_input->callback(cb, obj);
}
void FindReplace::enable_buttons() {
    if (_regex->value() != 0) {
        _case->deactivate();
        _find_nl->deactivate();
        _find_prev->deactivate();
        _word->deactivate();
    }
    else {
        _case->activate();
        _find_nl->activate();
        _find_prev->activate();
        _word->activate();
    }
}
FNLTAB FindReplace::fnltab() const {
    bool f = _regex->value() == 0 && _find_nl->value() != 0;
    bool r = _replace_nl->value() != 0;
    if (f == true && r == true) {
        return FNLTAB::YES;
    }
    else if (f == true) {
        return FNLTAB::FIND;
    }
    else if (r == true) {
        return FNLTAB::REPLACE;
    }
    else {
        return FNLTAB::NO;
    }
}
void FindReplace::fnltab(FNLTAB fnltab) {
    _find_nl->value(0);
    _replace_nl->value(0);
    if (fnltab == FNLTAB::YES) {
        _find_nl->value(1);
        _replace_nl->value(1);
    }
    else if (fnltab == FNLTAB::FIND) {
        _find_nl->value(1);
    }
    else if (fnltab == FNLTAB::REPLACE) {
        _replace_nl->value(1);
    }
}
int FindReplace::handle(int event) {
    if (event == FL_KEYBOARD) {
        if (Fl::event_key() == FL_Escape) {
            _config.send_message(message::HIDE_FIND, "", this);
        }
        else if (Fl::event_alt() != 0 && Fl::event_key() == '1') {
            _find_input->input()->take_focus();
            return 1;
        }
        else if (Fl::event_alt() != 0 && Fl::event_key() == '2') {
            _replace_input->input()->take_focus();
            return 1;
        }
        else if (Fl::event_key() == FL_Tab) {
            if (Fl::focus() == _replace_all) {
                _find_input->input()->take_focus();
                return 1;
            }
        }
    }
    return Fl_Group::handle(event);
}
void FindReplace::resize(int X, int Y, int W, int H) {
    flw::GridGroup::resize(X, Y, W, H);
    enable_buttons();
}
void FindReplace::show() {
    Fl_Group::show();
    take_focus();
}
void FindReplace::take_focus() {
    _find_input->input()->take_focus();
    _find_input->input()->insert_position(_find_input->input()->size(), 0);
}
void FindReplace::update_lists(bool find, bool replace) {
    if (find == true) {
        _find_input->values(_config.find_list, false);
    }
    if (replace == true) {
        _replace_input->values(_config.replace_list, false);
    }
}
void FindReplace::update_pref() {
    _find_input->update_pref(flw::PREF_FIXED_FONT, flw::PREF_FONTSIZE);
    _replace_input->update_pref(flw::PREF_FIXED_FONT, flw::PREF_FONTSIZE);
    flw::util::labelfont(this);
}
class _KeyboardDialog : public Fl_Double_Window {
public:
    Config&                     _config;
    Fl_Button*                  _close;
    Fl_Button*                  _reset;
    Fl_Button*                  _save;
    Fl_Check_Button*            _alt[fle::FKEY_SIZE];
    Fl_Box*                     _col[6];
    Fl_Box*                     _label[fle::FKEY_SIZE];
    Fl_Check_Button*            _ctrl[fle::FKEY_SIZE];
    Fl_Check_Button*            _shift[fle::FKEY_SIZE];
    Fl_Check_Button*            _kommand[fle::FKEY_SIZE];
    Fl_Int_Input*               _key[fle::FKEY_SIZE];
    Fl_Scroll*                  _scroll;
    flw::GridGroup*             _grid2;
    flw::GridGroup*             _grid3;
    flw::GridGroup*             _grid1;
    char                        _buff[256];
    int                         _count;
    int                         _height;
    _KeyboardDialog(Config& config) :
    Fl_Double_Window(0, 0, 10, 10, "Keyboard Setup"),
    _config(config) {
        end();
        _close  = new Fl_Button(0, 0, 0, 0, "&Close");
        _col[0] = new Fl_Box(0, 0, 0, 0, "Alt");
        _col[1] = new Fl_Box(0, 0, 0, 0, "Ctrl");
        _col[2] = new Fl_Box(0, 0, 0, 0, "Shift");
        _col[3] = new Fl_Box(0, 0, 0, 0, "Cmd");
        _col[4] = new Fl_Box(0, 0, 0, 0, "Key");
        _col[5] = new Fl_Box(0, 0, 0, 0, "Description");
        _grid2  = new flw::GridGroup(0, 0, w(), h());
        _grid3  = new flw::GridGroup(0, 0, w(), h());
        _grid1  = new flw::GridGroup(0, 0, w(), h());
        _reset  = new Fl_Button(0, 0, 0, 0, "&Reset");
        _save   = new Fl_Button(0, 0, 0, 0, "&Save");
        _scroll = new Fl_Scroll(0, 0, w(), h());
        _height = 0;
        end();
        for (int f = 0; f < fle::FKEY_SIZE; f++) {
            _alt[f] = new Fl_Check_Button(0, 0, 0, 0);
            _alt[f]->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            _grid2->add(_alt[f], 1, 5 * f + 1, 6, 4);
            _ctrl[f] = new Fl_Check_Button(0, 0, 0, 0);
            _ctrl[f]->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            _grid2->add(_ctrl[f], 7, 5 * f + 1, 6, 4);
            _shift[f] = new Fl_Check_Button(0, 0, 0, 0);
            _shift[f]->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            _grid2->add(_shift[f], 13, 5 * f + 1, 6, 4);
            _kommand[f] = new Fl_Check_Button(0, 0, 0, 0);
            _kommand[f]->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            _kommand[f]->tooltip("Check this option to turn on command mode for this shortcut key.");
            _grid2->add(_kommand[f], 19, 5 * f + 1, 6, 4);
            _key[f] = new Fl_Int_Input(0, 0, 0, 0);
            _key[f]->align(FL_ALIGN_RIGHT | FL_ALIGN_CLIP);
            _key[f]->textfont(flw::PREF_FIXED_FONT);
            _key[f]->textsize(flw::PREF_FONTSIZE);
            _key[f]->labelfont(flw::PREF_FIXED_FONT);
            _key[f]->labelsize(flw::PREF_FONTSIZE);
            _key[f]->tooltip(
                "Enter key value as an hex number.\n"
                "Valid values are from 0x20 to 0xFFFF.\n"
                "Either an ascii letter or a FLTK keycode.\n"
                "Beware there is no check that the key actual works."
            );
            _grid2->add(_key[f], 25, 5 * f + 1, 10, 4);
            _label[f] = new Fl_Box(0, 0, 0, 0, fle::KEYS[f].help);
            _label[f]->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            _grid2->add(_label[f], 49, 5 * f + 1, -1, 4);
        }
        _height = (fle::FKEY_SIZE + 1) * flw::PREF_FONTSIZE * 2.5;
        _grid3->add(_reset,  -50,  -5,  16,   4);
        _grid3->add(_save,   -33,  -5,  16,   4);
        _grid3->add(_close,  -16,  -5,  16,   4);
        _grid1->add(_col[0],   1,   1,   8,   4);
        _grid1->add(_col[1],   7,   1,   8,   4);
        _grid1->add(_col[2],  13,   1,   8,   4);
        _grid1->add(_col[3],  19,   1,  14,   4);
        _grid1->add(_col[4],  25,   1,  10,   4);
        _grid1->add(_col[5],  49,   1,   0,   4);
        _scroll->add(_grid2);
        add(_grid1);
        add(_scroll);
        add(_grid3);
        _scroll->box(FL_ENGRAVED_FRAME);
        _close->callback(_KeyboardDialog::Callback, this);
        _close->tooltip("Close the dialog.");
        _reset->callback(_KeyboardDialog::Callback, this);
        _reset->tooltip(
            "Reset all keyboard settings.\n"
            "Remember to save before closing the dialog."
        );
        _save->callback(_KeyboardDialog::Callback, this);
        _save->tooltip(
            "Save user changes.\n"
            "Until save is called no changes are saved.\n"
            "If keyboard version changes all settings will be reset at start."
        );
        flw::util::labelfont(this);
        for (int f = 0; f < 6; f++) {
            _col[f]->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        }
        callback(_KeyboardDialog::Callback, this);
        set_modal();
        resizable(this);
        resize(0, 0, flw::PREF_FONTSIZE * 58, flw::PREF_FONTSIZE * 40);
        size_range(flw::PREF_FONTSIZE * 28, flw::PREF_FONTSIZE * 30);
        load_keys();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_KeyboardDialog*>(o);
        if (w == self || w == self->_close) {
            self->hide();
        }
        else if (w == self->_reset) {
            self->reset_keys();
        }
        else if (w == self->_save) {
            self->save_keys();
        }
    }
    int from_hex(const char* val) {
        errno = 0;
        auto ival = strtoul(val, nullptr, 16);
        if (errno != 0 || ival > 0xFFFF) {
            return -1;
        }
        return ival;
    }
    void load_keys() {
        for (int f = 0; f < fle::FKEY_SIZE; f++) {
            _alt[f]->value(KEYS[f].alt_u);
            _ctrl[f]->value(KEYS[f].ctrl_u);
            _shift[f]->value(KEYS[f].shift_u);
            _kommand[f]->value(KEYS[f].kommand_u);
            _key[f]->value(to_hex(KEYS[f].key_u));
            _key[f]->copy_label(KeyConf::KeyDescr(KEYS[f].key_u).c_str());
        }
        Fl::redraw();
    }
    void reset_keys() {
        for (int f = 0; f < fle::FKEY_SIZE; f++) {
            _alt[f]->value(KEYS[f].alt_d);
            _ctrl[f]->value(KEYS[f].ctrl_d);
            _shift[f]->value(KEYS[f].shift_d);
            _kommand[f]->value(KEYS[f].kommand_d);
            _key[f]->value(to_hex(KEYS[f].key_d));
            _key[f]->copy_label(KeyConf::KeyDescr(KEYS[f].key_d).c_str());
        }
        Fl::redraw();
    }
    void resize(int X, int Y, int W, int H) override {
        auto fs = flw::PREF_FONTSIZE;
        Fl_Double_Window::resize(X, Y, W, H);
        _grid1->resize  (fs,      0,           W - fs * 2,  fs * 2);
        _scroll->resize (fs,      fs * 2,      W - fs * 2,  H - fs * 5);
        _grid2->resize  (fs + 2,  0,           W - fs * 4,  _height);
        _grid3->resize  (fs,      H - fs * 3,  W - fs * 2,  fs * 3);
    }
    void run() {
        flw::util::center_window(this, Fl::first_window());
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
    }
    void save_keys() {
        auto err = 0;
        for (int f = 0; f < fle::FKEY_SIZE; f++) {
            auto hex = from_hex(_key[f]->value());
            if (hex < 0x20) {
                _col[4]->label("ERROR");
                _col[4]->labelcolor(FL_RED);
                _key[f]->take_focus();
                _key[f]->color(FL_RED);
                err++;
            }
            else {
                _key[f]->color(FL_BACKGROUND2_COLOR);
            }
        }
        Fl::redraw();
        if (err > 0) {
            fl_beep(FL_BEEP_ERROR);
            return;
        }
        _col[4]->label("Key");
        _col[4]->labelcolor(FL_FOREGROUND_COLOR);
        for (int f = 0; f < fle::FKEY_SIZE; f++) {
            fle::KEYS[f].alt_u     = _alt[f]->value();
            fle::KEYS[f].ctrl_u    = _ctrl[f]->value();
            fle::KEYS[f].shift_u   = _shift[f]->value();
            fle::KEYS[f].kommand_u = _kommand[f]->value();
            fle::KEYS[f].key_u     = from_hex(_key[f]->value());
            _key[f]->copy_label(KeyConf::KeyDescr(KEYS[f].key_u).c_str());
        }
    }
    const char* to_hex(int val) {
        snprintf(_buff, 256, "0x%X", val);
        return _buff;
    }
};
void dlg::keyboard(Config& config) {
    auto dlg = _KeyboardDialog(config);
    dlg.run();
}
LineNumDialog::LineNumDialog() : Fl_Double_Window(0, 0, 0, 0) {
    end();
    _input = new Fl_Int_Input(0, 0, 0, 0);
    _line  = -1;
    add(_input);
    _input->callback(LineNumDialog::Callback, this);
    _input->label("Goto line:");
    _input->labelsize(flw::PREF_FONTSIZE);
    _input->textfont(flw::PREF_FIXED_FONT);
    _input->textsize(flw::PREF_FONTSIZE);
    _input->when(FL_WHEN_ENTER_KEY_ALWAYS);
    callback(LineNumDialog::Callback, this);
}
void LineNumDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<LineNumDialog*>(o);
    if (w == self) {
        self->hide();
    }
    else if (w == self->_input) {
        self->_line = atoi(self->_input->value());
        self->hide();
    }
}
int LineNumDialog::handle(int event) {
    if (event == FL_UNFOCUS) {
        hide();
    }
    return Fl_Double_Window::handle(event);
}
void LineNumDialog::resize(int X, int Y, int W, int H) {
    auto fs = flw::PREF_FONTSIZE;
    Fl_Double_Window::resize(X, Y, W, H);
    _input->resize(fs * 7, 4, W - fs * 7 - 4, fs * 2);
}
int LineNumDialog::run() {
    auto win = Fl::first_window();
#if defined(_WIN32)
    set_modal();
    border(0);
    resize(0, 0, flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 2 + 8);
    show();
#elif defined(__APPLE__)
    set_modal();
    border(0);
    resize(0, 0, flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 2 + 8);
    show();
#else
    border(0);
    resize(0, 0, flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 2 + 8);
    show();
    set_modal();
#endif
    flw::util::center_window(this, win);
    while (visible() != 0) {
        Fl::wait();
        Fl::flush();
    }
    return _line;
}
FCASECOMPARE ReplaceDialog::CASECOMPARE = FCASECOMPARE::NO;
FNLTAB       ReplaceDialog::NLTAB       = FNLTAB::NO;
FREGEX       ReplaceDialog::REGEX       = FREGEX::NO;
FSELECTION   ReplaceDialog::SELECTION   = FSELECTION::NO;
FWORDCOMPARE ReplaceDialog::WORDCOMPARE = FWORDCOMPARE::NO;
ReplaceDialog::ReplaceDialog(std::string label, std::string& find, std::string& replace, const std::vector<std::string>& find_list, const std::vector<std::string>& replace_list) :
Fl_Double_Window(0, 0, 10, 10),
_find(find),
_replace(replace) {
    end();
    _cancel        = new Fl_Button(0, 0, 0, 0, "Cancel");
    _case          = new Fl_Check_Button(0, 0, 0, 0, "&Case");
    _find_input    = new flw::InputMenu(0, 0, 0, 0, "Find");
    _find_nl       = new Fl_Check_Button(0, 0, 0, 0, widgets::BUTTON_NL);
    _grid          = new flw::GridGroup(0, 0, w(), h());
    _help          = new Fl_Button(0, 0, 0, 0, "&Help");
    _ok            = new Fl_Button(0, 0, 0, 0, "Replace &all");
    _regex         = new Fl_Check_Button(0, 0, 0, 0, "Rege&x");
    _replace_input = new flw::InputMenu(0, 0, 0, 0, "Replace");
    _replace_nl    = new Fl_Check_Button(0, 0, 0, 0, widgets::BUTTON_NL);
    _selection     = new Fl_Check_Button(0, 0, 0, 0, "&Selection");
    _test          = new Fl_Button(0, 0, 0, 0, "&Test");
    _word          = new Fl_Check_Button(0, 0, 0, 0, "&Word");
    _ret           = false;
    _grid->add(_find_input,     10,   1,  -8,   4);
    _grid->add(_find_nl,        -7,   1,   6,   4);
    _grid->add(_replace_input,  10,   6,  -8,   4);
    _grid->add(_replace_nl,     -7,   6,   6,   4);
    _grid->add(_case,           10,  11,  15,   4);
    _grid->add(_word,           25,  11,  15,   4);
    _grid->add(_selection,      40,  11,  15,   4);
    _grid->add(_regex,          55,  11,  15,   4);
    _grid->add(_help,          -68,  -5,  16,   4);
    _grid->add(_cancel,        -51,  -5,  16,   4);
    _grid->add(_test,          -34,  -5,  16,   4);
    _grid->add(_ok,            -17,  -5,  16,   4);
    add(_grid);
    _cancel->callback(ReplaceDialog::Callback, this);
    _case->tooltip(widgets::TOOLTIP_FIND_CASE);
    _case->value(ReplaceDialog::CASECOMPARE == FCASECOMPARE::YES);
    _find_input->align(FL_ALIGN_LEFT);
    _find_input->values(find_list);
    _find_input->take_focus();
    _find_input->update_pref(flw::PREF_FIXED_FONT, flw::PREF_FONTSIZE);
    _find_nl->tooltip(widgets::TOOLTIP_FIND_NL);
    _find_nl->value(ReplaceDialog::NLTAB == FNLTAB::YES || ReplaceDialog::NLTAB == FNLTAB::FIND);
    _help->callback(ReplaceDialog::Callback, this);
    _ok->callback(ReplaceDialog::Callback, this);
    _regex->callback(ReplaceDialog::Callback, this);
    _regex->tooltip(widgets::TOOLTIP_FIND_REGEX);
    _regex->value(ReplaceDialog::REGEX == FREGEX::YES);
    _replace_input->align(FL_ALIGN_LEFT);
    _replace_input->values(replace_list);
    _replace_input->update_pref(flw::PREF_FIXED_FONT, flw::PREF_FONTSIZE);
    _replace_nl->tooltip(widgets::TOOLTIP_REPLACE_NL);
    _replace_nl->value(ReplaceDialog::NLTAB == FNLTAB::YES || ReplaceDialog::NLTAB == FNLTAB::REPLACE);
    _selection->callback(ReplaceDialog::Callback, this);
    _selection->tooltip(widgets::TOOLTIP_FIND_SELECTION);
    _selection->value(ReplaceDialog::SELECTION == FSELECTION::YES);
    _test->tooltip(widgets::TOOLTIP_TEST_REGEX);
    _test->callback(ReplaceDialog::Callback, this);
    _word->tooltip(widgets::TOOLTIP_FIND_WORD);
    _word->value(ReplaceDialog::WORDCOMPARE == FWORDCOMPARE::YES);
    tooltip(widgets::TOOLTIP_FIND_JUMP);
    check_buttons();
    flw::util::labelfont(this);
    copy_label(label.c_str());
    callback(ReplaceDialog::Callback, this);
    set_modal();
    resizable(_grid);
    ReplaceDialog::resize(0, 0, flw::PREF_FONTSIZE * 48, flw::PREF_FONTSIZE * 10.5);
    size_range(flw::PREF_FONTSIZE * 36, flw::PREF_FONTSIZE * 10.5);
}
void ReplaceDialog::Callback(Fl_Widget* w, void* o) {
    auto* self = static_cast<ReplaceDialog*>(o);
    if (w == self) {
        self->hide();
    }
    else if (w == self->_cancel) {
        self->hide();
    }
    else if (w == self->_help) {
        flw::dlg::list("Replace Text Help", help::replace_text() + help::pcre(), Fl::first_window(), true, 60, 50);
    }
    else if (w == self->_regex || w == self->_selection) {
        self->check_buttons();
    }
    else if (w == self->_test) {
        self->test_pcre();
    }
    else if (w == self->_ok && *self->_find_input->value()) {
        if (self->_regex->value() != 0 && self->test_pcre() == false) {
            self->_find_input->take_focus();
        }
        else {
            self->_find    = self->_find_input->value();
            self->_replace = self->_replace_input->value();
            self->_ret     = true;
            bool f = self->_regex->value() == 0 && self->_find_nl->value() != 0;
            bool r = self->_replace_nl->value() != 0;
            if (f == true && r == true) {
                ReplaceDialog::NLTAB = FNLTAB::YES;
            }
            else if (f == true) {
                ReplaceDialog::NLTAB = FNLTAB::FIND;
            }
            else if (r == true) {
                ReplaceDialog::NLTAB = FNLTAB::REPLACE;
            }
            else {
                ReplaceDialog::NLTAB = FNLTAB::NO;
            }
            ReplaceDialog::CASECOMPARE = (self->_case->value() != 0 && self->_case->active() != 0) ? FCASECOMPARE::YES : FCASECOMPARE::NO;
            ReplaceDialog::REGEX       = (self->_regex->value() != 0 && self->_regex->active() != 0) ? FREGEX::YES : FREGEX::NO;
            ReplaceDialog::SELECTION   = (self->_selection->value() != 0 && self->_selection->active() != 0) ? FSELECTION::YES : FSELECTION::NO;
            ReplaceDialog::WORDCOMPARE = (self->_word->value() != 0 && self->_word->active() != 0) ? FWORDCOMPARE::YES : FWORDCOMPARE::NO;
            self->hide();
        }
    }
}
void ReplaceDialog::check_buttons() {
    if (_regex->value() != 0) {
        _test->activate();
        _case->deactivate();
        _find_nl->deactivate();
        _word->deactivate();
    }
    else {
        _test->deactivate();
        _case->activate();
        _find_nl->activate();
        _word->activate();
    }
}
int ReplaceDialog::handle(int event) {
    if (event == FL_KEYBOARD) {
        if (Fl::event_alt() != 0 && Fl::event_key() == '1') {
            _find_input->input()->take_focus();
            return 1;
        }
        else if (Fl::event_alt() != 0 && Fl::event_key() == '2') {
            _replace_input->input()->take_focus();
            return 1;
        }
    }
    return Fl_Group::handle(event);
}
bool ReplaceDialog::run() {
    flw::util::center_window(this, Fl::first_window());
    show();
    while (visible() != 0) {
        Fl::wait();
        Fl::flush();
    }
    return _ret;
}
bool ReplaceDialog::test_pcre() {
    std::string string = _find_input->value();
    fl_message_position(this);
    if (string == "") {
       fl_alert("%s", widgets::ERROR_EMPTY_STRING);
        return false;
    }
    auto re = gnu::PCRE(string);
    if (re.is_compiled() == false) {
        fl_alert(widgets::ERROR_PCRE, re.error().c_str());
        return false;
    }
    return true;
}
class _SchemeDialogButton : public Fl_Button {
    std::string                 _label;
    bool                        _set_label_color;
    unsigned char               _r;
    unsigned char               _g;
    unsigned char               _b;
public:
    _SchemeDialogButton(std::string label, bool set_label_color) : Fl_Button(0, 0, 0, 0) {
        _label           = label;
        _set_label_color = set_label_color;
        _r               = 0;
        _g               = 0;
        _b               = 0;
        align(FL_ALIGN_RIGHT);
        box(FL_BORDER_BOX);
        tooltip(
            "Select new color.\n"
            "Press ctrl + button to reset color.\n"
            "Right click to copy hex values.\n"
            "Middle click to paste hex values.\n"
            "\n"
            "External hex values should look like this:\n"
            "0xff, 0xff, 0xff."
        );
    }
    void copy_color() const {
        char buf[200];
        snprintf(buf, 200, "0x%02x, 0x%02x, 0x%02x", _r, _g, _b);
        Fl::copy(buf, strlen(buf), 2);
    }
    int handle(int event) override {
        if (event == FL_PASTE) {
            auto str = gnu::str::to_string(Fl::event_text());
            if (str != "") {
                gnu::PCRE rx(
                    "\\W*(0[xX][0-9a-fA-F]{2})"
                    "\\W*(0[xX][0-9a-fA-F]{2})"
                    "\\W*(0[xX][0-9a-fA-F]{2})"
                );
                if (rx.exec(str) == 4) {
                    auto r = (uint8_t) strtoul(rx.substr(1).c_str(), nullptr, 0);
                    auto g = (uint8_t) strtoul(rx.substr(2).c_str(), nullptr, 0);
                    auto b = (uint8_t) strtoul(rx.substr(3).c_str(), nullptr, 0);
                    auto c = fl_rgb_color(r, g, b);
                    set_color(c);
                }
            }
            return 1;
        }
        return Fl_Button::handle(event);
    }
    void paste_color() {
        Fl::paste(*this, 1);
    }
    void set_color(Fl_Color col) {
        char buf[200];
        Fl::get_color(col, _r, _g, _b);
        snprintf(buf, 200, "%s: (0x%02x, 0x%02x, 0x%02x)", _label.c_str(), _r, _g, _b);
        color(col);
        if (_set_label_color == true) {
            labelcolor(col);
        }
        copy_label(buf);
        redraw();
    }
    void set_new_color() {
        if (fl_color_chooser("Select Color", _r, _g, _b, 2) != 0) {
            auto col = fl_rgb_color(_r, _g, _b);
            set_color(col);
        }
    }
};
class _SchemeDialog : public Fl_Double_Window {
public:
    Config&                     _config;
    Fl_Button*                  _close;
    Fl_Button*                  _reset;
    Fl_Button*                  _save;
    Fl_Menu_Button*             _attr[style::STYLE_SIZE];
    Fl_Menu_Button*             _font[style::STYLE_SIZE];
    Fl_Menu_Button*             _scheme;
    _SchemeDialogButton*        _colors[style::STYLE_SIZE];
    flw::GridGroup*             _grid;
    _SchemeDialog(Config& config) :
    Fl_Double_Window(0, 0, 10, 10, "Select Color Scheme"),
    _config(config) {
        end();
        _close  = new Fl_Button(0, 0, 0, 0, "&Close");
        _grid   = new flw::GridGroup(0, 0, w(), h());
        _reset  = new Fl_Button(0, 0, 0, 0, "Reset");
        _save   = new Fl_Button(0, 0, 0, 0, "Save");
        _scheme = new Fl_Menu_Button(0, 0, 0, 0);
        _scheme->add(style::SCHEME_DEF, 0, nullptr, nullptr, FL_MENU_RADIO);
        _scheme->add(style::SCHEME_LIGHT, 0, nullptr, nullptr, FL_MENU_RADIO);
        _scheme->add(style::SCHEME_TAN, 0, nullptr, nullptr, FL_MENU_RADIO);
        _scheme->add(style::SCHEME_BLUE, 0, nullptr, nullptr, FL_MENU_RADIO);
        _scheme->add(style::SCHEME_DARK, 0, nullptr, nullptr, FL_MENU_RADIO);
        _scheme->add(style::SCHEME_NEON, 0, nullptr, nullptr, FL_MENU_RADIO);
        _scheme->copy_label(_config.pref_scheme.c_str());
        _scheme->callback(CallbackScheme, this);
        _scheme->textfont(flw::PREF_FONT);
        _scheme->textsize(flw::PREF_FONTSIZE);
        _scheme->tooltip("Select color scheme.");
        flw::menu::setonly_item(_scheme, _config.pref_scheme.c_str());
        _grid->add(_scheme, 1, 1, -1, 4);
        for (int f = 0; f <= style::STYLE_LAST; f++) {
            _colors[f] = new _SchemeDialogButton(style::STYLE_NAMES[f], style::STYLE_FONTS[f] != nullptr);
            _colors[f]->callback(_SchemeDialog::CallbackColorButton, this);
            _grid->add(_colors[f], 49, 6 + f * 5, -50, 4);
            if (style::STYLE_FONTS[f] != nullptr) {
                _font[f] = new Fl_Menu_Button(0, 0, 0, 0);
                _font[f]->add(widgets::SCHEME_FONT_REGULAR, 0, nullptr, nullptr, FL_MENU_RADIO);
                _font[f]->add(widgets::SCHEME_FONT_BOLD, 0, nullptr, nullptr, FL_MENU_RADIO);
                _font[f]->add(widgets::SCHEME_FONT_ITALIC, 0, nullptr, nullptr, FL_MENU_RADIO);
                _font[f]->add(widgets::SCHEME_FONT_BOLD_ITALIC, 0, nullptr, nullptr, FL_MENU_RADIO);
                _font[f]->callback(_SchemeDialog::CallbackFont, this);
                _font[f]->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
                _font[f]->label(widgets::SCHEME_FONT_REGULAR);
                _font[f]->textfont(flw::PREF_FONT);
                _font[f]->textsize(flw::PREF_FONTSIZE);
                _font[f]->tooltip(
                    "To use bold and italic fonts the main editor font must have them in right order.\n"
                    "Selected font must be a regular font.\n"
                    "Then these three must be straight after the regular font, 'bold', 'italic', 'bold italic'.\n"
                    "Press 'Save' before closing dialog to update changes."
                );
                flw::menu::setonly_item(_font[f], widgets::SCHEME_FONT_REGULAR);
                _grid->add(_font[f], 1, 6 + f * 5, 18, 4);
            }
            else {
                _font[f] = nullptr;
            }
            if (style::STYLE_FONTS[f] != nullptr && *style::STYLE_FONTS[f]) {
                _attr[f] = new Fl_Menu_Button(0, 0, 0, 0);
                _attr[f]->add(widgets::SCHEME_ATTR_NONE, 0, nullptr, nullptr, FL_MENU_RADIO);
                _attr[f]->add(widgets::SCHEME_ATTR_GRAMMAR, 0, nullptr, nullptr, FL_MENU_RADIO);
                _attr[f]->add(widgets::SCHEME_ATTR_STRIKE, 0, nullptr, nullptr, FL_MENU_RADIO);
                _attr[f]->add(widgets::SCHEME_ATTR_UNDERLINE, 0, nullptr, nullptr, FL_MENU_RADIO);
                _attr[f]->add(widgets::SCHEME_ATTR_BGCOLOR, 0, nullptr, nullptr, FL_MENU_RADIO);
                _attr[f]->add(widgets::SCHEME_ATTR_BGCOLOR_EXT, 0, nullptr, nullptr, FL_MENU_RADIO);
                _attr[f]->callback(_SchemeDialog::CallbackAttr, this);
                _attr[f]->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
                _attr[f]->label(widgets::SCHEME_ATTR_NONE);
                _attr[f]->textfont(flw::PREF_FONT);
                _attr[f]->textsize(flw::PREF_FONTSIZE);
                _attr[f]->tooltip(
                    "Additional style for text.\n"
                    "If background color attribute is selected then the color will apply only to the background.\n"
                    "And text color will be the foreground color.\n"
                    "Press 'Save' before closing dialog to update changes."
                );
                flw::menu::setonly_item(_attr[f], widgets::SCHEME_ATTR_NONE);
                _grid->add(_attr[f], 22, 6 + f * 5, 26, 4);
            }
            else {
                _attr[f] = nullptr;
            }
        }
        _grid->add(_reset,  -51,  -5,  16,   4);
        _grid->add(_save,   -34,  -5,  16,   4);
        _grid->add(_close,  -17,  -5,  16,   4);
        add(_grid);
        init_widgets();
        _close->callback(_SchemeDialog::Callback, this);
        _close->tooltip("Close the dialog.");
        _reset->callback(_SchemeDialog::Callback, this);
        _reset->tooltip(
            "Reset properties for the current scheme.\n"
            "Remember to save before closing the dialog."
        );
        _save->callback(_SchemeDialog::Callback, this);
        _save->tooltip("Save user changes to current scheme.");
        flw::util::labelfont(this);
        callback(_SchemeDialog::Callback, this);
        set_modal();
        resizable(_grid);
        resize(0, 0, flw::PREF_FONTSIZE * 60, flw::PREF_FONTSIZE * 49);
        size_range(flw::PREF_FONTSIZE * 54, flw::PREF_FONTSIZE * 49);
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_SchemeDialog*>(o);
        if (w == self || w == self->_close) {
            self->hide();
        }
        else if (w == self->_reset) {
            self->reset_scheme();
        }
        else if (w == self->_save) {
            self->save_scheme();
        }
    }
    static void CallbackAttr(Fl_Widget* w, void*) {
        auto menu  = static_cast<Fl_Menu_Button*>(w);
        auto label = gnu::str::to_string(menu->text());
        menu->copy_label(label.c_str());
    }
    static void CallbackColorButton(Fl_Widget* w, void* o) {
        auto self   = static_cast<_SchemeDialog*>(o);
        auto button = static_cast<_SchemeDialogButton*>(w);
        if (Fl::event_command() != 0) {
            auto index = self->get_index(button);
            if (index >= 0 && index <= style::STYLE_LAST) {
                auto prop = style::get_style_prop(self->_config.pref_scheme, static_cast<style::STYLE>(index));
                prop->reset();
                self->init_attr(self->_attr[index], prop->attr());
                self->init_font(self->_font[index], prop->font());
                button->set_color(prop->color());
            }
        }
        else if (Fl::event_button() == FL_RIGHT_MOUSE) {
            button->copy_color();
        }
        else if (Fl::event_button() == FL_MIDDLE_MOUSE) {
            button->paste_color();
        }
        else {
            button->set_new_color();
        }
    }
    static void CallbackFont(Fl_Widget* w, void*) {
        auto menu  = static_cast<Fl_Menu_Button*>(w);
        auto label = gnu::str::to_string(menu->text());
        menu->copy_label(label.c_str());
    }
    static void CallbackScheme(Fl_Widget*, void* o) {
        auto self  = static_cast<_SchemeDialog*>(o);
        auto label = gnu::str::to_string(self->_scheme->text());
        if (label != "") {
            self->_config.pref_scheme = label;
            self->init_widgets();
            flw::menu::setonly_item(self->_scheme, label.c_str());
            self->_scheme->copy_label(label.c_str());
            self->_config.send_message(message::PREF_CHANGED);
        }
    }
    int get_index(Fl_Widget* widget) {
        for (int f = 0; f <= style::STYLE_LAST; f++) {
            if (_colors[f] == widget || _font[f] == widget) {
                return f;
            }
        }
        return -1;
    }
    void init_attr(Fl_Menu_Button* attr, int value) {
        if (attr != nullptr) {
            std::string l = widgets::SCHEME_ATTR_NONE;
            if (value == Fl_Text_Display::ATTR_GRAMMAR) {
                l = widgets::SCHEME_ATTR_GRAMMAR;
            }
            else if (value == Fl_Text_Display::ATTR_STRIKE_THROUGH) {
                l = widgets::SCHEME_ATTR_STRIKE;
            }
            else if (value == Fl_Text_Display::ATTR_UNDERLINE) {
                l = widgets::SCHEME_ATTR_UNDERLINE;
            }
            else if (value == Fl_Text_Display::ATTR_BGCOLOR) {
                l = widgets::SCHEME_ATTR_BGCOLOR;
            }
            else if (value == Fl_Text_Display::ATTR_BGCOLOR_EXT) {
                l = widgets::SCHEME_ATTR_BGCOLOR_EXT;
            }
            attr->copy_label(l.c_str());
            flw::menu::setonly_item(attr, l.c_str());
        }
    }
    void init_font(Fl_Menu_Button* font, int value) {
        if (font != nullptr) {
            std::string l = widgets::SCHEME_FONT_REGULAR;
            if (value == 3) {
                l = widgets::SCHEME_FONT_BOLD_ITALIC;
            }
            else if (value == 2) {
                l = widgets::SCHEME_FONT_ITALIC;
            }
            else if (value == 1) {
                l = widgets::SCHEME_FONT_BOLD;
            }
            font->copy_label(l.c_str());
            flw::menu::setonly_item(font, l.c_str());
        }
    }
    void init_widgets() {
        for (int f = 0; f <= style::STYLE_LAST; f++) {
            auto prop = style::get_style_prop(_config.pref_scheme, static_cast<style::STYLE>(f));
            init_attr(_attr[f], prop->attr());
            init_font(_font[f], prop->font());
            _colors[f]->set_color(prop->color());
        }
    }
    void reset_scheme() {
        style::reset_style(_config.pref_scheme);
        init_widgets();
        Fl::redraw();
    }
    void run() {
        flw::util::center_window(this, Fl::first_window());
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
    }
    void save_scheme() {
        for (int f = 0; f <= style::STYLE_LAST; f++) {
            auto attr = _attr[f];
            auto font = _font[f];
            auto prop = style::get_style_prop(_config.pref_scheme, static_cast<style::STYLE>(f));
            prop->color_u = _colors[f]->color();
            if (attr != nullptr) {
                auto label = gnu::str::to_string(attr->label());
                prop->attr_u = 0;
                if (label == widgets::SCHEME_ATTR_GRAMMAR) {
                    prop->attr_u = Fl_Text_Display::ATTR_GRAMMAR;
                }
                else if (label == widgets::SCHEME_ATTR_STRIKE) {
                    prop->attr_u = Fl_Text_Display::ATTR_STRIKE_THROUGH;
                }
                else if (label == widgets::SCHEME_ATTR_UNDERLINE) {
                    prop->attr_u = Fl_Text_Display::ATTR_UNDERLINE;
                }
                else if (label == widgets::SCHEME_ATTR_BGCOLOR) {
                    prop->attr_u = Fl_Text_Display::ATTR_BGCOLOR;
                }
                else if (label == widgets::SCHEME_ATTR_BGCOLOR_EXT) {
                    prop->attr_u = Fl_Text_Display::ATTR_BGCOLOR_EXT;
                }
            }
            if (font != nullptr) {
                auto label = gnu::str::to_string(font->label());
                prop->bold_u   = false;
                prop->italic_u = false;
                if (label == widgets::SCHEME_FONT_BOLD) {
                    prop->bold_u = true;
                }
                else if (label == widgets::SCHEME_FONT_ITALIC) {
                    prop->italic_u = true;
                }
                else if (label == widgets::SCHEME_FONT_BOLD_ITALIC) {
                    prop->bold_u   = true;
                    prop->italic_u = true;
                }
            }
        }
        _config.send_message(message::PREF_CHANGED);
    }
};
void dlg::scheme(Config& config) {
    auto dlg = _SchemeDialog(config);
    dlg.run();
}
#define FLE_STATUSBAR_CB(X) [](Fl_Widget*, void* o) { static_cast<StatusBar*>(o)->X; }, this
StatusBar::StatusBar(Config& config) :
flw::GridGroup(),
_config(config)  {
    _label_cursor        = new Fl_Box(0, 0, 0, 0, "");
    _label_cursor_mode = new Fl_Box(0, 0, 0, 0, "");
    _label_message     = new Fl_Box(0, 0, 0, 0, "");
    _line_menu         = new Fl_Menu_Button(0, 0, 0, 0, "");
    _style_menu        = new Fl_Menu_Button(0, 0, 0, 0, "");
    _tab_menu          = new Fl_Menu_Button(0, 0, 0, 0, "");
    add(_label_cursor,        0,   0,  46,   0);
    add(_label_message,      46,   0, -50,   0);
    add(_label_cursor_mode, -50,   0,   6,   0);
    add(_tab_menu,          -44,   0,  16,   0);
    add(_style_menu,        -28,   0,  16,   0);
    add(_line_menu,         -12,   0,  12,   0);
    adjust(_label_cursor_mode, -1,   1,   0,   0);
    adjust(_label_message,     -1,   1,   0,   0);
    adjust(_line_menu,         -1,   1,   0,   0);
    adjust(_style_menu,        -1,   1,   0,   0);
    adjust(_tab_menu,          -1,   1,   0,   0);
    _label_cursor_mode->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
    _label_cursor_mode->box(FL_BORDER_BOX);
    _label_cursor_mode->tooltip("Cursor mode, insert or overwrite.");
    _label_cursor->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
    _label_cursor->box(FL_BORDER_BOX);
    _label_cursor->tooltip("Cursor pos and text selection.");
    _label_message->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
    _label_message->box(FL_BORDER_BOX);
    _label_message->tooltip("General messages are shown here.");
    _line_menu->box(FL_BORDER_BOX);
    _line_menu->tooltip("Set lineending that will be used when saving file.");
    _style_menu->box(FL_BORDER_BOX);
    _style_menu->tooltip("Select syntax highlighting color scheme for current editor.");
    _tab_menu->box(FL_BORDER_BOX);
    _tab_menu->tooltip(
        "Set tab width and mode for current text.\n"
        "Or change the default tab values (for current syntax).\n"
        "Default values will be used next time a file is loaded."
    );
    box(FL_FLAT_BOX);
    _line_menu->add(widgets::STATUSBAR_LINE_UNIX,       0, FLE_STATUSBAR_CB(callback_line()), FL_MENU_RADIO);
    _line_menu->add(widgets::STATUSBAR_LINE_WIN,        0, FLE_STATUSBAR_CB(callback_line()), FL_MENU_RADIO);
    _style_menu->add(style::CPP,                        0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::LUA,                        0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::PYTHON,                     0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::RUBY,                       0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::GO,                         0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::RUST,                       0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::KOTLIN,                     0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::JAVA,                       0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::CS,                         0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::WREN,                       0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::JS,                         0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::TS,                         0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::PHP,                        0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::MARKUP,                     0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::MAKEFILE,                   0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::SHELL,                      0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::BAT,                        0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _style_menu->add(style::TEXT,                       0, FLE_STATUSBAR_CB(callback_syntax()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_HARD,         0, FLE_STATUSBAR_CB(callback_tab()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_SOFT,         0, FLE_STATUSBAR_CB(callback_tab()), FL_MENU_RADIO | FL_MENU_DIVIDER);
    _tab_menu->add(widgets::STATUSBAR_TABS_TO_SPACES,   0, FLE_STATUSBAR_CB(callback_tab()));
    _tab_menu->add(widgets::STATUSBAR_SPACES_TO_TABS,   0, FLE_STATUSBAR_CB(callback_tab()), FL_MENU_DIVIDER);
    _tab_menu->add(widgets::STATUSBAR_TAB_WIDTH1,       0, FLE_STATUSBAR_CB(callback_tab()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_WIDTH2,       0, FLE_STATUSBAR_CB(callback_tab()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_WIDTH3,       0, FLE_STATUSBAR_CB(callback_tab()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_WIDTH4,       0, FLE_STATUSBAR_CB(callback_tab()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_WIDTH5,       0, FLE_STATUSBAR_CB(callback_tab()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_WIDTH6,       0, FLE_STATUSBAR_CB(callback_tab()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_WIDTH7,       0, FLE_STATUSBAR_CB(callback_tab()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_WIDTH8,       0, FLE_STATUSBAR_CB(callback_tab()), FL_MENU_RADIO | FL_MENU_DIVIDER);
    _tab_menu->add(widgets::STATUSBAR_TAB_DEF_HARD,     0, FLE_STATUSBAR_CB(callback_tab_def()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_DEF_SOFT,     0, FLE_STATUSBAR_CB(callback_tab_def()), FL_MENU_RADIO | FL_MENU_DIVIDER);
    _tab_menu->add(widgets::STATUSBAR_TAB_DEF_WIDTH1,   0, FLE_STATUSBAR_CB(callback_tab_def()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_DEF_WIDTH2,   0, FLE_STATUSBAR_CB(callback_tab_def()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_DEF_WIDTH3,   0, FLE_STATUSBAR_CB(callback_tab_def()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_DEF_WIDTH4,   0, FLE_STATUSBAR_CB(callback_tab_def()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_DEF_WIDTH5,   0, FLE_STATUSBAR_CB(callback_tab_def()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_DEF_WIDTH6,   0, FLE_STATUSBAR_CB(callback_tab_def()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_DEF_WIDTH7,   0, FLE_STATUSBAR_CB(callback_tab_def()), FL_MENU_RADIO);
    _tab_menu->add(widgets::STATUSBAR_TAB_DEF_WIDTH8,   0, FLE_STATUSBAR_CB(callback_tab_def()), FL_MENU_RADIO);
    if (_config.disable_lineending == true) {
        _line_menu->deactivate();
    }
    if (_config.disable_style == true) {
        _style_menu->deactivate();
    }
    if (_config.disable_tab == true) {
        _tab_menu->deactivate();
    }
    update_pref();
}
void StatusBar::callback_line() {
    std::string label = _line_menu->text();
    if (label == widgets::STATUSBAR_LINE_WIN) {
        _config.send_message(message::STATUSBAR_LINE_WIN);
    }
    else if (label == widgets::STATUSBAR_LINE_UNIX) {
        _config.send_message(message::STATUSBAR_LINE_UNIX);
    }
}
void StatusBar::callback_syntax() {
    std::string label = _style_menu->text();
    if (label == style::BAT) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::BAT);
    }
    else if (label == style::CPP) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::CPP);
    }
    else if (label == style::CS) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::CS);
    }
    else if (label == style::GO) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::GO);
    }
    else if (label == style::MARKUP) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::MARKUP);
    }
    else if (label == style::JAVA) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::JAVA);
    }
    else if (label == style::JS) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::JS);
    }
    else if (label == style::KOTLIN) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::KOTLIN);
    }
    else if (label == style::LUA) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::LUA);
    }
    else if (label == style::MAKEFILE) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::MAKEFILE);
    }
    else if (label == style::SHELL) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::SHELL);
    }
    else if (label == style::PHP) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::PHP);
    }
    else if (label == style::PYTHON) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::PYTHON);
    }
    else if (label == style::RUBY) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::RUBY);
    }
    else if (label == style::RUST) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::RUST);
    }
    else if (label == style::TEXT) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::TEXT);
    }
    else if (label == style::TS) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::TS);
    }
    else if (label == style::WREN) {
        _config.send_message(message::STATUSBAR_STYLE_CHANGED, style::WREN);
    }
}
void StatusBar::callback_tab() {
    std::string label = _tab_menu->text();
    std::string name  = _style_menu->label();
    if (label == widgets::STATUSBAR_TAB_HARD) {
        _config.send_message(message::STATUSBAR_TAB_CHANGED, "FTAB::HARD");
    }
    else if (label == widgets::STATUSBAR_TAB_SOFT) {
        _config.send_message(message::STATUSBAR_TAB_CHANGED, "FTAB::SOFT");
    }
    else if (label == widgets::STATUSBAR_SPACES_TO_TABS) {
        _config.send_message(message::STATUSBAR_TO_TABS, "TO_TABS");
        _config.send_message(message::STATUSBAR_TAB_CHANGED, "FTAB::HARD");
    }
    else if (label == widgets::STATUSBAR_TABS_TO_SPACES) {
        _config.send_message(message::STATUSBAR_TO_SPACES, "TO_SPACES");
        _config.send_message(message::STATUSBAR_TAB_CHANGED, "FTAB::SOFT");
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH1) {
        _config.send_message(message::STATUSBAR_TAB_CHANGED, "1");
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH2) {
        _config.send_message(message::STATUSBAR_TAB_CHANGED, "2");
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH3) {
        _config.send_message(message::STATUSBAR_TAB_CHANGED, "3");
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH4) {
        _config.send_message(message::STATUSBAR_TAB_CHANGED, "4");
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH5) {
        _config.send_message(message::STATUSBAR_TAB_CHANGED, "5");
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH6) {
        _config.send_message(message::STATUSBAR_TAB_CHANGED, "6");
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH7) {
        _config.send_message(message::STATUSBAR_TAB_CHANGED, "7");
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH8) {
        _config.send_message(message::STATUSBAR_TAB_CHANGED, "8");
    }
}
void StatusBar::callback_tab_def() {
    std::string label = _tab_menu->text();
    std::string name  = _style_menu->label();
    if (label == widgets::STATUSBAR_TAB_HARD) {
        style::set_tab_type(name, FTAB::HARD);
    }
    else if (label == widgets::STATUSBAR_TAB_SOFT) {
        style::set_tab_type(name, FTAB::SOFT);
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH1) {
        style::set_tab_width(name, 1);
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH2) {
        style::set_tab_width(name, 2);
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH3) {
        style::set_tab_width(name, 3);
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH4) {
        style::set_tab_width(name, 4);
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH5) {
        style::set_tab_width(name, 5);
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH6) {
        style::set_tab_width(name, 6);
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH7) {
        style::set_tab_width(name, 7);
    }
    else if (label == widgets::STATUSBAR_TAB_WIDTH8) {
        style::set_tab_width(name, 8);
    }
}
bool StatusBar::label_cursor(std::string val) {
    if (val == _label_cursor->label()) {
        return false;
    }
    _label_cursor->copy_label(val.c_str());
    _label_cursor->redraw();
    return true;
}
bool StatusBar::label_cursor_mode(std::string val) {
    if (_config.pref_insert == false) {
        _label_cursor_mode->deactivate();
    }
    else {
        _label_cursor_mode->activate();
    }
    if (val == _label_cursor_mode->label()) {
        return false;
    }
    _label_cursor_mode->copy_label(val.c_str());
    _label_cursor_mode->redraw();
    return true;
}
bool StatusBar::label_message(std::string val) {
    val = gnu::str::replace(val, "@", "@@");
    if (val == _label_message->label()) {
        return false;
    }
    _label_message->copy_label(val.c_str());
    _label_message->redraw();
    return true;
}
void StatusBar::update_menus(Editor* editor) {
    {
        if (editor->file_line_ending() == FLINEENDING::WINDOWS) {
            _line_menu->copy_label(widgets::STATUSBAR_LINE_WIN);
            flw::menu::setonly_item(_line_menu, widgets::STATUSBAR_LINE_WIN);
        }
        else {
            _line_menu->copy_label(widgets::STATUSBAR_LINE_UNIX);
            flw::menu::setonly_item(_line_menu, widgets::STATUSBAR_LINE_UNIX);
        }
    }
    {
        auto tab_mode    = editor->text_tab_mode();
        auto tab_width   = editor->text_tab_width();
        auto label_width = gnu::str::format("Tab width: %u", tab_width);
        auto label_mode  = (tab_mode == FTAB::HARD) ? widgets::STATUSBAR_TAB_HARD : widgets::STATUSBAR_TAB_SOFT;
        flw::menu::setonly_item(_tab_menu, label_mode);
        flw::menu::setonly_item(_tab_menu, label_width.c_str());
        _tab_menu->copy_label(gnu::str::format("%s: %u", (tab_mode == FTAB::HARD) ? "Tab size" : "Spaces", tab_width).c_str());
    }
    {
        auto tab_mode    = editor->style().tab_mode();
        auto tab_width   = editor->style().tab_width();
        auto label_width = gnu::str::format("Default/Tab width: %u", tab_width);
        auto label_mode  = (tab_mode == FTAB::HARD) ? widgets::STATUSBAR_TAB_DEF_HARD : widgets::STATUSBAR_TAB_DEF_SOFT;
        flw::menu::setonly_item(_tab_menu, label_mode);
        flw::menu::setonly_item(_tab_menu, label_width.c_str());
    }
    {
        auto style_name = editor->style().name();
        flw::menu::setonly_item(_style_menu, style_name.c_str());
        _style_menu->copy_label(style_name.c_str());
    }
}
void StatusBar::update_pref() {
    _style_menu->labelfont(flw::PREF_FONT);
    _style_menu->labelsize(flw::PREF_FONTSIZE - _config.pref_shrink_status);
    _style_menu->textfont(flw::PREF_FONT);
    _style_menu->textsize(flw::PREF_FONTSIZE - _config.pref_shrink_status);
    _tab_menu->labelfont(flw::PREF_FONT);
    _tab_menu->labelsize(flw::PREF_FONTSIZE - _config.pref_shrink_status);
    _tab_menu->textfont(flw::PREF_FONT);
    _tab_menu->textsize(flw::PREF_FONTSIZE - _config.pref_shrink_status);
    _line_menu->labelfont(flw::PREF_FONT);
    _line_menu->labelsize(flw::PREF_FONTSIZE - _config.pref_shrink_status);
    _line_menu->textfont(flw::PREF_FONT);
    _line_menu->textsize(flw::PREF_FONTSIZE - _config.pref_shrink_status);
    _label_cursor_mode->labelfont(flw::PREF_FONT);
    _label_cursor_mode->labelsize(flw::PREF_FONTSIZE - _config.pref_shrink_status);
    _label_cursor->labelfont(flw::PREF_FONT);
    _label_cursor->labelsize(flw::PREF_FONTSIZE - _config.pref_shrink_status);
    _label_message->labelfont(flw::PREF_FONT);
    _label_message->labelsize(flw::PREF_FONTSIZE - _config.pref_shrink_status);
    _h = flw::PREF_FONTSIZE + 8;
}
class _TweakDialog : public Fl_Double_Window {
    Fl_Box*                     _label;
    Fl_Button*                  _close;
    Fl_Button*                  _reset;
    Fl_Button*                  _save;
    Fl_Check_Button*            _force;
    Fl_Hor_Slider*              _auto_file_size;
    Fl_Hor_Slider*              _auto_lines;
    Fl_Hor_Slider*              _auto_word_size;
    Fl_Hor_Slider*              _backup_file_size;
    Fl_Hor_Slider*              _count_char;
    Fl_Hor_Slider*              _file_size;
    Fl_Hor_Slider*              _out_lines;
    Fl_Hor_Slider*              _out_line_length;
    Fl_Hor_Slider*              _style_size;
    Fl_Hor_Slider*              _wrap_length;
    flw::GridGroup*             _grid;
public:
    _TweakDialog() :
    Fl_Double_Window(0, 0, 10, 10, "Editor Tweaks") {
        end();
        _auto_file_size   = new Fl_Hor_Slider(0, 0, 0, 0);
        _auto_lines       = new Fl_Hor_Slider(0, 0, 0, 0);
        _auto_word_size   = new Fl_Hor_Slider(0, 0, 0, 0);
        _backup_file_size = new Fl_Hor_Slider(0, 0, 0, 0);
        _close            = new Fl_Button(0, 0, 0, 0, "&Close");
        _count_char       = new Fl_Hor_Slider(0, 0, 0, 0);
        _file_size        = new Fl_Hor_Slider(0, 0, 0, 0);
        _force            = new Fl_Check_Button(0, 0, 0, 0, "Force style update on every text change");
        _grid             = new flw::GridGroup(0, 0, w(), h());
        _label            = new Fl_Box(0, 0, 0, 0);
        _out_line_length  = new Fl_Hor_Slider(0, 0, 0, 0);
        _out_lines        = new Fl_Hor_Slider(0, 0, 0, 0);
        _reset            = new Fl_Button(0, 0, 0, 0, "Reset");
        _save             = new Fl_Button(0, 0, 0, 0, "Save");
        _style_size       = new Fl_Hor_Slider(0, 0, 0, 0);
        _wrap_length      = new Fl_Hor_Slider(0, 0, 0, 0);
        _grid->add(_force,              1,   1,  -1,   4);
        _grid->add(_file_size,          1,   8,  -1,   4);
        _grid->add(_style_size,         1,  16,  -1,   4);
        _grid->add(_backup_file_size,   1,  24,  -1,   4);
        _grid->add(_auto_lines,         1,  32,  -1,   4);
        _grid->add(_auto_file_size,     1,  40,  -1,   4);
        _grid->add(_auto_word_size,     1,  48,  -1,   4);
        _grid->add(_out_lines,          1,  56,  -1,   4);
        _grid->add(_out_line_length,    1,  64,  -1,   4);
        _grid->add(_wrap_length,        1,  72,  -1,   4);
        _grid->add(_count_char,         1,  80,  -1,   4);
        _grid->add(_label,              1,  85,  -1,   8);
        _grid->add(_reset,            -51,  -5,  16,   4);
        _grid->add(_save,             -34,  -5,  16,   4);
        _grid->add(_close,            -17,  -5,  16,   4);
        add(_grid);
        flw::util::labelfont(this);
        _auto_file_size->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _auto_file_size->callback(_TweakDialog::Callback, this);
        _auto_file_size->color(FL_BACKGROUND2_COLOR);
        _auto_file_size->range(limits::AUTOCOMPLETE_FILESIZE_MIN, limits::AUTOCOMPLETE_FILESIZE_MAX);
        _auto_file_size->precision(0);
        _auto_file_size->step((int) limits::AUTOCOMPLETE_FILESIZE_STEP);
        _auto_file_size->value(limits::AUTOCOMPLETE_FILESIZE_VAL);
        _auto_lines->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _auto_lines->callback(_TweakDialog::Callback, this);
        _auto_lines->color(FL_BACKGROUND2_COLOR);
        _auto_lines->range(limits::AUTOCOMPLETE_LINES_MIN, limits::AUTOCOMPLETE_LINES_MAX);
        _auto_lines->precision(0);
        _auto_lines->step((int) limits::AUTOCOMPLETE_LINES_STEP);
        _auto_lines->value(limits::AUTOCOMPLETE_LINES_VAL);
        _auto_word_size->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _auto_word_size->callback(_TweakDialog::Callback, this);
        _auto_word_size->color(FL_BACKGROUND2_COLOR);
        _auto_word_size->range(limits::AUTOCOMPLETE_WORD_SIZE_MIN, limits::AUTOCOMPLETE_WORD_SIZE_MAX);
        _auto_word_size->precision(0);
        _auto_word_size->step((int) limits::AUTOCOMPLETE_WORD_SIZE_STEP);
        _auto_word_size->value(limits::AUTOCOMPLETE_WORD_SIZE_VAL);
        _backup_file_size->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _backup_file_size->callback(_TweakDialog::Callback, this);
        _backup_file_size->color(FL_BACKGROUND2_COLOR);
        _backup_file_size->range(limits::FILE_BACKUP_SIZE_MIN, limits::FILE_BACKUP_SIZE_MAX);
        _backup_file_size->precision(0);
        _backup_file_size->step((int) limits::FILE_BACKUP_SIZE_STEP);
        _backup_file_size->value(limits::FILE_BACKUP_SIZE_VAL);
        _close->callback(_TweakDialog::Callback, this);
        _count_char->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _count_char->callback(_TweakDialog::Callback, this);
        _count_char->color(FL_BACKGROUND2_COLOR);
        _count_char->range(limits::COUNT_CHAR_MIN, limits::COUNT_CHAR_MAX);
        _count_char->precision(0);
        _count_char->step((int) limits::COUNT_CHAR_STEP);
        _count_char->value(limits::COUNT_CHAR_VAL);
        _file_size->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _file_size->callback(_TweakDialog::Callback, this);
        _file_size->color(FL_BACKGROUND2_COLOR);
        _file_size->range(limits::FILE_SIZE_MIN, limits::FILE_SIZE_MAX);
        _file_size->precision(0);
        _file_size->step((int) limits::FILE_SIZE_STEP);
        _file_size->value(limits::FILE_SIZE_VAL);
        _file_size->tooltip("Max size of hex view is 1/5 of file size.");
        _force->tooltip(
            "This will slow down text input considerably for large files (> 1-5MB).\n"
            "Turn on if styling is broken.\n"
            "Markup files are always restyled for every text change."
        );
        _force->value(limits::FORCE_RESTYLING);
        _label->align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _label->box(FL_BORDER_BOX);
        _label->label(
            "Press save to update values.\n"
            "These will be saved to preferences file.\n"
        );
        _out_lines->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _out_lines->callback(_TweakDialog::Callback, this);
        _out_lines->color(FL_BACKGROUND2_COLOR);
        _out_lines->range(limits::OUTPUT_LINES_MIN, limits::OUTPUT_LINES_MAX);
        _out_lines->precision(0);
        _out_lines->step((int) limits::OUTPUT_LINES_STEP);
        _out_lines->value(limits::OUTPUT_LINES_VAL);
        _out_line_length->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _out_line_length->callback(_TweakDialog::Callback, this);
        _out_line_length->color(FL_BACKGROUND2_COLOR);
        _out_line_length->range(limits::OUTPUT_LINE_LENGTH_MIN, limits::OUTPUT_LINE_LENGTH_MAX);
        _out_line_length->precision(0);
        _out_line_length->step((int) limits::OUTPUT_LINE_LENGTH_STEP);
        _out_line_length->value(limits::OUTPUT_LINE_LENGTH_VAL);
        _reset->callback(_TweakDialog::Callback, this);
        _reset->tooltip("Reset all values to default values.");
        _save->callback(_TweakDialog::Callback, this);
        _save->tooltip("Save must be pressed to update global settings.");
        _style_size->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _style_size->callback(_TweakDialog::Callback, this);
        _style_size->color(FL_BACKGROUND2_COLOR);
        _style_size->range(limits::STYLE_FILESIZE_MIN, limits::STYLE_FILESIZE_MAX);
        _style_size->precision(0);
        _style_size->step((int) limits::STYLE_FILESIZE_STEP);
        _style_size->value(limits::STYLE_FILESIZE_VAL);
        _wrap_length->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _wrap_length->callback(_TweakDialog::Callback, this);
        _wrap_length->color(FL_BACKGROUND2_COLOR);
        _wrap_length->range(limits::WRAP_LINE_LENGTH_MIN, limits::WRAP_LINE_LENGTH_MAX);
        _wrap_length->precision(0);
        _wrap_length->step((int) limits::WRAP_LINE_LENGTH_STEP);
        _wrap_length->value(limits::WRAP_LINE_LENGTH_VAL);
        _TweakDialog::Callback(_auto_file_size, this);
        _TweakDialog::Callback(_auto_lines, this);
        _TweakDialog::Callback(_auto_word_size, this);
        _TweakDialog::Callback(_backup_file_size, this);
        _TweakDialog::Callback(_count_char, this);
        _TweakDialog::Callback(_file_size, this);
        _TweakDialog::Callback(_out_line_length, this);
        _TweakDialog::Callback(_out_lines, this);
        _TweakDialog::Callback(_style_size, this);
        _TweakDialog::Callback(_wrap_length, this);
        callback(_TweakDialog::Callback, this);
        set_modal();
        resizable(_grid);
        size_range(flw::PREF_FONTSIZE * 25, flw::PREF_FONTSIZE * 50);
        size(flw::PREF_FONTSIZE * 50, flw::PREF_FONTSIZE * 50);
        flw::util::center_window(this, Fl::first_window());
        _grid->do_layout();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_TweakDialog*>(o);
        if (w == self || w == self->_close) {
            self->hide();
        }
        else if (w == self->_reset) {
            self->reset();
        }
        else if (w == self->_save) {
            self->update();
        }
        else if (w == self->_auto_file_size) {
            auto l = gnu::str::format("Max file size for autocomplete: %s", gnu::str::format_int(self->_auto_file_size->value()).c_str());
            self->_auto_file_size->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_auto_lines) {
            auto l = gnu::str::format("Max lines for autocomplete: %s", gnu::str::format_int(self->_auto_lines->value()).c_str());
            self->_auto_lines->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_auto_word_size) {
            auto l = gnu::str::format("Max word size for autocomplete: %s", gnu::str::format_int(self->_auto_word_size->value()).c_str());
            self->_auto_word_size->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_backup_file_size) {
            auto l = gnu::str::format("Max file size for backups: %s", gnu::str::format_int(self->_backup_file_size->value()).c_str());
            self->_backup_file_size->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_count_char) {
            auto l = gnu::str::format("Max text selection to count utf8 characters: %s", gnu::str::format_int(self->_count_char->value()).c_str());
            self->_count_char->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_file_size) {
            auto l = gnu::str::format("Max file size: %s", gnu::str::format_int(self->_file_size->value()).c_str());
            self->_file_size->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_out_lines) {
            auto l = gnu::str::format("Max lines for output: %s", gnu::str::format_int(self->_out_lines->value()).c_str());
            self->_out_lines->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_out_line_length) {
            auto l = gnu::str::format("Max line length for output: %s", gnu::str::format_int(self->_out_line_length->value()).c_str());
            self->_out_line_length->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_style_size) {
            auto l = gnu::str::format("Max file size for highlightning: %s", gnu::str::format_int(self->_style_size->value()).c_str());
            self->_style_size->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_wrap_length) {
            auto l = gnu::str::format("Max line length before wrap is turned on file load: %s", gnu::str::format_int(self->_wrap_length->value()).c_str());
            self->_wrap_length->copy_label(l.c_str());
            self->redraw();
        }
    }
    void reset() {
        limits::AUTOCOMPLETE_FILESIZE_VAL  = limits::AUTOCOMPLETE_FILESIZE_DEF;
        limits::AUTOCOMPLETE_LINES_VAL     = limits::AUTOCOMPLETE_LINES_DEF;
        limits::AUTOCOMPLETE_WORD_SIZE_VAL = limits::AUTOCOMPLETE_WORD_SIZE_DEF;
        limits::COUNT_CHAR_VAL             = limits::COUNT_CHAR_DEF;
        limits::FILE_BACKUP_SIZE_VAL       = limits::FILE_BACKUP_SIZE_DEF;
        limits::FILE_SIZE_VAL              = limits::FILE_SIZE_DEF;
        limits::FORCE_RESTYLING            = 0;
        limits::OUTPUT_LINES_VAL           = limits::OUTPUT_LINES_DEF;
        limits::OUTPUT_LINE_LENGTH_VAL     = limits::OUTPUT_LINE_LENGTH_DEF;
        limits::STYLE_FILESIZE_VAL         = limits::STYLE_FILESIZE_DEF;
        limits::WRAP_LINE_LENGTH_VAL       = limits::WRAP_LINE_LENGTH_DEF;
        _force->value(limits::FORCE_RESTYLING);
        _auto_file_size->value(limits::AUTOCOMPLETE_FILESIZE_VAL);
        _auto_lines->value(limits::AUTOCOMPLETE_LINES_VAL);
        _auto_word_size->value(limits::AUTOCOMPLETE_WORD_SIZE_VAL);
        _backup_file_size->value(limits::FILE_BACKUP_SIZE_VAL);
        _count_char->value(limits::COUNT_CHAR_VAL);
        _file_size->value(limits::FILE_SIZE_VAL);
        _out_line_length->value(limits::OUTPUT_LINE_LENGTH_VAL);
        _out_lines->value(limits::OUTPUT_LINES_VAL);
        _style_size->value(limits::STYLE_FILESIZE_VAL);
        _wrap_length->value(limits::WRAP_LINE_LENGTH_VAL);
        _TweakDialog::Callback(_auto_file_size, this);
        _TweakDialog::Callback(_auto_lines, this);
        _TweakDialog::Callback(_auto_word_size, this);
        _TweakDialog::Callback(_backup_file_size, this);
        _TweakDialog::Callback(_count_char, this);
        _TweakDialog::Callback(_file_size, this);
        _TweakDialog::Callback(_out_line_length, this);
        _TweakDialog::Callback(_out_lines, this);
        _TweakDialog::Callback(_style_size, this);
        _TweakDialog::Callback(_wrap_length, this);
        _label->label("Settings has been reset to default values.");
    }
    void run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
    }
    void update() {
        limits::AUTOCOMPLETE_FILESIZE_VAL  = _auto_file_size->value();
        limits::AUTOCOMPLETE_LINES_VAL     = _auto_lines->value();
        limits::AUTOCOMPLETE_WORD_SIZE_VAL = _auto_word_size->value();
        limits::COUNT_CHAR_VAL             = _count_char->value();
        limits::FILE_BACKUP_SIZE_VAL       = _backup_file_size->value();
        limits::FILE_SIZE_VAL              = _file_size->value();
        limits::FORCE_RESTYLING            = _force->value();
        limits::OUTPUT_LINES_VAL           = _out_lines->value();
        limits::OUTPUT_LINE_LENGTH_VAL     = _out_line_length->value();
        limits::STYLE_FILESIZE_VAL         = _style_size->value();
        limits::WRAP_LINE_LENGTH_VAL       = _wrap_length->value();
        _label->label("Settings has been updated.");
    }
};
void dlg::tweaks() {
    auto dlg = _TweakDialog();
    dlg.run();
}
}
#include <FL/fl_ask.H>
namespace fle {
View::View(Config& config, Editor* editor) : Fl_Text_Editor(0, 0, 0, 0), Message(config), _config(config) {
    end();
    _editor = editor;
    buffer(&_editor->buffer());
}
View::~View() {
    buffer(nullptr);
}
void View::CallbackStyleBuffer(const int pos, const int inserted_size, const int deleted_size, const int restyled_size, const char* deleted_text, void* o) {
    (void) restyled_size;
    auto editor = static_cast<Editor*>(o);
#ifdef DEBUG
#endif
    if (editor->buffer().has_restyle() == false) {
        return;
    }
    else if (editor->style().name() == style::TEXT) {
        return;
    }
    char* deleted_style = nullptr;
    if (deleted_size > 0) {
       editor->style_buffer().remove(pos, pos + deleted_size);
    }
    if (inserted_size > 0) {
        auto buffer = gnu::File::Allocate(nullptr, inserted_size + 1);
        memset(buffer, style::STYLE_INIT, inserted_size);
        editor->style_buffer().insert(pos, buffer);
        free(buffer);
    }
    editor->style().update(pos, inserted_size, deleted_size, deleted_text, deleted_style, editor);
    free(deleted_style);
#ifdef DEBUG
    if (editor->buffer().length() != editor->style_buffer().length()) {
        fl_alert("debug: error in View::CallbackStyleBuffer()\nTEXTSIZE=%d, STYLESIZE=%d !!",  editor->buffer().length(), editor->style_buffer().length());
    }
#endif
}
void View::callback_connect() {
    _editor->buffer().add_modify_callback(View::CallbackStyleBuffer, _editor);
}
void View::callback_disconnect() {
    _editor->buffer().remove_modify_callback(View::CallbackStyleBuffer, _editor);
}
int View::handle(int event) {
    auto ro = _editor->text_is_readonly();
    if (event == FL_DRAG) {
        _editor->update_statusbar();
    }
    if (event == FL_PUSH) {
        if (Fl::event_button() == FL_LEFT_MOUSE && Fl::event_clicks() == 1) {
            _editor->text_select_word();
        }
        else if (Fl::event_button() == FL_RIGHT_MOUSE) {
            _editor->show_menu();
            return 1;
        }
    }
    else if (event == FL_RELEASE) {
        auto r = Fl_Text_Editor::handle(event);
        _editor->update_statusbar();
        return r;
    }
    else if (event == FL_DND_RELEASE) {
        if (ro == true) {
            _editor->statusbar_set_message(errors::TEXT_IS_READ_ONLY);
            return 1;
        }
        _editor->text_set_dnd_event(true);
        return 1;
    }
    else if (event == FL_PASTE) {
        if (ro == true) {
            _editor->statusbar_set_message(errors::TEXT_IS_READ_ONLY);
            return 1;
        }
        else if (_editor->text_has_dnd_event() == true) {
            _editor->text_set_dnd_event(false);
            if (_handle_dnd() == true) {
                return 1;
            }
        }
    }
    else if (event == FL_FOCUS) {
        _editor->view_set(this);
        _editor->update_after_focus();
    }
    else if (event == FL_MOUSEWHEEL) {
        if (_handle_mousewheel() == true) {
            return 1;
        }
        else {
            Fl_Text_Editor::handle(event);
            return 1;
        }
    }
    else if (event == FL_KEYBOARD) {
        if (ro == true) {
            _editor->statusbar_set_message(errors::TEXT_IS_READ_ONLY);
            _editor->update_statusbar();
            return 0;
        }
        else if (_handle_key() == 1) {
            _editor->update_statusbar();
            return 1;
        }
        auto r = Fl_Text_Editor::handle(event);
        _editor->update_statusbar();
        return r;
    }
    return Fl_Text_Editor::handle(event);
}
bool View::_handle_dnd() {
    auto str = gnu::str::to_string(Fl::event_text());
    if (str != "") {
        auto discard = false;
        _config.send_message(message::DND_EVENT, str, &discard);
        return discard;
    }
    return false;
}
#define FLE_VIEW_CLEAR_KOMMAND_AND_RET(X) if (_editor->text_has_kommand() == true && _editor->statusbar_message() == strings::COMMAND_MODE) { _editor->statusbar_set_message(""); } _editor->text_set_kommand(false); return X;
#define FLE_VIEW_KEY(X) (KEYS[X].key_u == key && KEYS[X].alt_u == alt && KEYS[X].shift_u == shift && KEYS[X].ctrl_u == ctrl)
#define FLE_VIEW_KOMMAND_KEY(X) (KEYS[X].key_u == key && KEYS[X].kommand_u == kommand && KEYS[X].alt_u == alt && KEYS[X].shift_u == shift && KEYS[X].ctrl_u == ctrl)
int View::_handle_key() {
    int  key     = Fl::event_key();
    bool alt     = Fl::event_alt();
    bool ctrl    = Fl::event_ctrl();
    bool shift   = Fl::event_shift();
    bool kommand = _editor->text_has_kommand();
    if (key == FL_Insert) {
        if (_config.pref_insert == true) {
            Fl_Text_Editor::handle(FL_KEYBOARD);
            _editor->update_statusbar();
            if (_editor->view_not_active() != nullptr) {
                _editor->view_not_active()->insert_mode(_editor->view().insert_mode());
            }
        }
        return 1;
    }
    else if (ctrl == true && key == 'v') {
        Fl::paste(*this, 1);
        return 1;
    }
    else if (FLE_VIEW_KEY(FKEY_KOMMAND_START)) {
        _editor->statusbar_set_message(strings::COMMAND_MODE);
        _editor->text_set_kommand(1);
        return 1;
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_KOMMAND_STOP)) {
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (key == FL_Escape) {
        if (_editor->findreplace().visible() != 0) {
            _config.send_message(message::HIDE_FIND, "", &_editor->findreplace());
        }
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (_editor->text_has_kommand() == true && (key == FL_Right || key == FL_Left || key == FL_Up || key == FL_Down || key == FL_Page_Up || key == FL_Page_Down || key == FL_Home || key == FL_End)) {
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(0)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_AUTOCOMPLETE)) {
        _editor->show_autocomplete();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_BOOKMARKS_NEXT)) {
        _editor->bookmarks().goto_next();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_BOOKMARKS_PREV)) {
        _editor->bookmarks().goto_prev();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_BOOKMARKS_TOGGLE)) {
        _editor->bookmarks().toggle();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_CASE_LOWER)) {
        _editor->text_convert_case(FCASE::LOWER);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_CASE_UPPER)) {
        _editor->text_convert_case(FCASE::UPPER);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_COMMENT_BLOCK)) {
        _editor->text_comment_block();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_COMMENT_LINE)) {
        _editor->text_comment_line();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_COPY_TEXT)) {
        if (_editor->buffer().cut_or_copy_line(insert_position(), FCOPY::COPY_LINE) == true) {
            FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
        }
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_CUT_TEXT)) {
        if (_editor->buffer().cut_or_copy_line(insert_position(), FCOPY::CUT_LINE)) {
            FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
        }
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_DELETE_LINE_LEFT)) {
        _editor->buffer().set_backspace_key();
        _editor->buffer().delete_text_left(insert_position(), FDELTEXT::LINE);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_DELETE_LINE_RIGHT)) {
        _editor->buffer().set_delete_key();
        _editor->buffer().delete_text_right(insert_position(), FDELTEXT::LINE);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_DELETE_WORD_LEFT)) {
        _editor->buffer().set_backspace_key();
        auto ret = _editor->buffer().delete_text_left(insert_position(), FDELTEXT::WORD);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(ret)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_DELETE_WORD_RIGHT)) {
        _editor->buffer().set_delete_key();
        auto ret = _editor->buffer().delete_text_right(insert_position(), FDELTEXT::WORD);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(ret)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_DUP_TEXT)) {
        _editor->text_duplicate_line_or_selection();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_ENTER1)) {
        auto pos = _editor->buffer().indent(FINDENT::BREAKLINE);
        if (pos.has_cursor() == true) {
            _editor->cursor_move(pos);
            FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
        }
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_ENTER2)) {
        auto pos = _editor->buffer().indent(FINDENT::ADDLINE);
        if (pos.has_cursor() == true) {
            _editor->cursor_move(pos);
            FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
        }
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_FIND_LINES)) {
        _editor->show_find_lines_dialog_or_run_again(FFINDLINES::GETINPUT);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_FIND_LINES_AGAIN)) {
        _editor->show_find_lines_dialog_or_run_again(FFINDLINES::RUNAGAIN);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_GOTO_LINE)) {
        _editor->show_line_dialog();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_HELP)) {
        _editor->help();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_HOME)) {
        if (_editor->home() == true) {
            FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
        }
    }
    else if (FLE_VIEW_KEY(FKEY_MENU)) {
        _editor->show_menu();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_MOVE_UP)) {
        _editor->text_move_lines(FMOVEV::UP);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_MOVE_DOWN)) {
        _editor->text_move_lines(FMOVEV::DOWN);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_OUTPUT_NEXT)) {
        _editor->callback_output(1);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_OUTPUT_PREV)) {
        _editor->callback_output(-1);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_OUTPUT_TOGGLE)) {
        _editor->show_output(FOUTPUT::TOGGLE);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_QUICK_FIND)) {
        _editor->find_quick();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_SELECT_LINE)) {
        _editor->text_select_line();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_SHOW_FIND_REPLACE)) {
        _config.send_message(message::SHOW_FIND, "", &_editor->findreplace());
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_SORT_ASCENDING)) {
        _editor->text_sort_lines(FSORT::ASCENDING);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_SORT_DESCENDING)) {
        _editor->text_sort_lines(FSORT::DESCENDING);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_SEARCH_BACKWARD)) {
        _editor->find_replace(FSEARCHDIR::BACKWARD);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_SEARCH_FORWARD)) {
        _editor->find_replace(FSEARCHDIR::FORWARD);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_REPLACE)) {
        _editor->find_replace(_editor->find_search_dir(), true);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (_editor->undo_mode() == FUNDO::FLE && FLE_VIEW_KOMMAND_KEY(FKEY_REDO)) {
        _editor->undo_forward();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (_editor->undo_mode() == FUNDO::FLE && FLE_VIEW_KOMMAND_KEY(FKEY_UNDO)) {
        _editor->undo_back();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_VIEW_1)) {
        if (&_editor->view() != _editor->view1()) {
            _editor->view1()->take_focus();
        }
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_VIEW_2)) {
        if (_editor->view2() != nullptr && &_editor->view() != _editor->view2()) {
            _editor->view2()->take_focus();
        }
        else if (_editor->view2() == nullptr) {
            _editor->view_set_split(_editor->view_split());
        }
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_VIEW_CLOSE)) {
        _editor->view_set_split(FSPLITVIEW::NO);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_SHIFT_LEFT)) {
        _editor->text_insert_tab_or_move_lines_left_right(FMOVEH::LEFT);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_SHIFT_RIGHT)) {
        _editor->text_insert_tab_or_move_lines_left_right(FMOVEH::RIGHT);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (key == FL_BackSpace) {
        _editor->buffer().set_backspace_key();
        auto res = _editor->buffer().delete_indent(insert_position(), _editor->text_tab_mode(), _editor->text_tab_width());
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(res)
    }
    else if (key == FL_Delete) {
        _editor->buffer().set_delete_key();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(0)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_SELECT_PAIR1) || FLE_VIEW_KOMMAND_KEY(FKEY_SELECT_PAIR2)) {
        _editor->select_pair(shift);
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (FLE_VIEW_KOMMAND_KEY(FKEY_SELECT_STYLE)) {
        _editor->select_color();
        FLE_VIEW_CLEAR_KOMMAND_AND_RET(1)
    }
    else if (kommand == 1) {
        return 1;
    }
    return 0;
}
bool View::_handle_mousewheel() {
    auto org = _config.pref_tmp_fontsize;
    if (Fl::event_ctrl() != 0 && Fl::event_alt() == 0) {
        if (Fl::event_dy() > 0) {
            if (_config.pref_tmp_fontsize == 0) {
                _config.pref_tmp_fontsize = flw::PREF_FIXED_FONTSIZE - 1;
            }
            else if (_config.pref_tmp_fontsize > 6) {
                _config.pref_tmp_fontsize--;
            }
        }
        else if (Fl::event_dy() < 0) {
            if (_config.pref_tmp_fontsize == 0) {
                _config.pref_tmp_fontsize = flw::PREF_FIXED_FONTSIZE + 1;
            }
            else if (_config.pref_tmp_fontsize < 72) {
                _config.pref_tmp_fontsize++;
            }
        }
        _editor->statusbar_set_message(gnu::str::format("Fontsize changed to %d", _config.pref_tmp_fontsize));
    }
    else if (Fl::event_dy() > 0) {
        auto lines = (Fl::event_alt() != 0) ? _config.pref_mouse_scroll * 3 + 6 : _config.pref_mouse_scroll;
        mVScrollBar->value(mVScrollBar->value() + lines);
    }
    else if (Fl::event_dy() < 0) {
        auto lines = (Fl::event_alt() != 0) ? _config.pref_mouse_scroll * 3 + 6 : _config.pref_mouse_scroll;
        mVScrollBar->value(mVScrollBar->value() - lines);
    }
    if (org != _config.pref_tmp_fontsize) {
        _config.send_message(message::FONTSIZE2_CHANGED);
    }
    return Fl::event_ctrl();
}
void View::init(View* view1) {
    insert_mode(view1->insert_mode());
    update_pref(false);
    take_focus();
}
Message::CTRL View::message(const std::string& message, const std::string&, const void*) {
    if (message == message::FONTSIZE2_CHANGED) {
        update_pref(false);
    }
    else if (message == message::FILE_LOADED) {
        update_pref(false);
    }
    else if (message == message::RESET_INSERT_MODE) {
        insert_mode(1);
        if (_editor->view_not_active() != nullptr) {
            _editor->view_not_active()->insert_mode(1);
        }
        _editor->update_statusbar();
    }
    return Message::CTRL::CONTINUE;
}
int View::take_focus() {
    Fl_Text_Editor::take_focus();
    _editor->update_statusbar();
    return 1;
}
bool View::update_pref(bool wrap_for_view2) {
    auto fs         = (_config.pref_tmp_fontsize == 0) ? flw::PREF_FIXED_FONTSIZE : _config.pref_tmp_fontsize;
    auto width      = fs * 3.5;
    auto cursor_pos = insert_position();
    auto table      = style::get_table(_config.pref_scheme, fs);
    if (_config.pref_tmp_fontsize == flw::PREF_FIXED_FONTSIZE) {
        _config.pref_tmp_fontsize = 0;
    }
    if (_editor->buffer().length() > 200'000'000) {
        width = fs * 6;
    }
    else if (_editor->buffer().length() > 20'000'000) {
        width = fs * 5;
    }
    else if (_editor->buffer().length() > 1'000'000) {
        width = fs * 4.5;
    }
    else if (_editor->buffer().length() > 50'000) {
        width = fs * 4;
    }
    if (_config.pref_linenumber == true) {
        linenumber_width(width);
    }
    else {
        linenumber_width(0);
    }
    if (wrap_for_view2 == true || _editor->wrap_col() != _config.pref_wrap) {
        if (wrap_for_view2 == false) {
            _editor->wrap_set_col(_config.pref_wrap);
        }
        if (_editor->wrap_mode() == FWRAP::YES) {
            auto wc = flw::WaitCursor();
            if (_config.pref_wrap == 66) {
                wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
            }
            else {
                wrap_mode(Fl_Text_Display::WRAP_AT_COLUMN, _config.pref_wrap);
            }
        }
        else {
            wrap_mode(Fl_Text_Display::WRAP_NONE, 0);
        }
        wrap_for_view2 = true;
    }
    highlight_data(&_editor->style_buffer(), table, style::STYLE_SIZE, style::STYLE_FG, nullptr, 0);
    color(table[style::index(style::STYLE_BG)].color, table[style::index(style::STYLE_BG_SEL)].color);
    cursor_color(table[style::index(style::STYLE_CURSOR)].color);
    cursor_style(_config.pref_cursor);
    labelsize(flw::PREF_FONTSIZE);
    linenumber_align(FL_ALIGN_RIGHT);
    linenumber_bgcolor(table[style::index(style::STYLE_BG_NUM)].color);
    linenumber_fgcolor(table[style::index(style::STYLE_FG_NUM)].color);
    linenumber_font(table[style::index(style::STYLE_FG_NUM)].font);
    linenumber_format("%4d");
    linenumber_size(fs);
    scrollbar_width(flw::PREF_FONTSIZE);
    textcolor(table[style::index(style::STYLE_FG)].color);
    textfont(flw::PREF_FIXED_FONT);
    textsize(fs);
    grammar_underline_color(fl_contrast(table[style::index(style::STYLE_FG)].color, table[style::index(style::STYLE_BG)].color));
    if (_editor->view1() == this && _editor->text_tab_width() != (unsigned) _editor->buffer().tab_distance()) {
        _editor->buffer().tab_distance(_editor->text_tab_width());
    }
    insert_position(cursor_pos);
    redisplay_range(0, _editor->buffer().length());
    Fl::redraw();
    return wrap_for_view2;
}
}
#include <FL/Fl_Menu_Button.H>
namespace fle {
namespace menu {
#ifdef DEBUG
constexpr const char* DEBUG1                      = "Debug/Widget";
constexpr const char* DEBUG2                      = "Debug/Undo";
constexpr const char* DEBUG3                      = "Debug/Print style";
constexpr const char* DEBUG4                      = "Debug/Save style";
constexpr const char* DEBUG5                      = "Debug/Save style with no pos";
constexpr const char* DEBUG6                      = "Debug/Print style info";
constexpr const char* DEBUG7                      = "Debug/Compare text with file";
#endif
constexpr const char* BOOKMARKS_CLEAR             = "Bookmarks/Clear";
constexpr const char* BOOKMARKS_NEXT              = "Bookmarks/Next";
constexpr const char* BOOKMARKS_PREV              = "Bookmarks/Previous";
constexpr const char* BOOKMARKS_TOGGLE            = "Bookmarks/Toggle";
constexpr const char* CASE_LOWER                  = "Case/To lower";
constexpr const char* CASE_UPPER                  = "Case/To upper";
constexpr const char* COMMENT_BLOCK               = "Comment/Toggle block";
constexpr const char* COMMENT_LINE                = "Comment/Toggle line";
constexpr const char* EDIT_COPY                   = "Copy";
constexpr const char* EDIT_CUT                    = "Cut";
constexpr const char* EDIT_PASTE                  = "Paste";
constexpr const char* FIND_LINES                  = "Find lines...";
constexpr const char* KEYBOARD                    = "Keyboard config...";
constexpr const char* HELP                        = "Help...";
constexpr const char* OUTPUT_HORIZONTAL           = "Output/Horizontal";
constexpr const char* OUTPUT_TOGGLE               = "Output/Toggle";
constexpr const char* OUTPUT_VERTICAL             = "Output/Vertical";
constexpr const char* PRINT                       = "Print...";
constexpr const char* REDO                        = "Undo/Redo";
constexpr const char* REDO_ALL                    = "Undo/Redo all";
constexpr const char* SCHEME                      = "Color scheme...";
constexpr const char* SETTINGS                    = "Settings...";
constexpr const char* SORT_LINES_ASCENDING        = "Sort/Ascending";
constexpr const char* SORT_LINES_DESCENDING       = "Sort/Descending";
constexpr const char* SPLIT_CLOSE                 = "Split view/Close";
constexpr const char* SPLIT_HOR                   = "Split view/Horizontal";
constexpr const char* SPLIT_VER                   = "Split view/Vertical";
constexpr const char* TRIM_TRAILING               = "Trim trailing whitespace";
constexpr const char* TWEAKS                      = "Tweaks...";
constexpr const char* UNDO                        = "Undo/Undo";
constexpr const char* UNDO_ALL                    = "Undo/Undo all";
constexpr const char* UPDATE_AUTOCOMPLETE         = "Update/Autocomplete";
constexpr const char* UPDATE_STYLE                = "Update/Style";
constexpr const char* WRAP                        = "Word wrap";
}
#define FLE_EDITOR_CB(X)                    [](Fl_Widget*, void* o) { static_cast<Editor*>(o)->X; static_cast<Editor*>(o)->take_focus(); }, this
#define FLE_EDITOR_CB_CONFIG(X)             [](Fl_Widget*, void* o) { X(static_cast<Editor*>(o)->_config); static_cast<Editor*>(o)->take_focus(); }, this
#define FLE_EDITOR_RETURN_IF_READONLY_0()   if (text_is_readonly() == true) { statusbar_set_message(errors::TEXT_IS_READ_ONLY); return; }
#define FLE_EDITOR_RETURN_IF_READONLY_1(X)  if (text_is_readonly() == true) { statusbar_set_message(errors::TEXT_IS_READ_ONLY); return (X); }
Editor::Editor(Config& config, FindBar* findbar, int X, int Y, int W, int H) :
Fl_Group(X, Y, W, H),
Message(config),
_bookmarks(this),
_config(config),
_editor_flags() {
    assert(findbar);
    end();
    _style   = nullptr;
    _findbar = findbar;
    _regex   = new gnu::PCRE();
    _buf1    = new TextBuffer(this, _config);
    _buf2    = new TextBuffer(nullptr, _config);
    _editors = new flw::SplitGroup();
    _main    = new flw::SplitGroup();
    _menu    = new Fl_Menu_Button(0, 0, 0, 0);
    _output  = new flw::ScrollBrowser();
    _view1   = new View(_config, this);
    _view2   = nullptr;
    _view    = _view1;
    _main->add(_editors, flw::SplitGroup::CHILD::FIRST);
    _main->add(_output, flw::SplitGroup::CHILD::SECOND);
    _editors->add(_view1, flw::SplitGroup::CHILD::FIRST);
    add(_main);
    add(_menu);
    _main->direction(flw::SplitGroup::DIRECTION::HORIZONTAL);
    _output->callback(Editor::CallbackOutput, this);
    _output->hide();
    _output->textfont(flw::PREF_FONT);
    _output->when(FL_WHEN_ENTER_KEY_CHANGED);
    _findbar->findreplace().hide();
    callback_connect();
#ifdef DEBUG
    _menu->add(menu::DEBUG1, 0,                                                     FLE_EDITOR_CB(debug()));
    _menu->add(menu::DEBUG2, 0,                                                     FLE_EDITOR_CB(debug(2)), FL_MENU_DIVIDER);
    _menu->add(menu::DEBUG3, 0,                                                     FLE_EDITOR_CB(debug(1)));
    _menu->add(menu::DEBUG4, 0,                                                     FLE_EDITOR_CB(debug_save_style("style.txt")));
    _menu->add(menu::DEBUG5, 0,                                                     FLE_EDITOR_CB(debug_save_style("style.txt", false)));
    _menu->add(menu::DEBUG6, 0,                                                     FLE_EDITOR_CB(style().debug()), FL_MENU_DIVIDER);
    _menu->add(menu::DEBUG7, 0,                                                     FLE_EDITOR_CB(file_compare_buffer()));
#endif
    _menu->add(menu::EDIT_CUT,              FL_CTRL + 'x',                          FLE_EDITOR_CB(text_cut_to_clipboard()));
    _menu->add(menu::EDIT_COPY,             FL_CTRL + 'c',                          FLE_EDITOR_CB(text_copy_to_clipboard()));
    _menu->add(menu::EDIT_PASTE,            FL_CTRL + 'v',                          FLE_EDITOR_CB(text_insert_from_clipboard()), FL_MENU_DIVIDER);
    _menu->add(menu::HELP,                  KEYS[FKEY_HELP].to_int(),               FLE_EDITOR_CB(help()));
    _menu->add(menu::SETTINGS,              0,                                      FLE_EDITOR_CB_CONFIG(ShowSetup));
    _menu->add(menu::SCHEME,                0,                                      FLE_EDITOR_CB_CONFIG(ShowScheme));
    _menu->add(menu::KEYBOARD,              0,                                      FLE_EDITOR_CB_CONFIG(ShowKeyboardSetup));
    _menu->add(menu::TWEAKS,                0,                                      FLE_EDITOR_CB(ShowTweaks()), FL_MENU_DIVIDER);
    _menu->add(menu::PRINT   ,              0,                                      FLE_EDITOR_CB(show_print()));
    _menu->add(menu::TRIM_TRAILING,         0,                                      FLE_EDITOR_CB(text_remove_trailing()));
    _menu->add(menu::FIND_LINES,            KEYS[FKEY_FIND_LINES].to_int(),         FLE_EDITOR_CB(show_find_lines_dialog_or_run_again(FFINDLINES::GETINPUT)));
    _menu->add(menu::WRAP,                  0,                                      FLE_EDITOR_CB(wrap_toggle_mode()), FL_MENU_DIVIDER | FL_MENU_TOGGLE);
    _menu->add(menu::SPLIT_CLOSE,           KEYS[FKEY_VIEW_CLOSE].to_int(),         FLE_EDITOR_CB(view_set_split(FSPLITVIEW::NO)));
    _menu->add(menu::SPLIT_HOR,             0,                                      FLE_EDITOR_CB(view_set_split(FSPLITVIEW::HORIZONTAL)));
    _menu->add(menu::SPLIT_VER,             0,                                      FLE_EDITOR_CB(view_set_split(FSPLITVIEW::VERTICAL)));
    _menu->add(menu::OUTPUT_TOGGLE,         KEYS[FKEY_OUTPUT_TOGGLE].to_int(),      FLE_EDITOR_CB(show_output(FOUTPUT::TOGGLE)));
    _menu->add(menu::OUTPUT_HORIZONTAL,     0,                                      FLE_EDITOR_CB(show_output(FOUTPUT::SHOW_HORIZONTAL)));
    _menu->add(menu::OUTPUT_VERTICAL,       0,                                      FLE_EDITOR_CB(show_output(FOUTPUT::SHOW_VERTICAL)));
    _menu->add(menu::COMMENT_LINE,          KEYS[FKEY_COMMENT_LINE].to_int(),       FLE_EDITOR_CB(text_comment_line()));
    _menu->add(menu::COMMENT_BLOCK,         KEYS[FKEY_COMMENT_BLOCK].to_int(),      FLE_EDITOR_CB(text_comment_block()));
    _menu->add(menu::CASE_LOWER,            KEYS[FKEY_CASE_LOWER].to_int(),         FLE_EDITOR_CB(text_convert_case(FCASE::LOWER)));
    _menu->add(menu::CASE_UPPER,            KEYS[FKEY_CASE_UPPER].to_int(),         FLE_EDITOR_CB(text_convert_case(FCASE::UPPER)));
    _menu->add(menu::SORT_LINES_ASCENDING,  KEYS[FKEY_SORT_ASCENDING].to_int(),     FLE_EDITOR_CB(text_sort_lines(FSORT::ASCENDING)));
    _menu->add(menu::SORT_LINES_DESCENDING, KEYS[FKEY_SORT_DESCENDING].to_int(),    FLE_EDITOR_CB(text_sort_lines(FSORT::DESCENDING)));
    _menu->add(menu::BOOKMARKS_NEXT,        KEYS[FKEY_BOOKMARKS_NEXT].to_int(),     FLE_EDITOR_CB(_bookmarks.goto_next()));
    _menu->add(menu::BOOKMARKS_PREV,        KEYS[FKEY_BOOKMARKS_PREV].to_int(),     FLE_EDITOR_CB(_bookmarks.goto_prev()), FL_MENU_DIVIDER);
    _menu->add(menu::BOOKMARKS_TOGGLE,      KEYS[FKEY_BOOKMARKS_TOGGLE].to_int(),   FLE_EDITOR_CB(_bookmarks.toggle()));
    _menu->add(menu::BOOKMARKS_CLEAR,       0,                                      FLE_EDITOR_CB(_bookmarks.clear()));
    _menu->add(menu::UNDO,                  KEYS[FKEY_UNDO].to_int(),               FLE_EDITOR_CB(undo_back(false)));
    _menu->add(menu::UNDO_ALL,              0,                                      FLE_EDITOR_CB(undo_back(true)), FL_MENU_DIVIDER);
    _menu->add(menu::REDO,                  KEYS[FKEY_REDO].to_int(),               FLE_EDITOR_CB(undo_forward(false)));
    _menu->add(menu::REDO_ALL,              0,                                      FLE_EDITOR_CB(undo_forward(true)));
    _menu->add(menu::UPDATE_AUTOCOMPLETE,   0,                                      FLE_EDITOR_CB(update_autocomplete()));
    _menu->add(menu::UPDATE_STYLE,          0,                                      FLE_EDITOR_CB(style().update()));
    _menu->type(Fl_Menu_Button::POPUP3);
    if (_config.active == nullptr) {
        _config.active = this;
        _findbar->findreplace().callback(Editor::CallbackFind, this);
    }
    tooltip("");
    style(nullptr);
    update_pref();
    box(FL_THIN_DOWN_BOX);
}
Editor::~Editor() {
    if (_config.active == this) {
        _config.active = nullptr;
    }
    _editors->add(nullptr, flw::SplitGroup::CHILD::FIRST);
    _editors->add(nullptr, flw::SplitGroup::CHILD::SECOND);
    delete _regex;
    delete _style;
    delete _buf1;
    delete _buf2;
}
void Editor::activate() {
    Fl_Group::activate();
    _view1->activate();
    _findbar->findreplace().activate();
    _findbar->statusbar().activate();
    if (_view2 != nullptr) {
        _view2->activate();
    }
}
void Editor::CallbackFind(Fl_Widget* w, void* o) {
    auto  self = static_cast<Editor*>(o);
    auto& f    = self->findreplace();
    if (self->text_is_readonly() == true) {
        self->statusbar_set_message(errors::TEXT_IS_READ_ONLY);
    }
    else if (w == f.find_input()) {
        self->find_replace(self->_editor_flags.fsearchdir);
    }
    else if (w == f.next_button()) {
        self->_editor_flags.fsearchdir = FSEARCHDIR::FORWARD;
        self->find_replace(FSEARCHDIR::FORWARD);
    }
    else if (w == f.prev_button()) {
        self->_editor_flags.fsearchdir = FSEARCHDIR::BACKWARD;
        self->find_replace(self->_editor_flags.fsearchdir);
    }
    else if (w == f.replace_button()) {
        self->find_replace(self->_editor_flags.fsearchdir, true);
    }
    else if (w == f.replace_all_button()) {
        self->find_replace_all(f.find_string(), f.replace_string(), f.fnltab(), f.fselection(), f.fcasecompare(), f.fwordcompare(), f.fregex(), FSAVEWORD::YES, FHIDEFIND::YES);
    }
    else if (w == f.replace_input()) {
        self->find_replace(self->_editor_flags.fsearchdir, true);
    }
}
void Editor::CallbackOutput(Fl_Widget*, void* o) {
    auto self = static_cast<Editor*>(o);
    self->callback_output();
}
void Editor::callback_output(int add_line) {
    auto row = _output->value();
    auto re  = _regex;
    if (re->is_compiled() == false) {
        return;
    }
    row += add_line;
    if (row < 1) {
        row = _output->size();
    }
    else if (row > _output->size()) {
        row = 1;
    }
    if (row <= _output->size()) {
        auto text1   = _output->text(row);
        auto matches = re->exec(text1);
        _output->value(row);
        if (matches > 1) {
            auto line = gnu::str::to_int(re->substr("line"), 1);
            auto col  = gnu::str::to_int(re->substr("col"), 1);
            cursor_move_to_rowcol((int) line, (int) col);
            take_focus();
        }
    }
}
CursorPos Editor::cursor(bool top_set_line) {
    CursorPos res;
    if (_view2 == nullptr) {
        res.pos1 = _view1->insert_position();
        res.drag = _view1->drag_pos();
        if (top_set_line == true) {
            res.top1 = _view1->top_line();
        }
    }
    else if (_view1 == _view) {
        res.pos1 = _view1->insert_position();
        res.pos2 = _view2->insert_position();
        res.drag = _view1->drag_pos();
        if (top_set_line == true) {
            res.top1 = _view1->top_line();
            res.top2 = _view2->top_line();
        }
    }
    else {
        res.pos2 = _view1->insert_position();
        res.pos1 = _view2->insert_position();
        res.drag = _view2->drag_pos();
        res.swap = true;
        if (top_set_line == true) {
            res.top2 = _view1->top_line();
            res.top1 = _view2->top_line();
        }
    }
    auto start = 0;
    auto end   = 0;
    if (_buf1->selection_position(&start, &end) != 0) {
        res.start = start;
        res.end   = end;
    }
    else {
        res.drag = 0;
    }
    return res;
}
void Editor::cursor_move(CursorPos cursor) {
    if (cursor.start > cursor.end) {
        FLW_PRINT("ERROR", cursor.start, cursor.end)
        return;
    }
    auto pos1 = (cursor.swap == false) ? cursor.pos1 : cursor.pos2;
    auto pos2 = (cursor.swap == false) ? cursor.pos2 : cursor.pos1;
    auto top1 = (cursor.swap == false) ? cursor.top1 : cursor.top2;
    auto top2 = (cursor.swap == false) ? cursor.top2 : cursor.top1;
    if (_view2 == nullptr) {
        if (pos1 >= 0) {
            _view1->insert_position(_buf1->utf8_align(pos1));
            _view1->show_insert_position();
        }
        if (top1 >= 0 && wrap_mode() == FWRAP::NO) {
            _view1->top_set_line(top1);
        }
    }
    else {
        if (pos1 >= 0) {
            _view1->insert_position(_buf1->utf8_align(pos1));
            _view1->show_insert_position();
        }
        if (top1 >= 0 && wrap_mode() == FWRAP::NO) {
            _view1->top_set_line(top1);
        }
        if (pos2 >= 0) {
            _view2->insert_position(_buf1->utf8_align(pos2));
            _view2->show_insert_position();
        }
        if (top2 >= 0 && wrap_mode() == FWRAP::NO) {
            _view2->top_set_line(top2);
        }
    }
    if (pos1 >= 0 || pos2 >= 0) {
        auto drag_tmp = cursor.drag;
        if (cursor.start >= 0 && cursor.end > cursor.start) {
            _buf1->select(cursor.start, cursor.end);
        }
        else {
            _buf1->unselect();
            drag_tmp = 0;
        }
        if (drag_tmp >= 0) {
            _view1->drag_set_pos(drag_tmp);
            if (_view2 != nullptr) {
                _view2->drag_set_pos(drag_tmp);
            }
        }
    }
}
void Editor::cursor_move_to_pos(int pos, bool force_unselect) {
    _view->insert_position(_buf1->utf8_align(pos));
    _view->show_insert_position();
    if (force_unselect == true) {
        _buf1->unselect();
    }
    if (_findbar->statusbar().visible() != 0) {
        _findbar->statusbar().redraw();
    }
}
void Editor::cursor_move_to_rowcol(int row, int column) {
    auto pos = _buf1->skip_lines(0, row - 1);
    auto end = _buf1->line_end(pos);
    cursor_move_to_pos(pos + column > end ? end : pos + column - 1, true);
}
void Editor::deactivate() {
    Fl_Group::deactivate();
    _view1->deactivate();
    if (_view2 != nullptr) {
        _view2->deactivate();
    }
}
void Editor::debug(int what) {
#ifdef DEBUG
    printf("\n");
    printf("/***\n");
    printf(" *       __ _          ______    _ _ _             \n");
    printf(" *      / _| |     _ _|  ____|  | (_) |            \n");
    printf(" *     | |_| | ___(_|_) |__   __| |_| |_ ___  _ __ \n");
    printf(" *     |  _| |/ _ \\   |  __| / _` | | __/ _ \\| '__|\n");
    printf(" *     | | | |  __/_ _| |___| (_| | | || (_) | |   \n");
    printf(" *     |_| |_|\\___(_|_)______\\__,_|_|\\__\\___/|_|   \n");
    printf(" *                                                 \n");
    printf(" *                                                 \n");
    printf(" */\n");
    printf("\n");
    if (what == 1) {
        printf("%s\n", debug_save_style().c_str());
        _style->debug();
    }
    else if (what == 2) {
        if (_buf1->undo_mode() == FUNDO::FLE) {
            _buf1->undo()->debug(true);
        }
    }
    else {
        size_t b, s, u;
        auto t = memory_usage(b, s, u);
        printf("\nflw:\n");
        printf("    FONT               = %2d\n", flw::PREF_FONT);
        printf("    FONTNAME           = '%s'\n", flw::PREF_FONTNAME.c_str());
        printf("    FONTSIZE           = %2d\n", flw::PREF_FONTSIZE);
        printf("    FIXED_FONT         = %2d\n", flw::PREF_FIXED_FONT);
        printf("    FIXED_FONTNAME     = '%s'\n", flw::PREF_FIXED_FONTNAME.c_str());
        printf("    FIXED_FONTSIZE     = %2d\n", flw::PREF_FIXED_FONTSIZE);
        printf("\nWindow:\n");
        if (top_window() != nullptr) {
            printf("    (x, y, w, h)       = %04d, %04d, %04d, %04d\n", top_window()->x(), top_window()->y(), top_window()->w(), top_window()->h());
        }
        _config.debug();
        printf("\nEditor:\n");
        printf("    this               = %p\n", this);
        printf("    id                 = %9d\n", object_id());
        printf("    autocomplete words = %9d\n", (int) _words.size());
        printf("    changed_name       = '%s'\n", file_changed_name().c_str());
        printf("    output_regex       = %9d\n", _regex ? 1 : 0);
        printf("    buffer size        = %9llu\n", (long long unsigned) b);
        printf("    style size         = %9llu\n", (long long unsigned) s);
        printf("    undo capacity      = %9llu\n", (long long unsigned) u);
        printf("    total              = %9llu\n", (long long unsigned) t);
        printf("\nView:\n");
        printf("    view               = %9s\n", _view == _view1 ? _view2 ? "view1 (view2)" : "view1" : "view2 (view1)");
        printf("    view.id            = %9d\n", _view->object_id());
        printf("    view1.cursor       = %9d\n", _view1->insert_position());
        printf("    view1.dragPos      = %9d\n", _view1->drag_pos());
        printf("    view1.dragType     = %9d\n", _view1->drag_type());
        printf("    view2.cursor       = %9d\n", _view2 ? _view2->insert_position() : -1);
        printf("    view2.dragPos      = %9d\n", _view2 ? _view2->drag_pos() : -1);
        printf("    view2.dragType     = %9d\n", _view2 ? _view2->drag_type() : -1);
        printf("\nStyle:\n");
        printf("    style              = '%s'\n", _style->name().c_str());
        printf("    style_tab          = %9d\n", (int) _style->tab_mode());
        printf("    style_tab_width    = %9u\n", _style->tab_width());
        cursor(true).debug(__LINE__, __FILE__);
        _statusbar_info.debug();
        _file_info.debug();
        _editor_flags.debug();
        _bookmarks.debug();
        _buf1->debug();
        if (_buf1->undo_mode() == FUNDO::FLE) {
            _buf1->undo()->debug(false);
        }
    }
    printf("\n");
    fflush(stdout);
#else
    (void) what;
#endif
}
std::string Editor::debug_save_style(std::string filename, bool add_pos) const {
#ifdef DEBUG
    int line = 1;
    std::string h;
    h.reserve(_buf1->length());
    if (add_pos == true) {
        h += gnu::str::format("%6d: %8d: ", line, 0);
    }
    for (auto f = 0; f < _buf2->length(); f++) {
        auto c = _buf1->byte_at(f);
        auto s = _buf2->byte_at(f);
        if (c == '\n') {
            line++;
            if (add_pos == true) {
                h += gnu::str::format("\n%6d: %8d: ", line, f);
            }
            else {
                h += gnu::str::format("\n");
            }
        }
        else {
            h += s;
        }
    }
    if (filename != "") {
        auto file = gnu::File::Open(filename, "wb");
        if (file == nullptr) {
            return h;
        }
        fwrite(h.c_str(), 1, h.length(), file);
        fclose(file);
    }
    return h;
#else
    (void) filename;
    (void) add_pos;
    return "";
#endif
}
void Editor::file_check_reload() {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    static int RECURSIVE = 0;
    RECURSIVE++;
    if (RECURSIVE == 2) {
        RECURSIVE--;
        return;
    }
    auto file1 = _file_info.fi;
    if (_config.pref_autoreload == false || file1.mtime == -1) {
        RECURSIVE--;
        return;
    }
    file1.update();
    if (file1.is_missing() == true && file1.filename != "") {
        statusbar_set_message(errors::FILE_DELETED);
        text_set_dirty(true);
    }
    else if (file1.is_file() == true && (_file_info.fi.size != file1.size || _file_info.fi.mtime != file1.mtime)) {
        auto reload = true;
        fl_message_position(this);
        if (text_is_dirty() == true) {
            if (_file_info.reload_time == file1.mtime) {
                reload = false;
            }
            else if (fl_choice(info::ASK_RELOAD, nullptr, "&No", "&Yes", file1.c_str()) == 1) {
                reload = false;
                _file_info.reload_time = file1.mtime;
            }
        }
        if (reload == true) {
            auto style_name = _style->name();
            auto pos        = cursor(true);
            auto err        = file_load(file1.filename);
            if (err != "") {
                fl_alert("%s", err.c_str());
            }
            else {
                style_from_language(style_name);
                cursor_move(pos);
                statusbar_set_message(info::FILE_RELOAD);
            }
        }
    }
    RECURSIVE--;
}
void Editor::file_compare_buffer() {
    auto mem = gnu::File::Read(_file_info.fi.filename);
    fl_message_position(this);
    if (mem.p != nullptr) {
        auto t = text_get_buffer_with_trim_and_line();
        if (mem != t) {
            fl_alert(errors::TEXT_DIFF_FROM_FILE, _file_info.fi.c_str());
        }
        else {
            fl_message("%s", info::TEXT_SAME_AS_FILE);
        }
    }
    else {
        fl_alert(errors::LOADING_FILE, _file_info.fi.c_str());
    }
}
std::string Editor::file_load(std::string filename) {
    auto wc    = flw::WaitCursor();
    auto fbuf  = gnu::FileBuf();
    auto fi    = gnu::File(filename);
    auto line  = FLINEENDING::UNIX;
    auto dirty = false;
    size_t count[257];
    text_set();
    statusbar_set_message("");
    if (gnu::str::is_whitespace(filename) == true) {
        text_set_dirty(false);
        return statusbar_set_message(errors::NO_FILENAME);
    }
    if (fi.is_dir() == true) {
        return statusbar_set_message(gnu::str::format(errors::FILE_IS_DIR, fi.c_str()));
    }
    else if (fi.size > (int64_t) limits::FILE_SIZE_VAL) {
        return statusbar_set_message(gnu::str::format(errors::FILE_TOO_LARGE, fi.c_str()));
    }
    else if (fi.size == -1) {
        auto backup = gnu::File(_config.backup_name(fi.filename));
        if (backup.size > 0) {
            fbuf = gnu::File::Read(backup.filename);
            if (fbuf.p == nullptr) {
                fbuf = gnu::FileBuf("", 0);
            }
            else {
                statusbar_set_message(info::BACKUP_LOADED);
            }
        }
        else {
            fbuf = gnu::FileBuf("", 0);
        }
        dirty = true;
    }
    else {
        fbuf = gnu::File::Read(fi.filename);
        if (fbuf.p == nullptr) {
            return statusbar_set_message(gnu::str::format(errors::LOADING_FILE, fi.c_str()));
        }
    }
    fbuf.count(count);
    if (count[0] == 0) {
        if (count[256] > limits::WRAP_LINE_LENGTH_VAL) {
            wrap_set_mode(FWRAP::YES);
            statusbar_set_message(info::FILE_WRAPPED);
        }
        auto before_cr = gnu::FileBuf::Fletcher64(fbuf.p, fbuf.s);
        if (count[13] > 0) {
            fbuf = fbuf.remove_cr();
            line = FLINEENDING::WINDOWS;
        }
        text_set(fbuf.p, line, fbuf.fletcher64());
        _file_info.fi         = fi;
        _file_info.fletcher64 = before_cr;
    }
    else {
        if (_config.pref_binary == FBINFILE::NO) {
            return statusbar_set_message(gnu::str::format(errors::LOADING_BIN, fi.name.c_str()));
        }
        else if (_config.pref_binary == FBINFILE::HEX && fbuf.s > limits::FILE_SIZE_VAL / limits::HEXFILE_DIVIDER) {
            return statusbar_set_message(gnu::str::format(errors::HEX_TOO_LARGE, fi.c_str()));
        }
        else if (_config.pref_binary == FBINFILE::HEX) {
            fbuf = string::binary_to_hex(fbuf.p, fbuf.s);
            statusbar_set_message(info::HEX_LOADED);
        }
        else {
            fbuf = string::binary_to_text(fbuf.p, fbuf.s);
            statusbar_set_message(info::BIN_LOADED);
        }
        text_set(fbuf.p);
        _file_info.binary = true;
        dirty = true;
    }
    text_set_dirty(dirty, true);
    update_autocomplete(fbuf.p);
    _config.send_message(message::FILE_LOADED, "", this);
    return "";
}
std::string Editor::file_save() {
    FLE_EDITOR_RETURN_IF_READONLY_1("")
    auto wc         = flw::WaitCursor();
    auto bfile1     = gnu::File(_config.backup_name(_file_info.fi.filename));
    auto text1      = text_get_buffer_with_trim_and_line();
    auto fi         = gnu::File(_file_info.fi.filename);
    auto fletcher64 = text1.fletcher64();
    if (text1.s <= limits::FILE_BACKUP_SIZE_VAL && bfile1.filename != "") {
        auto bfile2 = gnu::File(bfile1.filename + "." + gnu::Time::FormatUnixToISO(_file_info.fi.mtime, false, true));
        if (fi.size > 0 && bfile2.is_missing() == true) {
            gnu::File::Copy(_file_info.fi.filename, bfile2.filename);
        }
        if (gnu::File::Write(bfile1.filename, text1) == true) {
            gnu::File::ChMod(bfile1.filename, fi.mode);
        }
    }
    auto saved = gnu::File::Write(_file_info.fi.filename, text1);
    update_autocomplete(text1.p);
    if (saved == false) {
        return statusbar_set_message(gnu::str::format(errors::SAVE_FILE, _file_info.fi.c_str()));
    }
    _file_info.fletcher64 = fletcher64;
    _buf1->update_saved_fletcher64();
    if (_buf1->undo_mode() == FUNDO::FLE) {
        _buf1->undo()->set_save_point();
    }
    _file_info.fi.update();
    gnu::File::ChMod(_file_info.fi.filename, fi.mode);
    text_set_dirty(false);
    update_pref();
    cursor_save();
    return "";
}
std::string Editor::file_save_as(std::string filename) {
    FLE_EDITOR_RETURN_IF_READONLY_1("")
    _file_info.fi = filename;
    return file_save();
}
size_t Editor::find_lines(std::string find, FREGEX fregex, FTRIM ftrim) {
    auto out   = (flw::ScrollBrowser*) _output;
    auto time  = gnu::Time::Milli();
    auto lines = std::vector<std::string>();
    auto rx    = (fregex == FREGEX::YES) ? new gnu::PCRE(find, true) : (gnu::PCRE*) nullptr;
    _buf1->find_lines("", find, rx, ftrim, lines);
    out->clear();
    if (lines.size() > 0) {
        for (const auto& line : lines) {
            out->add(line.c_str());
        }
        _regex->compile("\\s*(?<line>\\d+)\\s+-\\s+(?<col>\\d+)|.*", true);
        show_output(FOUTPUT::SHOW);
        statusbar_set_message(gnu::str::format(info::FOUND_LINES, (unsigned) lines.size(), gnu::Time::Milli() - time));
    }
    else {
        _regex->clear();
        statusbar_set_message(gnu::str::format(info::NO_STRING_FOUND, find.c_str()));
    }
    take_focus();
    delete rx;
    return lines.size();
}
size_t Editor::find_lines(std::string find, FREGEX fregex, FTRIM ftrim, std::vector<std::string>& out) {
    auto size = out.size();
    auto rx   = (fregex == FREGEX::YES) ? new gnu::PCRE(find, true) : (gnu::PCRE*) nullptr;
    _buf1->find_lines(_file_info.fi.name, find, rx, ftrim, out);
    delete rx;
    return out.size() - size;
}
void Editor::find_quick() {
    int start = 0;
    int end = 0;
    std::string selected;
    if (_buf1->get_selection(start, end, false) == true) {
        CursorPos old = cursor(false);
        selected = gnu::str::grab_string(_buf1->text_range(start, end));
        _findbar->findreplace().find_string(selected);
        _findbar->findreplace().add_find_word(selected);
        CursorPos pos = _buf1->find_replace(selected, nullptr, FSEARCHDIR::FORWARD, FCASECOMPARE::YES, FWORDCOMPARE::NO, FNLTAB::NO);
        if (old != pos && pos.has_cursor() == true) {
            auto line = 0;
            auto col  = 0;
            cursor_move(pos);
            if (cursor_pos_to_line_and_col(cursor_insert_position(), line, col) > 0) {
                statusbar_set_message(gnu::str::format(info::FOUND_STRING_LINECOL, line, col - selected.length() + 1));
            }
            else {
                statusbar_set_message(gnu::str::format(info::FOUND_STRING_POS, pos.pos1));
            }
        }
        else {
            statusbar_set_message(gnu::str::format(info::NO_STRING_FOUND, selected.c_str()));
        }
    }
    else {
        text_select_word();
    }
}
bool Editor::find_replace(FSEARCHDIR fsearchdir, bool replace_text) {
    FLE_EDITOR_RETURN_IF_READONLY_1(false)
    auto& fr   = _findbar->findreplace();
    auto  find = fr.find_string();
    if (find == "") {
        if (fr.visible() == 0) {
            _config.send_message(message::SHOW_FIND, "", &fr);
        }
        return false;
    }
    auto pos = CursorPos();
    if (fr.fregex() == FREGEX::YES) {
        auto rx = gnu::PCRE(find, true);
        if (rx.is_compiled() == false) {
            fl_beep(FL_BEEP_ERROR);
            statusbar_set_message(rx.error());
            return false;
        }
        pos = _buf1->find_replace_regex(
            find,
            (replace_text == true) ? fr.replace_string().c_str() : nullptr,
            fr.fnltab()
        );
    }
    else {
        pos = _buf1->find_replace(
            find,
            (replace_text == true) ? fr.replace_string().c_str() : nullptr,
            fsearchdir,
            fr.fcasecompare(),
            fr.fwordcompare(),
            fr.fnltab()
        );
    }
    if (pos.has_cursor() == false) {
        statusbar_set_message(gnu::str::format(info::NO_STRING_FOUND, find.c_str()));
        if (fr.visible() != 0) {
            fr.take_focus();
        }
        else {
            take_focus();
        }
        return false;
    }
    auto line = 0;
    auto col  = 0;
    cursor_move(pos);
    _view->display_insert();
    if (cursor_pos_to_line_and_col(cursor_insert_position(), line, col) > 0) {
        if (line > 1 && col == 0) {
            statusbar_set_message(gnu::str::format(info::FOUND_STRING_LINE, line - 1));
        }
        else {
            statusbar_set_message(gnu::str::format(info::FOUND_STRING_LINECOL, line, col - find.length() + 1));
        }
    }
    else {
        statusbar_set_message(gnu::str::format(info::FOUND_STRING_POS, pos.pos1));
    }
    _findbar->findreplace().add_find_word(find);
    _findbar->findreplace().add_replace_word((replace_text == true) ? fr.replace_string() : "");
    return true;
}
size_t Editor::find_replace_all(std::string find, std::string replace, FNLTAB fnltab, FSELECTION fselection, FCASECOMPARE fcase, FWORDCOMPARE fword, FREGEX fregex, FSAVEWORD fsave, FHIDEFIND fhide, bool disable_message) {
    FLE_EDITOR_RETURN_IF_READONLY_1(0)
    auto time     = gnu::Time::Milli();
    auto pos      = CursorPos();
    if (fregex == FREGEX::YES) {
        auto rx = gnu::PCRE(find, true);
        if (rx.is_compiled() == false) {
            fl_beep(FL_BEEP_ERROR);
            statusbar_set_message(rx.error());
            return false;
        }
        pos = _buf1->find_replace_regex_all(&rx, replace, fselection, fnltab);
    }
    else {
        pos = _buf1->find_replace_all(find, replace, fselection, fcase, fword, fnltab);
    }
    cursor_move(pos);
    if (_buf1->count_changes() == 0) {
        if (disable_message == false) {
            statusbar_set_message(gnu::str::format(info::NO_STRINGS_REPLACED, find.c_str(), replace.c_str()));
        }
        if (_findbar->findreplace().visible() != 0) {
            _findbar->findreplace().take_focus();
        }
    }
    else {
        if (fhide == FHIDEFIND::YES) {
            _config.send_message(message::HIDE_FIND, "", &_findbar->findreplace());
        }
        if (fsave == FSAVEWORD::YES) {
            _findbar->findreplace().add_find_word(find);
            _findbar->findreplace().add_replace_word(replace);
        }
        if (disable_message == false) {
            statusbar_set_message(gnu::str::format(info::REPLACED_STRINGS, (unsigned) _buf1->count_changes(), gnu::Time::Milli() - time));
        }
        take_focus();
    }
    return _buf1->count_changes();
}
int Editor::handle(int event) {
    if (event == FL_FOCUS) {
        update_after_focus();
    }
    return Fl_Group::handle(event);
}
void Editor::help() const {
    std::string help;
    help += help::general();
    help += help::flags(_config);
    flw::dlg::list("Help", help, Fl::first_window(), true, 60, 50);
}
bool Editor::home() {
    if (_editor_flags.fwrap == FWRAP::YES) {
        return false;
    }
    auto pos = _buf1->home(cursor_insert_position());
    if (pos == -1) {
        return false;
    }
    cursor_move_to_pos(pos, true);
    _buf1->unselect();
    return true;
}
size_t Editor::memory_usage(size_t& buffer, size_t& style, size_t& undo) const {
    buffer = _buf1->length();
    style  = _buf2->length();
    undo   = (_buf1->undo_mode() == FUNDO::FLE) ? _buf1->undo()->capacity() : 0;
    return buffer + style + undo;
}
Message::CTRL Editor::message(const std::string& message, const std::string& s, const void*) {
    if (message == message::PREF_CHANGED) {
        update_pref();
    }
    else if (message == message::UNDO_MODE_CHANGED) {
        _buf1->undo_set_mode_using_config();
    }
    else if (_config.active == this) {
        if (message == message::STATUSBAR_STYLE_CHANGED) {
            style_from_language(s);
            take_focus();
            return Message::CTRL::ABORT;
        }
        else if (message == message::STATUSBAR_LINE_UNIX) {
            _file_info.flineending = FLINEENDING::UNIX;
            _findbar->statusbar().update_menus(this);
            take_focus();
            return Message::CTRL::ABORT;
        }
        else if (message == message::STATUSBAR_LINE_WIN) {
            _file_info.flineending = FLINEENDING::WINDOWS;
            _findbar->statusbar().update_menus(this);
            take_focus();
            return Message::CTRL::ABORT;
        }
        else if (message == message::STATUSBAR_TAB_CHANGED) {
            _editor_flags.set_tab_from_string(s);
            _findbar->statusbar().update_menus(this);
            update_pref();
            take_focus();
            return Message::CTRL::ABORT;
        }
        else if (message == message::STATUSBAR_TO_SPACES) {
            text_to_space();
            return Message::CTRL::ABORT;
        }
        else if (message == message::STATUSBAR_TO_TABS) {
            text_to_tab();
            return Message::CTRL::ABORT;
        }
    }
    return Message::CTRL::CONTINUE;
}
void Editor::select_color() {
    auto pos = _buf1->select_color();
    cursor_move(pos);
    _view->display_insert();
}
void Editor::select_pair(bool move_cursor) {
    auto pos = _buf1->select_pair(move_cursor);
    cursor_move(pos);
    _view->display_insert();
}
void Editor::show_autocomplete() {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    auto X   = 0;
    auto Y   = 0;
    auto pos = cursor_insert_position();
    if (_words.size() == 0 || _view->position_to_xy(pos, &X, &Y) == 0) {
        take_focus();
        fl_beep(FL_BEEP_NOTIFICATION);
        return;
    }
    auto fs     = (_config.pref_tmp_fontsize > 0) ? _config.pref_tmp_fontsize : flw::PREF_FIXED_FONTSIZE;
    auto word   = _buf1->get_word_left(cursor_insert_position());
    auto dialog = AutoCompleteDialog(fs);
    if (dialog.populate(_words, word) == 0) {
        take_focus();
        fl_beep(FL_BEEP_NOTIFICATION);
        free(word);
        return;
    }
    auto W = fs * 18;
    auto H = fs * 14;
    X = top_window()->x() + X - fs;
    Y = top_window()->y() + Y + fs;
    if (X + W > Fl::w()) {
        X = Fl::w() - W;
    }
    if (Y + H > Fl::h()) {
        Y -= (H + fs - fl_descent());
    }
    auto selected = dialog.run(X, Y, W, H);
    if (selected != "" && selected != word) {
        auto start = selected.find(word);
        if (start != std::string::npos) {
           selected.erase(0, strlen(word));
        }
        auto i = _buf1->get_indent(pos);
        gnu::str::replace(selected, "|", std::string("\n") + i);
        _buf1->insert(pos, selected.c_str());
        cursor_move_to_pos(pos + selected.size(), true);
    }
    take_focus();
    free(word);
}
size_t Editor::show_find_lines_dialog_or_run_again(FFINDLINES ffindlines) {
    auto        list  = _config.find_list;
    size_t      count = 0;
    std::string find;
    if (ffindlines == FFINDLINES::RUNAGAIN && list.size() > 0) {
        find = list.front();
    }
    else {
        find = FindDialog("Find Lines", list).run();
    }
    if (find != "") {
        count = find_lines(find, FindDialog::REGEX, FindDialog::TRIM);
        if (count > 0) {
            _findbar->findreplace().add_find_word(find);
        }
    }
    return count;
}
void Editor::show_line_dialog() {
    auto line = LineNumDialog().run();
    if (line > 0) {
        cursor_move_to_rowcol(line, 1);
    }
}
void Editor::show_menu() {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    _menu->size(flw::PREF_FONTSIZE * 14, flw::PREF_FONTSIZE + 8);
    flw::menu::enable_item(_menu, menu::SETTINGS, true);
    flw::menu::enable_item(_menu, menu::BOOKMARKS_CLEAR, _bookmarks.size() > 0);
    flw::menu::enable_item(_menu, menu::BOOKMARKS_TOGGLE, true);
    flw::menu::enable_item(_menu, menu::BOOKMARKS_NEXT, _bookmarks.size() > 0);
    flw::menu::enable_item(_menu, menu::BOOKMARKS_PREV, _bookmarks.size() > 0);
    flw::menu::enable_item(_menu, menu::SPLIT_CLOSE, _view2 != nullptr);
    flw::menu::enable_item(_menu, menu::UPDATE_AUTOCOMPLETE, _config.pref_autocomplete);
    flw::menu::set_item(_menu, menu::WRAP, _editor_flags.fwrap == FWRAP::YES);
    auto r = (_buf1->undo_mode() == FUNDO::FLE) ? _buf1->undo()->has_redo() : _buf1->can_redo();
    auto u = (_buf1->undo_mode() == FUNDO::FLE) ? _buf1->undo()->has_undo() : _buf1->can_undo();
    flw::menu::enable_item(_menu, menu::REDO, r);
    flw::menu::enable_item(_menu, menu::REDO_ALL, r);
    flw::menu::enable_item(_menu, menu::UNDO, u);
    flw::menu::enable_item(_menu, menu::UNDO_ALL, u);
    if (Fl::clipboard_contains(Fl::clipboard_plain_text) != 0) {
        flw::menu::enable_item(_menu, menu::EDIT_PASTE, true);
    }
    else {
        flw::menu::enable_item(_menu, menu::EDIT_PASTE, false);
    }
    if (_buf1->selected() != 0) {
        flw::menu::enable_item(_menu, menu::EDIT_COPY, true);
        flw::menu::enable_item(_menu, menu::EDIT_CUT, true);
        flw::menu::enable_item(_menu, menu::SORT_LINES_ASCENDING, true);
        flw::menu::enable_item(_menu, menu::SORT_LINES_DESCENDING, true);
        flw::menu::enable_item(_menu, menu::CASE_LOWER, true);
        flw::menu::enable_item(_menu, menu::CASE_UPPER, true);
        flw::menu::enable_item(_menu, menu::COMMENT_BLOCK, true);
    }
    else {
        flw::menu::enable_item(_menu, menu::EDIT_COPY, false);
        flw::menu::enable_item(_menu, menu::EDIT_CUT, false);
        flw::menu::enable_item(_menu, menu::SORT_LINES_ASCENDING, false);
        flw::menu::enable_item(_menu, menu::SORT_LINES_DESCENDING, false);
        flw::menu::enable_item(_menu, menu::CASE_LOWER, false);
        flw::menu::enable_item(_menu, menu::CASE_UPPER, false);
        flw::menu::enable_item(_menu, menu::COMMENT_BLOCK, false);
    }
    _menu->popup();
}
void Editor::show_output(FOUTPUT foutput) {
    if (foutput == FOUTPUT::SHOW_HORIZONTAL || foutput == FOUTPUT::SHOW_VERTICAL) {
        auto dir = (foutput == FOUTPUT::SHOW_HORIZONTAL) ? flw::SplitGroup::DIRECTION::HORIZONTAL : flw::SplitGroup::DIRECTION::VERTICAL;
        if (_main->direction() != dir) {
            _main->direction(dir);
            _main->split_pos(-1);
            _output->hide();
        }
    }
    if (static_cast<Fl_Widget*>(_output)->visible() == 0) {
        _output->show();
        if (_main->split_pos() == -1) {
            if (_main->direction() == flw::SplitGroup::DIRECTION::VERTICAL) {
                _main->split_pos(w() - 200);
            }
            else {
                _main->split_pos(h() - 200);
            }
        }
        do_layout();
    }
    else if (foutput == FOUTPUT::TOGGLE) {
        _output->hide();
        do_layout();
    }
}
void Editor::show_print() {
    auto t = text_get_buffer();
    flw::dlg::print_text("Print Text To PostScript", t.p, Fl::first_window());
}
void Editor::style(Style* style) {
    delete _style;
    if ((size_t) text_length() > limits::STYLE_FILESIZE_VAL) {
        delete style;
        style = nullptr;
        statusbar_set_message(info::STYLE_OFF);
    }
    _style = (style == nullptr) ? new Style() : style;
    for (const auto& w : _style->words()) {
        _words.insert(w);
    }
    _editor_flags.tab_mode  = _style->tab_mode();
    _editor_flags.tab_width = _style->tab_width();
    style_resize_buffer();
    _style->set_buffers(_buf1, _buf2);
    _style->update();
    _view1->redisplay_range(0, _buf1->length());
    if (_view2 != nullptr) {
        _view2->redisplay_range(0, _buf1->length());
    }
    _findbar->statusbar().update_menus(this);
    update_pref();
    Fl::redraw();
}
void Editor::style_resize_buffer() {
    auto size   = (_style->name() == style::TEXT) ? 0 : _buf1->length();
    auto style1 = gnu::File::Allocate(nullptr, size + 1);
    memset(style1, style::STYLE_INIT, size);
    _buf2->text(style1);
    free(style1);
}
void Editor::text_comment_block() {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    auto wc  = flw::WaitCursor();
    auto pos = _buf1->comment_block(_style->block_start(), _style->block_end());
    cursor_move(pos);
}
void Editor::text_comment_line() {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    auto pos = _buf1->comment_line(_style->line_comment());
    cursor_move(pos);
}
void Editor::text_convert_case(FCASE transform) {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    auto pos = _buf1->case_for_selection(transform);
    cursor_move(pos);
}
int Editor::text_count_selection_len(bool* bytes) const {
    auto length1 = _statusbar_info.end - _statusbar_info.start;
    if (bytes != nullptr && (size_t) length1 <= limits::COUNT_CHAR_VAL) {
        auto t = _buf1->text_range(_statusbar_info.start, _statusbar_info.end);
        auto l = gnu::str::utf_len(t);
        free(t);
        if (l == 0) {
            *bytes = true;
        }
        else {
            length1 = l;
            *bytes = false;
        }
    }
    else if (bytes != nullptr) {
        *bytes = true;
    }
    return length1;
}
void Editor::text_duplicate_line_or_selection() {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    auto pos = _buf1->duplicate_text();
    cursor_move(pos);
}
void Editor::text_insert_tab_or_move_lines_left_right(FMOVEH fmoveh) {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    auto cur = cursor(true);
    if (_buf1->has_multiline_selection() == true) {
        cur = _buf1->insert_tab_multiline(cur, fmoveh, text_tab_mode(), text_tab_width());
    }
    else {
        cur = _buf1->insert_tab(cur, text_tab_mode(), text_tab_width());
    }
    cursor_move(cur);
}
void Editor::text_move_lines(FMOVEV move) {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    auto pos = _buf1->move_line(move);
    cursor_move(pos);
}
int Editor::text_remove_trailing() {
    FLE_EDITOR_RETURN_IF_READONLY_1(0)
    auto rx  = gnu::PCRE("(\\s+)$");
    auto pos = _buf1->find_replace_regex_all(&rx, "", FSELECTION::NO, FNLTAB::NO);
    cursor_move(pos);
    statusbar_set_message(gnu::str::format(info::REMOVED_TRAILING, (unsigned) _buf1->count_changes()));
    return _buf1->count_changes();
}
void Editor::text_select_line() {
    auto pos = _buf1->select_line(false);
    cursor_move(pos);
}
void Editor::text_select_word() {
    CursorPos   pos = _buf1->select_word();
    std::string selected;
    if (pos.text_has_selection() == true) {
        selected = gnu::str::grab_string(_buf1->text_range(pos.start, pos.end));
        _findbar->findreplace().find_string(selected);
        cursor_move(pos);
        statusbar_set_message("");
    }
}
void Editor::text_set(const char* TEXT, FLINEENDING flineending, uint64_t fletcher64) {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    assert(TEXT);
    style(nullptr);
    _buf1->set(TEXT, fletcher64);
    _file_info = FileInfo();
    _file_info.flineending = flineending;
    _statusbar_info = StatusBarInfo();
    _view1->insert_position(0);
    _view1->show_insert_position();
    if (_view2 != nullptr) {
        _view2->insert_position(0);
        _view2->show_insert_position();
    }
    text_set_dirty(false, true);
}
void Editor::text_set_readonly(bool value) {
    _editor_flags.ro = value;
    if (value == true) {
        _menu->deactivate();
    }
    else {
        _menu->activate();
    }
    update_pref();
}
void Editor::text_sort_lines(FSORT order) {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    if (_buf1->selected() == 0) {
        fl_beep(FL_BEEP_NOTIFICATION);
        return;
    }
    auto wc  = flw::WaitCursor();
    auto pos = _buf1->sort(order);
    cursor_move(pos);
}
void Editor::text_to_space() {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    auto count = find_replace_all("\t", strings::SOFT_TABS[text_tab_width()], FNLTAB::NO, FSELECTION::NO, FCASECOMPARE::NO, FWORDCOMPARE::NO, FREGEX::NO, FSAVEWORD::NO, FHIDEFIND::NO, true);
    if (count == 0) {
        statusbar_set_message(info::NO_TABS_REPLACED);
    }
    else {
        statusbar_set_message(gnu::str::format(info::TABS_REPLACED, (int) count));
    }
}
void Editor::text_to_tab() {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    auto count = find_replace_all(strings::SOFT_TABS[text_tab_width()], "\t", FNLTAB::NO, FSELECTION::NO, FCASECOMPARE::NO, FWORDCOMPARE::NO, FREGEX::NO, FSAVEWORD::NO, FHIDEFIND::NO, true);
    if (count == 0) {
        statusbar_set_message(info::NO_SPACES_REPLACED);
    }
    else {
        statusbar_set_message(gnu::str::format(info::SPACES_REPLACED, (int) count));
    }
}
void Editor::undo_back(bool all) {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    if (_buf1->undo_mode() == FUNDO::FLE) {
        auto cur = _buf1->undo_back(all, cursor(false));
        if (_buf1->count_changes() == 0) {
            statusbar_set_message("");
            return;
        }
        cursor_move(cur);
        statusbar_set_message(gnu::str::format(info::UNDID_CHANGES, _buf1->count_changes()));
        _buf1->undo_check_save_point();
    }
    else {
        if (all == true) {
            flw::WaitCursor wc;
            while (_buf1->can_undo() == true) {
                Fl_Text_Editor::kf_undo(0, _view);
            }
        }
        else {
            Fl_Text_Editor::kf_redo(0, _view);
        }
    }
}
void Editor::undo_forward(bool all) {
    FLE_EDITOR_RETURN_IF_READONLY_0()
    if (_buf1->undo_mode() == FUNDO::FLE) {
        auto cur = _buf1->undo_forward(all, cursor(false));
        if (_buf1->count_changes() == 0) {
            statusbar_set_message("");
            return;
        }
        cursor_move(cur);
        statusbar_set_message(gnu::str::format(info::REDID_CHANGES, _buf1->count_changes()));
        _buf1->undo_check_save_point();
    }
    else {
        if (all == true) {
            flw::WaitCursor wc;
            while (_buf1->can_redo() == true) {
                Fl_Text_Editor::kf_redo(0, _view);
            }
        }
        else {
            Fl_Text_Editor::kf_redo(0, _view);
        }
    }
}
void Editor::update_after_focus() {
    file_check_reload();
    if (_config.active != this) {
        _config.active = this;
        _findbar->statusbar().update_menus(this);
        _findbar->findreplace().callback(Editor::CallbackFind, this);
        _config.send_message(message::EDITOR_FOCUS, "", this);
        update_statusbar();
    }
}
void Editor::update_autocomplete(const char* text) {
    _words.clear();
    if (_config.pref_autocomplete == false || _buf1->length() > (int) limits::AUTOCOMPLETE_FILESIZE_VAL || _file_info.binary == true) {
        return;
    }
    char* tmp = nullptr;
    if (text == nullptr) {
        text = _buf1->text();
        tmp  = const_cast<char*>(text);
    }
    _words = _style->words();
    string::wordlist(text, _words);
    free(tmp);
}
void Editor::update_pref() {
    labelsize(flw::PREF_FONTSIZE);
    _menu->labelfont(flw::PREF_FONT);
    _menu->labelsize(flw::PREF_FONTSIZE);
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
    _output->textfont(flw::PREF_FIXED_FONT);
    _output->textsize(flw::PREF_FONTSIZE);
    if (_config.active == this) {
        _findbar->statusbar().update_menus(this);
    }
    auto wrap_for_view2 = _view1->update_pref(false);
    if (_view2 != nullptr) {
        _view2->update_pref(wrap_for_view2);
    }
    if (_config.pref_statusbar == true) {
        _findbar->statusbar().show();
    }
    else {
        _findbar->statusbar().hide();
    }
    if (_config.pref_autocomplete == false) {
        _words.clear();
    }
    else if (_words.size() == 0 && _config.pref_autocomplete == true) {
        update_autocomplete();
    }
    Fl_Group::resize(x(), y(), w(), h());
    Fl::redraw();
}
void Editor::update_statusbar() {
    if (_view == nullptr || _view->buffer() == nullptr) {
        return;
    }
    std::string label;
    update_textinfo();
    if (_statusbar_info.has_selection() == true) {
        auto bytes = false;
        auto len   = text_count_selection_len(&bytes);
        auto unit  = (bytes == true) ? "Bytes" : "Chars";
        auto r     = gnu::str::format_int(_statusbar_info.rows, '\'');
        auto l     = gnu::str::format_int(len, '\'');
        if (_statusbar_info.rows > 1) {
            label = gnu::str::format("Lines: %s %s: %s", r.c_str(), unit, l.c_str());
        }
        else {
            label = gnu::str::format("%s: %s", unit, l.c_str());
        }
    }
    else {
        auto r = gnu::str::format_int(_statusbar_info.row, '\'');
        auto c = gnu::str::format_int(_statusbar_info.col + 1, '\'');
        auto p = gnu::str::format_int(_statusbar_info.pos + 1, '\'');
        label = gnu::str::format("Ln: %s  Col: %s  Pos: %s", r.c_str(), c.c_str(), p.c_str());
    }
    _findbar->statusbar().label_cursor(label);
    _findbar->statusbar().label_cursor_mode((_view->insert_mode() == 0) ? "OVR" : "INS");
    if (_buf1->undo_mode() == FUNDO::FLE && _buf1->undo()->capacity() > (int64_t) limits::UNDO_WARNING) {
        statusbar_set_message(gnu::str::format(info::UNDO_MEMORY, (long long int) _buf1->undo()->capacity()));
    }
    if ((size_t) text_length() > limits::STYLE_FILESIZE_VAL && _style->name() != style::TEXT) {
        style(nullptr);
        statusbar_set_message(info::STYLE_OFF);
    }
}
void Editor::update_textinfo() {
    auto start = 0;
    auto end   = 0;
    auto row   = 0;
    auto col   = 0;
    _statusbar_info.pos = _view->insert_position();
    if (cursor_pos_to_line_and_col(_statusbar_info.pos, row, col) > 0) {
        _statusbar_info.row = row;
        _statusbar_info.col = col;
    }
    if (_buf1->selection_position(&start, &end) != 0) {
        if (_statusbar_info.start != start || _statusbar_info.end != end) {
            _statusbar_info.rows  = _buf1->count_lines(start, end);
            _statusbar_info.start = start;
            _statusbar_info.end   = end;
            if (_buf1->line_start(end) < end) {
                _statusbar_info.rows++;
            }
        }
    }
    else {
        _statusbar_info.start = 0;
        _statusbar_info.end   = 0;
        _statusbar_info.rows  = 0;
    }
}
void Editor::view_set_split(FSPLITVIEW fsplit) {
    if (fsplit == FSPLITVIEW::VERTICAL || fsplit == FSPLITVIEW::HORIZONTAL) {
        if (_view2 == nullptr) {
            _view2 = new View(_config, this);
            _editors->add(_view2, flw::SplitGroup::CHILD::SECOND);
            _view2->init(_view1);
        }
        _editors->direction((fsplit == FSPLITVIEW::HORIZONTAL) ? flw::SplitGroup::DIRECTION::HORIZONTAL : flw::SplitGroup::DIRECTION::VERTICAL);
        _editor_flags.fsplitview = fsplit;
    }
    else if (_view2 != nullptr) {
        _editors->add(nullptr, flw::SplitGroup::CHILD::SECOND);
        _view2 = nullptr;
        _view = _view1;
    }
    _editors->do_layout();
    Fl::redraw();
}
void Editor::wrap_toggle_mode() {
    wrap_set_mode(flw::menu::item_value(_menu, menu::WRAP) ? FWRAP::YES : FWRAP::NO);
}
}
static const char * icon_xpm[] = {
"48 48 134 2",
"  	c None",
". 	c #000000",
"+ 	c #010101",
"@ 	c #666666",
"# 	c #A3A3A3",
"$ 	c #5D5D5D",
"% 	c #4B4B4B",
"& 	c #FFFFFF",
"* 	c #FDFDFD",
"= 	c #222222",
"- 	c #646464",
"; 	c #343434",
"> 	c #FEFEFE",
", 	c #D9D9D9",
"' 	c #2C2C2C",
") 	c #141414",
"! 	c #A4A4A4",
"~ 	c #FCFCFC",
"{ 	c #1B1B1B",
"] 	c #262626",
"^ 	c #E9E9E9",
"/ 	c #535353",
"( 	c #0D0D0D",
"_ 	c #676767",
": 	c #D5D5D5",
"< 	c #686868",
"[ 	c #FBFBFB",
"} 	c #292929",
"| 	c #8B8B8B",
"1 	c #A9A9A9",
"2 	c #151515",
"3 	c #C6C6C6",
"4 	c #909090",
"5 	c #AEAEAE",
"6 	c #A6A6A6",
"7 	c #3A3A3A",
"8 	c #D8D8D8",
"9 	c #555555",
"0 	c #050505",
"a 	c #DCDCDC",
"b 	c #4C4C4C",
"c 	c #030303",
"d 	c #EEEEEE",
"e 	c #ADADAD",
"f 	c #0A0A0A",
"g 	c #8D8D8D",
"h 	c #A8A8A8",
"i 	c #161616",
"j 	c #131313",
"k 	c #AFAFAF",
"l 	c #F8F8F8",
"m 	c #0E0E0E",
"n 	c #373737",
"o 	c #747474",
"p 	c #020202",
"q 	c #3B3B3B",
"r 	c #E5E5E5",
"s 	c #4E4E4E",
"t 	c #505050",
"u 	c #E7E7E7",
"v 	c #C2C2C2",
"w 	c #7B7B7B",
"x 	c #BFBFBF",
"y 	c #232323",
"z 	c #090909",
"A 	c #878787",
"B 	c #121212",
"C 	c #7D7D7D",
"D 	c #F3F3F3",
"E 	c #6D6D6D",
"F 	c #353535",
"G 	c #D7D7D7",
"H 	c #D6D6D6",
"I 	c #E6E6E6",
"J 	c #393939",
"K 	c #0B0B0B",
"L 	c #F7F7F7",
"M 	c #242424",
"N 	c #BBBBBB",
"O 	c #F2F2F2",
"P 	c #EFEFEF",
"Q 	c #060606",
"R 	c #494949",
"S 	c #E8E8E8",
"T 	c #DBDBDB",
"U 	c #F9F9F9",
"V 	c #282828",
"W 	c #B1B1B1",
"X 	c #8E8E8E",
"Y 	c #3F3F3F",
"Z 	c #656565",
"` 	c #6C6C6C",
" .	c #D2D2D2",
"..	c #606060",
"+.	c #F6F6F6",
"@.	c #9E9E9E",
"#.	c #707070",
"$.	c #181818",
"%.	c #757575",
"&.	c #080808",
"*.	c #999999",
"=.	c #CDCDCD",
"-.	c #3D3D3D",
";.	c #E2E2E2",
">.	c #5B5B5B",
",.	c #414141",
"'.	c #212121",
").	c #CBCBCB",
"!.	c #1D1D1D",
"~.	c #F0F0F0",
"{.	c #9F9F9F",
"].	c #ECECEC",
"^.	c #838383",
"/.	c #040404",
"(.	c #BCBCBC",
"_.	c #595959",
":.	c #E3E3E3",
"<.	c #989898",
"[.	c #7F7F7F",
"}.	c #BABABA",
"|.	c #F1F1F1",
"1.	c #D0D0D0",
"2.	c #B6B6B6",
"3.	c #EDEDED",
"4.	c #CFCFCF",
"5.	c #9A9A9A",
"6.	c #A7A7A7",
"7.	c #B2B2B2",
"8.	c #2F2F2F",
"9.	c #F4F4F4",
"0.	c #101010",
"a.	c #313131",
"b.	c #737373",
"c.	c #2B2B2B",
"                                                                                                ",
"                                                                                                ",
"                                                                                                ",
"                                                                                                ",
"    . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .       ",
"  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .   ",
"  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .   ",
". . . + @ # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # $ . . . . ",
". . . % & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & * = . . . ",
". . . - & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & & & & & & & & & & & > * & & & & & & & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & & & & & & & & & & , ' ) ! & & & & & & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & & & & & & & & & & $ . . ) * & & & & & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & & ~ & & & & & & > { . . ] & & & & & & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & ^ / ( _ > & & & & : . . . < & [ _ } | ~ & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & 1 2 . . . 3 & & & & 4 . . . 5 & 6 . . . 7 8 & & & & & & & & & ; . . . ",
". . . - & & & & & & & ^ 9 . . . . 0 a & & & & b . . c d & e . . . . f g ~ & & & & & & & ; . . . ",
". . . - & & & & & & h i . . . . j k & & & & l m . . n & & * o p . . . . q 8 & & & & & & ; . . . ",
". . . - & & & & r s . . . . . t u & & & & & v . . . w & & & & x y . . . . z A [ & & & & ; . . . ",
". . . - & & & h B . . . . j ! & & & & & & & C . . . x & & & & & D E p . . . . F G & & & ; . . . ",
". . . - & & H c . . . . t I & & & & & & & & J . . K L & & & & & & & x M . . . . { L & & ; . . . ",
". . . - & & N . . . . ' O & & & & & & & & P Q . . R & & & & & & & & & S 2 . . . . T & & ; . . . ",
". . . - & & U 7 . . . . V : & & & & & & & W . . . X & & & & & & & & 8 ' . . . . Y * & & ; . . . ",
". . . - & & & L Z . . . . ( h & & & & & & ` . . .  .& & & & & & & e m . . . . ..+.& & & ; . . . ",
". . . - & & & & & @.f . . . . #.U & & & & ] . . $.* & & & & & [ %.+ . . . &.*.& & & & & ; . . . ",
". . . - & & & & & & =.y . . . . -.r & & ;.+ . . >.& & & & & u ,.. . . . '.).& & & & & & ; . . . ",
". . . - & & & & & & & d % . . . . !.~.& {.. . . {.& & & & r !.. . . . R ].& & & & & & & ; . . . ",
". . . - & & & & & & & & * ^./.. . . (.& _.. . + :.& & & & <.. . . c [.* & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & }.2 . !.|.* $.. . V & & & & & 1.Q . B 2.& & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & 3.2.P & 1.. . . E & & & & & & 4.5.r & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & & & & & 6.. . . 7.& & & & & & & & & & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & & & & & 3.= . 8.L & & & & & & & & & & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & & & & & & 9.=.l & & & & & & & & & & & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & ; . . . ",
". . . - & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & ; . . . ",
". . . } L & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & & d 0.. . . ",
". . . . a.b.b.b.b.b.b.b.b.o b.b.o b.b.o b.b.o b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.b.c.. . . . ",
"  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .   ",
"    . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .     ",
"      . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .       ",
"                                                                                                ",
"                                                                                                ",
"                                                                                                ",
"                                                                                                "};
constexpr static const char* MENU_DEBUG                         = "&Debug/Debug";
constexpr static const char* MENU_DEBUG_COMPARE                 = "&Debug/Compare buffer with file";
constexpr static const char* MENU_DEBUG_SIZE                    = "&Debug/Debug size";
constexpr static const char* MENU_FILE_CLOSE                    = "&File/Close file";
constexpr static const char* MENU_FILE_CLOSEALL                 = "&File/Close all files";
constexpr static const char* MENU_FILE_NEW                      = "&File/New file";
constexpr static const char* MENU_FILE_NEWWINDOW                = "&File/New window";
constexpr static const char* MENU_FILE_OPEN                     = "&File/Open file...";
constexpr static const char* MENU_FILE_QUIT                     = "&File/Quit";
constexpr static const char* MENU_FILE_READONLY                 = "&File/Read only mode";
constexpr static const char* MENU_FILE_RELOAD                   = "&File/Reload file...";
constexpr static const char* MENU_FILE_SAVE                     = "&File/Save";
constexpr static const char* MENU_FILE_SAVEALL                  = "&File/Save all";
constexpr static const char* MENU_FILE_SAVEAS                   = "&File/Save as...";
constexpr static const char* MENU_FILE_TERMINATE                = "&File/Terminate";
constexpr static const char* MENU_FIND                          = "F&ind/";
constexpr static const char* MENU_FIND_LINES                    = "F&ind/Find lines in all files...";
constexpr static const char* MENU_FIND_REPLACE                  = "F&ind/Replace in all files...";
constexpr static const char* MENU_FIND_SHOW                     = "F&ind/Show find";
constexpr static const char* MENU_FIND_TRAILING                 = "F&ind/Remove trailing whitespace in all files";
constexpr static const char* MENU_HELP_ABOUT                    = "&Help/About...";
constexpr static const char* MENU_HELP_EDITOR                   = "&Help/Editor...";
constexpr static const char* MENU_HELP_FLEDIT                   = "&Help/Help...";
constexpr static const char* MENU_HELP_PCRE                     = "&Help/PCRE...";
constexpr static const char* MENU_PROJECT_CLOSE                 = "&Project/Close project";
constexpr static const char* MENU_PROJECT_CLOSE2                = "&Project/Close project without saving";
constexpr static const char* MENU_PROJECT_DB_CLOSE              = "&Project/Close database";
constexpr static const char* MENU_PROJECT_DB_DEFRAG             = "&Project/Defrag database";
constexpr static const char* MENU_PROJECT_DB_OPEN               = "&Project/Open database...";
constexpr static const char* MENU_PROJECT_DIR                   = "&Project/Set project directory...";
constexpr static const char* MENU_PROJECT_LOAD                  = "&Project/Load project...";
constexpr static const char* MENU_PROJECT_SAVE                  = "&Project/Save project";
constexpr static const char* MENU_PROJECT_SAVEAS                = "&Project/Save project as...";
constexpr static const char* MENU_SETTINGS_BACKUP               = "&Settings/Set backup directory...";
constexpr static const char* MENU_SETTINGS_EDITOR               = "&Settings/Editor...";
constexpr static const char* MENU_SETTINGS_LOADPREF             = "&Settings/Reload preferences";
constexpr static const char* MENU_SETTINGS_OUTPUT_CLEAR         = "&Settings/Output group/Clear terminal every time";
constexpr static const char* MENU_SETTINGS_OUTPUT_HORIZONTAL    = "&Settings/Output group/Horizontal";
constexpr static const char* MENU_SETTINGS_OUTPUT_SWAP          = "&Settings/Output group/Swap side";
constexpr static const char* MENU_SETTINGS_OUTPUT_UNKNOWN       = "&Settings/Output group/Show unknown characters in terminal";
constexpr static const char* MENU_SETTINGS_OUTPUT_VERTICAL      = "&Settings/Output group/Vertical";
constexpr static const char* MENU_SETTINGS_SAVEPREF             = "&Settings/Save preferences";
constexpr static const char* MENU_SETTINGS_SCHEME               = "&Settings/Color scheme...";
constexpr static const char* MENU_SETTINGS_SPLITHORIZONTAL      = "&Settings/Split group/Horizontal";
constexpr static const char* MENU_SETTINGS_SPLITVERTICAL        = "&Settings/Split group/Vertical";
constexpr static const char* MENU_SETTINGS_TABG11               = "&Settings/Tabs group 1/Top";
constexpr static const char* MENU_SETTINGS_TABG12               = "&Settings/Tabs group 1/Bottom";
constexpr static const char* MENU_SETTINGS_TABG13               = "&Settings/Tabs group 1/Left";
constexpr static const char* MENU_SETTINGS_TABG14               = "&Settings/Tabs group 1/Right";
constexpr static const char* MENU_SETTINGS_TABG21               = "&Settings/Tabs group 2/Top";
constexpr static const char* MENU_SETTINGS_TABG22               = "&Settings/Tabs group 2/Bottom";
constexpr static const char* MENU_SETTINGS_TABG23               = "&Settings/Tabs group 2/Left";
constexpr static const char* MENU_SETTINGS_TABG24               = "&Settings/Tabs group 2/Right";
constexpr static const char* MENU_SETTINGS_THEME                = "&Settings/Theme...";
constexpr static const char* MENU_TOOLS_CLEARLIST               = "&Tools/Clear list";
constexpr static const char* MENU_TOOLS_CLEAROUTPUT             = "&Tools/Clear list && terminal";
constexpr static const char* MENU_TOOLS_CLEARTERMINAL           = "&Tools/Clear terminal";
constexpr static const char* MENU_TOOLS_CMD                     = "&Tools/Run command...";
constexpr static const char* MENU_TOOLS_CMDREPEAT               = "&Tools/Run last command";
constexpr static const char* MENU_TOOLS_NEXTOUTPUT              = "&Tools/Next row in list output";
constexpr static const char* MENU_TOOLS_PREVOUTPUT              = "&Tools/Previous row in list output";
constexpr static const char* MENU_TOOLS_SAVE_CLIPBOARD          = "&Tools/Save clipboard as snippet...";
constexpr static const char* MENU_TOOLS_SAVE_SELECTION          = "&Tools/Save selection as snippet...";
constexpr static const char* MENU_TOOLS_SAVE_TEXT               = "&Tools/Save current text as snippet";
constexpr static const char* MENU_TOOLS_SNIPPETS                = "&Tools/Snippets...";
constexpr static const char* MENU_VIEW_ACTIVATEONE              = "&View/Activate group 1";
constexpr static const char* MENU_VIEW_ACTIVATETWO              = "&View/Activate group 2";
constexpr static const char* MENU_VIEW_MOVEGROUP                = "&View/Move file to opposite group";
constexpr static const char* MENU_VIEW_SORT_LEFT_TABS_ASC       = "&View/Sort left tabs ascending";
constexpr static const char* MENU_VIEW_SORT_LEFT_TABS_DESC      = "&View/Sort left tabs descending";
constexpr static const char* MENU_VIEW_SORT_RIGHT_TABS_ASC      = "&View/Sort right tabs ascending";
constexpr static const char* MENU_VIEW_SORT_RIGHT_TABS_DESC     = "&View/Sort right tabs descending";
constexpr static const char* MENU_VIEW_TOGGLEBROWSER            = "&View/Toggle directory browser";
constexpr static const char* MENU_VIEW_TOGGLEFULL               = "&View/Toggle full screen";
constexpr static const char* MENU_VIEW_TOGGLEMENU               = "&View/Toggle menu";
constexpr static const char* MENU_VIEW_TOGGLEONE                = "&View/Toggle group 1";
constexpr static const char* MENU_VIEW_TOGGLEOUTPUT             = "&View/Toggle output panel";
constexpr static const char* MENU_VIEW_TOGGLETABS               = "&View/Toggle tabs";
constexpr static const char* MENU_VIEW_TOGGLETWO                = "&View/Toggle group 2";
#ifdef DEBUG
constexpr static const char*    USER_NAME                       = "gnuwimp_test";
#else
constexpr static const char*    USER_NAME                       = "gnuwimp";
#endif
static const bool           ASK_SAVE                            = true;
static const bool           DONT_ASK_SAVE                       = false;
static const bool           CLOSE_EDITOR                        = true;
static const bool           DONT_CLOSE_EDITOR                   = false;
static const std::string    DBKEY_PROJECTS                      = "projects_";
static const std::string    DBKEY_SNIPPETS                      = "snippets_";
static std::string FLEDIT_ABOUT = R"(flEdit r1

Copyright 2024 gnuwimp@gmail.com.
Released under the GNU General Public License 3.0
https://github.com/gnuwimp/flEdit.

flEdit is a basic text editor.
Use flEdit with caution and at your own risk.

Third-Party open source code that have been used:
FLTK:   https://www.fltk.org/
SQLite: http://www.sqlite.org
PCRE:   https://www.pcre.org/

)";
static std::string FLEDIT_HELP = R"(Manage editor tabs with keyboard:
alt + '0' - '9'        select edit view 1 to 10
alt + 'left'           switch to previous editor
alt + 'right'          switch to next editor
alt + shift + 'left'   move current editor to the left/up
alt + shift + 'right'  move current editor to the right/down

Projects:
Files can be saved to a project and be restored later.
All projects are stored in an sqlite3 database.
So open/create the database first.
To use the built in file browser set a directory first for the project.

Autocomplete:
If it is turned on it will create the word list when file is loaded.
And then every time it is saved.

Backup files:
If a backup path has been set then the file is also saved there.
If a file has the filename /home/user/world.txt The backup file name will be
/backupdir/_home_user_world.txt (or something similar for windows).
Also a copy of the file will be saved to /backupdir/_home_user_world.txt.YYYY-MM-DD.

Command:
Run external commands directly from flEdit by using the command menu.
You can either run commands in their own processes or capture the output.

)";
static std::string COMMAND_HELP = R"(Run a command as an external process.
Or run and capture output from an command.
Capturing output is done by appending 2>&1 to your command.
It is also executed in an thread in the background, only one command can be run at the same time.
Display the result in a listbox or a (read only) terminal widget.

Warning:
If you are running an command and it doesn't stop you have to kill it manually!
Either by using some kind of task manager or kill command from the terminal.

Name:
Enter a unique name to describe the command.

Command:
Enter command to execute.
Use $FILE to replace it with current active file.
Use $PROJECT to replace it with project directory (if it has been set).
Use $SELECTION to replace it with selected text in current editor (if it has been set).
Use "" around paths that contains spaces.

Work directory:
Optional value.
Enter work directory or use empty to use current.
Use $FILE or $PROJECT for working directory.
Relative path can be added ($FILE/..).
Use "" around paths that contains spaces.

Run:
Run command in background as an seperate process.
It does no capturing.

Capture:
Run command and capture output to a listbox.
Use optional regular expressions to filter and capture file/line/columns.

Terminal:
Capture all output and send it to terminal widget.

Stream To Terminal:
Read output in chunks and append it to terminal widget.

Filter parser:
Optional value.
Enter a word or regular expression to filter what lines should be captured.
Use "error:" to capture only error lines from gcc.

Line parser:
Optional value.
Enter regular expression to capture filename/line/column data.
They should be named ('file' and optional 'line' and/or 'col').

Use this expression for parsing the output from a compiler like gcc:
(?<file>.+):(?<line>\d+):(?<col>\d+).*
If colors are shown or there are other format problems use -fdiagnostics-plain-output flag.

Parse output from a lua interpreter:
(lua:\s*)(?<file>.*):(?<line>\d+):.*
(.*)\s+(?<file>.*):(?<line>\d+):.*

)";
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/Fl_Terminal.H>
#include <FL/fl_ask.H>
#include <thread>
enum class SPLIT {
    SHOWONE = 1,
    SHOWTWO = 2,
    HIDEONE = 4,
    HIDETWO = 8,
    HOR     = 16,
    VER     = 32,
};
struct Command;
typedef std::vector<Command*> CommandVector;
extern fle::Config CONFIG;
struct Command {
    enum OUTPUT {
                                RUN,
                                CAPTURE_LIST,
                                CAPTURE_TERMINAL,
                                STREAM_TERMINAL,
                                INVALID,
    };
    static Command*             CURRENT;
    static CommandVector        COMMANDS;
    static gnu::FileBuf         BUF;
    static int                  SELECT_LINE;
    static std::string          LINE_REGEX;
    static std::string          WORKDIR;
    static std::thread*         THREAD;
    static std::vector<std::string> LINES;
    std::string                 name;
    std::string                 command;
    OUTPUT                      output;
    std::string                 workdir;
    std::string                 filter_regex;
    std::string                 line_regex;
                                Command()
                                    { output = RUN ; }
                                Command(std::string name, std::string command, OUTPUT output, std::string workdir, std::string filter_regex, std::string line_regex) {
                                    this->name         = name;
                                    this->command      = command;
                                    this->output       = output;
                                    this->workdir      = workdir;
                                    this->filter_regex = filter_regex;
                                    this->line_regex   = line_regex;
                                }
    static Command*             Current(int command);
    static inline bool          Sort(const Command* a, const Command* b) { return a->name < b->name; }
};
class CommandDialog : public Fl_Double_Window {
public:
                                CommandDialog(CommandVector& commands, Command* select);
    void                        resize(int X, int Y, int W, int H) override;
    Command*                    run(Fl_Window* parent);
    static void                 Callback(Fl_Widget* w, void* o);
private:
    bool                        data_delete();
    void                        data_load();
    void                        data_new(Command* copy = nullptr);
    void                        data_select(Command* select);
    void                        data_set(Command::OUTPUT radio = Command::INVALID);
    bool                        data_save();
    int                         row(const Command* command) const;
    void                        test();
    Command*                    _current;
    CommandVector&              _commands;
    Fl_Box*                     _label;
    Fl_Button*                  _copy;
    Fl_Button*                  _delete;
    Fl_Button*                  _execute;
    Fl_Button*                  _help;
    Fl_Button*                  _new;
    Fl_Button*                  _test;
    Fl_Hold_Browser*            _list;
    Fl_Input*                   _command;
    Fl_Input*                   _filter;
    Fl_Input*                   _line;
    Fl_Input*                   _name;
    Fl_Input*                   _string;
    Fl_Input*                   _workdir;
    Fl_Radio_Round_Button*      _capture;
    Fl_Check_Button*            _history;
    Fl_Radio_Round_Button*      _run;
    Fl_Radio_Round_Button*      _stream;
    Fl_Radio_Round_Button*      _terminal;
    Fl_Return_Button*           _close;
    flw::GridGroup*             _grid;
    int                         _last;
    int                         _res;
};
class CommandOutput : public flw::TabsGroup {
public:
    static const int            MAX_BUFFER_SIZE    = 10'000'000;
    static const int            MAX_LIST_LINES     =    100'000;
    static const int            MAX_TERMINAL_LINES =     10'000;
                                CommandOutput();
    void                        clear_list()
                                    { _list->clear(); Fl::redraw(); }
    void                        create_command_thread(std::string workdir, std::string filename, std::string selection);
    void                        join();
    flw::ScrollBrowser*         list()
                                    { return _list; }
    void                        list_next()
                                    { auto row = (_list->value() == 0) ? 1 : _list->value() + 1; _list->value((row > _list->size()) ? 1 : row); _list->do_callback(); }
    void                        list_prev()
                                    { auto row = (_list->value() == 0) ? _list->size() : _list->value() - 1; _list->value((row < 1) ? _list->size() : row); _list->do_callback(); }
    gnu::PCRE&                  list_rx()
                                    { return _list_rx; }
    void                        reset_terminal(bool force);
    void                        run_command(std::string workdir, std::string filename, std::string selection, bool repeat);
    void                        set_list_data(const std::vector<std::string>& lines, std::string parser, int select);
    void                        set_terminal_data(const gnu::FileBuf& buf);
    void                        show_editor();
    void                        show_list()
                                    { show(); value(_list); }
    void                        show_terminal()
                                    { show(); value(_terminal); }
    void                        update_pref();
    static void                 Join(void* message);
    static void                 ThreadFuncForList(std::string cmd, std::string work, gnu::PCRE* filter_regex, std::string line_regex, CommandOutput* self);
    static void                 ThreadFuncForTerminal(std::string cmd, std::string work, CommandOutput* self);
    static void                 ThreadFuncForTerminalStream(std::string cmd, std::string work, CommandOutput* self);
private:
    flw::ScrollBrowser*         _list;
    Fl_Terminal*                _terminal;
    gnu::PCRE                   _list_rx;
};
class DirBrowser : public Fl_Group {
public:
                                DirBrowser();
    std::string                 file() const
                                    { return _file; }
    void                        load_dir(std::string path);
    void                        load_root(std::string path);
    std::string                 path() const
                                    { return _path; }
    void                        resize(int X, int Y, int W, int H) override;
    std::string                 root() const
                                    { return _root; }
    void                        select_file();
    void                        update_pref();
    static void                 Callback(Fl_Widget* w, void* o);
private:
    Fl_Button*                  _refresh;
    Fl_File_Browser*            _browser;
    std::string                 _path;
    std::string                 _root;
    std::string                 _file;
};
class ProjectDialog : public Fl_Double_Window {
public:
    explicit                    ProjectDialog(gnu::DB& db);
    void                        resize(int X, int Y, int W, int H) override;
    std::string                 run(Fl_Window* parent);
    void                        update_pref();
    static void                 Callback(Fl_Widget* w, void* o);
private:
    Fl_Button*                  _cancel;
    Fl_Button*                  _load;
    Fl_Button*                  _remove;
    Fl_Hold_Browser*            _projects;
    flw::GridGroup*             _grid;
    gnu::DB&                    _db;
    std::string                 _name;
};
class TextDialog : public Fl_Double_Window {
public:
    explicit                     TextDialog(gnu::DB& db);
                                ~TextDialog();
    void                        close();
    void                        delete_text();
    void                        load_text();
    void                        resize(int X, int Y, int W, int H) override;
    void                        rename_text();
    std::string                 run(Fl_Window* parent);
    void                        update_text();
    static void                 Callback(Fl_Widget* w, void* o);
private:
    constexpr static const char* LABEL = "Saved Text Snippets - Copy Text To Clipboard";
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Button*                  _delete;
    Fl_Button*                  _rename;
    Fl_Button*                  _update;
    Fl_Hold_Browser*            _names;
    Fl_Text_Buffer*             _buffer;
    Fl_Text_Editor*             _editor;
    flw::GridGroup*             _grid;
    gnu::DB&                    _db;
    std::string                 _res;
};
class FlEdit : public Fl_Double_Window, fle::Message {
public:
    constexpr static size_t     MAX_SNIPPET_LENGTH = 1'000'000;
    enum class SNIPPET {
                                CLIPBOARD,
                                SELECTION,
                                TEXT,
    };
    std::string                 executable;
                                FlEdit(int W, int H);
                                ~FlEdit();
    void                        callback_list();
    void                        debug();
    void                        debug_compare();
    void                        debug_size()
                                    { flw::debug::print(this); _tabs.tabs1->debug(); }
    void                        do_layout()
                                    { _rect = Fl_Rect(); resize(x(), y(), w(), h()); Fl::redraw(); Fl::check(); }
    bool                        editor_close(fle::Editor* editor, bool ask);
    void                        editor_set_style(fle::Editor* editor, std::string override_style = "");
    void                        editor_take_focus()
                                    { if (_editor != nullptr) _editor->take_focus(); }
    void                        editor_update_status(fle::Editor* editor);
    void                        file_close();
    void                        file_close_all();
    fle::Editor*                file_load(Fl_Widget* after, std::string filename, bool add_recent = true, int line = 0);
    std::vector<std::string>    file_load_dialog();
    void                        file_load_list(Fl_Widget* after, std::vector<std::string> files, std::vector<int> filelines = std::vector<int>());
    void                        file_new(std::string filename);
    void                        file_open()
                                    { file_load_list(_editor, file_load_dialog()); }
    void                        file_readonly_mode();
    void                        file_reload()
                                    { file_reload(_editor); }
    void                        file_reload(fle::Editor* editor);
    void                        file_save()
                                    { file_save(_editor); }
    bool                        file_save(fle::Editor* editor);
    bool                        file_save_as()
                                    { return file_save_as(_editor); }
    bool                        file_save_as(fle::Editor* editor);
    void                        file_terminate()
                                    { ::exit(1); }
    int                         handle(int event) override;
    void                        help_about();
    void                        help_editor()
                                    { flw::dlg::list("Keyboard Shortcuts", fle::help::general(), this, true, 60, 50); }
    void                        help_fledit();
    void                        help_pcre()
                                    { flw::dlg::list("PCRE", fle::help::pcre(), this, true, 60, 50); }
    void                        menu_enable(std::string path, bool enable = true);
    Fl_Menu_Item*               menu_get(std::string path);
    void                        new_window();
    void                        pref_load(bool all = true);
    void                        pref_save();
    void                        pref_set_tabspos1(int value);
    void                        pref_set_tabspos2(int value);
    bool                        project_close(bool save);
    bool                        project_close_db();
    void                        project_defrag_db();
    void                        project_dir();
    bool                        project_exist(std::string name);
    void                        project_load();
    void                        project_load_from_db(std::string name);
    std::vector<std::string>    project_load_list(std::string key, gnu::Pile& data);
    void                        project_open_db();
    bool                        project_save()
                                    { return project_save(_project.name); }
    bool                        project_save(std::string name);
    void                        project_save_as();
    void                        project_save_list(std::string key, const std::vector<std::string>& list, gnu::Pile& data);
    void                        project_snippets();
    void                        project_snippets_save(SNIPPET snippet, std::string clip = "");
    bool                        quit();
    fle::Message::CTRL          message(const std::string& message, const std::string& s, const void* p) override;
    void                        resize(int X, int Y, int W, int H) override;
    void                        settings_backup()
                                    { CONFIG.pref_backup = gnu::str::to_string(fl_dir_chooser("Select Backup Directory Or Press Cancel To Disable Backup", (CONFIG.pref_backup.is_dir() == true) ? CONFIG.pref_backup.c_str() : gnu::File::HomeDir().c_str())); }
    void                        settings_editor()
                                    { fle::dlg::config(CONFIG); }
    void                        settings_load_pref()
                                    { pref_load(false); }
    void                        settings_output_horizontal();
    void                        settings_output_swap()
                                    { _split_edit->swap(); }
    void                        settings_output_vertical();
    void                        settings_save_pref()
                                    { pref_save(); }
    void                        settings_scheme()
                                    { fle::dlg::scheme(CONFIG); }
    void                        settings_split_horizontal()
                                    { menu_get(MENU_SETTINGS_SPLITHORIZONTAL)->set(); split_view(SPLIT::HOR); }
    void                        settings_split_vertical()
                                    { menu_get(MENU_SETTINGS_SPLITVERTICAL)->set(); split_view(SPLIT::VER); }
    void                        settings_tabg11()
                                    { pref_set_tabspos1((int) flw::TabsGroup::TABS::NORTH); }
    void                        settings_tabg12()
                                    { pref_set_tabspos1((int) flw::TabsGroup::TABS::SOUTH); }
    void                        settings_tabg13()
                                    { pref_set_tabspos1((int) flw::TabsGroup::TABS::WEST); }
    void                        settings_tabg14()
                                    { pref_set_tabspos1((int) flw::TabsGroup::TABS::EAST); }
    void                        settings_tabg21()
                                    { pref_set_tabspos2((int) flw::TabsGroup::TABS::NORTH); }
    void                        settings_tabg22()
                                    { pref_set_tabspos2((int) flw::TabsGroup::TABS::SOUTH); }
    void                        settings_tabg23()
                                    { pref_set_tabspos2((int) flw::TabsGroup::TABS::WEST); }
    void                        settings_tabg24()
                                    { pref_set_tabspos2((int) flw::TabsGroup::TABS::EAST); }
    void                        settings_theme()
                                    { flw::dlg::theme(true, true, this); CONFIG.send_message(fle::message::PREF_CHANGED); }
    void                        show_find()
                                    { CONFIG.send_message(fle::message::SHOW_FIND, "", &_findbar->findreplace()); }
    void                        show_one()
                                    { split_view(SPLIT::SHOWONE); }
    void                        show_two()
                                    { split_view(SPLIT::SHOWTWO); }
    void                        split_view(SPLIT value);
    void                        tabs_activate(fle::Editor* editor);
    void                        tabs_activate_cursor(std::string filename, int row, int col);
    int                         tabs_changed() const;
    void                        tabs_check_empty();
    void                        tabs_check_external_update();
    void                        tabs_close_all();
    int                         tabs_count() const;
    void                        tabs_delete(fle::Editor* editor);
    fle::Editor*                tabs_editor_by_index(int& index);
    fle::Editor*                tabs_editor_by_path(std::string path);
    void                        tabs_find_lines();
    void                        tabs_move_editor()
                                    { tabs_move_group(_editor); do_layout(); }
    void                        tabs_move_group(fle::Editor* editor);
    void                        tabs_replace_all();
    void                        tabs_reset_split_size();
    void                        tabs_restore_visibility();
    bool                        tabs_save_all(bool ask);
    void                        tabs_save_visibility();
    void                        tabs_sort(bool left, bool ascending);
    void                        tabs_trailing_all();
    void                        tools_next_output();
    void                        tools_prev_output();
    void                        tools_run_command(bool repeat);
    void                        toggle_browser();
    void                        toggle_fullscreen()
                                    { if (fullscreen_active() == 0) fullscreen(); else fullscreen_off(); }
    void                        toggle_menu()
                                    { if (_menu->visible() != 0) _menu->hide(); else _menu->show(); do_layout(); }
    void                        toggle_one()
                                    { if (_tabs.tabs1->visible()) split_view(SPLIT::HIDEONE); else split_view(SPLIT::SHOWONE); }
    void                        toggle_output()
                                    { if (_output->visible() != 0) _output->hide(); else _output->show(); do_layout(); }
    void                        toggle_tabs()
                                    { if (_tabs.tabs1->is_tabs_visible() == false) { _tabs.tabs1->show_tabs(); _tabs.tabs2->show_tabs(); } else { _tabs.tabs1->hide_tabs(); _tabs.tabs2->hide_tabs(); } }
    void                        toggle_two()
                                    { if (_tabs.tabs2->visible()) split_view(SPLIT::HIDETWO); else split_view(SPLIT::SHOWTWO); }
    void                        update_menu();
    void                        update_pref();
    static void                 CallbackFileBrowser(Fl_Widget* sender, void* data);
    static void                 CallbackList(Fl_Widget* sender, void* data);
    static void                 CallbackRecent(Fl_Widget* sender, void* data);
    static void                 CallbackWindow(Fl_Widget* sender, void* data);
    static inline void          CheckExternalUpdate()
                                    { SELF->tabs_check_external_update(); }
    static inline void          ShowOutputList()
                                    { SELF->_output->show_list(); SELF->do_layout(); Fl::redraw(); Fl::check(); }
    static inline void          ShowOutputTerminal()
                                    { SELF->_output->show_terminal(); SELF->do_layout(); Fl::redraw(); Fl::check(); }
    static inline bool          SettingsClearTerminal()
                                    { return SELF->menu_get(MENU_SETTINGS_OUTPUT_CLEAR)->value(); }
    static inline bool          settingsShowUnknown()
                                    { return SELF->menu_get(MENU_SETTINGS_OUTPUT_UNKNOWN)->value(); }
private:
    static FlEdit*              SELF;
    CommandOutput*              _output;
    DirBrowser*                 _dir_browser;
    Fl_Menu_Bar*                _menu;
    Fl_Rect                     _rect;
    fle::Editor*                _editor;
    fle::FindBar*               _findbar;
    flw::RecentMenu*            _recent;
    flw::SplitGroup*            _split_edit;
    flw::SplitGroup*            _split_main;
    gnu::DB                     _db;
    gnu::PCRE                   _list_rx;
    std::string                 _search;
    std::string                 _search_all;
    std::vector<std::string>    _old_find_list;
    std::vector<std::string>    _old_replace_list;
    struct {
        bool                    tabs1_vis;
        bool                    tabs2_vis;
        flw::SplitGroup*        split;
        flw::TabsGroup*         active;
        flw::TabsGroup*         tabs1;
        flw::TabsGroup*         tabs2;
        flw::TabsGroup::TABS    pref1;
        flw::TabsGroup::TABS    pref2;
    }                           _tabs;
    struct {
        std::string             dir;
        std::string             name;
    }                           _project;
    struct {
        std::string             start_dir;
        std::string             open_dir;
    }                           _paths;
};
#include <algorithm>
CommandDialog::CommandDialog(CommandVector& commands, Command* select) : Fl_Double_Window(0, 0, 0, 0, "Command Settings"), _commands(commands) {
    end();
    _capture  = new Fl_Radio_Round_Button(0, 0, 0, 0, "Capture");
    _close    = new Fl_Return_Button(0, 0, 0, 0, "Close");
    _command  = new Fl_Input(0, 0, 0, 0, "Command");
    _copy     = new Fl_Button(0, 0, 0, 0, "&Copy");
    _delete   = new Fl_Button(0, 0, 0, 0, "Delete");
    _execute  = new Fl_Button(0, 0, 0, 0, "&Execute");
    _filter   = new Fl_Input(0, 0, 0, 0, "Filter parser");
    _grid     = new flw::GridGroup();
    _help     = new Fl_Button(0, 0, 0, 0, "&Help");
    _history  = new Fl_Check_Button(0, 0, 0, 0, "Clear history");
    _label    = new Fl_Box(0, 0, 0, 0);
    _line     = new Fl_Input(0, 0, 0, 0, "Line parser");
    _list     = new Fl_Hold_Browser(0, 0, 0, 0);
    _name     = new Fl_Input(0, 0, 0, 0, "Name");
    _new      = new Fl_Button(0, 0, 0, 0, "&New");
    _run      = new Fl_Radio_Round_Button(0, 0, 0, 0, "Run");
    _stream   = new Fl_Radio_Round_Button(0, 0, 0, 0, "Stream to terminal");
    _string   = new Fl_Input(0, 0, 0, 0, "Test string for line regex");
    _terminal = new Fl_Radio_Round_Button(0, 0, 0, 0, "Capture to terminal");
    _test     = new Fl_Button(0, 0, 0, 0, "&Test");
    _workdir  = new Fl_Input(0, 0, 0, 0, "Work directory");
    _current  = nullptr;
    _res      = -1;
    _last     = 0;
    _grid->add(_list,          1,   1,  40,   -6);
    _grid->add(_name,         42,   3,  -1,   4);
    _grid->add(_command,      42,  10,  -1,   4);
    _grid->add(_workdir,      42,  17,  -1,   4);
    _grid->add(_run,          42,  23,  -1,   4);
    _grid->add(_capture,      42,  28,  -1,   4);
    _grid->add(_terminal,     42,  33,  -1,   4);
    _grid->add(_stream,       42,  38,  -1,   4);
    _grid->add(_filter,       42,  46,  -1,   4);
    _grid->add(_line,         42,  53,  -1,   4);
    _grid->add(_string,       42,  60,  -1,   4);
    _grid->add(_label,        42,  67,  -1,   8);
    _grid->add(_help,       -119,  -5,  16,   4);
    _grid->add(_delete,     -102,  -5,  16,   4);
    _grid->add(_copy,        -85,  -5,  16,   4);
    _grid->add(_new,         -68,  -5,  16,   4);
    _grid->add(_test,        -51,  -5,  16,   4);
    _grid->add(_execute,     -34,  -5,  16,   4);
    _grid->add(_close,       -17,  -5,  16,   4);
    add(_grid);
    _capture->callback(CommandDialog::Callback, this);
    _capture->setonly();
    _capture->tooltip("Capture output from command to listbox.");
    _close->callback(CommandDialog::Callback, this);
    _command->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
    _command->textfont(flw::PREF_FIXED_FONT);
    _command->textsize(flw::PREF_FONTSIZE);
    _command->tooltip("Enter command to execute.");
    _copy->callback(CommandDialog::Callback, this);
    _copy->tooltip("Copy current command to new command.");
    _delete->callback(CommandDialog::Callback, this);
    _execute->callback(CommandDialog::Callback, this);
    _filter->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
    _filter->textfont(flw::PREF_FIXED_FONT);
    _filter->textsize(flw::PREF_FONTSIZE);
    _filter->tooltip(
        "Enter a word for filtering captured lines.\n"
        "Or a PCRE compatible regular expression.\n"
        "Leave empty for all lines."
    );
    _help->callback(CommandDialog::Callback, this);
    _label->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT| FL_ALIGN_TOP);
    _label->box(FL_BORDER_BOX);
    _label->color(FL_BACKGROUND2_COLOR);
    _label->labelfont(FL_COURIER);
    _line->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
    _line->textfont(flw::PREF_FIXED_FONT);
    _line->textsize(flw::PREF_FONTSIZE);
    _line->tooltip("Enter a PCRE regular compatible expression to retrieve file and line number.");
    _list->callback(CommandDialog::Callback, this);
    _list->textfont(flw::PREF_FIXED_FONT);
    _list->textsize(flw::PREF_FONTSIZE);
    _name->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
    _name->textfont(flw::PREF_FIXED_FONT);
    _name->textsize(flw::PREF_FONTSIZE);
    _name->tooltip("Give command a unique name.");
    _new->callback(CommandDialog::Callback, this);
    _new->tooltip("Create new empty command.");
    _run->callback(CommandDialog::Callback, this);
    _run->tooltip("Run command as seperate process.");
    _stream->callback(CommandDialog::Callback, this);
    _stream->tooltip("Stream output from command to terminal widget.");
    _string->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
    _string->textfont(flw::PREF_FIXED_FONT);
    _string->textsize(flw::PREF_FONTSIZE);
    _string->tooltip("Enter test string to check regular expressions on.");
    _terminal->callback(CommandDialog::Callback, this);
    _terminal->tooltip("Capture output from command to terminal widget.");
    _test->callback(CommandDialog::Callback, this);
    _test->tooltip("Test regular expressions.");
    _workdir->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
    _workdir->textfont(flw::PREF_FIXED_FONT);
    _workdir->textsize(flw::PREF_FONTSIZE);
    _workdir->tooltip("Enter work directory or leave empty to use current.");
    flw::util::labelfont(this);
    data_load();
    data_select(select);
    data_set();
    callback(CommandDialog::Callback, this);
    set_modal();
    resizable(this);
    CommandDialog::resize(0, 0, flw::PREF_FONTSIZE * 68, flw::PREF_FONTSIZE * 42);
    size_range(0, 0, flw::PREF_FONTSIZE * 68, flw::PREF_FONTSIZE * 42);
}
void CommandDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<CommandDialog*>(o);
    if (w == self) {
        CommandDialog::Callback(self->_close, self);
    }
    else if (w == self->_close) {
        if (self->data_save()) {
            self->_current = nullptr;
            self->hide();
        }
    }
    else if (w == self->_delete) {
        self->data_delete();
        self->data_set();
    }
    else if (w == self->_execute) {
        self->_res = self->row(self->_current);
        self->_res--;
        if (self->_res >= 0 && self->data_save() == true) {
            self->hide();
        }
    }
    else if (w == self->_list) {
        if (self->data_save() == true) {
            auto row = self->_list->value();
            if (row > 0) {
                self->_current = static_cast<Command*>(self->_list->data(row));
                self->data_set();
            }
        }
        else if (self->_last > 0) {
            self->_list->value(self->_last);
        }
        if (Fl::event_clicks() > 0) {
            CommandDialog::Callback(self->_execute, self);
        }
    }
    else if (w == self->_copy) {
        if (self->data_save() == true) {
            self->data_new(self->_current);
            self->data_set();
            self->_name->take_focus();
        }
    }
    else if (w == self->_new) {
        if (self->data_save() == true) {
            self->data_new();
            self->data_set();
            self->_name->take_focus();
        }
    }
    else if (w == self->_test) {
        self->test();
        self->_string->take_focus();
    }
    else if (w == self->_run) {
        self->data_save();
        self->data_set(Command::RUN);
    }
    else if (w == self->_capture) {
        self->data_save();
        self->data_set(Command::CAPTURE_LIST);
    }
    else if (w == self->_terminal) {
        self->data_save();
        self->data_set(Command::CAPTURE_TERMINAL);
    }
    else if (w == self->_stream) {
        self->data_save();
        self->data_set(Command::STREAM_TERMINAL);
    }
    else if (w == self->_help) {
        flw::dlg::list("Command Help", COMMAND_HELP + fle::help::pcre(), self, true, 60, 40);
    }
}
bool CommandDialog::data_delete() {
    if (_current != nullptr) {
        for (auto it = _commands.begin(); it != _commands.end(); ++it) {
            if (*it == _current) {
                delete *it;
                _current = nullptr;
                _commands.erase(it);
                data_load();
                return true;
            }
        }
    }
    return true;
}
void CommandDialog::data_load() {
    _current = nullptr;
    _list->clear();
    std::sort(_commands.begin(), _commands.end(), &Command::Sort);
    for (auto c : _commands) {
        _list->add(c->name.c_str(), c);
    }
}
void CommandDialog::data_new(Command* copy) {
    _current = new Command(gnu::str::format("new command %d", rand() % 256'000), "", Command::RUN, "", "", "");
    if (copy != nullptr) {
        _current->command      = copy->command;
        _current->filter_regex = copy->filter_regex;
        _current->line_regex   = copy->line_regex;
        _current->name         = copy->name + gnu::str::format(" %d", rand() % 256'000);
        _current->output       = copy->output;
        _current->workdir      = copy->workdir;
    }
    _commands.push_back(_current);
    _list->add(_current->name.c_str(), _current);
    _list->value(_list->size());
    _list->show(_list->size());
}
bool CommandDialog::data_save() {
    if (_current != nullptr) {
        auto r       = row(_current);
        auto name    = gnu::str::to_string(_name->value());
        auto command = gnu::str::to_string(_command->value());
        auto workdir = gnu::str::to_string(_workdir->value());
        auto filter  = gnu::str::to_string(_filter->value());
        auto line    = gnu::str::to_string(_line->value());
        auto output  = Command::RUN;
        if (_capture->value() != 0) {
            output = Command::CAPTURE_LIST;
        }
        else if (_terminal->value() != 0) {
            output = Command::CAPTURE_TERMINAL;
        }
        else if (_stream->value() != 0) {
            output = Command::STREAM_TERMINAL;
        }
        gnu::PCRE re1(filter);
        gnu::PCRE re2(line);
        if (name == "") {
            _label->copy_label("error: name is empty");
            _name->take_focus();
            fl_beep(FL_BEEP_ERROR);
            return false;
        }
        else if (filter != "" && re1.is_compiled() == false) {
            _label->copy_label(re1.error().c_str());
            _filter->take_focus();
            fl_beep(FL_BEEP_ERROR);
            return false;
        }
        else if (line != "" && re2.is_compiled() == false) {
            _label->copy_label(re2.error().c_str());
            _line->take_focus();
            fl_beep(FL_BEEP_ERROR);
            return false;
        }
        else if (r == 0) {
            _label->copy_label("error: internal problem CommandDialog::data_save()");
            fl_beep(FL_BEEP_ERROR);
            return false;
        }
        else {
            _list->text(r, name.c_str());
            _label->copy_label("");
            _current->name         = gnu::str::trim(name);
            _current->command      = gnu::str::trim(command);
            _current->workdir      = gnu::str::trim(workdir);
            _current->filter_regex = gnu::str::trim(filter);
            _current->line_regex   = gnu::str::trim(line);
            _current->output       = output;
        }
    }
    return true;
}
void CommandDialog::data_select(Command* select) {
    auto r = 1;
    for (const auto c : _commands) {
        if (c == select) {
            _list->value(r);
            _list->show(r);
            _list->do_callback();
            if (_list->size() > 25) {
                _list->topline(r - 25);
            }
            else {
                _list->topline(1);
            }
            return;
        }
        r++;
    }
}
void CommandDialog::data_set(Command::OUTPUT radio) {
    if (_current != nullptr) {
        _last = _list->value();
        _name->value(_current->name.c_str());
        _command->value(_current->command.c_str());
        _workdir->value(_current->workdir.c_str());
        _filter->value(_current->filter_regex.c_str());
        _line->value(_current->line_regex.c_str());
        if (radio == Command::RUN) {
            _run->setonly();
        }
        else if (radio == Command::CAPTURE_LIST) {
            _capture->setonly();
        }
        else if (radio == Command::CAPTURE_TERMINAL) {
            _terminal->setonly();
        }
        else if (radio == Command::STREAM_TERMINAL) {
            _stream->setonly();
        }
        else if (_current->output == Command::RUN) {
            _run->setonly();
        }
        else if (_current->output == Command::CAPTURE_LIST) {
            _capture->setonly();
        }
        else if (_current->output == Command::CAPTURE_TERMINAL) {
            _terminal->setonly();
        }
        else if (_current->output == Command::STREAM_TERMINAL) {
            _stream->setonly();
        }
        _capture->activate();
        _command->activate();
        _copy->activate();
        _delete->activate();
        _execute->activate();
        _name->activate();
        _run->activate();
        _terminal->activate();
        _stream->activate();
        _workdir->activate();
        if (_capture->value() != 0) {
            _filter->activate();
            _label->activate();
            _line->activate();
            _string->activate();
            _test->activate();
        }
        else {
            _filter->deactivate();
            _label->deactivate();
            _line->deactivate();
            _string->deactivate();
            _test->deactivate();
        }
    }
    else {
        _last = 0;
        _command->value("");
        _name->value("");
        _filter->value("");
        _label->label("");
        _line->value("");
        _workdir->value("");
        _capture->deactivate();
        _command->deactivate();
        _copy->deactivate();
        _delete->deactivate();
        _execute->deactivate();
        _filter->deactivate();
        _label->deactivate();
        _line->deactivate();
        _name->deactivate();
        _run->deactivate();
        _stream->deactivate();
        _string->deactivate();
        _terminal->deactivate();
        _test->deactivate();
        _workdir->deactivate();
    }
    redraw();
}
void CommandDialog::resize(int X, int Y, int W, int H) {
    Fl_Double_Window::resize(X, Y, W, H);
    _grid->resize(0, 0, W, H);
}
int CommandDialog::row(const Command* command) const {
    for (int f = 1; command && f <= _list->size(); f++) {
        if (_list->data(f) == command) {
            return f;
        }
    }
    return 0;
}
Command* CommandDialog::run(Fl_Window* parent) {
    flw::util::center_window(this, parent);
    show();
    while (visible()) {
        Fl::wait();
        Fl::flush();
    }
    return _res >= 0 && _res < (int) _commands.size() ? _commands[_res] : nullptr;
}
void CommandDialog::test() {
    auto filter = gnu::str::to_string(_filter->value());
    auto string = gnu::str::to_string(_string->value());
    std::string   info;
    gnu::PCRE re1(filter);
    gnu::PCRE re2(_line->value());
    info += "filter: ";
    if (re1.is_compiled() == false) {
        info += re1.error();
    }
    else if (string == "") {
        info += "regex ok but empty test string!";
    }
    else if (re1.exec(string) == 0) {
        info += "no match";
    }
    else {
        info += "matches string";
    }
    info += "\n";
    info += "line: ";
    if (re2.is_compiled() == false) {
        info += re2.error();
    }
    else if (string == "") {
        info += "regex ok but empty test string!";
    }
    else if (re2.exec(string) == 0) {
        info += "no match";
    }
    else {
        auto f = re2.substr("file");
        auto l = re2.substr("line");
        auto c = re2.substr("col");
        info += "file=";
        info += f;
        info += ", line=";
        info += l;
        info += ", col=";
        info += c;
    }
    _label->copy_label(info.c_str());
}
Command*                    Command::CURRENT        = nullptr;
int                         Command::SELECT_LINE    = 0;
std::thread*                Command::THREAD         = nullptr;
CommandVector               Command::COMMANDS;
gnu::FileBuf                Command::BUF;
std::string                 Command::LINE_REGEX;
std::string                 Command::WORKDIR;
std::vector<std::string>    Command::LINES;
CommandOutput::CommandOutput() : flw::TabsGroup(0, 0, 0, 0) {
    end();
    _list     = new flw::ScrollBrowser();
    _terminal = new Fl_Terminal(0, 0, 0, 0);
    add("Terminal", _terminal);
    add("List", _list);
    tabs(flw::TabsGroup::TABS::WEST);
    _list->callback(FlEdit::CallbackList, this);
    _list->when(FL_WHEN_ENTER_KEY_CHANGED);
}
void CommandOutput::create_command_thread(std::string workdir, std::string filename, std::string selection) {
    auto work = Command::CURRENT->workdir;
    auto cmd  = Command::CURRENT->command;
    auto fi   = gnu::File(filename);
    if (filename != "" && work.find("$FILE") != std::string::npos) {
        gnu::str::replace(work, "$FILE", fi.path);
    }
    else if (filename == "" && work.find("$FILE") != std::string::npos) {
        fl_alert("error: file has not been set");
        return;
    }
    else if (workdir != "" && work.find("$PROJECT") != std::string::npos) {
        gnu::str::replace(work, "$PROJECT", workdir);
    }
    else if (workdir == "" && work.find("$PROJECT") != std::string::npos) {
        fl_alert("error: project directory has not been set");
        return;
    }
    if (filename != "" && cmd.find("$FILE") != std::string::npos) {
        gnu::str::replace(cmd, "$FILE", filename);
    }
    else if (filename == "" && cmd.find("$FILE") != std::string::npos) {
        fl_alert("error: file has not been set");
        return;
    }
    if (workdir != "" && cmd.find("$PROJECT") != std::string::npos) {
        gnu::str::replace(cmd, "$PROJECT", workdir);
    }
    else if (workdir == "" && cmd.find("$PROJECT") != std::string::npos) {
        fl_alert("error: project has not been set");
        return;
    }
    if (selection != "" && cmd.find("$SELECTION") != std::string::npos) {
        if (selection.length() > 0 && selection.front() != '"') {
            selection = "\"" + selection;
        }
        if (selection.length() > 0 && selection.back() != '"') {
            selection += "\"";
        }
        gnu::str::replace(cmd, "$SELECTION", selection);
    }
    else if (selection == "" && cmd.find("$SELECTION") != std::string::npos) {
        fl_alert("error: selection has not been set");
        return;
    }
#ifdef _WIN32
    cmd = std::string("CMD.EXE /C ") + cmd + " 2>&1";
#else
    cmd = cmd + " 2>&1";
#endif
    fl_message_position(top_window());
    if (Command::CURRENT->output == Command::CAPTURE_LIST) {
        auto rx = new gnu::PCRE(Command::CURRENT->filter_regex, true);
        if (Command::CURRENT->filter_regex.empty() == false && rx->is_compiled() == false) {
            fl_alert("error: regex expression!\n%s\n%s", rx->error().c_str(), rx->pattern().c_str());
            delete rx;
        }
        else {
            _list_rx.clear();
            _list->clear();
            _list->add(gnu::str::format("executing %s", cmd.c_str()).c_str());
            _list->deactivate();
            FlEdit::ShowOutputList();
            Command::THREAD = new std::thread(CommandOutput::ThreadFuncForList, cmd, work, rx, Command::CURRENT->line_regex, this);
        }
    }
    else if (Command::CURRENT->output == Command::CAPTURE_TERMINAL) {
        FlEdit::ShowOutputTerminal();
        reset_terminal(false);
        Command::THREAD = new std::thread(CommandOutput::ThreadFuncForTerminal, cmd, work, this);
    }
    else if (Command::CURRENT->output == Command::STREAM_TERMINAL) {
        FlEdit::ShowOutputTerminal();
        reset_terminal(false);
        Command::THREAD = new std::thread(CommandOutput::ThreadFuncForTerminalStream, cmd, work, this);
    }
    else {
        auto ret = gnu::File::Run(cmd.c_str(), true, false);
        if (ret != 0) {
            fl_alert("error: failed to execute '%s'\nreturn code is %d", cmd.c_str(), ret);
        }
    }
}
void CommandOutput::join() {
    if (Command::THREAD == nullptr) {
        return;
    }
    Command::THREAD->join();
    delete Command::THREAD;
    Command::THREAD = nullptr;
    if (Command::CURRENT->output == Command::CAPTURE_LIST) {
        set_list_data(Command::LINES, Command::LINE_REGEX, Command::SELECT_LINE);
        show_editor();
    }
    else if (Command::CURRENT->output == Command::CAPTURE_TERMINAL) {
        show_editor();
    }
    else if (Command::CURRENT->output == Command::STREAM_TERMINAL) {
        show_editor();
    }
    Command::LINES.clear();
    Command::LINE_REGEX = "";
    Command::WORKDIR = "";
    Command::SELECT_LINE = 0;
    Command::BUF.clear();
    FlEdit::CheckExternalUpdate();
}
void CommandOutput::Join(void* o) {
    auto self = static_cast<CommandOutput*>(o);
    self->join();
}
void CommandOutput::reset_terminal(bool force) {
    if (FlEdit::SettingsClearTerminal() == true || force == true) {
        _terminal->reset_terminal();
        _terminal->history_lines(CommandOutput::MAX_TERMINAL_LINES);
        _terminal->margin_left(3);
        _terminal->margin_right(3);
        _terminal->margin_top(3);
        _terminal->margin_bottom(3);
        _terminal->ansi(true);
        _terminal->scrollbar->linesize(6);
    }
    _terminal->show_unknown(FlEdit::settingsShowUnknown());
}
void CommandOutput::run_command(std::string workdir, std::string filename, std::string selection, bool repeat) {
    fl_message_position(top_window());
    if (Command::THREAD != nullptr) {
        fl_alert("%s", "error: previous command is still running...");
    }
    else if (repeat == true && Command::CURRENT != nullptr) {
        create_command_thread(workdir, filename, selection);
    }
    else {
        CommandDialog dialog(Command::COMMANDS, Command::CURRENT);
        auto command = dialog.run(top_window());
        if (command != nullptr) {
            Command::CURRENT = command;
            create_command_thread(workdir, filename, selection);
        }
    }
}
void CommandOutput::set_list_data(const std::vector<std::string>& lines, std::string parser, int select) {
    _list->clear();
    _list->activate();
    for (const auto& line : lines) {
        if (line.length() > fle::limits::OUTPUT_LINE_LENGTH_VAL) {
            auto l = line;
            l.resize(fle::limits::OUTPUT_LINE_LENGTH_VAL);
            l += "|";
            _list->add(l.c_str());
        }
        else {
            _list->add(line.c_str());
        }
    }
    _list_rx.compile(parser);
    if (select > 0) {
        _list->value(select);
    }
    value(_list);
}
void CommandOutput::set_terminal_data(const gnu::FileBuf& buf) {
    reset_terminal(false);
    _terminal->append(buf.p, buf.s);
    value(_terminal);
}
void CommandOutput::show_editor() {
    static_cast<FlEdit*>(top_window())->editor_take_focus();
}
void CommandOutput::ThreadFuncForList(std::string cmd, std::string work, gnu::PCRE* filter_regex, std::string line_regex, CommandOutput* self) {
    static const size_t BUFFER_READ = 16'384;
    auto start  = gnu::Time::Milli();
    auto handle = static_cast<FILE*>(nullptr);
    auto old    = gnu::File::WorkDir().filename;
    gnu::File::ChDir(work);
    work = gnu::File::WorkDir().filename;
    Command::SELECT_LINE = 0;
    Command::LINE_REGEX = line_regex;
    Command::BUF.clear();
    Command::LINES.clear();
    Command::LINES.push_back(gnu::str::format("running in directory %s", work.c_str()));
    Command::LINES.push_back(gnu::str::format("executing %s", cmd.c_str()));
    handle = gnu::File::Popen(cmd);
    if (handle == nullptr) {
        Command::LINES.push_back("error: failed to execute command");
    }
    else {
        while (true) {
            char buffer[BUFFER_READ];
            auto read = fread(buffer, 1, BUFFER_READ, handle);
            if (read > 0) {
                Command::BUF.add(buffer, read);
            }
            else if (feof(handle) != 0) {
                break;
            }
            else if (Command::BUF.s >= CommandOutput::MAX_BUFFER_SIZE) {
                Command::LINES.push_back("error: max read bytes limits has been reached!");
                break;
            }
        }
        pclose(handle);
        Command::BUF.add("\0", 1);
        Command::LINES.push_back("Output:");
        Command::SELECT_LINE = (int) Command::LINES.size();
        for (const auto& line : gnu::str::split(Command::BUF.p, '\n')) {
            if (filter_regex->is_compiled() == false || filter_regex->exec(line) > 0) {
                if (Command::LINES.size() == CommandOutput::MAX_LIST_LINES) {
                    Command::LINES.push_back("error: line limits have been reached!");
                    break;
                }
                else {
                    auto tmp = gnu::str::replace_const(line, "\r");
                    Command::LINES.push_back(tmp);
                }
            }
        }
        Command::LINES.push_back(std::string("Finished in ") + gnu::Time::FormatMilliToTime((gnu::Time::Milli() - start) / 1000));
    }
    delete filter_regex;
    Fl::awake(CommandOutput::Join, self);
    gnu::File::ChDir(old);
}
void CommandOutput::ThreadFuncForTerminal(std::string cmd, std::string work, CommandOutput* self) {
    static const size_t BUFFER_READ = 16'384;
    auto old = gnu::File::WorkDir().filename;
    gnu::File::ChDir(work);
    work = gnu::File::WorkDir().filename;
    auto handle = gnu::File::Popen(cmd);
    std::string message;
    if (handle == nullptr) {
        message = "error: failed to execute command " + cmd + " in " + work + "\n";
    }
    else {
        while (true) {
            char buffer[BUFFER_READ];
            auto read = fread(buffer, 1, BUFFER_READ, handle);
            if (read > 0) {
                Command::BUF.add(buffer, read);
            }
            else if (feof(handle) != 0) {
                break;
            }
            if (Command::BUF.s >= CommandOutput::MAX_BUFFER_SIZE) {
                message = gnu::str::format("error: max read bytes limits has been reached (%d)!\n", (int) Command::BUF.s);
                break;
            }
        }
        pclose(handle);
    }
    Command::BUF.add(message.c_str(), message.length());
    Fl::lock();
    self->_terminal->append(Command::BUF.p, Command::BUF.s);
    self->_terminal->append(nullptr);
    Fl::unlock();
    Fl::awake(CommandOutput::Join, self);
    gnu::File::ChDir(old);
}
void CommandOutput::ThreadFuncForTerminalStream(std::string cmd, std::string work, CommandOutput* self) {
    static const size_t BUFFER_READ = 128;
    auto old = gnu::File::WorkDir().filename;
    gnu::File::ChDir(work);
    work = gnu::File::WorkDir().filename;
    auto handle = gnu::File::Popen(cmd);
    auto tot = 0;
    std::string message;
    if (handle == nullptr) {
        message = "error: failed to execute command " + cmd + " in " + work + "\n";
    }
    else {
        while (true) {
            char buffer[BUFFER_READ + 1];
            memset(buffer, 0, BUFFER_READ + 1);
            auto read = fread(buffer, 1, BUFFER_READ, handle);
            if (read > 0) {
                Fl::lock();
                self->_terminal->append(buffer, read);
                Fl::check();
                Fl::unlock();
                tot += read;
            }
            else if (feof(handle) != 0) {
                break;
            }
            if (tot >= CommandOutput::MAX_BUFFER_SIZE) {
                message = gnu::str::format("error: max read bytes limits has been reached (%d)!\n", (int) tot);
                break;
            }
        }
        pclose(handle);
    }
    Command::BUF.add(message.c_str(), message.length());
    Fl::lock();
    self->_terminal->append(message.c_str(), message.length());
    self->_terminal->append(nullptr);
    Fl::unlock();
    Fl::awake(CommandOutput::Join, self);
    gnu::File::ChDir(old);
}
void CommandOutput::update_pref() {
    flw::util::labelfont(this);
    _list->textfont(flw::PREF_FIXED_FONT);
    _list->textsize(flw::PREF_FIXED_FONTSIZE);
    _terminal->textfont(flw::PREF_FIXED_FONT);
    _terminal->textsize(flw::PREF_FIXED_FONTSIZE);
}
DirBrowser::DirBrowser() : Fl_Group(0, 0, 0, 0) {
    end();
    _browser = new Fl_File_Browser(0, 0, 0, 0);
    _refresh = new Fl_Button(0, 0, 0, 0, "Refresh");
    add(_browser);
    add(_refresh);
    _browser->callback(DirBrowser::Callback, this);
    _browser->tooltip("Only files in project directory or child directories can be used.\nDouble click a directory to change it.\nDouble click a file to open that file.");
    _browser->type(FL_HOLD_BROWSER);
    _refresh->callback(DirBrowser::Callback, this);
    _refresh->tooltip("Refresh directory");
}
void DirBrowser::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<DirBrowser*>(o);
    if (w == self->_browser) {
        if (Fl::event_clicks() > 0) {
            self->select_file();
        }
    }
    else if (w == self->_refresh) {
        self->load_dir(self->_path);
    }
}
void DirBrowser::load_dir(std::string path) {
    _browser->load(path.c_str());
    _path = path;
}
void DirBrowser::load_root(std::string path) {
     gnu::File f(path);
     if (f.is_dir() == true) {
        _file = "";
        _root = f.filename;
        load_dir(_root);
     }
     else {
        _browser->clear();
        _path = "";
        _root = "";
        _file = "";
     }
}
void DirBrowser::resize(int X, int Y, int W, int H) {
    int fs = flw::PREF_FONTSIZE;
    Fl_Widget::resize(X,  Y,               W,  H);
    _browser->resize (X,  Y,               W,  H - fs * 2);
    _refresh->resize (X,  Y + H - fs * 2,  W,  fs * 2);
}
void DirBrowser::select_file() {
    auto row = _browser->value();
    if (row > 0) {
        std::string name = _browser->text(row);
        gnu::File   f(_path + "/" + name);
        if (f.name == "..") {
            f = gnu::File(f.path);
            if (f.filename != _root) {
                load_dir(f.path);
            }
        }
        else if (f.is_dir() == true) {
            load_dir(f.filename);
        }
        else if (f.is_file() == true) {
            _file = f.filename;
            do_callback();
        }
    }
}
void DirBrowser::update_pref() {
    flw::util::labelfont(this);
    _browser->textfont(flw::PREF_FONT);
    _browser->textsize(flw::PREF_FONTSIZE);
}
ProjectDialog::ProjectDialog(gnu::DB& db) :
Fl_Double_Window(0, 0, 100, 100, "Load Project"),
_db(db) {
    end();
    _cancel   = new Fl_Button(0, 0, 0, 0, "&Cancel");
    _grid     = new flw::GridGroup(0, 0, w(), h());
    _load     = new Fl_Button(0, 0, 0, 0, "&Load");
    _projects = new Fl_Hold_Browser(0, 0, 0, 0);
    _remove   = new Fl_Button(0, 0, 0, 0, "&Remove");
    _grid->add(_projects,   1,   1,  -1,  -6);
    _grid->add(_cancel,   -51,  -5,  16,   4);
    _grid->add(_remove,   -34,  -5,  16,   4);
    _grid->add(_load,     -17,  -5,  16,   4);
    add(_grid);
    for (auto& row : db.keys(DBKEY_PROJECTS + "%", DBKEY_PROJECTS)) {
        _projects->add(row.key.c_str());
    }
    _cancel->callback(ProjectDialog::Callback, this);
    _load->callback(ProjectDialog::Callback, this);
    _load->deactivate();
    _projects->callback(ProjectDialog::Callback, this);
    _remove->callback(ProjectDialog::Callback, this);
    _remove->deactivate();
    callback(ProjectDialog::Callback, this);
    set_modal();
    resizable(_grid);
    update_pref();
}
void ProjectDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<ProjectDialog*>(o);
    fl_message_position(self);
    if (w == self) {
        self->hide();
    }
    else if (w == self->_cancel) {
        self->hide();
    }
    else if (w == self->_load) {
        self->_name = gnu::str::to_string(self->_projects->text(self->_projects->value()));
        if (self->_name != "") {
            self->hide();
        }
        else {
            fl_beep(FL_BEEP_NOTIFICATION);
        }
    }
    else if (w == self->_remove) {
        auto row = self->_projects->value();
        if (row > 0) {
            auto name = gnu::str::to_string(self->_projects->text(row));
            if (self->_db.remove(DBKEY_PROJECTS + name) == false) {
                fl_alert("error: failed to delete project from database!\n%s", self->_db.err_msg.c_str());
                return;
            }
            self->_projects->remove(row);
        }
    }
    else if (w == self->_projects) {
        auto row = self->_projects->value();
        if (row > 0) {
            self->_load->activate();
            self->_remove->activate();
        }
        if (row > 0 && Fl::event_clicks() > 0) {
            Fl::event_clicks(0);
            Callback(self->_load, self);
        }
    }
}
void ProjectDialog::resize(int X, int Y, int W, int H) {
    Fl_Double_Window::resize(X, Y, W, H);
    _grid->resize(0, 0, W, H);
}
std::string ProjectDialog::run(Fl_Window* parent) {
    _projects->take_focus();
    flw::util::center_window(this, parent);
    show();
    while (visible()) {
        Fl::wait();
        Fl::flush();
    }
    return _name;
}
void ProjectDialog::update_pref() {
    _projects->textfont(flw::PREF_FIXED_FONT);
    _projects->textsize(flw::PREF_FONTSIZE);
    flw::util::labelfont(this);
    ProjectDialog::resize(0, 0, flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 40);
    size_range(flw::PREF_FONTSIZE * 20, flw::PREF_FONTSIZE * 30);
}
TextDialog::TextDialog(gnu::DB& db) :
Fl_Double_Window(0, 0, 0, 0, TextDialog::LABEL),
_db(db) {
    end();
    _buffer = new Fl_Text_Buffer();
    _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
    _close  = new Fl_Button(0, 0, 0, 0, "&Ok");
    _delete = new Fl_Button(0, 0, 0, 0, "Delete");
    _editor = new Fl_Text_Editor(0, 0, 0, 0);
    _grid   = new flw::GridGroup();
    _names  = new Fl_Hold_Browser(0, 0, 0, 0);
    _rename = new Fl_Button(0, 0, 0, 0, "&Rename");
    _update = new Fl_Button(0, 0, 0, 0, "&Update");
    _grid->add(_names,    1,   1,  40,  -6);
    _grid->add(_editor,  42,   1,  -1,  -6);
    _grid->add(_delete, -85,  -5,  16,   4);
    _grid->add(_update, -68,  -5,  16,   4);
    _grid->add(_rename, -51,  -5,  16,   4);
    _grid->add(_cancel, -34,  -5,  16,   4);
    _grid->add(_close,  -17,  -5,  16,   4);
    add(_grid);
    _cancel->callback(TextDialog::Callback, this);
    _close->callback(TextDialog::Callback, this);
    _close->tooltip("Copy selected text to clipboard and close dialog.");
    _delete->callback(TextDialog::Callback, this);
    _delete->tooltip("Delete selected text.");
    _rename->callback(TextDialog::Callback, this);
    _rename->tooltip("Rename selected text.");
    _editor->buffer(_buffer);
    _editor->labelsize(flw::PREF_FONTSIZE);
    _editor->linenumber_align(FL_ALIGN_RIGHT);
    _editor->linenumber_bgcolor(FL_BACKGROUND_COLOR);
    _editor->linenumber_font(flw::PREF_FIXED_FONT);
    _editor->linenumber_format("%4d");
    _editor->linenumber_size(flw::PREF_FONTSIZE);
    _editor->linenumber_width(flw::PREF_FIXED_FONTSIZE * 3);
    _editor->textfont(flw::PREF_FIXED_FONT);
    _editor->textsize(flw::PREF_FIXED_FONTSIZE);
    _names->callback(TextDialog::Callback, this);
    _names->textfont(flw::PREF_FIXED_FONT);
    _names->textsize(flw::PREF_FONTSIZE);
    _update->callback(TextDialog::Callback, this);
    _update->tooltip("Save and update current text.");
    for (auto& row : _db.keys(DBKEY_SNIPPETS + "%", DBKEY_SNIPPETS)) {
        _names->add(row.key.c_str());
    }
    flw::util::labelfont(this);
    callback(TextDialog::Callback, this);
    set_modal();
    resizable(this);
    TextDialog::resize(0, 0, flw::PREF_FONTSIZE * 80, flw::PREF_FONTSIZE * 50);
    size_range(480, 320);
}
TextDialog::~TextDialog() {
    _editor->buffer(nullptr);
    delete _buffer;
}
void TextDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<TextDialog*>(o);
    fl_message_position(self);
    if (w == self) {
        self->hide();
    }
    else if (w == self->_cancel) {
        self->hide();
    }
    else if (w == self->_close) {
        self->close();
    }
    else if (w == self->_delete) {
        self->delete_text();
    }
    else if (w == self->_names) {
        self->load_text();
    }
    else if (w == self->_rename) {
        self->rename_text();
    }
    else if (w == self->_update) {
        self->update_text();
    }
}
void TextDialog::close() {
    auto name = gnu::str::to_string(_names->text(_names->value()));
    auto row  = _db.get(DBKEY_SNIPPETS + name);
    if (row.value != nullptr) {
        _res = row.value;
    }
    else {
        _res = "";
    }
    if (_res != "") {
        hide();
    }
    else {
        fl_beep(FL_BEEP_NOTIFICATION);
    }
}
void TextDialog::delete_text() {
    auto row = _names->value();
    copy_label(TextDialog::LABEL);
    if (row < 1) {
        return;
    }
    auto name =_names->text(row);
    if (_db.remove(DBKEY_SNIPPETS + name) == false) {
        fl_alert("error: failed to delete text from database!\n%s", _db.err_msg.c_str());
        return;
    }
    _names->remove(row);
    _buffer->text("");
    _names->take_focus();
    _names->value(row <= _names->size() ? row : _names->size());
    TextDialog::Callback(_names, this);
    copy_label((std::string(TextDialog::LABEL) + " - Deleted " + name).c_str());
}
void TextDialog::load_text() {
    auto r = _names->value();
    if (r < 1) {
        _buffer->text("");
        _close->deactivate();
        _delete->deactivate();
        _rename->deactivate();
        _update->deactivate();
        return;
    }
    else if (Fl::event_clicks() > 0) {
        Fl::event_clicks(0);
        TextDialog::Callback(_close, this);
        return;
    }
    auto name = gnu::str::to_string(_names->text(r));
    auto row  = _db.get(DBKEY_SNIPPETS + name);
    if (row.value != nullptr) {
        _buffer->text(row.value);
        _close->activate();
        _delete->activate();
        _rename->activate();
        _update->activate();
    }
    else {
        _buffer->text("");
        _close->deactivate();
        _delete->deactivate();
        _rename->deactivate();
        _update->deactivate();
    }
}
void TextDialog::rename_text() {
    auto r = _names->value();
    copy_label(TextDialog::LABEL);
    if (r < 1) {
        return;
    }
    auto name   = gnu::str::to_string(_names->text(r));
    auto rename = gnu::str::to_string(fl_input("%s", name.c_str(), "Enter new name"));
    gnu::str::trim(rename);
    if (rename == "") {
        return;
    }
    auto row = _db.get(DBKEY_SNIPPETS + rename);
    if (row.value != nullptr) {
        fl_alert("%s", "error: name exist in database");
        return;
    }
    if (_db.rename(DBKEY_SNIPPETS + name, DBKEY_SNIPPETS + rename) == false) {
        fl_alert("%s\nsqlite: %s", "error: failed to rename snippet", _db.err_msg.c_str());
        return;
    }
    _names->text(r, rename.c_str());
    copy_label((std::string(TextDialog::LABEL) + " - Renamed " + name).c_str());
}
void TextDialog::resize(int X, int Y, int W, int H) {
    Fl_Double_Window::resize(X, Y, W, H);
    _grid->resize(0, 0, W, H);
}
std::string TextDialog::run(Fl_Window* parent) {
    _names->take_focus();
    TextDialog::Callback(_names, this);
    flw::util::center_window(this, parent);
    show();
    while (visible()) {
        Fl::wait();
        Fl::flush();
    }
    return _res;
}
void TextDialog::update_text() {
    auto r = _names->value();
    copy_label(TextDialog::LABEL);
    if (r < 1) {
        return;
    }
    auto name = gnu::str::to_string(_names->text(r));
    auto text = gnu::str::grab_string(_buffer->text());
    if (text.length() > FlEdit::MAX_SNIPPET_LENGTH) {
        fl_alert("error: text is too large (max %u bytes) (%u)", (unsigned) FlEdit::MAX_SNIPPET_LENGTH, (unsigned) text.length());
        return;
    }
    if (_db.put(DBKEY_SNIPPETS + name, text, text.length()) == false) {
        fl_alert("%s\nsqlite: %s", "error: failed to save snippet", _db.err_msg.c_str());
    }
    else {
        copy_label((std::string(TextDialog::LABEL) + " - Updated " + name).c_str());
    }
}
#define FLEDIT_CB1(X) [](Fl_Widget*, void* o) { static_cast<FlEdit*>(o)->X; static_cast<FlEdit*>(o)->update_menu(); }, this
#define FLEDIT_CB2(X,Y) [](Fl_Widget*, void* o) { static_cast<FlEdit*>(o)->X; static_cast<FlEdit*>(o)->Y; static_cast<FlEdit*>(o)->update_menu(); }, this
FlEdit*     FlEdit::SELF = nullptr;
fle::Config CONFIG;
FlEdit::FlEdit(int W, int H) : Fl_Double_Window(W, H, "flEdit"), Message(CONFIG) {
    end();
    FlEdit::SELF     = this;
    _dir_browser     = new DirBrowser();
    _findbar         = new fle::FindBar(CONFIG);
    _menu            = new Fl_Sys_Menu_Bar(0, 0, 0, 0);
    _output          = new CommandOutput();
    _split_edit      = new flw::SplitGroup();
    _split_main      = new flw::SplitGroup();
    _tabs.split      = new flw::SplitGroup();
    _tabs.tabs1      = new flw::TabsGroup();
    _tabs.tabs2      = new flw::TabsGroup();
    _tabs.active     = _tabs.tabs1;
    _editor          = nullptr;
    _tabs.pref1      = flw::TabsGroup::TABS::NORTH;
    _tabs.pref2      = flw::TabsGroup::TABS::NORTH;
    _project         = { "", "", };
    _paths.start_dir = gnu::File::WorkDir().filename;
    add(_menu);
    add(_split_main);
    add(_findbar);
    _dir_browser->hide();
    _dir_browser->callback(FlEdit::CallbackFileBrowser, this);
    _split_main->add(_dir_browser, flw::SplitGroup::CHILD::FIRST);
    _split_main->add(_split_edit, flw::SplitGroup::CHILD::SECOND);
    _split_main->direction(flw::SplitGroup::DIRECTION::VERTICAL);
    _split_main->split_pos(flw::PREF_FONTSIZE * 12);
    _split_edit->add(_tabs.split, flw::SplitGroup::CHILD::FIRST);
    _split_edit->add(_output, flw::SplitGroup::CHILD::SECOND);
    _split_edit->direction(flw::SplitGroup::DIRECTION::HORIZONTAL);
    _output->hide();
    _tabs.split->add(_tabs.tabs1, flw::SplitGroup::CHILD::FIRST);
    _tabs.split->add(_tabs.tabs2, flw::SplitGroup::CHILD::SECOND);
    _tabs.split->direction(flw::SplitGroup::DIRECTION::VERTICAL);
    _tabs.tabs1->tabs(_tabs.pref1);
    _tabs.tabs2->tabs(_tabs.pref2);
    _tabs.tabs1->hide();
    _tabs.tabs2->hide();
    _menu->global();
    _menu->add(MENU_FILE_NEW,                   FL_COMMAND + 'n',               FLEDIT_CB1(file_new("")));
    _menu->add(MENU_FILE_OPEN,                  FL_COMMAND + 'o',               FLEDIT_CB1(file_open()));
    _recent = new flw::RecentMenu(_menu, FlEdit::CallbackRecent, this);
    _menu->add(MENU_FILE_RELOAD,                0,                              FLEDIT_CB1(file_reload()));
    _menu->add(MENU_FILE_READONLY,              0,                              FLEDIT_CB1(file_readonly_mode()), FL_MENU_DIVIDER | FL_MENU_TOGGLE);
    _menu->add(MENU_FILE_SAVE,                  FL_COMMAND + 's',               FLEDIT_CB1(file_save()));
    _menu->add(MENU_FILE_SAVEAS,                0,                              FLEDIT_CB1(file_save_as()));
    _menu->add(MENU_FILE_SAVEALL,               0,                              FLEDIT_CB1(tabs_save_all(false)), FL_MENU_DIVIDER);
    _menu->add(MENU_FILE_CLOSE,                 FL_COMMAND + 'w',               FLEDIT_CB1(file_close()));
    _menu->add(MENU_FILE_CLOSEALL,              0,                              FLEDIT_CB1(file_close_all()), FL_MENU_DIVIDER);
    _menu->add(MENU_FILE_NEWWINDOW,             0,                              FLEDIT_CB1(new_window()), FL_MENU_DIVIDER);
    _menu->add(MENU_FILE_TERMINATE,             0,                              FLEDIT_CB1(file_terminate()));
    _menu->add(MENU_FILE_QUIT,                  FL_COMMAND + 'q',               FLEDIT_CB1(quit()));
    _menu->add(MENU_FIND_SHOW,                  FL_CTRL + 'f',                  FLEDIT_CB1(show_find()));
    _menu->add(MENU_FIND_LINES,                 FL_CTRL + 'j',                  FLEDIT_CB1(tabs_find_lines()), FL_MENU_DIVIDER);
    _menu->add(MENU_FIND_REPLACE,               FL_CTRL + FL_SHIFT + 'j',       FLEDIT_CB1(tabs_replace_all()));
    _menu->add(MENU_FIND_TRAILING,              0,                              FLEDIT_CB1(tabs_trailing_all()));
    _menu->add(MENU_TOOLS_CMDREPEAT,            FL_F + 8,                       FLEDIT_CB1(tools_run_command(true)));
    _menu->add(MENU_TOOLS_CMD,                  FL_SHIFT + FL_F + 8,            FLEDIT_CB1(tools_run_command(false)), FL_MENU_DIVIDER);
    _menu->add(MENU_TOOLS_NEXTOUTPUT,           FL_F + 10,                      FLEDIT_CB1(_output->list_next()));
    _menu->add(MENU_TOOLS_PREVOUTPUT,           FL_F + FL_SHIFT + 10,           FLEDIT_CB1(_output->list_prev()), FL_MENU_DIVIDER);
    _menu->add(MENU_TOOLS_CLEARTERMINAL,        0,                              FLEDIT_CB1(_output->reset_terminal(true)));
    _menu->add(MENU_TOOLS_CLEARLIST,            0,                              FLEDIT_CB1(_output->clear_list()));
    _menu->add(MENU_TOOLS_CLEAROUTPUT,          FL_F + FL_CTRL + FL_SHIFT + 10, FLEDIT_CB2(_output->reset_terminal(true), _output->clear_list()), FL_MENU_DIVIDER);
    _menu->add(MENU_TOOLS_SNIPPETS,             FL_F + 7,                       FLEDIT_CB1(project_snippets()));
    _menu->add(MENU_TOOLS_SAVE_CLIPBOARD,       0,                              FLEDIT_CB1(project_snippets_save(SNIPPET::CLIPBOARD)));
    _menu->add(MENU_TOOLS_SAVE_SELECTION,       0,                              FLEDIT_CB1(project_snippets_save(SNIPPET::SELECTION)));
    _menu->add(MENU_TOOLS_SAVE_TEXT,            0,                              FLEDIT_CB1(project_snippets_save(SNIPPET::TEXT)));
#ifndef __APPLE__
    _menu->add(MENU_VIEW_TOGGLEFULL,            FL_F + 11,                      FLEDIT_CB1(toggle_fullscreen()));
    _menu->add(MENU_VIEW_TOGGLEMENU,            FL_F + 11 + FL_SHIFT,           FLEDIT_CB1(toggle_menu()));
#endif
    _menu->add(MENU_VIEW_TOGGLETABS,            FL_F + 11 + FL_SHIFT + FL_CTRL, FLEDIT_CB1(toggle_tabs()));
    _menu->add(MENU_VIEW_TOGGLEBROWSER,         FL_CTRL + '5',                  FLEDIT_CB1(toggle_browser()));
    _menu->add(MENU_VIEW_TOGGLEOUTPUT,          FL_CTRL + '4',                  FLEDIT_CB1(toggle_output()), FL_MENU_DIVIDER);
    _menu->add(MENU_VIEW_TOGGLEONE,             FL_CTRL + FL_SHIFT + '1',       FLEDIT_CB1(toggle_one()));
    _menu->add(MENU_VIEW_TOGGLETWO,             FL_CTRL + FL_SHIFT + '2',       FLEDIT_CB1(toggle_two()));
    _menu->add(MENU_VIEW_ACTIVATEONE,           FL_CTRL + '1',                  FLEDIT_CB1(show_one()));
    _menu->add(MENU_VIEW_ACTIVATETWO,           FL_CTRL + '2',                  FLEDIT_CB1(show_two()), FL_MENU_DIVIDER);
    _menu->add(MENU_VIEW_MOVEGROUP,             FL_CTRL + '3',                  FLEDIT_CB1(tabs_move_editor()), FL_MENU_DIVIDER);
    _menu->add(MENU_VIEW_SORT_LEFT_TABS_ASC,    0,                              FLEDIT_CB1(tabs_sort(true, true)));
    _menu->add(MENU_VIEW_SORT_LEFT_TABS_DESC,   0,                              FLEDIT_CB1(tabs_sort(true, false)));
    _menu->add(MENU_VIEW_SORT_RIGHT_TABS_ASC,   0,                              FLEDIT_CB1(tabs_sort(false, true)));
    _menu->add(MENU_VIEW_SORT_RIGHT_TABS_DESC,  0,                              FLEDIT_CB1(tabs_sort(false, false)));
    _menu->add(MENU_PROJECT_LOAD,               0,                              FLEDIT_CB1(project_load()));
    _menu->add(MENU_PROJECT_SAVE  ,             0,                              FLEDIT_CB1(project_save()));
    _menu->add(MENU_PROJECT_SAVEAS,             0,                              FLEDIT_CB1(project_save_as()));
    _menu->add(MENU_PROJECT_DIR  ,              0,                              FLEDIT_CB1(project_dir()));
    _menu->add(MENU_PROJECT_CLOSE,              0,                              FLEDIT_CB1(project_close(true)));
    _menu->add(MENU_PROJECT_CLOSE2,             0,                              FLEDIT_CB1(project_close(false)), FL_MENU_DIVIDER);
    _menu->add(MENU_PROJECT_DB_OPEN,            0,                              FLEDIT_CB1(project_open_db()));
    _menu->add(MENU_PROJECT_DB_DEFRAG,          0,                              FLEDIT_CB1(project_defrag_db()));
    _menu->add(MENU_PROJECT_DB_CLOSE,           0,                              FLEDIT_CB1(project_close_db()));
    _menu->add(MENU_SETTINGS_THEME,             0,                              FLEDIT_CB1(settings_theme()));
    _menu->add(MENU_SETTINGS_EDITOR,            0,                              FLEDIT_CB1(settings_editor()));
    _menu->add(MENU_SETTINGS_SCHEME,            0,                              FLEDIT_CB1(settings_scheme()), FL_MENU_DIVIDER);
    _menu->add(MENU_SETTINGS_BACKUP,            0,                              FLEDIT_CB1(settings_backup()));
    _menu->add(MENU_SETTINGS_LOADPREF,          0,                              FLEDIT_CB1(settings_load_pref()));
    _menu->add(MENU_SETTINGS_SAVEPREF,          0,                              FLEDIT_CB1(settings_save_pref()), FL_MENU_DIVIDER);
    _menu->add(MENU_SETTINGS_SPLITVERTICAL,     0,                              FLEDIT_CB1(settings_split_vertical()), FL_MENU_RADIO);
    _menu->add(MENU_SETTINGS_SPLITHORIZONTAL,   0,                              FLEDIT_CB1(settings_split_horizontal()), FL_MENU_RADIO);
    _menu->add(MENU_SETTINGS_OUTPUT_SWAP,       0,                              FLEDIT_CB1(settings_output_swap()));
    _menu->add(MENU_SETTINGS_OUTPUT_VERTICAL,   0,                              FLEDIT_CB1(settings_output_vertical()), FL_MENU_RADIO);
    _menu->add(MENU_SETTINGS_OUTPUT_HORIZONTAL, 0,                              FLEDIT_CB1(settings_output_horizontal()), FL_MENU_RADIO | FL_MENU_DIVIDER);
    _menu->add(MENU_SETTINGS_OUTPUT_CLEAR,      0,                              nullptr, nullptr, FL_MENU_TOGGLE);
    _menu->add(MENU_SETTINGS_OUTPUT_UNKNOWN,    0,                              nullptr, nullptr, FL_MENU_TOGGLE);
    _menu->add(MENU_SETTINGS_TABG11,            0,                              FLEDIT_CB1(settings_tabg11()), FL_MENU_RADIO);
    _menu->add(MENU_SETTINGS_TABG12,            0,                              FLEDIT_CB1(settings_tabg12()), FL_MENU_RADIO);
    _menu->add(MENU_SETTINGS_TABG13,            0,                              FLEDIT_CB1(settings_tabg13()), FL_MENU_RADIO);
    _menu->add(MENU_SETTINGS_TABG14,            0,                              FLEDIT_CB1(settings_tabg14()), FL_MENU_RADIO);
    _menu->add(MENU_SETTINGS_TABG21,            0,                              FLEDIT_CB1(settings_tabg21()), FL_MENU_RADIO);
    _menu->add(MENU_SETTINGS_TABG22,            0,                              FLEDIT_CB1(settings_tabg22()), FL_MENU_RADIO);
    _menu->add(MENU_SETTINGS_TABG23,            0,                              FLEDIT_CB1(settings_tabg23()), FL_MENU_RADIO);
    _menu->add(MENU_SETTINGS_TABG24,            0,                              FLEDIT_CB1(settings_tabg24()), FL_MENU_RADIO);
    _menu->add(MENU_HELP_ABOUT,                 0,                              FLEDIT_CB1(help_about()), FL_MENU_DIVIDER);
    _menu->add(MENU_HELP_FLEDIT,                FL_F + 1,                       FLEDIT_CB1(help_fledit()));
    _menu->add(MENU_HELP_EDITOR,                0,                              FLEDIT_CB1(help_editor()));
    _menu->add(MENU_HELP_PCRE,                  0,                              FLEDIT_CB1(help_pcre()));
#ifdef DEBUG
    _menu->add(MENU_DEBUG,                      0,                              FLEDIT_CB1(debug()));
    _menu->add(MENU_DEBUG_SIZE,                 0,                              FLEDIT_CB1(debug_size()));
    _menu->add(MENU_DEBUG_COMPARE,              0,                              FLEDIT_CB1(debug_compare()));
#endif
    _recent->max_items(20);
    resizable(this);
    size_range(320, 240);
    flw::theme::load_icon(this, 666, icon_xpm, "flEdit");
    fl_message_title_default("flEdit");
    fl_message_hotspot(1);
    callback(FlEdit::CallbackWindow, this);
    tabs_check_empty();
    pref_load();
    split_view(SPLIT::SHOWONE);
    update_menu();
}
FlEdit::~FlEdit() {
    for (auto c : Command::COMMANDS) {
        delete c;
    }
    delete _recent;
}
void FlEdit::CallbackFileBrowser(Fl_Widget*, void*) {
    FlEdit::SELF->file_load(FlEdit::SELF->_editor, FlEdit::SELF->_dir_browser->file());
}
void FlEdit::CallbackList(Fl_Widget*, void*) {
    FlEdit::SELF->callback_list();
}
void FlEdit::callback_list() {
    auto  list = _output->list();
    auto& rx   = _output->list_rx();
    auto  row  = list->value();
    fl_message_position(this);
    if (row == 0) {
        return;
    }
    else if (rx.pattern() == "") {
        return;
    }
    else if (rx.is_compiled() == false) {
        fl_alert("error: regex expression!\n%s\n%s", _list_rx.error().c_str(), _list_rx.pattern().c_str());
        return;
    }
    auto text    = list->text(row);
    auto matches = rx.exec(text, strlen(text));
    if (matches > 1) {
        auto file = rx.substr("file");
        auto line = gnu::str::to_int(rx.substr("line"), 1);
        auto col  = gnu::str::to_int(rx.substr("col"), 1);
        if (file != "") {
            return tabs_activate_cursor(file, line, col);
        }
    }
    if (_editor != nullptr) {
        _editor->view().take_focus();
    }
}
void FlEdit::CallbackRecent(Fl_Widget*, void*) {
    FlEdit::SELF->file_load(FlEdit::SELF->_editor, FlEdit::SELF->_menu->text(), true);
    FlEdit::SELF->tabs_check_empty();
    FlEdit::SELF->do_layout();
}
void FlEdit::CallbackWindow(Fl_Widget*, void*) {
    if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) {
        return;
    }
    FlEdit::SELF->quit();
}
void FlEdit::debug() {
#ifdef DEBUG
    printf("\nFlEdit:\n");
    printf("Tabs1 has %02d editors, %s and is %s\n", _tabs.tabs1->children(), _tabs.active == _tabs.tabs1 ? "FOCUSED" : "UNFOCUSED", _tabs.tabs1->visible() ? "VISIBLE" : "HIDDEN");
    printf("Tabs2 has %02d editors, %s and is %s\n", _tabs.tabs2->children(), _tabs.active == _tabs.tabs2 ? "FOCUSED" : "UNFOCUSED", _tabs.tabs2->visible() ? "VISIBLE" : "HIDDEN");
    if (_editor != nullptr) {
        printf("_editor       = '%s'\n", _editor->filename().c_str());
        printf("_editor       = '%s'\n", &_editor->view() == _editor->view1() ? "SPLIT1" : "SPLIT2");
    }
    printf("start_dir     = '%s'\n", _paths.start_dir.c_str());
    printf("open_dir      = '%s'\n", _paths.open_dir.c_str());
    printf("_db           = '%s'\n", _db.filename().c_str());
    printf("_project.dir  = '%s'\n", _project.dir.c_str());
    printf("_project.name = '%s'\n", _project.name.c_str());
    auto tabindex = 0;
    auto editor   = tabs_editor_by_index(tabindex);
    while (editor != nullptr) {
        printf("file_%02d       = %p| %s\n", tabindex, editor, editor->filename().c_str());
        editor = tabs_editor_by_index(tabindex);
    }
    fflush(stdout);
#endif
}
void FlEdit::debug_compare() {
#ifdef DEBUG
    if (_editor != nullptr) {
        _editor->file_compare_buffer();
    }
#endif
}
int FlEdit::handle(int event) {
    if (event == FL_PASTE) {
        if (Fl::clipboard_contains(Fl::clipboard_plain_text) != 0) {
            project_snippets_save(SNIPPET::CLIPBOARD, gnu::str::to_string(Fl::event_text()));
            return 1;
        }
    }
    return Fl_Double_Window::handle(event);
}
void FlEdit::help_about() {
    std::string text = FLEDIT_ABOUT;
    auto tabindex = 0;
    auto editor   = tabs_editor_by_index(tabindex);
    size_t        B = 0;
    size_t        S = 0;
    size_t        U = 0;
    size_t        T = 0;
    size_t        C = 0;
    while (editor != nullptr) {
        size_t b = 0;
        size_t s = 0;
        size_t u = 0;
        T += editor->memory_usage(b, s, u);
        B += b;
        S += s;
        U += u;
        C += 1;
        editor = tabs_editor_by_index(tabindex);
    }
    text += gnu::str::format("Build info:\n");
    text += gnu::str::format("Date:   %s - %s\n", __DATE__, __TIME__);
#if __GNUC__
    text += gnu::str::format("GCC:    %s\n", __VERSION__);
#endif
    text += gnu::str::format("FLTK:   %d.%d.%d\n", FL_MAJOR_VERSION, FL_MINOR_VERSION, FL_PATCH_VERSION);
    text += gnu::str::format("SQLite: %s\n", gnu::DB::Version().c_str());
    text += gnu::str::format("PCRE:   %s\n", gnu::PCRE::Version().c_str());
    text += "\n";
    text += "Current session:\n";
    text += "Database name:       " + _db.filename() + "\n";
    text += "Project name:        " + _project.name + "\n";
    text += "Files:               " + gnu::str::format("%13s", gnu::str::format_int(C, '\'').c_str()) + "\n";
    text += "Buffer memory:       " + gnu::str::format("%13s", gnu::str::format_int(B, '\'').c_str()) + "\n";
    text += "Style memory:        " + gnu::str::format("%13s", gnu::str::format_int(S, '\'').c_str()) + "\n";
    if (CONFIG.pref_undo == fle::FUNDO::FLE) {
    text += "Undo capacity:       " + gnu::str::format("%13s", gnu::str::format_int(U, '\'').c_str()) + "\n";
    }
    text += "Total memory:        " + gnu::str::format("%13s", gnu::str::format_int(T, '\'').c_str()) + "\n";
    text += "\n";
    text += "Backup directory:    " + CONFIG.pref_backup.filename + "\n";
    text += "Start directory:     " + _paths.start_dir + "\n";
    text += "Project directory:   " + _project.dir + "\n";
    text += "Open file directory: " + _paths.open_dir + "\n";
    text += "Current directory:   " + gnu::File::WorkDir().filename + "\n";
    text += "\n";
    flw::dlg::list("About", text, this, true, 45, 55);
}
void FlEdit::help_fledit() {
    std::string text = FLEDIT_HELP;
    text += fle::help::flags(CONFIG);
    flw::dlg::list("Help", text, this, true, 60, 50);
}
void FlEdit::menu_enable(std::string path, bool enable) {
    auto menu = menu_get(path);
    if (menu == nullptr) {
        return;
    }
    if (enable == true) {
        menu->activate();
    }
    else {
        menu->deactivate();
    }
}
Fl_Menu_Item* FlEdit::menu_get(std::string path) {
    return (Fl_Menu_Item*) _menu->find_item(path.c_str());
}
fle::Message::CTRL FlEdit::message(const std::string& message, const std::string& s, const void* p) {
    if (message == fle::message::TEXT_CHANGED || message == fle::message::FILE_LOADED) {
        assert(p);
        auto editor = static_cast<fle::Editor*>(const_cast<void*>(p));
        editor_update_status(editor);
        update_menu();
    }
    else if (message == fle::message::EDITOR_FOCUS) {
        assert(p);
        assert(_tabs.active);
        _editor      = (fle::Editor*) p;
        _tabs.active = (flw::TabsGroup*) _editor->parent();
        update_menu();
    }
    else if (message == fle::message::DND_EVENT) {
        auto lines   = gnu::str::split(s, '\n');
        auto discard = static_cast<bool*>(const_cast<void*>(p));
        auto files   = std::vector<std::string>();
        *discard = true;
        for (auto line : lines) {
            gnu::str::replace(line, "file://");
            if (line != "") {
                auto file = gnu::File(line);
                if (file.is_file() == true) {
                    files.push_back(file.filename);
                }
            }
        }
        if (files.size() == 0) {
            *discard = false;
            return Message::CTRL::ABORT;
        }
        else {
            file_load_list(_editor, files);
            return Message::CTRL::ABORT;
        }
    }
    else if (message == fle::message::PREF_CHANGED) {
        update_pref();
    }
    else if (message == fle::message::HIDE_FIND) {
        _findbar->hide();
        _editor->take_focus();
        do_layout();
        Fl::redraw();
        return Message::CTRL::ABORT;
    }
    else if (message == fle::message::SHOW_FIND) {
        _findbar->show();
        do_layout();
        Fl::redraw();
        return Message::CTRL::ABORT;
    }
    return Message::CTRL::CONTINUE;
}
void FlEdit::new_window() {
    if (gnu::File::Run(executable, true) != 0) {
        fl_message_position(this);
        fl_alert("error: failed to open a new window!");
    }
}
bool FlEdit::quit() {
    if (Command::THREAD != nullptr && fl_choice("%s",  "Quit anyway", "&Wait", nullptr, "Command is still running....\nIt might continue to do that forever.\nIf so you must kill it manually!") == 1) {
        return false;
    }
    else if (project_close("DUMMY_MENU") == false) {
        return false;
    }
    pref_save();
    hide();
    return true;
}
void FlEdit::resize(int X, int Y, int W, int H) {
#ifdef DEBUG
#endif
#ifdef __APPLE__
    static bool v = false;
#else
    static bool v = true;
#endif
    Fl_Double_Window::resize(X, Y, W, H);
    if (W == _rect.w() && H == _rect.h()) {
        return;
    }
    auto fh = _findbar->height();
    if (_menu->visible() == 0 || v == false) {
        _split_main->resize(0, 0, W, H - fh);
    }
    else {
        _menu->resize(0, 0, W, flw::PREF_FONTSIZE * 2);
        _split_main->resize(0, flw::PREF_FONTSIZE * 2, W, H - flw::PREF_FONTSIZE * 2 - fh);
    }
    _findbar->resize(0, H - fh, W, fh);
    _rect = Fl_Rect(this);
    Fl::redraw();
#ifdef DEBUG
#endif
}
void FlEdit::settings_output_horizontal() {
    menu_get(MENU_SETTINGS_OUTPUT_HORIZONTAL)->set();
    _split_edit->direction(flw::SplitGroup::DIRECTION::HORIZONTAL);
    _split_edit->split_pos(h() - 200);
    _output->tabs(flw::TabsGroup::TABS::WEST);
    do_layout();
}
void FlEdit::settings_output_vertical() {
    menu_get(MENU_SETTINGS_OUTPUT_VERTICAL)->set();
    _split_edit->direction(flw::SplitGroup::DIRECTION::VERTICAL);
    _split_edit->split_pos(w() - 200);
    _output->tabs(flw::TabsGroup::TABS::NORTH);
    do_layout();
}
void FlEdit::split_view(SPLIT value) {
    bool r = false;
    if (_tabs.split->visible() == 0) {
        return;
    }
    else if (value == SPLIT::SHOWONE || value == SPLIT::SHOWTWO) {
        auto other = _tabs.tabs2;
        if (value == SPLIT::SHOWONE) {
            _tabs.active = _tabs.tabs1;
        }
        else {
            _tabs.active = _tabs.tabs2;
            other        = _tabs.tabs1;
        }
        if (_tabs.active->visible() == false) {
            r = true;
            _tabs.active->show();
            if (other->visible() != 0) {
                _tabs.split->split_pos(-1);
            }
        }
        _tabs.active->take_focus();
    }
    else if (value == SPLIT::HIDEONE) {
        if (_tabs.tabs1->visible() != 0 && _tabs.tabs2->visible() != 0) {
            _tabs.tabs1->hide();
            _tabs.active = _tabs.tabs2;
            r = true;
        }
    }
    else if (value == SPLIT::HIDETWO) {
        if (_tabs.tabs2->visible() != 0 && _tabs.tabs1->visible() != 0) {
            _tabs.tabs2->hide();
            _tabs.active = _tabs.tabs1;
            r = true;
        }
    }
    else if (value == SPLIT::VER && _tabs.split->direction() == flw::SplitGroup::DIRECTION::HORIZONTAL) {
        _tabs.split->direction(flw::SplitGroup::DIRECTION::VERTICAL);
        r = true;
    }
    else if (value == SPLIT::HOR && _tabs.split->direction() == flw::SplitGroup::DIRECTION::VERTICAL) {
        _tabs.split->direction(flw::SplitGroup::DIRECTION::HORIZONTAL);
        r = true;
    }
    if (r == true) {
        do_layout();
    }
    else {
        redraw();
    }
}
void FlEdit::tools_run_command(bool repeat) {
    auto filename  = (_editor != nullptr) ? _editor->filename() : std::string();
    auto selection = (_editor != nullptr) ? _editor->text_get_selection_string() : std::string();
    _output->run_command(_project.dir, filename, selection, repeat);
}
void FlEdit::toggle_browser() {
    if (_project.dir == "") {
        _dir_browser->hide();
        _dir_browser->load_root("");
    }
    else {
        if (_dir_browser->visible() == 0) {
            _dir_browser->show();
        }
        else {
            _dir_browser->hide();
        }
        if (_dir_browser->root() == "") {
            _dir_browser->load_root(_project.dir);
        }
    }
    do_layout();
}
void FlEdit::update_menu() {
    gnu::File   file;
    std::string changed_name;
    std::string path;
    if (_editor != nullptr) {
        file         = _editor->filename();
        changed_name = _editor->file_changed_name();
        path         = _editor->filepath();
    }
    if (changed_name != "") {
        if (file.is_file() == true) {
            _paths.open_dir = file.path;
        }
        if (_editor->text_is_readonly() == true) {
            menu_enable(MENU_FILE_SAVE, false);
            menu_enable(MENU_FILE_SAVEAS, false);
        }
        else {
            menu_enable(MENU_FILE_SAVE);
            menu_enable(MENU_FILE_SAVEAS);
        }
        menu_enable(MENU_FILE_CLOSE);
        menu_enable(MENU_FILE_RELOAD);
        menu_enable(MENU_FILE_READONLY);
        menu_enable(MENU_VIEW_MOVEGROUP);
        menu_get(MENU_FILE_READONLY)->value(_editor->text_is_readonly());
    }
    else {
        menu_enable(MENU_FILE_CLOSE, true);
        menu_enable(MENU_FILE_SAVE, false);
        menu_enable(MENU_FILE_SAVEAS, false);
        menu_enable(MENU_FILE_RELOAD, false);
        menu_enable(MENU_FILE_READONLY, false);
        menu_enable(MENU_VIEW_MOVEGROUP, false);
        menu_get(MENU_FILE_READONLY)->value(0);
    }
    menu_enable(MENU_FILE_SAVEALL, tabs_count());
    menu_enable(MENU_FILE_CLOSEALL, tabs_count());
    menu_enable(MENU_FIND_LINES, tabs_count());
    menu_enable(MENU_FIND_REPLACE, tabs_count());
    menu_enable(MENU_FIND_TRAILING, tabs_count());
    if (_db.is_open() == true) {
        menu_enable(MENU_PROJECT_DB_OPEN, false);
        menu_enable(MENU_PROJECT_DB_CLOSE);
        menu_enable(MENU_PROJECT_DB_DEFRAG);
        menu_enable(MENU_PROJECT_LOAD);
        menu_enable(MENU_PROJECT_SAVEAS);
        menu_enable(MENU_TOOLS_SNIPPETS);
        menu_enable(MENU_TOOLS_SAVE_CLIPBOARD);
        menu_enable(MENU_TOOLS_SAVE_SELECTION);
        menu_enable(MENU_TOOLS_SAVE_TEXT);
    }
    else {
        menu_enable(MENU_PROJECT_DB_OPEN);
        menu_enable(MENU_PROJECT_DB_CLOSE, false);
        menu_enable(MENU_PROJECT_DB_DEFRAG, false);
        menu_enable(MENU_PROJECT_LOAD, false);
        menu_enable(MENU_PROJECT_SAVEAS, false);
        menu_enable(MENU_TOOLS_SNIPPETS, false);
        menu_enable(MENU_TOOLS_SAVE_CLIPBOARD, false);
        menu_enable(MENU_TOOLS_SAVE_SELECTION, false);
        menu_enable(MENU_TOOLS_SAVE_TEXT, false);
    }
    if (changed_name == "**") {
        changed_name = "*untitled*";
    }
    if (_project.name != "") {
        menu_enable(MENU_PROJECT_SAVE);
        menu_enable(MENU_PROJECT_CLOSE);
        menu_enable(MENU_PROJECT_CLOSE2);
        menu_enable(MENU_PROJECT_DIR);
        menu_enable(MENU_VIEW_TOGGLEBROWSER, _project.dir != "");
        if (changed_name != "") {
            copy_label(std::string("flEdit [" + _project.name + "] - " + changed_name + " - " + path).c_str());
        }
        else {
            copy_label(std::string("flEdit [" + _project.name + "]").c_str());
        }
    }
    else {
        menu_enable(MENU_PROJECT_SAVE, false);
        menu_enable(MENU_PROJECT_CLOSE, false);
        menu_enable(MENU_PROJECT_CLOSE2, false);
        menu_enable(MENU_PROJECT_DIR, false);
        menu_enable(MENU_VIEW_TOGGLEBROWSER, false);
        if (changed_name != "") {
            copy_label(std::string("flEdit - " + changed_name + " - " + path).c_str());
        }
        else {
            copy_label("flEdit");
        }
    }
    if (_tabs.tabs1->visible() != 0 && _tabs.tabs2->visible() == 0) {
        menu_enable(MENU_VIEW_TOGGLEONE, false);
    }
    else if (_tabs.tabs1->visible() == 0 && _tabs.tabs2->visible() != 0) {
        menu_enable(MENU_VIEW_TOGGLETWO, false);
    }
    else {
        menu_enable(MENU_VIEW_TOGGLEONE);
        menu_enable(MENU_VIEW_TOGGLETWO);
    }
}
void FlEdit::update_pref() {
    _output->update_pref();
    _findbar->update_pref();
    _dir_browser->update_pref();
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
    _tabs.split->min_pos(flw::PREF_FONTSIZE * 14);
    _tabs.tabs1->update_pref(12);
    _tabs.tabs2->update_pref(12);
    FlEdit::do_layout();
    Fl::redraw();
}
bool FlEdit::editor_close(fle::Editor* editor, bool ask) {
    if (editor == nullptr) return true;
    if (editor->text_is_dirty() == true && ask == ASK_SAVE) {
        tabs_activate(editor);
       auto answer = 0;
        if (editor->filename() == "") {
            answer = fl_choice("%s",  "Cancel", "&Save", "Close without saving", "New file is unsaved");
        }
        else {
            answer = fl_choice("%s",  "Cancel", "&Save", "Close without saving", (editor->filename() + " has been changed").c_str());
        }
        if (answer == 0) {
            return false;
        }
        else if (answer == 1 && file_save(editor) == false) {
            return false;
        }
    }
    tabs_delete(editor);
    return true;
}
void FlEdit::editor_set_style(fle::Editor* editor, std::string override_style) {
    if (editor == nullptr) return;
    if (override_style != "") {
        editor->style_from_language(override_style);
    }
    else {
        editor->style_from_filename();
    }
}
void FlEdit::editor_update_status(fle::Editor* editor) {
    if (editor == nullptr) {
        return;
    }
    auto tabs = (flw::TabsGroup*) editor->parent();
    if (tabs == nullptr) {
        return;
    }
    auto name      = editor->file_changed_name();
    auto do_resize = (name == "**") ? true : false;
    if (name == "**") {
        name = "*untitled*";
    }
    tabs->label(name, editor);
    if (do_resize == true) {
        _tabs.active->do_layout();
    }
}
void FlEdit::file_close() {
    editor_close(_editor, ASK_SAVE);
    tabs_check_empty();
    _tabs.split->do_layout();
}
void FlEdit::file_close_all() {
    tabs_save_visibility();
    if (tabs_save_all(true) == true) {
        tabs_close_all();
        tabs_check_empty();
        tabs_restore_visibility();
        _tabs.split->do_layout();
    }
}
fle::Editor* FlEdit::file_load(Fl_Widget* after, std::string filename, bool add_recent, int line) {
    fl_message_position(this);
    auto fi     = gnu::File(filename, true);
    auto editor = tabs_editor_by_path(fi.filename);
    filename = fi.filename;
    if (fi.is_dir() == true) {
        return nullptr;
    }
    else if (editor != nullptr) {
        tabs_activate(editor);
        return editor;
    }
    editor = new fle::Editor(CONFIG, _findbar, _tabs.active->x(), _tabs.active->y(), _tabs.active->w(), _tabs.active->h());
    auto err = editor->file_load(filename);
    if (err != "") {
        fl_alert("%s", err.c_str());
        delete editor;
        return nullptr;
    }
    if (add_recent == true) {
        _recent->insert(fi.filename);
    }
    if (line > 0) {
        editor->cursor_move_to_rowcol(line, 1);
    }
    _tabs.active->add(editor->file_changed_name(), editor, after);
    editor_set_style(editor);
    editor_update_status(editor);
    return editor;
}
std::vector<std::string> FlEdit::file_load_dialog() {
    auto fc  = Fl_File_Chooser(_paths.open_dir.c_str(), fle::style::FILE_FILTER, Fl_File_Chooser::MULTI, "Select Files");
    auto res = std::vector<std::string>();
    fc.show();
    while (fc.visible() != 0) {
        Fl::wait();
        Fl::flush();
    }
    for (int f = 1; f <= fc.count(); f++) {
        res.push_back(fc.value(f));
    }
    return res;
}
void FlEdit::file_load_list(Fl_Widget* after, std::vector<std::string> filenames, std::vector<int> filelines) {
#ifdef DEBUG
    auto time = gnu::Time::Milli();
#endif
    auto wc = flw::WaitCursor();
    _tabs.split->hide();
    for (size_t f = 0; f < filenames.size(); f++) {
        file_load(f == 0 ? after : _editor, filenames[f], true, (filelines.size() > f) ? filelines[f] : 0);
    }
    tabs_check_empty();
    _tabs.split->show();
    do_layout();
#ifdef DEBUG
    printf("loaded %d files in %d mS\n", (int) filenames.size(), (int) (gnu::Time::Milli() - time));
    fflush(stdout);
#endif
}
void FlEdit::file_new(std::string filename) {
    auto editor = new fle::Editor(CONFIG, _findbar);
    _tabs.active->add(editor->file_changed_name(), editor, _editor);
    if (filename != "") {
        editor->file_set_new_filename(filename);
    }
    editor_set_style(editor);
    editor_update_status(editor);
    editor->view().take_focus();
    _tabs.split->do_layout();
}
void FlEdit::file_readonly_mode() {
    if (_editor != nullptr) {
        _editor->text_set_readonly(menu_get(MENU_FILE_READONLY)->value());
    }
}
void FlEdit::file_reload(fle::Editor* editor) {
    if (editor != nullptr) {
        auto info = std::string("Would you like to reload file ") + editor->filename() + " ?";
        if (fl_choice("%s", nullptr, "&No", "&Yes", info.c_str()) == 2) {
            fle::CursorPos pos = editor->cursor(true);
            editor->file_load(editor->filename());
            editor->cursor_move(pos);
            editor_set_style(editor);
            editor_update_status(editor);
        }
    }
}
bool FlEdit::file_save(fle::Editor* editor) {
    if (editor == nullptr || editor->text_is_dirty() == false) {
        return true;
    }
    if (editor->filename() == "") {
        if (_editor != editor) {
            tabs_activate(editor);
        }
        return file_save_as(editor);
    }
    auto err = editor->file_save();
    if (err != "") {
        fl_message_position(this);
        fl_alert("%s", err.c_str());
        if (_editor != editor) {
            tabs_activate(editor);
        }
        return false;
    }
    editor->update_autocomplete();
    editor_update_status(editor);
    _recent->insert(editor->filename());
    return true;
}
bool FlEdit::file_save_as(fle::Editor* editor) {
    if (editor == nullptr) {
        return true;
    }
    fl_message_position(this);
    auto filename = gnu::str::to_string(fl_file_chooser("Save File As", fle::style::FILE_FILTER, editor->filepath() != "" ? editor->filename().c_str() : _paths.open_dir.c_str()));
    auto new_file = gnu::File(filename, true);
    if (new_file.filename == "" || new_file.filename == editor->filename() || new_file.is_other() == true) {
        return false;
    }
    else if (new_file.is_dir() == true) {
        fl_alert("%s\n%s", "error: destination is a directory!", filename.c_str());
        return false;
    }
    else if (tabs_editor_by_path(new_file.filename) != nullptr) {
        fl_alert("%s\n%s", "error: file with this name already opened!", filename.c_str());
        return false;
    }
    auto err = editor->file_save_as(new_file.filename);
    if (err != "") {
        fl_alert("%s", err.c_str());
        return false;
    }
    editor_set_style(editor);
    editor_update_status(editor);
    _recent->insert(editor->filename());
    return true;
}
void FlEdit::pref_load(bool all) {
    auto pref = Fl_Preferences(Fl_Preferences::USER_L, USER_NAME, "fledit");
    auto val  = 0;
    if (all == true) {
        std::string s;
        flw::theme::load_theme_pref(pref);
        flw::theme::load_win_pref(pref, this, 2);
        pref.get("gui.path", s, "");
        gnu::File file(s);
        if (file.is_dir() == true) {
            _paths.open_dir = file.filename;
        }
        else {
            _paths.open_dir = gnu::File::WorkDir().filename;
        }
        pref.get("gui.split", val, (int) flw::SplitGroup::DIRECTION::VERTICAL);
        if (val == (int) flw::SplitGroup::DIRECTION::HORIZONTAL) {
            _tabs.split->direction(flw::SplitGroup::DIRECTION::HORIZONTAL);
            menu_get(MENU_SETTINGS_SPLITHORIZONTAL)->set();
        }
        else {
            _tabs.split->direction(flw::SplitGroup::DIRECTION::VERTICAL);
            menu_get(MENU_SETTINGS_SPLITVERTICAL)->set();
        }
        pref.get("gui.output", val, (int) flw::SplitGroup::DIRECTION::HORIZONTAL);
        if (val == (int) flw::SplitGroup::DIRECTION::HORIZONTAL) {
            _split_edit->direction(flw::SplitGroup::DIRECTION::HORIZONTAL);
            _split_edit->split_pos(h() - 200);
            menu_get(MENU_SETTINGS_OUTPUT_HORIZONTAL)->set();
        }
        else {
            _split_edit->direction(flw::SplitGroup::DIRECTION::VERTICAL);
            _split_edit->split_pos(w() - 200);
            menu_get(MENU_SETTINGS_OUTPUT_VERTICAL)->set();
        }
        pref.get("gui.output.clear", val, (int) 0);
        if (val != 0) menu_get(MENU_SETTINGS_OUTPUT_CLEAR)->set(); else menu_get(MENU_SETTINGS_OUTPUT_CLEAR)->clear();
        pref.get("gui.output.show", val, (int) 0);
        if (val != 0) menu_get(MENU_SETTINGS_OUTPUT_UNKNOWN)->set(); else menu_get(MENU_SETTINGS_OUTPUT_UNKNOWN)->clear();
        pref.get("gui.tabspos1", val, 0);
        pref_set_tabspos1(val);
        pref.get("gui.tabspos2", val, 0);
        pref_set_tabspos2(val);
        pref.get("project.db", s, "");
        if (s != "") {
            _db.open(s);
        }
        fle::style::reset_all_styles();
        fle::style::load_pref(pref);
        fle::KeyConf::LoadPref(pref);
        CONFIG.load_pref(pref, &_findbar->findreplace());
    }
    else {
        flw::theme::load_theme_pref(pref);
        fle::style::reset_all_styles();
        fle::style::load_pref(pref);
        fle::KeyConf::LoadPref(pref);
        CONFIG.load_pref(pref, &_findbar->findreplace());
    }
    _recent->load_pref(pref);
    _findbar->findreplace().update_lists();
    val = 0;
    Command::COMMANDS.clear();
    while (true) {
        std::string name;
        std::string command;
        std::string workdir;
        std::string filter;
        std::string line;
        std::string s;
        auto output = Command::RUN;
        pref.get(gnu::str::format("cmd_name%d", val).c_str(), s, "");
        if (s == "") {
            break;
        }
        else {
            name = s;
        }
        pref.get(gnu::str::format("cmd_run%d", val).c_str(), s, "");
        command = s;
        pref.get(gnu::str::format("cmd_capture%d", val).c_str(), s, "");
        if (s == "1") {
            output = Command::CAPTURE_LIST;
        }
        else if (s == "2") {
            output = Command::CAPTURE_TERMINAL;
        }
        else if (s == "3") {
            output = Command::STREAM_TERMINAL;
        }
        pref.get(gnu::str::format("cmd_directory%d", val).c_str(), s, "");
        workdir = s;
        pref.get(gnu::str::format("cmd_filter%d", val).c_str(), s, "");
        filter = s;
        pref.get(gnu::str::format("cmd_line%d", val).c_str(), s, "");
        line = s;
        Command::COMMANDS.push_back(new Command(name, command, output, workdir, filter, line));
        val++;
    }
    CONFIG.send_message(fle::message::PREF_CHANGED);
}
void FlEdit::pref_save() {
    auto pref = Fl_Preferences(Fl_Preferences::USER_L, USER_NAME, "fledit");
    pref.clear();
    pref.set("gui.path", _paths.open_dir);
    pref.set("gui.split", (int) _tabs.split->direction());
    pref.set("gui.output", (int) _split_edit->direction());
    pref.set("gui.output.clear", FlEdit::SettingsClearTerminal());
    pref.set("gui.output.show", FlEdit::settingsShowUnknown());
    pref.set("gui.tabspos1", (int) _tabs.pref1);
    pref.set("gui.tabspos2", (int) _tabs.pref2);
    flw::theme::save_theme_pref(pref);
    flw::theme::save_win_pref(pref, this);
    CONFIG.save_pref(pref, &_findbar->findreplace());
    fle::style::save_pref(pref);
	fle::KeyConf::SavePref(pref);
    _recent->save_pref(pref);
    if (_db.is_open() == true) {
        pref.set("project.db", _db.filename());
    }
    auto c = 0;
    for (const auto& command : Command::COMMANDS) {
        pref.set(gnu::str::format("cmd_name%d", c).c_str(), command->name);
        pref.set(gnu::str::format("cmd_run%d", c).c_str(), command->command);
        pref.set(gnu::str::format("cmd_directory%d", c).c_str(), command->workdir);
        pref.set(gnu::str::format("cmd_filter%d", c).c_str(), command->filter_regex);
        pref.set(gnu::str::format("cmd_line%d", c).c_str(), command->line_regex);
        if (command->output == Command::CAPTURE_LIST) {
            pref.set(gnu::str::format("cmd_capture%d", c).c_str(), "1");
        }
        else if (command->output == Command::CAPTURE_TERMINAL) {
            pref.set(gnu::str::format("cmd_capture%d", c).c_str(), "2");
        }
        else if (command->output == Command::STREAM_TERMINAL) {
            pref.set(gnu::str::format("cmd_capture%d", c).c_str(), "3");
        }
        else {
            pref.set(gnu::str::format("cmd_capture%d", c).c_str(), "0");
        }
        c++;
    }
}
void FlEdit::pref_set_tabspos1(int value) {
    switch (value) {
        case 1:  _tabs.pref1 = flw::TabsGroup::TABS::SOUTH; menu_get(MENU_SETTINGS_TABG12)->set(); break;
        case 2:  _tabs.pref1 = flw::TabsGroup::TABS::WEST;  menu_get(MENU_SETTINGS_TABG13)->set(); break;
        case 3:  _tabs.pref1 = flw::TabsGroup::TABS::EAST;  menu_get(MENU_SETTINGS_TABG14)->set(); break;
        default: _tabs.pref1 = flw::TabsGroup::TABS::NORTH; menu_get(MENU_SETTINGS_TABG11)->set(); break;
    }
    _tabs.tabs1->tabs(_tabs.pref1);
    do_layout();
}
void FlEdit::pref_set_tabspos2(int value) {
    switch (value) {
        case 1:  _tabs.pref2 = flw::TabsGroup::TABS::SOUTH; menu_get(MENU_SETTINGS_TABG22)->set(); break;
        case 2:  _tabs.pref2 = flw::TabsGroup::TABS::WEST;  menu_get(MENU_SETTINGS_TABG23)->set(); break;
        case 3:  _tabs.pref2 = flw::TabsGroup::TABS::EAST;  menu_get(MENU_SETTINGS_TABG24)->set(); break;
        default: _tabs.pref2 = flw::TabsGroup::TABS::NORTH; menu_get(MENU_SETTINGS_TABG21)->set(); break;
    }
    _tabs.tabs2->tabs(_tabs.pref2);
    do_layout();
}
bool FlEdit::project_close(bool save) {
    if (tabs_save_all(true) == false) {
        return false;
    }
    if (_db.is_open() == true && _project.name != "") {
        if (gnu::File::ChDir(_paths.start_dir) == false) {
            fl_alert("error: failed to restore original work directory to %s", _paths.start_dir.c_str());
        }
        if (save == true && project_save(_project.name) == false) {
            return false;
        }
        _project = { "", "", };
        tabs_close_all();
        split_view(SPLIT::HIDETWO);
        split_view(SPLIT::SHOWONE);
        tabs_check_empty();
        _dir_browser->hide();
        _dir_browser->load_root("");
        CONFIG.find_list = _old_find_list;
        CONFIG.replace_list = _old_replace_list;
        _findbar->findreplace().update_lists();
        do_layout();
    }
    return true;
}
bool FlEdit::project_close_db() {
    if (project_close(true) == false) {
        return false;
    }
    _db.close();
    return true;
}
void FlEdit::project_defrag_db() {
    if (_db.defrag() == false) {
        fl_message_position(this);
        fl_alert("%s\nsqlite: %s", "error: could defrag database file", _db.err_msg.c_str());
    }
}
void FlEdit::project_dir() {
    auto start_dir = _project.dir;
    if (start_dir == "") {
        if (_editor != nullptr) {
            start_dir = _editor->filepath();
        }
        else {
            start_dir = _paths.start_dir;
        }
    }
    auto path = fl_dir_chooser("Select Project Directory - Press Cancel To Remove Current", start_dir.c_str());
    if (path == nullptr) {
        _project.dir = "";
        _dir_browser->hide();
        _dir_browser->load_root("");
        do_layout();
        if (gnu::File::ChDir(_paths.start_dir) == false) {
            fl_alert("error: failed to restore previous work directory to %s",  _paths.start_dir.c_str());
        }
    }
    else {
        _project.dir = gnu::File(path).filename;
        if (_dir_browser->visible() != 0) {
            _dir_browser->load_root(path);
        }
        if (gnu::File::ChDir(_project.dir) == false) {
            fl_alert("error: failed to change work directory to %s", _project.dir.c_str());
        }
    }
}
bool FlEdit::project_exist(std::string name) {
    if (_db.is_open() == false) {
        return false;
    }
    return _db.key(DBKEY_PROJECTS + name);
}
void FlEdit::project_load() {
    auto dialog = ProjectDialog(_db);
    auto name   = dialog.run(this);
    if (name == "") {
        return;
    }
    if (tabs_save_all(true) == false) {
        return;
    }
    if (_project.name == "") {
        tabs_close_all();
    }
    else if (project_save() == true) {
        tabs_close_all();
    }
    else {
        return;
    }
    _dir_browser->hide();
    _dir_browser->load_root("");
    project_load_from_db(name);
    tabs_check_empty();
    do_layout();
    tabs_reset_split_size();
}
void FlEdit::project_load_from_db(std::string name) {
    auto wc   = flw::WaitCursor();
    auto time = gnu::Time::Milli();
    fl_message_position(this);
    if (gnu::File::ChDir(_paths.start_dir) == false) {
        fl_alert("error: failed to restore original work directory to %s", _paths.start_dir.c_str());
    }
    if (_db.is_open() == false) {
        fl_alert("%s", "error: database is closed");
        return;
    }
    auto row = _db.get(DBKEY_PROJECTS + name);
    if (row.value == nullptr) {
        fl_alert("%s\nsqlite: %s", "error: failed to load project", _db.err_msg.c_str());
        return;
    }
    auto pile = gnu::Pile(row.value);
    if (pile.size() == 0) {
        fl_alert("%s", "error: no data found");
        return;
    }
    _old_find_list    = CONFIG.find_list;
    _old_replace_list = CONFIG.replace_list;
    CONFIG.find_list.clear();
    CONFIG.replace_list.clear();
    for (auto& s : project_load_list("find", pile)) {
        CONFIG.add_find_word(s, true);
    }
    for (auto& s : project_load_list("replace", pile)) {
        CONFIG.add_replace_word(s, true);
    }
    _findbar->findreplace().update_lists();
    _tabs.tabs1->hide();
    _tabs.tabs2->hide();
    _tabs.split->hide();
    auto files = pile.get_int("gui", "files");
    for (auto f = 1; f <= files; f++) {
        auto section = pile.make_key(f);
        _tabs.active = (pile.get_string(section, "tabs") == "right") ? _tabs.tabs2 : _tabs.tabs1;
        auto filename = pile.get_string(section, "path");
        auto editor   = file_load(nullptr, filename, false, 0);
        if (editor != nullptr) {
            auto fletcher64 = (uint64_t) pile.get_int(section, "checksum");
            auto split      = pile.get_int(section, "split");
            auto cursor     = fle::CursorPos();
            if (split == (int) fle::FSPLITVIEW::HORIZONTAL) {
                editor->view_set_split(fle::FSPLITVIEW::HORIZONTAL);
            }
            else if (split == (int) fle::FSPLITVIEW::VERTICAL) {
                editor->view_set_split(fle::FSPLITVIEW::VERTICAL);
            }
            cursor.pos1  = pile.get_int(section, "cursor1");
            cursor.top1  = pile.get_int(section, "top1");
            cursor.pos2  = (editor->view2() != nullptr) ? pile.get_int(section, "cursor2") : -1;
            cursor.top2  = (editor->view2() != nullptr) ? pile.get_int(section, "top2") : -1;
            cursor.drag  = pile.get_int(section, "drag");
            cursor.start = pile.get_int(section, "start");
            cursor.end   = pile.get_int(section, "end");
            editor->view1()->take_focus();
            editor->wrap_set_mode(pile.get_int(section, "wrap") ? fle::FWRAP::YES : fle::FWRAP::NO);
            editor_set_style(editor, pile.get_string(section, "style", "Text"));
            auto tab_mode  = pile.get_int(section, "tab_mode", -1);
            auto tab_width = pile.get_int(section, "tab_width", -1);
            if (tab_mode != -1) {
                editor->text_tab_mode(tab_mode == 0 ? fle::FTAB::HARD : fle::FTAB::SOFT);
            }
            if (tab_width != -1) {
                editor->text_tab_width(tab_width);
            }
            if (editor->file_fletcher64() == fletcher64) {
                editor->cursor_move(cursor);
                editor->bookmarks() = fle::Bookmarks(editor, pile.get_string(section, "bookmarks"));
            }
            editor->cursor_save();
        }
    }
    _tabs.split->show();
    time = gnu::Time::Milli() - time;
    auto left  = pile.get_int("gui", "left", 0);
    auto right = pile.get_int("gui", "right", 0);
    if (left == 0 && right == 1) {
        split_view(SPLIT::SHOWTWO);
    }
    else if (left == 1 && right == 1) {
        split_view(SPLIT::SHOWONE);
        split_view(SPLIT::SHOWTWO);
    }
    else {
        split_view(SPLIT::SHOWONE);
    }
    auto editor = tabs_editor_by_path(pile.get_string("active", "right"));
    if (editor != nullptr) {
        tabs_activate(editor);
    }
    editor = tabs_editor_by_path(pile.get_string("active", "left"));
    if (editor != nullptr) {
        tabs_activate(editor);
    }
    _project.name = name;
    _project.dir  = pile.get_string("gui", "dir");
    if (gnu::File(_project.dir).is_dir() == false) {
        _project.dir = "";
    }
    update_menu();
    if (_project.dir != "" && gnu::File::ChDir(_project.dir) == false) {
        fl_alert("error: failed to restore project work directory to %s", _project.dir.c_str());
    }
#ifdef DEBUG
    printf("loaded project %s in %d mS with %d files\n", _project.name.c_str(), (int) time, (int) files);
    fflush(stdout);
#endif
}
std::vector<std::string> FlEdit::project_load_list(std::string key, gnu::Pile& pile) {
    auto count = 1;
    auto res   = std::vector<std::string>();
    while (true) {
        std::string s = pile.get_string(key, pile.make_key(count++));
        if (s == "") {
            break;
        }
        else {
            res.push_back(s);
        }
    }
    return res;
}
void FlEdit::project_open_db() {
    fl_message_position(this);
    auto filename = gnu::str::to_string(fl_file_chooser("Open/Create Project Database", "FlEdit Project Files (*.fledit)", _paths.open_dir.c_str()));
    if (filename == "") {
        return;
    }
    gnu::File file(filename);
    if (file.is_missing() == true && file.ext == "") {
        file += ".fledit";
    }
    _db = gnu::DB(file.filename);
    if (_db.is_open() == false) {
        fl_alert("%s\nsqlite: %s", "error: could not open database file", _db.err_msg.c_str());
    }
}
bool FlEdit::project_save(std::string name) {
    fl_message_position(this);
    if (_db.is_open() == false) {
        fl_alert("%s", "error: database is closed");
        return false;
    }
    auto pile = gnu::Pile();
    if (_tabs.tabs1->visible() != 0) {
        pile.set_int("gui", "left", 1);
    }
    if (_tabs.tabs2->visible() != 0) {
        pile.set_int("gui", "right", 1);
    }
    project_save_list("find", CONFIG.find_list, pile);
    project_save_list("replace", CONFIG.replace_list, pile);
    auto count    = 1;
    auto tabindex = 0;
    auto editor   = tabs_editor_by_index(tabindex);
    while (editor != nullptr) {
        if (editor->filename() != "") {
            auto cursor  = (editor->text_is_dirty() == true) ? editor->cursor_saved() : editor->cursor(true);
            auto section = pile.make_key(count);
            cursor.to_default();
            pile.set_string(section, "path", editor->filename());
            pile.set_int(section, "checksum", editor->file_fletcher64());
            pile.set_string(section, "tabs", editor->parent() == _tabs.tabs1 ? "left" : "right");
            pile.set_int(section, "split", (int) editor->view_split());
            pile.set_string(section, "style", editor->style().name());
            pile.set_int(section, "wrap", editor->wrap_mode() == fle::FWRAP::YES ? 1 : 0);
            pile.set_string(section, "bookmarks", editor->bookmarks().tostring());
            if (editor->text_tab_width() != editor->style().tab_width()) {
                pile.set_int(section, "tab_width", editor->text_tab_width());
            }
            if (editor->text_tab_mode() != editor->style().tab_mode()) {
                pile.set_int(section, "tab_mode", (editor->text_tab_mode() == fle::FTAB::HARD) ? 0 : 1);
            }
            pile.set_int(section, "cursor1", cursor.pos1);
            pile.set_int(section, "top1", cursor.top1);
            pile.set_int(section, "cursor2", cursor.pos2);
            pile.set_int(section, "top2", cursor.top2);
            pile.set_int(section, "drag", cursor.drag);
            pile.set_int(section, "start", cursor.start);
            pile.set_int(section, "end", cursor.end);
            count++;
            if (_tabs.tabs1->value() == editor) {
                pile.set_string("active", "left", editor->filename());
            }
            else if (_tabs.tabs2->value() == editor) {
                pile.set_string("active", "right", editor->filename());
            }
        }
        editor = tabs_editor_by_index(tabindex);
    }
    pile.set_int("gui", "files", count - 1);
    pile.set_string("gui", "dir", _project.dir);
    auto str = pile.export_data();
    if (_db.put(DBKEY_PROJECTS + name, str.c_str(), str.length()) == false) {
        fl_alert("%s\nsqlite: %s", "error: failed to save project", _db.err_msg.c_str());
        return false;
    }
    _project.name = name;
    return true;
}
void FlEdit::project_save_as() {
    fl_message_position(this);
    auto name = gnu::str::to_string(fl_input("%s", "", "Enter name of the project"));
    gnu::str::trim(name);
    if (name != "") {
        project_save(name);
    }
}
void FlEdit::project_save_list(std::string key, const std::vector<std::string>& list, gnu::Pile& pile) {
    auto count = 1;
    for (const auto& word : list) {
        pile.set_string(key, pile.make_key(count++), word);
    }
}
void FlEdit::project_snippets() {
    fl_message_position(this);
    if (_db.is_open() == false) {
        fl_alert("%s", "error: database is closed");
        return;
    }
    auto dialog = TextDialog(_db);
    auto text   = dialog.run(this);
    if (text != "") {
        Fl::copy(text.c_str(), text.length(), 2);
    }
}
void FlEdit::project_snippets_save(SNIPPET snippet, std::string clip) {
    fl_message_position(this);
    if (_db.is_open() == false) {
        fl_alert("%s", "error: database is closed");
        return;
    }
    std::string text;
    std::string name;
    if (snippet == SNIPPET::CLIPBOARD) {
        if (clip != "") {
            text = clip;
        }
        else if (Fl::clipboard_contains(Fl::clipboard_plain_text) != 0) {
            Fl::paste(*this, 1, Fl::clipboard_plain_text);
            return;
        }
    }
    else if (snippet == SNIPPET::SELECTION) {
        if (_editor != nullptr) {
            text = _editor->text_get_selection_string();
        }
    }
    else if (snippet == SNIPPET::TEXT) {
        if (_editor != nullptr) {
            text = gnu::str::grab_string(_editor->buffer().text());
            name = _editor->file_name() + " " + gnu::Time::FormatUnixToISO(_editor->file_mtime());
        }
    }
    if (text == "") {
        fl_alert("%s", "error: nothing to save");
        return;
    }
    if (text.length() > FlEdit::MAX_SNIPPET_LENGTH) {
        fl_alert("error: text is too large (max %u bytes) (%u)", (unsigned) FlEdit::MAX_SNIPPET_LENGTH, (unsigned) text.length());
        return;
    }
    if (name == "") {
        name = gnu::str::to_string(fl_input("%s", "", "Enter name of the snippet"));
    }
    gnu::str::trim(name);
    if (name == "") {
        return;
    }
    if (_db.put(DBKEY_SNIPPETS + name, text, text.length()) == false) {
        fl_alert("%s\nsqlite: %s", "error: failed to save snippet", _db.err_msg.c_str());
    }
}
#include <algorithm>
void FlEdit::tabs_activate(fle::Editor* editor) {
    if (editor == nullptr) {
        _editor = nullptr;
    }
    else if (_tabs.tabs1->find(editor) != -1) {
        if (_tabs.active != _tabs.tabs1 || _tabs.tabs1->visible() == 0) {
            _tabs.active = _tabs.tabs1;
            split_view(SPLIT::SHOWONE);
        }
        if (_editor != editor) {
            _editor = editor;
            if (_tabs.active->value() != _editor) {
                _tabs.active->value(_editor);
            }
        }
    }
    else if (_tabs.tabs2->find(editor) != -1) {
        if (_tabs.active != _tabs.tabs2 || _tabs.active->visible() == 0) {
            _tabs.active = _tabs.tabs2;
            split_view(SPLIT::SHOWTWO);
        }
        if (_editor != editor) {
            _editor = editor;
            if (_tabs.active->value() != _editor) {
                _tabs.active->value(_editor);
            }
        }
    }
    if (_editor != nullptr && Fl::focus() != _editor) {
        _editor->view().take_focus();
    }
    redraw();
}
void FlEdit::tabs_activate_cursor(std::string filename, int row, int col) {
    auto tabindex = 0;
    auto editor   = tabs_editor_by_index(tabindex);
    auto found    = (fle::Editor*) nullptr;
    auto partly1  = (fle::Editor*) nullptr;
    auto partly2  = (fle::Editor*) nullptr;
    auto abs_name = gnu::File(filename).filename;
    while (editor != nullptr) {
        auto filename2 = editor->filename();
        if (filename2 == filename || filename2 == abs_name) {
            found = editor;
            break;
        }
        else if (partly1 == nullptr && filename.find("/") != std::string::npos && filename2.rfind(filename) + filename.length() == filename2.length()) {
            partly1 = editor;
        }
        else if (partly2 == nullptr && filename == editor->file_name()) {
            partly2 = editor;
        }
        editor = tabs_editor_by_index(tabindex);
    }
    if (found != nullptr) {
        found->cursor_move_to_rowcol(row, col);
        tabs_activate(found);
    }
    else if (partly1 != nullptr) {
        partly1->cursor_move_to_rowcol(row, col);
        tabs_activate(partly1);
    }
    else if (partly2 != nullptr) {
        partly2->cursor_move_to_rowcol(row, col);
        tabs_activate(partly2);
    }
    else if (_editor != nullptr) {
        _editor->view().take_focus();
    }
}
int FlEdit::tabs_changed() const {
    auto res      = 0;
    auto tabindex = 0;
    auto editor   = const_cast<FlEdit*>(this)->tabs_editor_by_index(tabindex);
    while (editor != nullptr) {
        if (editor->text_is_readonly() == false && editor->text_is_dirty() == true) {
            res++;
        }
        editor = const_cast<FlEdit*>(this)->tabs_editor_by_index(tabindex);
    }
    return res;
}
void FlEdit::tabs_check_empty() {
    auto t1 = (_tabs.tabs1->visible() != 0) ? true : false;
    auto e1 = (fle::Editor*) _tabs.tabs1->value();
    auto t2 = (_tabs.tabs2->visible() != 0) ? true : false;
    auto e2 = (fle::Editor*) _tabs.tabs2->value();
    auto t  = (_tabs.active == _tabs.tabs1) ? 1 : 2;
    if (_tabs.tabs1->children() == 0) {
        auto editor = new fle::Editor(CONFIG, _findbar);
        _tabs.tabs1->add("", editor);
        if (_tabs.active == _tabs.tabs1) {
            editor->view().take_focus();
            e1 = editor;
        }
    }
    else if (_tabs.tabs1->children() > 1) {
        auto editor = (fle::Editor*) _tabs.tabs1->child(0);
        if (editor->file_changed_name() == "" && _tabs.tabs1->remove(editor) != nullptr) {
            delete editor;
        }
    }
    if (_tabs.tabs2->children() == 0) {
        auto editor = new fle::Editor(CONFIG, _findbar);
        _tabs.tabs2->add("", editor);
    }
    else if (_tabs.tabs2->children() > 1) {
        auto editor = (fle::Editor*) _tabs.tabs2->child(0);
        if (editor->file_changed_name() == "" && _tabs.tabs2->remove(editor) != nullptr) {
            delete editor;
        }
    }
    if (e1 != nullptr && t == 1) {
        tabs_activate(e1);
    }
    else if (e2 != nullptr && t == 2) {
        tabs_activate(e2);
    }
    if (t1 == false && _tabs.tabs1->visible() != 0) {
        _tabs.tabs1->hide();
    }
    if (t2 == false && _tabs.tabs2->visible() != 0) {
        _tabs.tabs2->hide();
    }
}
void FlEdit::tabs_check_external_update() {
    auto tabindex = 0;
    auto editor   = tabs_editor_by_index(tabindex);
    while (editor != nullptr) {
        editor->file_check_reload();
        editor = tabs_editor_by_index(tabindex);
    }
}
void FlEdit::tabs_close_all() {
    auto wc = flw::WaitCursor();
    while (_tabs.tabs1->children() > 0) {
        delete _tabs.tabs1->remove(0);
    }
    while (_tabs.tabs2->children() > 0) {
        delete _tabs.tabs2->remove(0);
    }
    tabs_activate(nullptr);
}
int FlEdit::tabs_count() const {
    auto res      = 0;
    auto tabindex = 0;
    auto editor   = const_cast<FlEdit*>(this)->tabs_editor_by_index(tabindex);
    while (editor != nullptr) {
        if (editor->file_changed_name() != "") {
            res++;
        }
        editor = const_cast<FlEdit*>(this)->tabs_editor_by_index(tabindex);
    }
    return res;
}
void FlEdit::tabs_delete(fle::Editor* editor) {
    if (_tabs.tabs1->remove(editor)) {
        delete editor;
    }
    else if (_tabs.tabs2->remove(editor)) {
        delete editor;
    }
    if (_editor == editor) {
        _editor = nullptr;
    }
    do_layout();
}
fle::Editor* FlEdit::tabs_editor_by_index(int& index) {
    while (index >= 0 && index < _tabs.tabs1->children()) {
        auto editor = (fle::Editor*) _tabs.tabs1->child(index++);
        if (editor->file_is_empty() == false) {
            return editor;
        }
    }
    auto index2 = index - _tabs.tabs1->children();
    while (index2 >= 0 && index2 < _tabs.tabs2->children()) {
        auto editor = (fle::Editor*) _tabs.tabs2->child(index2);
        index++;
        index2++;
        if (editor->file_is_empty() == false) {
            return editor;
        }
    }
    return nullptr;
}
fle::Editor* FlEdit::tabs_editor_by_path(std::string path) {
    if (path == "") {
        return nullptr;
    }
    auto tabindex = 0;
    auto editor   = tabs_editor_by_index(tabindex);
    while (editor != nullptr) {
        if (path == editor->filename()) {
            return editor;
        }
        editor = tabs_editor_by_index(tabindex);
    }
    return nullptr;
}
void FlEdit::tabs_find_lines() {
    fl_message_position(this);
    if (Command::THREAD != nullptr) {
        fl_alert("%s", "error: Can't search for text while running an command!");
        return;
    }
    tabs_check_external_update();
    auto list   = CONFIG.find_list;
    auto dialog = fle::FindDialog("Find Lines In All Files", list);
    auto find   = dialog.run();
    if (find == "") {
        return;
    }
    auto tabindex = 0;
    auto editor   = tabs_editor_by_index(tabindex);
    auto lines    = std::vector<std::string>();
    auto files    = (size_t) 0;
    while (editor != nullptr) {
        if (editor->find_lines(find, fle::FindDialog::REGEX, fle::FindDialog::TRIM, lines) > 0) {
            files++;
        }
        editor = tabs_editor_by_index(tabindex);
    }
    if (lines.size() > 0) {
        _findbar->findreplace().add_find_word(find);
        _findbar->statusbar().label_message(gnu::str::format("found %u lines in %u files", (unsigned) lines.size(), (unsigned) files));
    }
    else {
        _findbar->statusbar().label_message(gnu::str::format("unable to find <%s>", find.c_str()));
    }
    _output->set_list_data(lines, "(?<file>.+):\\s+(?<line>\\d+)\\s+-\\s+(?<col>\\d+).*", 0);
    _output->show();
    _output->show_editor();
    do_layout();
}
void FlEdit::tabs_move_group(fle::Editor* editor) {
    if (editor == nullptr || editor->file_is_empty() == true) {
        return;
    }
    auto tabs = (flw::TabsGroup*) editor->parent();
    if (tabs == nullptr || tabs->remove(editor) == nullptr) {
        return;
    }
    if (tabs == _tabs.tabs1) {
        _tabs.tabs2->add(editor->file_changed_name(), editor, _tabs.tabs2->value());
    }
    else if (tabs == _tabs.tabs2) {
        _tabs.tabs1->add(editor->file_changed_name(), editor, _tabs.tabs1->value());
    }
    tabs_check_empty();
    editor_update_status(editor);
    tabs_activate(editor);
}
void FlEdit::tabs_replace_all() {
    tabs_check_external_update();
    auto find    = std::string();
    auto replace = std::string();
    auto dialog  = fle::ReplaceDialog("Replace Text In All Files", find, replace, CONFIG.find_list, CONFIG.replace_list);
    if (dialog.run() == false) {
        return;
    }
    auto time     = gnu::Time::Milli();
    auto tabindex = 0;
    auto editor   = tabs_editor_by_index(tabindex);
    auto files    = 0;
    auto replaces = 0;
    auto org      = _editor;
    while (editor != nullptr) {
        auto count = editor->find_replace_all(find, replace,
            fle::ReplaceDialog::NLTAB,
            fle::ReplaceDialog::SELECTION,
            fle::ReplaceDialog::CASECOMPARE,
            fle::ReplaceDialog::WORDCOMPARE,
            fle::ReplaceDialog::REGEX,
            fle::FSAVEWORD::YES,
            fle::FHIDEFIND::NO);
        editor    = tabs_editor_by_index(tabindex);
        replaces += count;
        if (count > 0) {
            files++;
        }
    }
    if (org != nullptr) {
        org->view().take_focus();
    }
    if (replaces == 0) {
        _findbar->statusbar().label_message(gnu::str::format("unable to find <%s> in any file!", find.c_str()));
    }
    else {
        time = gnu::Time::Milli() - time;
        _findbar->statusbar().label_message(gnu::str::format("replaced %d strings in %d files in %d mS", replaces, files, time));
    }
}
void FlEdit::tabs_reset_split_size() {
    auto tabindex = 0;
    auto editor   = tabs_editor_by_index(tabindex);
    while (editor != nullptr) {
        editor->view_reset_split_size();
        editor->do_layout();
        editor = tabs_editor_by_index(tabindex);
    }
}
void FlEdit::tabs_restore_visibility() {
    if (_tabs.tabs1_vis == true) {
        _tabs.tabs1->take_focus();
        _tabs.tabs1->show();
    }
    else {
        _tabs.tabs1->hide();
    }
    if (_tabs.tabs2_vis == true) {
        _tabs.tabs2->take_focus();
        _tabs.tabs2->show();
    }
    else {
        _tabs.tabs2->hide();
    }
}
bool FlEdit::tabs_save_all(bool ask) {
    auto unsaved_files = tabs_changed();
    if (unsaved_files == 0) {
        return true;
    }
    if (ask == true) {
        auto answer = fl_choice("You have %d unsaved files!",  "Cancel", "&Save All", "Don't Save", unsaved_files);
        if (answer == 0) {
            return false;
        }
        else if (answer == 2) {
            return true;
        }
    }
    auto tabindex = 0;
    auto editor   = tabs_editor_by_index(tabindex);
    while (editor != nullptr) {
        if (editor->text_is_readonly() == false && file_save(editor) == false) {
            return false;
        }
        editor = tabs_editor_by_index(tabindex);
    }
    return true;
}
void FlEdit::tabs_save_visibility() {
    _tabs.tabs1_vis = _tabs.tabs1->visible();
    _tabs.tabs2_vis = _tabs.tabs2->visible();
}
void FlEdit::tabs_sort(bool left, bool ascending) {
    if (left == true) {
        _tabs.tabs1->sort(ascending);
    }
    else {
        _tabs.tabs2->sort(ascending);
    }
}
void FlEdit::tabs_trailing_all() {
    tabs_check_external_update();
    auto tabindex = 0;
    auto editor   = tabs_editor_by_index(tabindex);
    auto files    = 0;
    auto lines    = 0;
    auto org      = _editor;
    while (editor != nullptr) {
        auto count = editor->text_remove_trailing();
        editor = tabs_editor_by_index(tabindex);
        lines += count;
        if (count > 0) {
            files++;
        }
    }
    if (org != nullptr) {
        org->view().take_focus();
    }
    if (lines == 0) {
        _findbar->statusbar().label_message("unable to remove any whitespace!");
    }
    else {
        _findbar->statusbar().label_message(gnu::str::format("removed whitespace from %d lines in %d files", lines, files));
    }
}
int main(int argc, const char** argv) {
    Fl::keyboard_screen_scaling(0);
    flw::theme::load("oxy");
    if (Fl::lock() != 0) {
        fl_alert("%s", "error: missing thread support\nhave to quit");
        exit(1);
    }
    try {
        auto arg    = 1;
        auto fledit = FlEdit(800, 600);
        fledit.executable = argv[0];
        flw::util::sleep(18);
        Fl::check();
        if (argc > arg && fledit.project_exist(argv[arg]) == true) {
            fledit.tabs_close_all();
            fledit.project_load_from_db(argv[arg]);
            arg++;
        }
        if (argc > arg) {
            auto files = std::vector<std::string>();
            auto lines = std::vector<int>();
            auto rx    = gnu::PCRE("(.*):(\\d+)$");
            for (int f = arg; f < argc; f++) {
                auto filename = std::string(argv[f]);
                auto fileline = 0;
                if (rx.exec(filename) == 3) {
                    filename = rx.substr(1);
                    fileline = gnu::str::to_int(rx.substr(2), 0);
                }
                auto file = gnu::File(filename);
                if (file.is_file() || file.is_missing()) {
                    files.push_back(file.filename);
                    lines.push_back(fileline);
                }
            }
            if (files.size() > 0) {
                fledit.file_load_list(nullptr, files, lines);
            }
        }
        Fl::run();
    }
    catch (const std::string& e) {
        fl_alert("%s\n%s", "error: exception has been thrown, have to quit!", e.c_str());
    }
    catch (const char* e) {
        fl_alert("%s\n%s", "error: exception has been thrown, have to quit!", e);
    }
    catch (...) {
        fl_alert("%s", "error: unknown exception has been thrown, have to quit!");
    }
    return 0;
}