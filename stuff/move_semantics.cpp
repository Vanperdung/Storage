#include <iostream>
#include <string.h>

class String
{
public:
    String() = default;

    String(const String& str)
    {
        std::cout << "Copied\n";
        m_Size = str.m_Size;
        m_Data = new char[m_Size];
        memcpy(m_Data, str.m_Data, m_Size);
    }

    String(String&& str)
    {
        std::cout << "Moved\n";
        m_Size = str.m_Size;
        m_Data = str.m_Data;

        str.m_Size = 0;
        str.m_Data = nullptr;
    }

    String(const char* data)
    {
        std::cout << "Created\n";
        m_Size = strlen(data);
        m_Data = new char[m_Size];
        memcpy(m_Data, data, m_Size);
    }

    ~String()
    {
        std::cout << "Distroy\n";
        delete [] m_Data;
    }

private:
    char* m_Data = nullptr;
    int m_Size;
};

class Entity
{
public:
    Entity() = default;

    Entity(String str) : m_Str(std::move(str))
    {
        std::cout << "Entity created\n"; 
    }

private:
    String m_Str;
};

int main(int argc, char* argv[])
{
    Entity e("hello");
    return 0;
}