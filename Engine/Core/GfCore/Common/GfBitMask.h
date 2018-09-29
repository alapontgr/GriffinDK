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
class GfBitMask
{
public:
	GfBitMask(FlagT uiFlags = uiDefault);

	void Reset();

	void Clear();

	FlagT CFlags() const;

	FlagT& Flags();

	void Set(const FlagT uiValue, bool bEnable);

	void Set(const FlagT uiValue);

	bool IsEnable(const FlagT uiValue) const;

	void Toggle(const FlagT uiValue);

	GfBitMask& operator&=(const GfBitMask& uiOther);

	GfBitMask& operator|=(const GfBitMask& uiOther);

	GfBitMask operator&(const GfBitMask& uiOther) const;

	GfBitMask operator|(const GfBitMask& uiOther) const;

	GfBitMask operator~() const;

	GfBitMask operator^(const GfBitMask& uiOther) const;

	bool operator== (const GfBitMask& uiOther);

	bool operator!= (const GfBitMask& uiOther);

private:
	FlagT m_uiFlags;
};

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
FlagT GfBitMask<FlagT, uiDefault>::CFlags() const
{
	return m_uiFlags;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
FlagT& GfBitMask<FlagT, uiDefault>::Flags()
{
	return m_uiFlags;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
void GfBitMask<FlagT, uiDefault>::Set(const FlagT uiValue, bool bEnable)
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
void GfBitMask<FlagT, uiDefault>::Set(const FlagT uiValue)
{
	m_uiFlags = uiValue; 
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
bool GfBitMask<FlagT, uiDefault>::IsEnable(const FlagT uiValue) const
{
	return (m_uiFlags & uiValue) > 0;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
void GfBitMask<FlagT, uiDefault>::Toggle(const FlagT uiValue)
{
	m_uiFlags ^= uiValue;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
GfBitMask<FlagT, uiDefault>& GfBitMask<FlagT, uiDefault>::operator&=(const GfBitMask& uiOther)
{
	m_uiFlags &= uiOther.m_uiFlags;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
GfBitMask<FlagT, uiDefault>& GfBitMask<FlagT, uiDefault>::operator|=(const GfBitMask& uiOther)
{
	m_uiFlags |= uiOther.m_uiFlags;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
GfBitMask<FlagT, uiDefault> GfBitMask<FlagT, uiDefault>::operator&(const GfBitMask& uiOther) const
{
	return GfBitMask(m_uiFlags & uiOther.m_uiFlags);
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
GfBitMask<FlagT, uiDefault> GfBitMask<FlagT, uiDefault>::operator|(const GfBitMask& uiOther) const
{
	return GfBitMask(m_uiFlags | uiOther.m_uiFlags);
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
GfBitMask<FlagT, uiDefault> GfBitMask<FlagT, uiDefault>::operator~() const
{
	return GfBitMask(~m_uiFlags);
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
GfBitMask<FlagT, uiDefault> GfBitMask<FlagT, uiDefault>::operator^(const GfBitMask& uiOther) const
{
	return GfBitMask(m_uiFlags ^ uiOther.m_uiFlags);
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
bool GfBitMask<FlagT, uiDefault>::operator==(const GfBitMask& uiOther)
{
	return m_uiFlags == uiOther.m_uiFlags;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
bool GfBitMask<FlagT, uiDefault>::operator!=(const GfBitMask& uiOther)
{
	return m_uiFlags != uiOther.m_uiFlags;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
GfBitMask<FlagT, uiDefault>::GfBitMask(FlagT uiFlags /*= uiDefault*/) : m_uiFlags{ uiFlags }
{
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
void GfBitMask<FlagT, uiDefault>::Reset()
{
	m_uiFlags = uiDefault;
}

////////////////////////////////////////////////////////////////////////////////

template <typename FlagT, FlagT uiDefault /*= 0*/>
void GfBitMask<FlagT, uiDefault>::Clear()
{
	m_uiFlags = 0;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFBITMASK_H__