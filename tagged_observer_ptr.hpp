/*
 * @Author: Betty
 * @Date: 2021-07-17 19:41:10
 * @LastEditTime: 2021-07-19 21:35:04
 * @Description: an enhanced C++ observer_ptr which can zip
 *               at most 16bits addtional information in 64bits
 *               program on x86_64 machine
 * @FilePath: \tagged_ptr\tagged_observer_ptr.hpp
 */
#ifndef __tagged_observer_ptr__

#define __tagged_observer_ptr__

#include <type_traits>
#include <utility>

namespace tagged_ptr
{
namespace __tagged_ptr_base
{
    constexpr static bool __in_x86_64_mode() noexcept
    {
    #if defined _M_X64 || defined __x86_64__ || defined __amd64
        return true;
    #else
        return false;
    #endif
    }

    template <typename T>
    struct addtional_info
    {
        T info;
    };

    template <typename T>
    constexpr static bool isEnableZip() noexcept
    {
        return sizeof(T) <= 2 && __in_x86_64_mode();
    }

    template <typename T, typename Info, bool EnableZip>
    class observer_ptr;

    //enable zip
    template <typename T, typename Info>
    class observer_ptr<T, Info, true>
    {
    private:
        T *ptr_info;
        static bool get_47th_bit(T *address) noexcept
        {
            static_assert(sizeof(std::size_t) == 8); //check if is 64bit platform
            return reinterpret_cast<std::size_t>(address) & (0x0000800000000000);
        }

        constexpr static T *clearLow48Bit(T *address) noexcept
        {
            return reinterpret_cast<T *>(0xFFFF000000000000 & reinterpret_cast<std::size_t>(address));
        }

        constexpr static T *clearHigh16Bit(T *address) noexcept
        {
            return reinterpret_cast<T *>(0x0000FFFFFFFFFFFF & reinterpret_cast<std::size_t>(address));
        }

        constexpr Info *getInfoAddress() noexcept
        {
            return reinterpret_cast<Info *>(reinterpret_cast<char *>(&this->ptr_info) + 6);
        }

        constexpr Info *const getInfoAddress() const noexcept
        {
            return reinterpret_cast<Info *const>(reinterpret_cast<char *const>(&this->ptr_info) + 6);
        }

        constexpr static T *unzip_ptr(T *address) noexcept
        {
            return reinterpret_cast<T *>(get_47th_bit(address)?
                (0xFFFF000000000000 | reinterpret_cast<std::size_t>(address)):
                (0x0000FFFFFFFFFFFF & reinterpret_cast<std::size_t>(address)));
        }

    public:

        static constexpr bool is_enable_zip = true;

        Info &info() noexcept
        {
            return *getInfoAddress();
        }

        const Info & info() const noexcept
        {
            return *getInfoAddress();
        }

        T *const get() const noexcept 
        {
            return unzip_ptr(ptr_info);
        }

        T *get() noexcept 
        {
            return unzip_ptr(ptr_info);
        }

        observer_ptr(const observer_ptr &other)
            : ptr_info(other.ptr_info)
        {
            new (getInfoAddress()) Info(other.info());
        }
        observer_ptr(observer_ptr &&other)
            : ptr_info(other.ptr_info)
        {
            new (getInfoAddress()) Info(std::move(other.info()));
        }

        observer_ptr() : ptr_info(nullptr)
        {
            new (getInfoAddress()) Info();
        }
        observer_ptr(T *p) : ptr_info(p)
        {
            new (getInfoAddress()) Info();
        }
        observer_ptr(T *p, const Info &other) : ptr_info(p)
        {
            new (getInfoAddress()) Info(other);
        }
        observer_ptr(T *p, Info &&other) : ptr_info(p)
        {
            new (getInfoAddress()) Info(std::move(other));
        }

        template <typename... Args>
        observer_ptr(T *p, Args &&...args) : ptr_info(p)
        {
            new (getInfoAddress()) Info(std::forward<Args>(args)...);
        }

        observer_ptr &operator=(const observer_ptr &other)
        {
            ptr_info=other.ptr_info;
            new (getInfoAddress()) Info(other.info());
            return *this;
        }

        observer_ptr &operator=(observer_ptr &&other)
        {
            ptr_info=other.ptr_info;
            new (getInfoAddress()) Info(std::move(other.info()));
            return *this;
        }

