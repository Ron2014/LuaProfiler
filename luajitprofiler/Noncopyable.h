/**
 * @file Noncopyable.h
 * @brief ��ֹ���ƻ���,�̳и�������಻������
 *
 *
 * @version 1.0
 * @author OWenT
 * @date 2012.02.21
 *
 * @history
 *
 *
 */

#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

class Noncopyable
{
protected:
    Noncopyable() {}
    ~Noncopyable() {}
private:
    Noncopyable( const Noncopyable& );
    const Noncopyable& operator=( const Noncopyable& );
};

#endif
