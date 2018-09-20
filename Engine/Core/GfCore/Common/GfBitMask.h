////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/20
//	File: 	GfBitMask.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFBITMASK_H__
#define __GFBITMASK_H__
////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault = 0>
class Mask
{
public:
	Mask(FlagT uiFlags = uiDefault);

	void Reset();

	void Clear();

	FlagT CFlags() const;

	FlagT& Flags();

	void Set(const FlagT uiValue, bool bEnable);

	void Set(const FlagT uiValue);

	bool IsEnable(const FlagT uiValue) const;

	void toggle(const FlagT uiValue);

	Mask& operator&=(const Mask& uiOther);

	Mask& operator|=(const Mask& uiOther);

	Mask operator&(const Mask& uiOther) const;

	Mask operator|(const Mask& uiOther) const;

	Mask operator~() const;

	Mask operator^(const Mask& uiOther) const;

	bool operator== (const Mask& uiOther);

	bool operator!= (const Mask& uiOther);

private:
	FlagT m_uiFlags;
};

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
FlagT Mask<FlagT, uiDefault>::CFlags() const
{
	return m_uiFlags;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
FlagT& Mask<FlagT, uiDefault>::Flags()
{
	return m_uiFlags;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
void Mask<FlagT, uiDefault>::Set(const FlagT uiValue, bool bEnable)
{
	if (bEnable)
	{
		m_uiFlags |= uiValue;
	}
	else
	{
		m_uiFlags &= (~uiValue);
	}
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
void Mask<FlagT, uiDefault>::Set(const FlagT uiValue)
{
	m_uiFlags = uiValue; 
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
bool Mask<FlagT, uiDefault>::IsEnable(const FlagT uiValue) const
{
	return (m_uiFlags & uiValue) > 0;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
void Mask<FlagT, uiDefault>::toggle(const FlagT uiValue)
{
	m_uiFlags ^= uiValue;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
Mask& Mask<FlagT, uiDefault>::operator&=(const Mask& uiOther)
{
	m_uiFlags &= uiOther.m_uiFlags;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
Mask& Mask<FlagT, uiDefault>::operator|=(const Mask& uiOther)
{
	m_uiFlags |= uiOther.m_uiFlags;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
Mask Mask<FlagT, uiDefault>::operator&(const Mask& uiOther) const
{
	return Mask(m_uiFlags & uiOther.m_uiFlags);
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
Mask Mask<FlagT, uiDefault>::operator|(const Mask& uiOther) const
{
	return Mask(m_uiFlags | uiOther.m_uiFlags);
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
Mask Mask<FlagT, uiDefault>::operator~() const
{
	return Mask(~m_uiFlags);
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
Mask Mask<FlagT, uiDefault>::operator^(const Mask& uiOther) const
{
	return Mask(m_uiFlags ^ uiOther.m_uiFlags);
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
bool Mask<FlagT, uiDefault>::operator==(const Mask& uiOther)
{
	return m_uiFlags == uiOther.m_uiFlags;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
bool Mask<FlagT, uiDefault>::operator!=(const Mask& uiOther)
{
	return m_uiFlags != uiOther.m_uiFlags;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
Mask<FlagT, uiDefault>::Mask(FlagT uiFlags /*= uiDefault*/) : m_uiFlags{ uiFlags }
{

}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
void Mask<FlagT, uiDefault>::Reset()
{
	m_uiFlags = uiDefault;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
void Mask<FlagT, uiDefault>::Clear()
{
	m_uiFlags = 0;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFBITMASK_H__