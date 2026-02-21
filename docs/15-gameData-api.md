# StrikeEngine GameData API Documentation

## Overview

The `GameData` class is a singleton for managing persistent game data as key-value pairs. It provides storage and retrieval with support for multiple data types: `int`, `float`, `double`, `bool`, and `std::string`.

Data is stored in memory only — persistence across sessions must be handled externally.

> **Note:** `GameData` is not thread-safe. If you access it from multiple threads, synchronization must be handled externally.

---

## Singleton Access

#### `static GameData& get()`
Returns the singleton instance of `GameData`.

```cpp
Strike::GameData& data = Strike::GameData::get();
```

---

## Type-Specific Setters

#### `void setInt(const std::string& key, int value)`
Stores an integer value under the given key.

#### `void setFloat(const std::string& key, float value)`
Stores a float value under the given key.

#### `void setDouble(const std::string& key, double value)`
Stores a double value under the given key.

#### `void setBool(const std::string& key, bool value)`
Stores a boolean value under the given key.

#### `void setString(const std::string& key, const std::string& value)`
Stores a string value under the given key.

---

## Type-Specific Getters

All getters accept an optional `defaultValue` that is returned if the key does not exist or a type mismatch occurs.

#### `int getInt(const std::string& key, int defaultValue = 0) const`
Returns the integer stored at `key`, or `defaultValue` if not found.

#### `float getFloat(const std::string& key, float defaultValue = 0.0f) const`
Returns the float stored at `key`, or `defaultValue` if not found.

#### `double getDouble(const std::string& key, double defaultValue = 0.0) const`
Returns the double stored at `key`, or `defaultValue` if not found.

#### `bool getBool(const std::string& key, bool defaultValue = false) const`
Returns the bool stored at `key`, or `defaultValue` if not found.

#### `std::string getString(const std::string& key, const std::string& defaultValue = "") const`
Returns the string stored at `key`, or `defaultValue` if not found.

---

## Key Management

#### `bool hasKey(const std::string& key) const`
Returns true if the key exists in the store, false otherwise.

#### `void deleteKey(const std::string& key)`
Removes a specific key and its associated value.

#### `void deleteAll()`
Removes all stored key-value pairs.

#### `std::vector<std::string> getAllKeys() const`
Returns a vector of all currently stored keys.

---

## Utility Methods

#### `size_t getDataCount() const`
Returns the number of currently stored key-value pairs.

#### `void printAllData() const`
Prints all stored key-value pairs to the console including their types. Intended for debugging.

---

## Usage Example

```cpp
auto& data = Strike::GameData::get();

// Store values
data.setInt("score", 100);
data.setFloat("health", 0.75f);
data.setBool("levelComplete", true);
data.setString("playerName", "Hero");

// Retrieve values
int score        = data.getInt("score");           // 100
float health     = data.getFloat("health");        // 0.75f
bool complete    = data.getBool("levelComplete");  // true
std::string name = data.getString("playerName");   // "Hero"

// Check and delete
if (data.hasKey("score")) {
    data.deleteKey("score");
}

// Debug
data.printAllData();
```

---

## Notes

**Not Thread-Safe** — `GameData` is designed for single-threaded access (main thread game logic). If you need to access it from multiple threads, add external synchronization.

**Singleton Pattern** — Copy and move constructors and assignment operators are all deleted, ensuring only one instance exists throughout the application lifetime.

**Supported Types** — Only `int`, `float`, `double`, `bool`, and `std::string` are supported. Use the appropriate typed getter/setter for each type.

**Default Values on Mismatch** — All typed getters return `defaultValue` not only when the key is missing but also when a type mismatch occurs (e.g. calling `getInt()` on a key that holds a `float`).

**Data Persistence** — `GameData` stores data in memory only. It does not save to disk automatically. To persist data between sessions, serialize it yourself using `getAllKeys()` and the typed getters/setters.

## Next Step

- [Utilities](16-utilities.md)