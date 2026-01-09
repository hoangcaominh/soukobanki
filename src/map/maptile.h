#pragma once
#include <cstdint>

/**
 * bit 0: wall
 * bit 1: floor
 * bit 2: objective
 * bit 3: box
 * bit 4: player
 */
#define DATA_SIZE 5

typedef uint8_t TileData;

class MapTile {
public:
    enum Type {
        WALL = 0x1,
        FLOOR = 0x2,
        OBJECTIVE = 0x4,
        BOX = 0x8,
        PLAYER = 0x10,
    };

    MapTile() noexcept;

    TileData val() const noexcept;
    bool has(const Type &type) const noexcept;
    void set(const Type &type) noexcept;
    void append(const Type &type) noexcept;
    void remove(const Type &type) noexcept;
    MapTile& operator+=(const Type &type) noexcept;
    MapTile& operator-=(const Type &type) noexcept;
private:
    TileData data;
};

typedef MapTile::Type MTType;
