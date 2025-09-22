#include "Pch.hpp"
#include "Natrium/Core/UUID.hpp"

#include "Natrium/Core/Context.hpp"

namespace Na {
	const UUID_t& UUID::Null(void)
	{
		static const UUID_t x_NullId;
		return x_NullId;
	}

	UUID_t UUID::Generate(void)
	{
		return Context::Get().uuid_generator()();
	}

	UUID_t UUID::Generate(const std::string& name)
	{
		return Context::Get().uuid_name_generator()(name);
	}

	UUID_t UUID::Generate(const std::filesystem::path& path)
	{
		return Context::Get().uuid_name_generator()(path.string());
	}

	UUID_t UUID::FromString(const std::string& uuid_string)
	{
		try {
			return UUID_t::from_string(uuid_string).value();
		} catch (...)
		{
			return UUID_t();
		}
	}

	[[nodiscard]] std::string UUID::ToString(const UUID_t& uuid)
	{
		return uuids::to_string(uuid);
	}


	UUID_t UUID::FromBytes(const Byte* bytes)
	{
		return UUID_t(bytes, bytes + 16);
	}

	UUID_t FromBytes(const std::array<Byte, 16>& bytes)
	{
		return UUID_t(bytes);
	}

} // namespace Na