        friend bool operator ==(const observer_ptr &a,const observer_ptr &b)
        {
            return unzip_ptr(a.ptr_info)==unzip_ptr(b.ptr_info)&&a.info()==b.info();
        }

        friend bool operator !=(const observer_ptr &a,const observer_ptr &b)
        {
            return !(a==b);
        }

        template <typename... Args>
        friend observer_ptr make_tagged_observer(Args &&...args)
        {
            return observer_ptr{std::forward<Args>(args)...};
        }

        constexpr explicit operator bool() const noexcept
        {
            return unzip_ptr(ptr_info)!=nullptr;
        }

        constexpr void release() noexcept
        {
            ptr_info=clearLow48Bit(ptr_info);
        }

        constexpr void reset(T* p = nullptr) noexcept
        {
            ptr_info=reinterpret_cast<T*>(
                reinterpret_cast<std::size_t>(clearLow48Bit(ptr_info))|reinterpret_cast<std::size_t>(clearHigh16Bit(p)));
        }

        constexpr void swap(observer_ptr& other)
        {
            observer_ptr temp{std::move(other)};
            other=std::move(*this);
            *this=std::move(temp);
        }

        constexpr std::add_lvalue_reference_t<T> operator*() const
        {
            return *unzip_ptr(ptr_info);
        }

        constexpr T* operator->() const noexcept
        {
            return unzip_ptr(ptr_info);
        }

        ~observer_ptr()
        {
            getInfoAddress()->~Info();
        }
    };

    //disable zip
    template <typename T, typename Info>
    class observer_ptr<T, Info, false>
    {
    private:
        T *ptr_info;
        Info addtional_info;
        
    public:

        static constexpr bool is_enable_zip = false;

        Info &info() noexcept
        {
            return addtional_info;
        }

        const Info & info() const noexcept
        {
            return addtional_info;
        }

        T *const get() const noexcept 
        {
            return ptr_info;
        }

        T *get() noexcept 
        {
            return ptr_info;
        }

        observer_ptr(const observer_ptr &other)
            : ptr_info(other.ptr_info),addtional_info(other.addtional_info){}

        observer_ptr(observer_ptr &&other)
            : ptr_info(other.ptr_info),addtional_info(std::move(other.addtional_info)){}

        observer_ptr() 
            : ptr_info(nullptr),addtional_info(){}

        observer_ptr(T *p) 
            : ptr_info(p),addtional_info(){}

        observer_ptr(T *p, const Info &other) 
            : ptr_info(p),addtional_info(other){}

        observer_ptr(T *p, Info &&other) 
            : ptr_info(p),addtional_info(std::move(other)){}
        
        template <typename... Args>
        observer_ptr(T *p, Args &&...args)
            : ptr_info(p),addtional_info(std::forward<Args>(args)...){}

        observer_ptr &operator=(const observer_ptr &other)=default;

        observer_ptr &operator=(observer_ptr &&other)=default;

        friend bool operator ==(const observer_ptr &a,const observer_ptr &b)
        {
            return a.ptr_info==b.ptr_info&&a.addtional_info==b.addtional_info;
        }

        friend bool operator !=(const observer_ptr &a,const observer_ptr &b)
        {
            return !(a==b);
        }

        template <typename... Args>
        friend observer_ptr make_tagged_observer(Args &&...args)
        {
            return observer_ptr{std::forward<Args>(args)...};
        }

        constexpr explicit operator bool() const noexcept
        {
            return ptr_info!=nullptr;
        }

        constexpr void release() noexcept
        {
            ptr_info=nullptr;
        }

        constexpr void reset(T* p = nullptr) noexcept
        {
            ptr_info=p;
        }

        constexpr void swap(observer_ptr& other)
        {
            swap(ptr_info,other.ptr_info);
            swap(addtional_info,other.addtional_info);
        }

        constexpr std::add_lvalue_reference_t<T> operator*() const
        {
            return *ptr_info;
        }

        constexpr T* operator->() const noexcept
        {
            return ptr_info;
        }

        ~observer_ptr()=default;
    };
}

template <typename T, typename Info>
    using observer_ptr = __tagged_ptr_base::observer_ptr<T, Info, __tagged_ptr_base::isEnableZip<Info>()>;
}

#endif

