#pragma once

//---------------------------------------------------------------------------//
//
// Any.hpp
//  汎用コンテナ
//   Copyright (C) 2015 tapetums
//
// 格納可能な方は以下の通り。
//  nullptr, bool, 符号付き64ビット整数, 符号なし64ビット整数, UTF-8文字列,
//  汎用コンテナの配列, UTF-8文字列をキーとする汎用コンテナのハッシュマップ
//
//---------------------------------------------------------------------------//

#include <cstdint>

#include <vector>
#include <unordered_map>
#include <string>

//---------------------------------------------------------------------------//

namespace tapetums {

//---------------------------------------------------------------------------//

struct null { };

//---------------------------------------------------------------------------//

class any
{
public: // types
    using array_t = std::vector<any>;
    using map_t   = std::unordered_map<std::string, any>;

protected:
    enum class type_t : uint32_t
    {
        null, boolean, si64, ui64, f64, string, array, map,
    };

    struct data_t
    {
        type_t type;
        union
        {
            nullptr_t    null;
            bool         boolean;
            int64_t      si64;
            uint64_t     ui64;
            double       f64;
            std::string* string;
            array_t*     array;
            map_t*       map;
        };
    };

public: // ctor / dtor
    any();
    explicit any(nullptr_t);
    explicit any(bool);
    explicit any(int64_t);
    explicit any(uint64_t);
    explicit any(double);
    explicit any(const char*);
    explicit any(const std::string&);
    explicit any(std::string&&);
    explicit any(const array_t&);
    explicit any(array_t&&);
    explicit any(const map_t&);
    explicit any(map_t&&);
    any(const any&);
    any(any&&) noexcept;
    ~any();

public: // methods
    bool empty() const noexcept;
    size_t size() const noexcept;
    template<typename T> bool is() const noexcept;
    template<typename T> const T& as() const;
    template<typename T> T& as();
    template<typename T> any& emplace_back(const T& value);
    template<typename T> any& emplace_back(T&& value);
    any& push_back(const any& value);
    any& push_back(any&& value);
    bool contains(const char* key) const;
    bool contains(const std::string& key) const;
    bool contains(std::string&& key) const;
    void clear();

public: // operators
    explicit operator bool() const noexcept;
    bool operator==(const any&) const noexcept;
    bool operator!=(const any&) const noexcept;
    any& operator=(any&);
    any& operator=(const any&);
    any& operator=(any&&) noexcept;
    const any& operator[](size_t idx) const;
    any& operator[](size_t idx);
    const any& operator[](const char* key) const;
    const any& operator[](const std::string& key) const;
    const any& operator[](std::string&& key) const;
    any& operator[](const char* key);
    any& operator[](const std::string& key);
    any& operator[](std::string&& key);
    template<typename T> any& operator=(T& value);
    template<typename T> any& operator=(const T& value);
    template<typename T> any& operator=(T&& value);

private: // initializers
    void init();
    void init(nullptr_t);
    void init(bool);
    void init(int64_t);
    void init(uint64_t);
    void init(double);
    void init(const char*);
    void init(const std::string&);
    void init(std::string&&);
    void init(const array_t&);
    void init(array_t&&);
    void init(const map_t&);
    void init(map_t&&);

protected: // members
    data_t m_data;

public: // unit test
    static void unittest();
};

//---------------------------------------------------------------------------//
// Initializers
//---------------------------------------------------------------------------//

inline void any::init()
{
    m_data.type = type_t::null;
    m_data.ui64 = 0Ui64;
}

//---------------------------------------------------------------------------//

inline void any::init(nullptr_t)
{
    init();
}

//---------------------------------------------------------------------------//

inline void any::init(bool value)
{
    m_data.type = type_t::boolean;
    m_data.boolean = value;
}

//---------------------------------------------------------------------------//

inline void any::init(int64_t value)
{
    m_data.type = type_t::si64;
    m_data.si64 = value;
}

//---------------------------------------------------------------------------//

inline void any::init(uint64_t value)
{
    m_data.type = type_t::ui64;
    m_data.ui64 = value;
}

//---------------------------------------------------------------------------//

inline void any::init(double value)
{
    m_data.type = type_t::f64;
    m_data.f64 = value;
}

//---------------------------------------------------------------------------//

inline void any::init(const char* value)
{
    m_data.type = type_t::string;
    m_data.string = new std::string(value);
}

//---------------------------------------------------------------------------//

inline void any::init(const std::string& value)
{
    m_data.type = type_t::string;
    m_data.string = new std::string(value);
}

//---------------------------------------------------------------------------//

inline void any::init(std::string&& value)
{
    m_data.type = type_t::string;
    m_data.string = new std::string(value);
}

//---------------------------------------------------------------------------//

inline void any::init(const array_t& value)
{
    m_data.type = type_t::array;
    m_data.array = new array_t(value);
}

//---------------------------------------------------------------------------//

inline void any::init(array_t&& value)
{
    m_data.type = type_t::array;
    m_data.array = new array_t(value);
}

//---------------------------------------------------------------------------//

inline void any::init(const map_t& value)
{
    m_data.type = type_t::map;
    m_data.map = new map_t(value);
}

//---------------------------------------------------------------------------//

inline void any::init(map_t&& value)
{
    m_data.type = type_t::map;
    m_data.map = new map_t(value);
}

//---------------------------------------------------------------------------//
// ctor / dtor
//---------------------------------------------------------------------------//

inline any::any()
{
    init();
}

//---------------------------------------------------------------------------//

inline any::any(nullptr_t)
{
    init();
}

//---------------------------------------------------------------------------//

inline any::any(bool value)
{
    init(value);
}

//---------------------------------------------------------------------------//

inline any::any(int64_t value)
{
    init(value);
}

//---------------------------------------------------------------------------//

inline any::any(uint64_t value)
{
    init(value);
}

//---------------------------------------------------------------------------//

inline any::any(double value)
{
    init(value);
}

//---------------------------------------------------------------------------//

inline any::any(const char* value)
{
    init(value);
}

//---------------------------------------------------------------------------//

inline any::any(const std::string& value)
{
    init(value);
}

//---------------------------------------------------------------------------//

inline any::any(std::string&& value)
{
    init(std::move(value));
}

//---------------------------------------------------------------------------//

inline any::any(const array_t& value)
{
    init(value);
}

//---------------------------------------------------------------------------//

inline any::any(array_t&& value)
{
    init(std::move(value));
}

//---------------------------------------------------------------------------//

inline any::any(const map_t& value)
{
    init(value);
}

//---------------------------------------------------------------------------//

inline any::any(map_t&& value)
{
    init(std::move(value));
}

//---------------------------------------------------------------------------//

inline any::any(const any& lhs) : any()
{
    operator=(lhs);
}

//---------------------------------------------------------------------------//

inline any::any(any&& rhs) noexcept : any()
{
    operator=(std::move(rhs));
}

//---------------------------------------------------------------------------//

inline any::~any()
{
    clear();
}

//---------------------------------------------------------------------------//
// Methods
//---------------------------------------------------------------------------//

inline bool any::empty() const noexcept
{
    return m_data.type == type_t::null;
}

//---------------------------------------------------------------------------//

inline size_t any::size() const noexcept
{
    switch ( m_data.type )
    {
        case type_t::boolean: { return sizeof(bool); }
        case type_t::si64:    { return sizeof(int64_t); }
        case type_t::ui64:    { return sizeof(uint64_t); }
        case type_t::f64:     { return sizeof(double); }
        case type_t::string:  { return m_data.string->size(); }
        case type_t::array:   { return m_data.array->size(); }
        case type_t::map:     { return m_data.map->size(); }
        default:              { return 0; }
    }
}

//---------------------------------------------------------------------------//

#define IS(ctype, jtype) \
template<> inline bool any::is<ctype>() const noexcept \
{ \
    return m_data.type == type_t::##jtype; \
}

IS(null,         null)
IS(bool,         boolean)
IS(int64_t,      si64)
IS(uint64_t,     ui64)
IS(double,       f64)
IS(std::string,  string)
IS(any::array_t, array)
IS(any::map_t,   map)

#undef IS

//---------------------------------------------------------------------------//

#define AS(ctype, var) \
template<> inline const ctype& any::as<ctype>() const \
{ \
    if ( !is<ctype>() ) { throw std::runtime_error("type mismatch"); } \
    return var; \
} \
template<> inline ctype& any::as<ctype>() \
{ \
    if ( !is<ctype>() ) { throw std::runtime_error("type mismatch"); } \
    return var; \
}

AS(bool,         m_data.boolean)
AS(int64_t,      m_data.si64)
AS(uint64_t,     m_data.ui64)
AS(double,       m_data.f64)
AS(std::string,  *m_data.string)
AS(any::array_t, *m_data.array)
AS(any::map_t,   *m_data.map)

#undef AS

//---------------------------------------------------------------------------//

template<typename T> inline any& any::emplace_back(const T& value)
{
    if ( empty() ) { m_data.type = type_t::array; m_data.array = new array_t(); }
    else if ( !is<array_t>() ) { throw std::runtime_error("not an array"); }

    m_data.array->emplace_back(value);

    return *this;
}

//---------------------------------------------------------------------------//

template<typename T> inline any& any::emplace_back(T&& value)
{
    if ( empty() ) { m_data.type = type_t::array; m_data.array = new array_t(); }
    else if ( !is<array_t>() ) { throw std::runtime_error("not an array"); }

    m_data.array->emplace_back(value);

    return *this;
}

//---------------------------------------------------------------------------//

inline any& any::push_back(const any& value)
{
    if ( empty() ) { m_data.type = type_t::array; m_data.array = new array_t(); }
    else if ( !is<array_t>() ) { throw std::runtime_error("not an array"); }

    m_data.array->push_back(value);

    return *this;
}

//---------------------------------------------------------------------------//

inline any& any::push_back(any&& value)
{
    if ( empty() ) { m_data.type = type_t::array; m_data.array = new array_t(); }
    else if ( !is<array_t>() ) { throw std::runtime_error("not an array"); }

    m_data.array->push_back(value);

    return *this;
}

//---------------------------------------------------------------------------//

inline bool any::contains(const char* key) const
{
    if ( !is<map_t>() ) { throw std::runtime_error("not a map"); }

    return m_data.map->find(key) != m_data.map->cend();
}

//---------------------------------------------------------------------------//

inline bool any::contains(const std::string& key) const
{
    if ( !is<map_t>() ) { throw std::runtime_error("not a map"); }

    return m_data.map->find(key) != m_data.map->cend();
}

//---------------------------------------------------------------------------//

inline bool any::contains(std::string&& key) const
{
    if ( !is<map_t>() ) { throw std::runtime_error("not a map"); }

    return m_data.map->find(key) != m_data.map->cend();
}

//---------------------------------------------------------------------------//

inline void any::clear()
{
    if ( m_data.type == type_t::string && m_data.string )
    {
        delete m_data.string;
    }
    else if ( m_data.type == type_t::array && m_data.array )
    {
        delete m_data.array;
    }
    else if ( m_data.type == type_t::map && m_data.map )
    {
        delete m_data.map;
    }

    init();
}

//---------------------------------------------------------------------------//
// Operators
//---------------------------------------------------------------------------//

inline any::operator bool() const noexcept
{
    switch ( m_data.type )
    {
        case type_t::boolean:
        {
            return m_data.boolean;
        }
        case type_t::si64:
        case type_t::ui64:
        {
            return m_data.ui64 > 0Ui64;
        }
        case type_t::f64:
        {
            return m_data.f64 != 0.0;
        }
        case type_t::string:
        case type_t::array:
        case type_t::map:
        {
            return m_data.null != nullptr;
        }
        default:
        {
            return false;
        }
    }
}

//---------------------------------------------------------------------------//

inline bool any::operator==(const any& other) const noexcept
{
    if ( m_data.type != other.m_data.type )
    {
        return false;
    }

    switch ( m_data.type )
    {
        case type_t::null:
        {
            return true;
        }
        case type_t::boolean:
        {
            return m_data.boolean == other.m_data.boolean;
        }
        case type_t::si64:
        case type_t::ui64:
        {
            return m_data.ui64 == other.m_data.ui64;
        }
        case type_t::f64:
        {
            return m_data.f64 == other.m_data.f64;
        }
        case type_t::string:
        case type_t::array:
        case type_t::map:
        {
            return m_data.null == other.m_data.null;
        }
        default:
        {
            return false;
        }
    }
}

//---------------------------------------------------------------------------//

inline bool any::operator!=(const any& other) const noexcept
{
    return !operator==(other);
}

//---------------------------------------------------------------------------//

inline any& any::operator=(any& lhs)
{
    return operator=((const any&)lhs);
}

//---------------------------------------------------------------------------//

inline any& any::operator=(const any& lhs)
{
    if ( this == &lhs ) { return *this; }

    if ( !empty() ) { clear(); }

    m_data.type = lhs.m_data.type;

    if ( m_data.type == type_t::string && lhs.m_data.string )
    {
        m_data.string = new std::string(*lhs.m_data.string);
    }
    else if ( m_data.type == type_t::array && lhs.m_data.array )
    {
        m_data.array = new array_t(*lhs.m_data.array);
    }
    else if ( m_data.type == type_t::map && lhs.m_data.map )
    {
        m_data.map = new map_t(*lhs.m_data.map);
    }
    else
    {
        m_data.ui64 = lhs.m_data.ui64;
    }

    return *this;
}

//---------------------------------------------------------------------------//

inline any& any::operator=(any&& rhs) noexcept
{
    if ( this == &rhs ) { return *this; }

    if ( !empty() ) { clear(); }

    m_data = rhs.m_data;

    rhs.init();

    return *this;
}

//---------------------------------------------------------------------------//

inline const any& any::operator[](size_t idx) const
{
    if ( !is<array_t>() ) { throw std::runtime_error("not an array"); }

    return m_data.array->at(idx);
}

//---------------------------------------------------------------------------//

inline any& any::operator[](size_t idx)
{
    if ( !is<array_t>() ) { throw std::runtime_error("not an array"); }

    return m_data.array->at(idx);
}

//---------------------------------------------------------------------------//

inline const any& any::operator[](const char* key) const
{
    if ( !is<map_t>() ) { throw std::runtime_error("not a map"); }

    return m_data.map->at(key);
}

//---------------------------------------------------------------------------//

inline const any& any::operator[](const std::string& key) const
{
    if ( !is<map_t>() ) { throw std::runtime_error("not a map"); }

    return m_data.map->at(key);
}

//---------------------------------------------------------------------------//

inline const any& any::operator[](std::string&& key) const
{
    if ( !is<map_t>() ) { throw std::runtime_error("not a map"); }

    return m_data.map->at(std::move(key));
}

//---------------------------------------------------------------------------//

inline any& any::operator[](const char* key)
{
    if ( empty() ) { m_data.type = type_t::map; m_data.map = new map_t(); }
    else if ( !is<map_t>() ) { throw std::runtime_error("not a map"); }

    return (*m_data.map)[key];
}

//---------------------------------------------------------------------------//

inline any& any::operator[](const std::string& key)
{
    if ( empty() ) { m_data.type = type_t::map; m_data.map = new map_t(); }
    else if ( !is<map_t>() ) { throw std::runtime_error("not a map"); }

    return (*m_data.map)[key];
}

//---------------------------------------------------------------------------//

inline any& any::operator[](std::string&& key)
{
    if ( empty() ) { m_data.type = type_t::map; m_data.map = new map_t(); }
    else if ( !is<map_t>() ) { throw std::runtime_error("not a map"); }

    return (*m_data.map)[key];
}

//---------------------------------------------------------------------------//

template<typename T> inline any& any::operator=(T&& value)
{
    if ( !empty() ) { clear(); }

    init(std::move(value));

    return *this;
}

//---------------------------------------------------------------------------//

template<typename T> inline any& any::operator=(T& value)
{
    return operator=((const T&)value);
}

//---------------------------------------------------------------------------//

template<typename T> inline any& any::operator=(const T& value)
{
    if ( !empty() ) { clear(); }

    init(value);

    return *this;
}

//---------------------------------------------------------------------------//
// Unit Test
//---------------------------------------------------------------------------//

#if defined(TEST)

#include <cassert>

inline void any::unittest()
{
    printf("sizeof(any) == %zu bytes\n", sizeof(any));

    // ctor
    const any v01;
    const any v02{ nullptr };
    const any v03{ false };
    const any v04{ true };
    const any v05{ -1i64 };
    const any v06{ 0Ui64 };
    const any v07{ 21'4748'3647Ui64 };
    const any v08{ 1844'6744'0737'0955'1615 };
    const any v09{ 0.0 };

    const std::string s{ "copied string" };
    const any v10{ "Hello, World!" };
    const any v11{ s };
    any v12{ std::string{ "moved string" } };

    const any::array_t array1{ v01, v02 };
    const any v13{ array1 };

    any::array_t array2{ v03, v04, v05, v06, v07 };
    any v14{ std::move(array2) };

    const any::map_t map1{ {"A", v08}, {"B", v09}, {"C", v10}, {"X", any{ }} };
    const any v15{ map1 };

    any::map_t map2;
    map2.emplace("D", v11);
    map2.emplace("E", v12);
    any v16{ std::move(map2) };

    // copy ctor
    auto v17 = v13;

    // move ctor
    const auto v18 = std::move(v17);

    // dtor
    { any{ 1Ui64 }; }

    // methods
    assert(v01.empty());
    assert(v02.empty());
    assert(!v03.empty());
    assert(!v04.empty());
    assert(!v05.empty());
    assert(!v06.empty());
    assert(!v07.empty());
    assert(!v08.empty());
    assert(!v09.empty());
    assert(!v10.empty());
    assert(!v11.empty());
    assert(!v12.empty());
    assert(!v13.empty());
    assert(!v14.empty());
    assert(!v15.empty());
    assert(!v16.empty());
    assert(v17.empty());
    assert(!v18.empty());
    
    assert(v01.size() == 0);
    assert(v02.size() == 0);
    assert(v03.size() == sizeof(bool));
    assert(v04.size() == sizeof(bool));
    assert(v05.size() == sizeof(int64_t));
    assert(v06.size() == sizeof(uint64_t));
    assert(v07.size() == sizeof(uint64_t));
    assert(v08.size() == sizeof(uint64_t));
    assert(v09.size() == sizeof(double));
    assert(v10.size() == 13);
    assert(v11.size() == 13);
    assert(v12.size() == 12);
    assert(v13.size() == 2);
    assert(v14.size() == 5);
    assert(v15.size() == 4);
    assert(v16.size() == 2);
    assert(v17.size() == 0);
    assert(v18.size() == 2);

    assert(v01.is<null>());
    assert(v02.is<null>());
    assert(v03.is<bool>());
    assert(v04.is<bool>());
    assert(v05.is<int64_t>());
    assert(v06.is<uint64_t>());
    assert(v07.is<uint64_t>());
    assert(v08.is<uint64_t>());
    assert(v09.is<double>());
    assert(v10.is<std::string>());
    assert(v11.is<std::string>());
    assert(v12.is<std::string>());
    assert(v13.is<any::array_t>());
    assert(v14.is<any::array_t>());
    assert(v15.is<any::map_t>());
    assert(v16.is<any::map_t>());
    assert(v17.is<null>());
    assert(v18.is<any::array_t>());

    v12.as<std::string>() += "!";
    auto& as01 = v04.as<bool>();
    auto& as02 = v05.as<int64_t>();
    auto& as03 = v07.as<uint64_t>();
    auto& as04 = any(1.23).as<double>();
    auto& as05 = v10.as<std::string>();
    auto  as06 = v12.as<std::string>().data();
    auto& as07 = v13.as<any::array_t>();
    auto& as08 = v14.as<any::array_t>();
    auto& as09 = v15.as<any::map_t>();
    auto& as10 = v16.as<any::map_t>();
    // as04, as08, as10 は non-const
    // それ以外は const
    try
    {
        v01.as<std::string>();
        assert(false && "is(): nothing has been thrown");
    }
    catch ( std::runtime_error& e )
    {
        e.what();
    }
    
    const auto b = false;
    v14.emplace_back(b);
    v14.emplace_back(true);
    v14.emplace_back(-999i64);
    v14.emplace_back(0Ui64);
    v14.emplace_back(80.2);
    v14.emplace_back("emplace_back");
    v14.emplace_back(v13);
    v14.emplace_back(v16);
    v14.push_back(v16);
    v14.push_back(any{ "temp" });

    try
    {
        v16.emplace_back("Hi");
        assert(false && "emplace_back(): nothing has been thrown");
    }
    catch ( std::runtime_error& e )
    {
        e.what();
    }
    try
    {
        v16.push_back(any{ "Wi" });
        assert(false && "push_back(): nothing has been thrown");
    }
    catch ( std::runtime_error& e )
    {
        e.what();
    }

    try
    {
        v01.contains("Fi");
        assert(false && "contains(): nothing has been thrown");
    }
    catch ( std::runtime_error& e )
    {
        e.what();
    }

    assert(v15.contains("X"));
    assert(!v16.contains(s));
    assert(v16.contains(std::string{ "E" }));

    v16.clear();

    // operators
    assert(!v01);
    assert(!v02);
    assert(!v03);
    assert(v04);
    assert(v05);
    assert(!v06);
    assert(v07);
    assert(v08);
    assert(!v09);
    assert(v10);
    assert(v11);
    assert(v12);
    assert(v13);
    assert(v14);
    assert(v15);
    assert(!v16);
    assert(!v17);
    assert(v18);

    v16 = v15;
    v16 = v16;
    v16 = v17;
    v16 = std::move(v17);

    auto& array3 = v13.as<any::array_t>();
    // array3 は const
    for ( auto size = array3.size(); size; --size )
    {
        auto& v = v13[size - 1];
        // v は const
    }

    auto& array4 = v14.as<any::array_t>();
    // array4 は non-const
    const auto size = array4.size();
    v14[size - 1] = "w";
    v14[size - 2] = 3.45;
    v14[size - 3] = v13;
    v14[size - 3] = v16;
    v14[size - 4] = -1024i64;
    v14[size - 5] = std::string{ "foo" };
    v14[size - 6] = 4096Ui64;
    v14[size - 7] = s;
    v14[size - 8] = v14;

    auto& vv = v14[size - 1];
    // vv は non-const

    auto& vvv = v15["A"];
    // vvv は const

    v16["1"] = nullptr;
    auto& vvvv = v16["1"];
    // vvvv は non-const

    try
    {
        v13["A"];
        assert(false && "operator[](): nothing has been thrown");
    }
    catch ( std::runtime_error& e )
    {
        e.what();
    }
    try
    {
        v14[(size_t)-1];
        assert(false && "operator[](): nothing has been thrown");
    }
    catch ( std::out_of_range& e )
    {
        e.what();
    }
    try
    {
        v15["QWERTY"];
        assert(false && "operator[](): nothing has been thrown");
    }
    catch ( std::out_of_range& e )
    {
        e.what();
    }
    try
    {
        v16[(size_t)0];
        assert(false && "operator[](): nothing has been thrown");
    }
    catch ( std::runtime_error& e )
    {
        e.what();
    }

    auto& map3 = v15.as<any::map_t>();
    // map3 は const
    for ( auto it = map3.begin(); it != map3.end(); ++it )
    {
        auto& key   = it->first;
        auto& value = it->second;
        // key, value ともに const
    }

    auto& map4 = v16.as<any::map_t>();
    // map4 は non-const
    for ( auto it = map4.begin(); it != map4.end(); ++it )
    {
        auto& key   = it->first;
        auto& value = it->second;
        // key は const, value は non-const

        value = nullptr;
        value = true;
        value = -1i64;
        value = 0Ui64;
        value = 7.89;
        value = "changed value";
        value = s;
        value = std::string{ "hoge" };
        value = v01;
        value = v17;
    }

    const any v19 { any::array_t { any{ 0.0 }, any{ "!" } } };
    assert(v19[size_t{0}].is<double>());
    assert(v19[size_t{1}].is<std::string>());
    assert(v19[size_t{0}].as<double>() == 0.0);
    assert(v19[size_t{1}].as<std::string>() == "!");

    const any v20 { any::map_t { {"1", any{ 777Ui64 }}, {"2", any{ 1.0 }}, {"3", v19} } };
}

#else

inline void any::unittest() { }

#endif

//---------------------------------------------------------------------------//

} // namespace tapetums

//---------------------------------------------------------------------------//

// Any.hpp