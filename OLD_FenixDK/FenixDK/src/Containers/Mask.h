#pragma once

namespace fdk
{
namespace Containers
{
	template <typename FlagT, FlagT Default = 0>
	class Mask
	{
	public:
		Mask(FlagT flags = Default)
				: m_flags{flags}
		{
		}

    void reset() 
    {
      m_flags = Default;
    }

    void clear() 
    {
      m_flags = 0;
    }

		FlagT c_flags() const { return m_flags; }

		FlagT& flags() { return m_flags; }

    void set(const FlagT value, bool enable) 
    {
      if (enable) 
      {
        m_flags |= value;
      }
      else 
      {
        m_flags &= (~value);
      }
    }

    void set(const FlagT value) { m_flags = value; }

    bool is_enable(const FlagT value) const { return (m_flags & value) > 0; }

    void toggle(const FlagT value) { m_flags ^= value; }

		
    Mask& operator&=(const Mask& rOther)
		{
			m_flags &= rOther.m_flags;
			return *this;
		}

		Mask& operator|=(const Mask& rOther)
		{
			m_flags |= rOther.m_flags;
			return *this;
		}

		Mask operator&(const Mask& rOther) const {	return Mask(m_flags & rOther.m_flags); }

		Mask operator|(const Mask& rOther) const { return Mask(m_flags | rOther.m_flags); }

		Mask operator~() const {	return Mask(~m_flags); }

		Mask operator^(const Mask& rOther) const { return Mask(m_flags ^ rOther.m_flags); }

    bool operator== (const Mask& rOther) { return m_flags == rOther.m_flags; }

    bool operator!= (const Mask& rOther) { return m_flags != rOther.m_flags; }

	private:
		FlagT m_flags;
	};
}
}