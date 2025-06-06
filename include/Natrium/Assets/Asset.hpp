#if !defined(NA_ASSET_HPP)
#define NA_ASSET_HPP

#include "Natrium/Core.hpp"

namespace Na {
	class Asset {
	public:
		Asset(void) = default;
		virtual ~Asset(void) = default;

		[[nodiscard]] virtual inline operator bool(void) const = 0;
	};

	template<typename t_Asset = Asset>
	using AssetHandle = Ref<t_Asset>;

	template<typename t_Asset = Asset>
	using WeakAssetHandle = WeakRef<t_Asset>;

	template<typename T>
	concept DerivedAsset = std::is_base_of<Asset, T>::value && !std::is_same<Asset, T>::value;

	template<typename T>
	concept LoadableAsset =
		DerivedAsset<T> &&
		requires(const std::filesystem::path& path)
		{
			{ T::Load(path) } -> std::same_as<AssetHandle<T>>;
		};
} // namespace Na

#endif // NA_ASSET_HPP