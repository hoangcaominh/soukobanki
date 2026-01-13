#pragma once
#include <cstdint>

/**
 * bit 0: player
 * bit 1: render wall/floor
 * bit 2: wall/floor
 * bit 3: objective
 * bit 4: box
 */
#define DATA_SIZE 5

typedef uint8_t TileData;

enum MTType {
    BLANK = 0x0,
    PLAYER = 0x1,
    WORLD = 0x2,
    REACHABLE = 0x4,
    OBJECTIVE = 0x8,
    BOX = 0x10,
};

class MapTile {
public:
    MapTile() noexcept;
    MapTile(TileData data) noexcept;

    TileData val() const noexcept;
    void set_val(TileData data) noexcept;
    bool has(MTType type) const noexcept;
    void set(MTType type) noexcept;
    void append(MTType type) noexcept;
    void remove(MTType type) noexcept;
    MapTile& operator+=(MTType type) noexcept;
    MapTile& operator-=(MTType type) noexcept;
    bool operator==(const MapTile& rhs) const noexcept;
private:
    TileData data;
};
