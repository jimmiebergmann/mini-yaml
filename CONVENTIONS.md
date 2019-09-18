# Conventions

List of conventions for mini-yaml.

## Code

All namespaces, enums, classes, methods, and variables are in lowercase and words are separated by underscore. Private and protected class variables are prefixed by "m_".
Following code illustrates the coding style conventions for spacing, naming and ordering. Methods within a class may be grouped together by context, but should be in alphabetic order within each group.


```cpp
namespace my_namespace
{

    enum class my_enum
    {
        foo_bar,
        hello_word
    };

    class my_class
    {
    
      public: 
      
        static int my_static_func();
        ...
      
        my_class();
        my_class(const int value); 
        ...
        my_class(const my_class &);
        ~my_class();
      
        int method_1() const;
        int method_2() const;
        template<typename T>
        void method_3();
        ...
        
        my_class & operator =(const int &);
        my_class & operator +=(const int &);
        my_class operator +(const int &) const;
        my_class operator -(const int &) const;
        ...
      
      protected:
      
        ...
      
      private:
      
        void private_method();
        ...
      
        int m_foo;
        int m_hello_world;
        ...
        
    };
    
}

// Inline implementations.
namespace my_namespace
{

    template<typename T>
    inline void my_class::method_3()
    {
        
    }
    
}
```
