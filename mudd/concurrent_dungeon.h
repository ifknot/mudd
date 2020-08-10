#pragma once

#include <iostream>
#include <array>
#include <mutex>
#include <shared_mutex>

namespace mudd {

	template<typename item_t, size_t width, size_t height>
	class concurrent_dungeon {

	public:

		concurrent_dungeon() = default;

		const item_t& read (size_t x, size_t y) const;

		void write (item_t& item, size_t x, size_t y);

		void write(item_t&& item, size_t x, size_t y); 

		bool try_read(const item_t& item, size_t x, size_t y) const;

		bool try_write(item_t& item, size_t x, size_t y);

	private:

		mutable std::shared_mutex mutex_;

		std::array<std::array<item_t, width>, height> dungeon_;

	};

	template<typename item_t, size_t width, size_t height>
	inline const item_t& concurrent_dungeon<item_t, width, height>::read(size_t x, size_t y) const {
		std::shared_lock lock(mutex_, std::defer_lock);	//create the lock guard without acquiring it (that's the point of std::defer_lock) 
		try {
			lock.lock();
			return dungeon_[x][y];
		}
		catch (const std::system_error& e) {
			std::cout << "Caught system_error with code " << e.code() << " " << e.what() << '\n';
		}
	}

	template<typename item_t, size_t width, size_t height>
	inline void concurrent_dungeon<item_t, width, height>::write(item_t& item, size_t x, size_t y) {
		std::unique_lock lock(mutex_, std::defer_lock);
		try {
			lock.lock();
			dungeon_[x][y] = item;
		}
		catch (const std::system_error& e) {
			std::cout << "Caught system_error with code " << e.code() << " " << e.what() << '\n';
		}
	}

	template<typename item_t, size_t width, size_t height>
	inline void concurrent_dungeon<item_t, width, height>::write(item_t&& item, size_t x, size_t y)
	{
	}

	template<typename item_t, size_t width, size_t height>
	inline bool concurrent_dungeon<item_t, width, height>::try_read(const item_t& item, size_t x, size_t y) const {
		std::shared_lock lock(mutex_, std::defer_lock);
		if (lock.try_lock()) {
			item = dungeon_[x][y];
			return true;
		}
		return false;
	}

	template<typename item_t, size_t width, size_t height>
	inline bool concurrent_dungeon<item_t, width, height>::try_write(item_t& item, size_t x, size_t y) {
		std::unique_lock lock(mutex_, std::defer_lock);
		if(lock.try_lock()) {
			dungeon_[x][y] = item;
			return true;
		}
		return false;
	}

}