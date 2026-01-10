#include "maptile.h"

MapTile::MapTile() noexcept : data(0) {}

TileData MapTile::val() const noexcept {
    return data;
}

bool MapTile::has(MTType type) const noexcept {
    return data & type;
}

void MapTile::set(MTType type) noexcept {
    data = type;
}

void MapTile::append(MTType type) noexcept {
    data |= type;
}

void MapTile::remove(MTType type) noexcept {
    data &= ~type;
}

MapTile& MapTile::operator+=(MTType type) noexcept {
    append(type);
    return *this;
}

MapTile& MapTile::operator-=(MTType type) noexcept {
    remove(type);
    return *this;
}
