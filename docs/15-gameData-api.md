# StrikeEngine GameData API Documentation

## Overview

The `GameData` class is a singleton for managing persistent game data across all scenes as key-value pairs. It provides storage and retrieval with support for multiple data types: `int`, `float`, `double`, `bool`, and `std::string`.

Data is stored in memory only - persistence across sessions must be handled externally.


---

## Singleton Access

#### `static GameData& get()`
Returns the singleton instance of `GameData`.

```cpp
Strike::GameData& data = Strike::GameData::get();
```

---

## Type-Specific Setters

All setters store the value under the given key and **return the stored value**.

#### `int setInt(const std::string& key, int value)`
Stores an integer value under the given key and returns it.

#### `float setFloat(const std::string& key, float value)`
Stores a float value under the given key and returns it.

#### `double setDouble(const std::string& key, double value)`
Stores a double value under the given key and returns it.

#### `bool setBool(const std::string& key, bool value)`
Stores a boolean value under the given key and returns it.

#### `std::string setString(const std::string& key, const std::string& value)`
Stores a string value under the given key and returns it.

---

## Type-Specific Getters

> **Important:** Getters do **not** accept a default value. If the key does not exist or a type mismatch occurs, an assertion is triggered. Always verify a key exists with `hasKey()` before retrieving it.

#### `int getInt(const std::string& key) const`
Returns the integer stored at `key`.
Asserts if the key does not exist or the stored type is not `int`.

#### `float getFloat(const std::string& key) const`
Returns the float stored at `key`.
Asserts if the key does not exist or the stored type is not `float`.

#### `double getDouble(const std::string& key) const`
Returns the double stored at `key`.
Asserts if the key does not exist or the stored type is not `double`.

#### `bool getBool(const std::string& key) const`
Returns the bool stored at `key`.
Asserts if the key does not exist or the stored type is not `bool`.

#### `std::string getString(const std::string& key) const`
Returns the string stored at `key`.
Asserts if the key does not exist or the stored type is not `std::string`.

---

## Key Management

#### `bool hasKey(const std::string& key) const`
Returns `true` if the key exists in the store, `false` otherwise.

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

// Store values (setters return the stored value)
int score        = data.setInt("score", 100);       // 100
float health     = data.setFloat("health", 0.75f);  // 0.75f
bool complete    = data.setBool("levelComplete", true);
std::string name = data.setString("playerName", "Hero");

// Retrieve values - always check hasKey() first to avoid assertion failures
if (data.hasKey("score")) {
    int s = data.getInt("score");  // 100
}

if (data.hasKey("health")) {
    float h = data.getFloat("health");  // 0.75f
}

// Check and delete
if (data.hasKey("score")) {
    data.deleteKey("score");
}

// Debug
data.printAllData();
```



## Next Step

- [Utilities](16-utilities.md)