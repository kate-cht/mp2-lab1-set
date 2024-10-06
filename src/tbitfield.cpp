// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"
#include <string>
#include <algorithm>
using namespace std;
// Fake variables used as placeholders in tests
static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);

/*int  BitLen; // длина битового поля - макс. к-во битов
  TELEM *pMem; // память для представления битового поля
  int  MemLen; // к-во эл-тов Мем для представления бит.поля*/
  // Структура хранения битового поля
  //   бит.поле - набор битов с номерами от 0 до BitLen
  //   массив pМем рассматривается как последовательность MemLen элементов
  //   биты в эл-тах pМем нумеруются справа налево (от младших к старшим)

TBitField::TBitField(int len)
{
    if (len >= 0) {
        BitLen = len; // длина битового поля - макс. к-во битов -- просто количество элементов
        MemLen = (len / (sizeof(TELEM) * 8)) + 1;  // кол-ва Мем = вся длина/размер Мем
        pMem = new TELEM[MemLen];
        if (pMem == nullptr) // добавлено
            throw domain_error("domain_error");
        
        memset(pMem, 0, MemLen * sizeof(TELEM));
    }
    else throw out_of_range("invalid length");
}

TBitField::TBitField(const TBitField &bf) // конструктор копирования
{
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    pMem = new TELEM[MemLen];
    if (pMem == nullptr) 
        throw domain_error("domain_error");
    for (int i = 0; i < MemLen; i++) {
        pMem[i] = bf.pMem[i];
    }
}

TBitField::~TBitField() 
{
    delete[] pMem;
    BitLen = 0;
    MemLen = 0;
    pMem = nullptr;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
    if (n >= BitLen || n < 0) 
        throw out_of_range("out of range");
    return n / (sizeof(TELEM) * 8); 
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
    if (n >= BitLen || n < 0)
        throw out_of_range("out of range");
    return ((TELEM)1) << (n % (sizeof(TELEM) * 8));
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
    return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
    if (n >= BitLen || n < 0)
        throw out_of_range("out of range");
    pMem[GetMemIndex(n)] |= GetMemMask(n);
}

void TBitField::ClrBit(const int n) // очистить бит
{
    if (n >= BitLen || n < 0)
        throw out_of_range("out of range");
    pMem[GetMemIndex(n)] &= ~GetMemMask(n);
}

int TBitField::GetBit(const int n) const // получить значение бита
{
    if (n >= BitLen || n < 0)
        throw out_of_range("out of range");
    return (pMem[GetMemIndex(n)] & GetMemMask(n)) > 0;
}

// битовые операции

TBitField& TBitField::operator=(const TBitField &bf) // присваивание
{
    if (this != &bf) {
        if (MemLen != bf.MemLen) { //  исправлено
            delete[] pMem;
            pMem = new TELEM[MemLen];
            if (pMem == nullptr)
                throw domain_error("domain_error");
            }
        BitLen = bf.BitLen; 
        for (int i = 0; i < MemLen; i++)
            pMem[i] = bf.pMem[i];
    }
    return *this;
}

int TBitField::operator==(const TBitField &bf) const // сравнение==
{
    if (BitLen != bf.BitLen) 
        return 0;
    else
        for (int i = 0; i < MemLen; i++)
            if (pMem[i] != bf.pMem[i]) 
                return 0;
    return 1;
}

int TBitField::operator!=(const TBitField &bf) const // сравнение!=
{
    return !(*this == bf);
}

TBitField TBitField::operator|(const TBitField &bf) // операция "или"
{
    TBitField res(max(BitLen, bf.BitLen));
    int end = min(MemLen, bf.MemLen);
    int b = max(BitLen, bf.BitLen);
    for (int i = 0; i < end; i++)
        res.pMem[i] = pMem[i] | bf.pMem[i];
    if (MemLen < bf.MemLen)
        for (int i = end; i < bf.MemLen; i++)
            res.pMem[i] = bf.pMem[i];
    else
        for (int i = end; i < MemLen; i++)
            res.pMem[i] = pMem[i];
    // исправлено
    return res;
}

TBitField TBitField::operator&(const TBitField &bf) // операция "и"
{
    TBitField res(max(BitLen, bf.BitLen));
    int end = min(MemLen, bf.MemLen);
    for (int i = 0; i < end; i++)
        res.pMem[i] = bf.pMem[i] & pMem[i];
    return res;
}

TBitField TBitField::operator~(void) // отрицание
{
    TBitField res(*this);
    for (int i = 0; i < MemLen; i++)
        res.pMem[i] = ~(pMem[i]);
    TELEM d = ((TELEM)1 << (BitLen % (sizeof(TELEM) * 8))) - (TELEM)1;
    res.pMem[MemLen - 1] &= d;
    return res;
}

// ввод/вывод

istream &operator>>(istream &istr, TBitField &bf) // ввод
{ 
    string in;
    istr >> in;
    if (in.size() > bf.BitLen) 
        throw out_of_range("out of range");
    else 
        for (int i = 0; i < in.size(); i++) 
            if (in[i] != '0') 
                bf.SetBit(i);
    return istr;

}

ostream &operator<<(ostream &ostr, const TBitField &bf) // вывод
{
    for (int i = 0; i < bf.BitLen; i++) {
        if (bf.GetBit(i) == 1) 
            ostr << 1; 
        else ostr << 0;
    }
    return ostr;
} 