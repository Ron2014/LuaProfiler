/**
 * @file Singleton.h
 * @brief ����ģʽ����, �ο� boost::serialization::singleton,ȥ����Debug������ <br />
 *        ʵ���ĳ�ʼ������ģ�����루����Ƕ�̬���ӿ��������붯̬���ӿ⣩ʱ���� <br />
 *        ��ģ��ж��ʱ���Զ����� <br />
 *
 * Note that this singleton class is thread-safe.
 *
 * @version 1.0
 * @author OWenT
 * @date 2012.02.13
 *
 * @history
 *   2012.07.20 Ϊ�̰߳�ȫ���Ľ�ʵ�ַ�ʽ
 *
 */

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include "Noncopyable.h"

#include <cstddef>

namespace wrapper
{
    template<class T>
    class SingletonWrapper : public T
    {
    public:
        static bool ms_bDestroyed;
        ~SingletonWrapper()
        {
            ms_bDestroyed = true;
        }
    };

    template<class T>
    bool SingletonWrapper< T >::ms_bDestroyed = false;

}

template <typename T>
class Singleton : public Noncopyable
{
public:
	/**
	 * @brief ������������
	 */
    typedef T self_type;

protected:

	/**
	 * @brief ���࣬��ֱֹ�ӹ���
	 */
    Singleton() {}

    /**
     * @brief �����ڳ�ʼ���׶�ǿ�ƹ��쵥��ʵ��
     */
    static void use(self_type const &) {}

public:
	/**
	 * @brief ��ȡ������������
	 * @return T& instance
	 */
    static T& GetInstance()
    {
        static wrapper::SingletonWrapper<self_type> stInstance;
        use(stInstance);
        return static_cast<T&>(stInstance);
    }

	/**
	 * @brief ��ȡ��������������
	 * @return const T& instance
	 */
    static const T& GetConstInstance()
    {
        return GetInstance();
    }

    /**
     * @brief ��ȡʵ��ָ�� (���Ƽ�ʹ�ã�������ǰ����)
     * @return T* instance
     */
    static self_type* Instance()
    {
        return &GetInstance();
    }

	/**
	 * @brief �ж��Ƿ��ѱ�����
	 * @return bool 
	 */
    static bool IsInstanceDestroyed()
    {
        return wrapper::SingletonWrapper<T>::ms_bDestroyed;
    }
};

#endif
