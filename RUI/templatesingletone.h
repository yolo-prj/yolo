#ifndef TEMPLATESINGLETONE_H
#define TEMPLATESINGLETONE_H

#include <memory>

template < typename T >
class TemplateSingleton
{
protected:
    TemplateSingleton()
    {

    }
    virtual ~TemplateSingleton()
    {

    }

public:
    static T& GetInstance()
    {
        if( m_pInstance.get() == 0 )
        {
            m_pInstance.reset( new T() );
        }
        return *m_pInstance;
    }

    static void DestroyInstance()
    {
        if (m_pInstance)
        {
            m_pInstance.release();
        }
    }

private:
    static std::unique_ptr<T> m_pInstance;
};

template <typename T> std::unique_ptr<T> TemplateSingleton<T>::m_pInstance;

#endif // TEMPLATESINGLETONE_H
